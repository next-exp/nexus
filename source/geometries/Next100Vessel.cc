// ----------------------------------------------------------------------------
// nexus | Next100Vessel.cc
//
// Vessel of the NEXT-100 geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100Vessel.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "OpticalMaterialProperties.h"
#include "CylinderPointSampler2020.h"
#include "SpherePointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>
#include <G4UnitsTable.hh>

#include <CLHEP/Units/SystemOfUnits.h>

#include <math.h>
#include <algorithm>

namespace nexus {

  using namespace CLHEP;

  Next100Vessel::Next100Vessel():
    GeometryBase(),

    // general vessel dimensions
    vessel_in_rad_    (68.0  * cm),
    vessel_thickness_ (1.  * cm),

    // Body
    body_length_     (198.6 * cm),

    // Endcaps dimensions
    endcap_in_rad_      (123.61 * cm),
    endcap_in_body_     (15.15  * cm),
    endcap_theta_       (29.1   * deg),
    endcap_in_z_width_  (15.6   * cm),
    endcap_tp_distance_ (44.92  * cm),

    // Ports
    port_base_height_(37. * mm),
    port_tube_height_(159.* mm),
    port_tube_tip_   (4.  * mm),
    // They are defined global because are needed at the vertex generation
    port_x_ ((vessel_in_rad_ + port_base_height_ - (port_tube_height_ + port_tube_tip_)/2.)/sqrt(2.)), // inner port pos
    port_y_ (port_x_),
    source_height_ (5. * mm), // preliminar

    // Vessel gas
    sc_yield_(16670. * 1/MeV),
    e_lifetime_(1000. * ms),
    pressure_   (13.5 * bar),
    temperature_(293. * kelvin),
    // Visibility
    visibility_(0),
    gas_("enrichedXe"),
    helium_mass_num_(4),
    xe_perc_(100.)
  {

    // Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/Next100/", "Control commands of geometry Next100.");

    msg_->DeclareProperty("vessel_vis", visibility_, "Vessel Visibility");
    msg_->DeclareProperty("gas", gas_, "Gas being used");
    msg_->DeclareProperty("XePercentage", xe_perc_,
			  "Percentage of xenon used in mixtures");
    msg_->DeclareProperty("helium_A", helium_mass_num_,
			  "Mass number for helium used, 3 or 4");

    G4GenericMessenger::Command& pressure_cmd = msg_->DeclareProperty("pressure", pressure_, "Xenon pressure");
    pressure_cmd.SetUnitCategory("Pressure");
    pressure_cmd.SetParameterName("pressure", false);
    pressure_cmd.SetRange("pressure>0.");

    new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);

    G4GenericMessenger::Command& sc_yield_cmd =
      msg_->DeclareProperty("sc_yield", sc_yield_,
			    "Set scintillation yield for GXe. It is in photons/MeV");
    sc_yield_cmd.SetParameterName("sc_yield", true);
    sc_yield_cmd.SetUnitCategory("1/Energy");

    G4GenericMessenger::Command& e_lifetime_cmd =
      msg_->DeclareProperty("e_lifetime", e_lifetime_,
			    "Electron lifetime in gas.");
    e_lifetime_cmd.SetParameterName("e_lifetime", false);
    e_lifetime_cmd.SetUnitCategory("Time");
    e_lifetime_cmd.SetRange("e_lifetime>0.");

  }


  void Next100Vessel::Construct()
  {
    // Body solid
    G4double vessel_out_rad = vessel_in_rad_ + vessel_thickness_;

    G4Tubs* vessel_body_solid = new G4Tubs("VESSEL_BODY", 0., vessel_out_rad, body_length_/2.,
					   0.*deg, 360.*deg);

    G4Tubs* vessel_gas_body_solid = new G4Tubs("VESSEL_GAS_BODY", 0., vessel_in_rad_, body_length_/2.,
					       0.*deg, 360.*deg);

    // Endcaps solids
    G4double endcap_out_rad = endcap_in_rad_ + vessel_thickness_;

    G4Sphere* vessel_endcap_solid = new G4Sphere("VESSEL_ENDCAP",
							  0. * cm,  endcap_out_rad,
							  0. * deg, 360. * deg,
							  0. * deg, endcap_theta_);

    G4Sphere* vessel_gas_endcap_solid = new G4Sphere("VESSEL_GAS_ENDCAP",
							      0. * cm,  endcap_in_rad_,
							      0. * deg, 360. * deg,
							      0. * deg, endcap_theta_);

    // Flange solid
    G4double flange_out_rad   = 74.0 * cm;
    G4double flange_tp_length = 41.5 * mm + 41.5 * mm; // 41.5 mm from the endcap
    G4double flange_ep_length = 77.5 * mm + 41.5 * mm;
    G4double flange_ep_z_pos  =   body_length_/2. - flange_ep_length/2. - endcap_in_body_;
    G4double flange_tp_z_pos  = -(body_length_/2. - flange_tp_length/2. - endcap_in_body_);

    G4Tubs* vessel_tp_flange_solid = new G4Tubs("VESSEL_TRACKING_FLANGE", vessel_in_rad_, flange_out_rad,
                                                flange_tp_length/2., 0.*deg, 360.*deg);
    G4Tubs* vessel_ep_flange_solid = new G4Tubs("VESSEL_ENERGY_FLANGE"  , vessel_in_rad_, flange_out_rad,
                                                flange_ep_length/2., 0.*deg, 360.*deg);

    // Calibration ports: nozzel and inner tube
    G4double port_in_rad      = 15.5 * mm;
    G4double port_cap_height  = 15.  * mm;
    G4double port_cap_rad     = 37.5 * mm;
    G4double port_base_rad    = 21.1 * mm;
    G4double offset = 1. * mm; // add offset to overlap G4UnionSolid

    G4Tubs* port_solid_cap  = new G4Tubs("PORT_cap" , 0., port_cap_rad, port_cap_height/2.,
                                         0.*deg, 360.*deg);
    G4Tubs* port_solid_base = new G4Tubs("PORT_base", 0., port_base_rad, (port_base_height_+offset)/2.,
                                         0.*deg, 360.*deg);
    G4UnionSolid* port_solid = new G4UnionSolid("PORT", port_solid_base, port_solid_cap,
               0, G4ThreeVector(0., 0., (port_base_height_ + offset + port_cap_height)/2. - offset));
    G4Tubs* port_gas_solid = new G4Tubs("PORT_GAS", 0., port_in_rad, (port_base_height_ + offset)/2.,
                                        0.*deg, 360.*deg);

    // Port inner tube
    G4double port_tube_rad   = 4.  * mm;
    G4double port_tube_thick = 1.2 * mm;
    G4Tubs* port_tube_solid = new G4Tubs("PORT_TUBE", 0., (port_tube_rad + port_tube_thick),
                                         (port_tube_height_ + port_tube_tip_)/2., 0.*deg, 360.*deg);
    G4Tubs* port_tube_gas_solid = new G4Tubs("PORT_TUBE_GAS", 0., port_tube_rad,
                                             port_tube_height_/2., 0.*deg, 360.*deg);

    //// Unions
    G4double endcap_z_pos = (body_length_ / 2.) + endcap_in_z_width_ - endcap_in_rad_;
    G4ThreeVector energy_endcap_pos  (0, 0,  endcap_z_pos);
    G4ThreeVector tracking_endcap_pos(0, 0, -endcap_z_pos);
    G4ThreeVector energy_flange_pos  (0, 0, flange_ep_z_pos);
    G4ThreeVector tracking_flange_pos(0, 0, flange_tp_z_pos);

    // tracking endcap z --> -z (we can rotate either in X or Y)
    G4RotationMatrix* xRot = new G4RotationMatrix;
    xRot->rotateX(180. * deg);

    // Body + Energy endcap
    G4UnionSolid* vessel_solid = new G4UnionSolid("VESSEL", vessel_body_solid, vessel_endcap_solid,
						  0, energy_endcap_pos);

    // Body + Energy endcap + Tracking endcap
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, vessel_endcap_solid,
				    xRot, tracking_endcap_pos);

    // Body + Energy endcap + Tracking endcap + Energy flange
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid,
				    vessel_ep_flange_solid, 0, energy_flange_pos);

    // Body + Energy endcap + Tracking endcap + Energy flange + Tracking flange
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid,
				    vessel_tp_flange_solid, 0, tracking_flange_pos);

    // Add port nozzles (x,y at 45*deg)
    G4double port_nozzle_x = (vessel_in_rad_ + port_base_height_/2.)/sqrt(2.);
    G4double port_nozzle_y = port_nozzle_x;

    G4double port_reference_z = -body_length_/2. + endcap_in_body_;
    port_z_1a_ = port_reference_z +   702. * mm;
    port_z_2a_ = port_reference_z +  1292. * mm;
    port_z_1b_ = port_reference_z +  533.6 * mm;
    port_z_2b_ = port_reference_z + 1033.6 * mm;

    G4RotationMatrix* port_a_Rot = new G4RotationMatrix;
    port_a_Rot->rotateX( 90. * deg);
    port_a_Rot->rotateY(-45. * deg);

    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, port_solid,
                                    port_a_Rot, G4ThreeVector(port_nozzle_x, port_nozzle_y, port_z_1a_));
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, port_solid,
                                    port_a_Rot, G4ThreeVector(port_nozzle_x, port_nozzle_y, port_z_2a_));

    G4RotationMatrix* port_b_Rot = new G4RotationMatrix;
    port_b_Rot->rotateX( 90. * deg);
    port_b_Rot->rotateY( 45. * deg);

    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, port_solid,
                                    port_b_Rot, G4ThreeVector(-port_nozzle_x, port_nozzle_y, port_z_1b_));
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, port_solid,
                                    port_b_Rot, G4ThreeVector(-port_nozzle_x, port_nozzle_y, port_z_2b_));

    // Body gas + Energy endcap gas
    G4UnionSolid* vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_body_solid,
						      vessel_gas_endcap_solid, 0, energy_endcap_pos);

    //  Body gas + Energy endcap gas + Tracking endcap gas
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid,
					vessel_gas_endcap_solid, xRot, tracking_endcap_pos);

    // Add gas inside ports
    G4double port_gas_x = port_nozzle_x - (offset/2.)/sqrt(2.);
    G4double port_gas_y = port_gas_x;

    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, port_gas_solid,
                                        port_a_Rot, G4ThreeVector(port_gas_x, port_gas_y, port_z_1a_));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, port_gas_solid,
                                        port_a_Rot, G4ThreeVector(port_gas_x, port_gas_y, port_z_2a_));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, port_gas_solid,
                                        port_b_Rot, G4ThreeVector(-port_gas_x, port_gas_y, port_z_1b_));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, port_gas_solid,
                                        port_b_Rot, G4ThreeVector(-port_gas_x, port_gas_y, port_z_2b_));

    //// The logics
    // vessel and vessel gas
    G4LogicalVolume* vessel_logic = new G4LogicalVolume(vessel_solid,
							materials::Steel316Ti(),
							"VESSEL");
    this->SetLogicalVolume(vessel_logic);

    G4Material* vessel_gas_mat = nullptr;
    if (gas_ == "naturalXe") {
      vessel_gas_mat = materials::GXe(pressure_, temperature_);
    } else if (gas_ == "enrichedXe") {
      vessel_gas_mat =  materials::GXeEnriched(pressure_, temperature_);
    } else if  (gas_ == "depletedXe") {
      vessel_gas_mat =  materials::GXeDepleted(pressure_, temperature_);
    } else if  (gas_ == "XeHe") {
      vessel_gas_mat = materials::GXeHe(pressure_, 300. * kelvin,
					    xe_perc_, helium_mass_num_);
    } else {
      G4Exception("[Next100Vessel]", "Construct()", FatalException,
		  "Unknown kind of xenon, valid options are: natural, enriched, depleted, or XeHe.");
    }

    vessel_gas_mat->SetMaterialPropertiesTable(opticalprops::GXe(pressure_, temperature_, sc_yield_, e_lifetime_));

    G4LogicalVolume* vessel_gas_logic = new G4LogicalVolume(vessel_gas_solid, vessel_gas_mat, "VESSEL_GAS");
    internal_logic_vol_ = vessel_gas_logic;
    SetELzCoord(-body_length_/2. - endcap_in_z_width_ + endcap_tp_distance_ + gate_tp_distance_);
    internal_phys_vol_ =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), vessel_gas_logic,
                        "VESSEL_GAS", vessel_logic, false, 0);

    // Port tubes
    G4LogicalVolume* port_tube_logic =
      new G4LogicalVolume(port_tube_solid, materials::Steel316Ti(), "PORT_TUBE");

    G4LogicalVolume* port_tube_gas_logic =
      new G4LogicalVolume(port_tube_gas_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),
                          "PORT_TUBE_AIR");

    new G4PVPlacement(0, G4ThreeVector(0., 0., port_tube_tip_/2.), port_tube_gas_logic,
                      "PORT_TUBE_AIR", port_tube_logic, false, 0);

    new G4PVPlacement(port_a_Rot, G4ThreeVector(port_x_, port_y_, port_z_1a_), port_tube_logic,
                      "PORT_TUBE_1a", vessel_gas_logic, false, 0);
    new G4PVPlacement(port_a_Rot, G4ThreeVector(port_x_, port_y_, port_z_2a_), port_tube_logic,
                      "PORT_TUBE_2a", vessel_gas_logic, false, 0);
    new G4PVPlacement(port_b_Rot, G4ThreeVector(-port_x_, port_y_, port_z_1b_), port_tube_logic,
                      "PORT_TUBE_1b", vessel_gas_logic, false, 0);
    new G4PVPlacement(port_b_Rot, G4ThreeVector(-port_x_, port_y_, port_z_2b_), port_tube_logic,
                      "PORT_TUBE_2b", vessel_gas_logic, false, 0);

    // SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes grey = nexus::TitaniumGreyAlpha();
      G4VisAttributes yellow = nexus::Yellow();
      grey  .SetForceSolid(true);
      yellow.SetForceSolid(true);
      vessel_logic       ->SetVisAttributes(grey);
      vessel_gas_logic   ->SetVisAttributes(G4VisAttributes::GetInvisible());
      port_tube_logic    ->SetVisAttributes(grey);
      port_tube_gas_logic->SetVisAttributes(yellow);
    }
    else {
      vessel_logic       ->SetVisAttributes(G4VisAttributes::GetInvisible());
      vessel_gas_logic   ->SetVisAttributes(G4VisAttributes::GetInvisible());
      port_tube_logic    ->SetVisAttributes(G4VisAttributes::GetInvisible());
      port_tube_gas_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    // VERTEX GENERATORS   //////////
    body_gen_  = new CylinderPointSampler2020(vessel_in_rad_, vessel_out_rad, body_length_/2.,
                                              0., 360.*deg, 0, G4ThreeVector(0., 0., 0.));

    energy_endcap_gen_ = new SpherePointSampler( endcap_in_rad_, vessel_thickness_, energy_endcap_pos, 0,
						   0., twopi, 0., endcap_theta_);

    tracking_endcap_gen_ = new SpherePointSampler( endcap_in_rad_, vessel_thickness_, tracking_endcap_pos, xRot,
						 0., twopi, 0., endcap_theta_);

    tracking_flange_gen_ = new CylinderPointSampler2020(vessel_in_rad_, flange_out_rad, flange_tp_length/2.,
                                                        0., 360.*deg, 0, tracking_flange_pos);

    energy_flange_gen_ = new CylinderPointSampler2020(vessel_in_rad_, flange_out_rad, flange_ep_length/2.,
                                                      0., 360.*deg, 0, energy_flange_pos);

    port_gen_ = new CylinderPointSampler2020(0., port_tube_rad, source_height_/2.,
                                             0., 360.*deg, 0, G4ThreeVector(0., 0., 0.));

    // Calculating some prob
    G4double body_vol   = vessel_body_solid  ->GetCubicVolume() - vessel_gas_body_solid  ->GetCubicVolume();
    G4double endcap_vol = vessel_endcap_solid->GetCubicVolume() - vessel_gas_endcap_solid->GetCubicVolume();
    G4double flange_ep_vol = vessel_ep_flange_solid->GetCubicVolume();
    G4double flange_tp_vol = vessel_tp_flange_solid->GetCubicVolume();
    G4double vessel_vol = body_vol + 2.*endcap_vol + flange_ep_vol + flange_tp_vol;

    perc_endcap_vol_    = 2.*endcap_vol / vessel_vol;
    perc_ep_flange_vol_ = flange_ep_vol / vessel_vol;
    perc_tp_flange_vol_ = flange_tp_vol / vessel_vol;
  }


  Next100Vessel::~Next100Vessel()
  {
    delete body_gen_;
    delete tracking_endcap_gen_;
    delete energy_endcap_gen_;
    delete tracking_flange_gen_;
    delete energy_flange_gen_;
  }


  G4LogicalVolume* Next100Vessel::GetInternalLogicalVolume()
  {
    return internal_logic_vol_;
  }


  G4VPhysicalVolume* Next100Vessel::GetInternalPhysicalVolume()
  {
    return internal_phys_vol_;
  }


  G4ThreeVector Next100Vessel::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    G4double source_x = port_x_ - (port_tube_height_ - port_tube_tip_ - source_height_)/2./sqrt(2.);
    G4double source_y = source_x;

    // Vertex in the whole VESSEL volume
    if (region == "VESSEL") {
      G4double rand = G4UniformRand();
      if (rand < perc_endcap_vol_) { // Endcaps
        if (G4UniformRand()<0.5){ // Tracking endcap
        vertex = tracking_endcap_gen_->GenerateVertex("VOLUME");
        }
        else{ // Energy endcap
          vertex = energy_endcap_gen_->GenerateVertex("VOLUME");
        }
      }
      else if (rand < (perc_endcap_vol_ + perc_ep_flange_vol_)){// Energy flange
        vertex = energy_flange_gen_->GenerateVertex("VOLUME");
      }
      else if (rand < (perc_endcap_vol_ + perc_ep_flange_vol_ + perc_tp_flange_vol_)){// Tracking flange
        vertex = tracking_flange_gen_->GenerateVertex("VOLUME");
      }
      else // Body
        	vertex = body_gen_->GenerateVertex("VOLUME");
    }

    else if (region == "PORT_1a"){
      vertex = port_gen_->GenerateVertex("VOLUME");

      vertex = vertex.rotateX( 90. * deg);
      vertex = vertex.rotateZ(-45. * deg);

      G4ThreeVector translate (source_x, source_y, port_z_1a_);
      vertex = vertex + translate;
    }

    else if (region == "PORT_2a"){
      vertex = port_gen_->GenerateVertex("VOLUME");

      vertex = vertex.rotateX( 90. * deg);
      vertex = vertex.rotateZ(-45. * deg);

      G4ThreeVector translate (source_x, source_y, port_z_2a_);
      vertex = vertex + translate;
    }

    else if (region == "PORT_1b"){
      vertex = port_gen_->GenerateVertex("VOLUME");

      vertex = vertex.rotateX( 90. * deg);
      vertex = vertex.rotateZ( 45. * deg);

      G4ThreeVector translate (-source_x, source_y, port_z_1b_);
      vertex = vertex + translate;
    }

    else if (region == "PORT_2b"){
      vertex = port_gen_->GenerateVertex("VOLUME");

      vertex = vertex.rotateX( 90. * deg);
      vertex = vertex.rotateZ( 45. * deg);

      G4ThreeVector translate (-source_x, source_y, port_z_2b_);
      vertex = vertex + translate;
    }

    else {
      G4Exception("[Next100Vessel]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }

  G4double* Next100Vessel::GetPortZpositions(){
    static G4double port_positions[]{port_z_1a_, port_z_2a_, port_z_1b_, port_z_2b_};
    return port_positions;
  }

} //end namespace nexus
