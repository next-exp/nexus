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
#include "CylinderPointSampler.h"
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

namespace nexus {

  using namespace CLHEP;

  Next100Vessel::Next100Vessel(const G4double nozzle_ext_diam,
			       const G4double up_nozzle_ypos,
			       const G4double central_nozzle_ypos,
			       const G4double down_nozzle_ypos,
			       const G4double bottom_nozzle_ypos):
    GeometryBase(),

    // Body dimensions
    vessel_in_rad_ (68.0  * cm),
    vessel_body_length_ (160 * cm),
    vessel_length_ (181.74 * cm),  // Vessel length = 160 cm (from body) + 2. * 10.87cm (from endcaps)
    vessel_thickness_ (1.  * cm),
    distance_gate_body_end_ (110. * mm), // to be checked with designs

    // Endcaps dimensions
    endcap_in_rad_ (108.94 * cm),
    endcap_theta_ (38.6 * deg),
    endcap_thickness_ (1. * cm),
    endcap_in_z_width_ (23.83 * cm),  // in_z_width = 35.7 cm - 1. cm (thickness) - 10.87 cm (from cylindric part)

    // Flange dimensions
    flange_out_rad_ (73.5 * cm),
    flange_length_ (8.0 * cm),
    flange_z_pos_ (80.0 * cm),

    // Nozzle dimensions
    //  large_nozzle_length_ (320.0 * cm),
    //  small_nozzle_length_ (240.0 * cm),
     large_nozzle_length_ (250.0 * cm),
    small_nozzle_length_ (240.0 * cm),

    // Vessel gas
    sc_yield_(16670. * 1/MeV),
    e_lifetime_(1000. * ms),
    pressure_(15 * bar),
    temperature_ (303 * kelvin),
    // Visibility
    visibility_(0),
    gas_("naturalXe"),
    helium_mass_num_(4),
    xe_perc_(100.)
  {

    /// Needed External variables
    nozzle_ext_diam_ = nozzle_ext_diam;
    up_nozzle_ypos_ = up_nozzle_ypos;
    central_nozzle_ypos_ = central_nozzle_ypos;
    down_nozzle_ypos_ = down_nozzle_ypos;
    bottom_nozzle_ypos_ = bottom_nozzle_ypos;


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

    G4Tubs* vessel_body_solid = new G4Tubs("VESSEL_BODY", 0., vessel_out_rad, vessel_length_/2.,
					   0.*deg, 360.*deg);

    G4Tubs* vessel_gas_body_solid = new G4Tubs("VESSEL_GAS_BODY", 0., vessel_in_rad_, vessel_length_/2.,
					       0.*deg, 360.*deg);

    // Endcaps solids
    G4double endcap_out_rad = endcap_in_rad_ + endcap_thickness_;

    G4Sphere* vessel_tracking_endcap_solid = new G4Sphere("VESSEL_TRACKING_ENDCAP",
							  0. * cm,  endcap_out_rad,   //radius
							  0. * deg, 360. * deg,       // phi
							  0. * deg, endcap_theta_);   // theta

    G4Sphere* vessel_gas_tracking_endcap_solid = new G4Sphere("VESSEL_GAS_TRACKING_ENDCAP",
							      0. * cm,  endcap_in_rad_,   //radius
							      0. * deg, 360. * deg,       // phi
							      0. * deg, endcap_theta_);   // theta

    G4Sphere* vessel_energy_endcap_solid = new G4Sphere("VESSEL_ENERGY_ENDCAP",
							0. * cm,  endcap_out_rad,                   //radius
							0. * deg, 360. * deg,                       // phi
							180. * deg - endcap_theta_, endcap_theta_); // theta

    G4Sphere* vessel_gas_energy_endcap_solid = new G4Sphere("VESSEL_GAS_ENERGY_ENDCAP",
							    0. * cm,  endcap_in_rad_,                   //radius
							    0. * deg, 360. * deg,                       // phi
							    180. * deg - endcap_theta_, endcap_theta_); // theta


    // Flange solid
    G4Tubs* vessel_flange_solid = new G4Tubs("VESSEL_TRACKING_FLANGE", 0., flange_out_rad_,
						      flange_length_/2., 0.*deg, 360.*deg);

    // Nozzle solids
    G4Tubs* large_nozzle_solid = new G4Tubs("LARGE_NOZZLE", 0.*cm, nozzle_ext_diam_/2.,
					    large_nozzle_length_/2., 0.*deg, 360.*deg);

    G4Tubs* small_nozzle_solid = new G4Tubs("SMALL_NOZZLE", 0.*cm, nozzle_ext_diam_/2.,
					    small_nozzle_length_/2., 0.*deg, 360.*deg);

    G4Tubs* large_nozzle_gas_solid = new G4Tubs("LARGE_NOZZLE_GAS", 0.*cm, (nozzle_ext_diam_/2. - vessel_thickness_),
						large_nozzle_length_/2., 0.*deg, 360.*deg);

    G4Tubs* small_nozzle_gas_solid = new G4Tubs("SMALL_NOZZLE_GAS", 0.*cm, (nozzle_ext_diam_/2. - vessel_thickness_),
						small_nozzle_length_/2., 0.*deg, 360.*deg);


    //// Unions
    G4double endcap_z_pos = (vessel_length_ / 2.) - (endcap_in_rad_ - endcap_in_z_width_);
    G4ThreeVector tracking_endcap_pos(0, 0, endcap_z_pos);
    G4ThreeVector energy_endcap_pos(0, 0, -1. * endcap_z_pos);
    G4ThreeVector tracking_flange_pos(0, 0, flange_z_pos_);
    G4ThreeVector energy_flange_pos(0, 0, -1. * flange_z_pos_);

    // Body + Tracking endcap
    G4UnionSolid* vessel_solid = new G4UnionSolid("VESSEL", vessel_body_solid, vessel_tracking_endcap_solid,
						  0, tracking_endcap_pos);

    // Body + Tracking endcap + Energy endcap
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, vessel_energy_endcap_solid,
				    0, energy_endcap_pos);

    // Body + Tracking endcap + Energy endcap + Tracking flange
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid,
				    vessel_flange_solid, 0, tracking_flange_pos);

    // Body + Tracking endcap + Energy endcap + Tracking flange + Energy flange
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid,
				    vessel_flange_solid, 0, energy_flange_pos);

    // Adding nozzles
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, small_nozzle_solid,
				    0, G4ThreeVector(0., up_nozzle_ypos_, 0.) );
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, large_nozzle_solid,
				    0, G4ThreeVector(0., central_nozzle_ypos_, 0.) );
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, large_nozzle_solid,
				    0, G4ThreeVector(0., down_nozzle_ypos_, 0.) );
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, small_nozzle_solid,
				    0, G4ThreeVector(0., bottom_nozzle_ypos_, 0.) );



    // Body gas + Tracking endcap gas
    G4UnionSolid* vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_body_solid,
						      vessel_gas_tracking_endcap_solid, 0, tracking_endcap_pos);

    // Body gas + Tracking endcap gas + Energy endcap gas
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid,
					vessel_gas_energy_endcap_solid, 0, energy_endcap_pos);
    // Adding nozzles
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, small_nozzle_gas_solid,
					0, G4ThreeVector(0., up_nozzle_ypos_, 0.) );
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, large_nozzle_gas_solid,
					0, G4ThreeVector(0., central_nozzle_ypos_, 0.) );
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, large_nozzle_gas_solid,
					0, G4ThreeVector(0., down_nozzle_ypos_, 0.) );
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, small_nozzle_gas_solid,
					0, G4ThreeVector(0., bottom_nozzle_ypos_, 0.) );



    //// The logics
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
    SetELzCoord(-vessel_body_length_/2. + distance_gate_body_end_);
    internal_phys_vol_ =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), vessel_gas_logic,
                        "VESSEL_GAS", vessel_logic, false, 0);

    // SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes grey = DarkGrey();
      //grey.SetForceSolid(true);
      vessel_logic->SetVisAttributes(grey);
      grey.SetForceSolid(true);
      vessel_gas_logic->SetVisAttributes(grey);
      //grey.SetForceSolid(true);
    }
    else {
      vessel_logic->SetVisAttributes(G4VisAttributes::Invisible);
      vessel_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // VERTEX GENERATORS   //////////
    body_gen_  = new CylinderPointSampler(vessel_in_rad_, vessel_length_, vessel_thickness_, 0.);

    tracking_endcap_gen_ = new SpherePointSampler( endcap_in_rad_, endcap_thickness_, tracking_endcap_pos, 0,
						   0., twopi, 0., endcap_theta_);

    energy_endcap_gen_ = new SpherePointSampler( endcap_in_rad_, endcap_thickness_, energy_endcap_pos, 0,
						 0., twopi, 180.*deg - endcap_theta_, endcap_theta_);

    tracking_flange_gen_  = new CylinderPointSampler(vessel_out_rad, flange_length_,
						     flange_out_rad_-vessel_out_rad, 0., tracking_flange_pos);

    energy_flange_gen_  = new CylinderPointSampler(vessel_out_rad, flange_length_,
						   flange_out_rad_-vessel_out_rad, 0., energy_flange_pos);

    // Calculating some prob
    G4double body_vol = vessel_body_solid->GetCubicVolume() - vessel_gas_body_solid->GetCubicVolume();
    G4double endcap_vol =  vessel_tracking_endcap_solid->GetCubicVolume() - vessel_gas_tracking_endcap_solid->GetCubicVolume();
    perc_endcap_vol_ = endcap_vol / (body_vol + 2. * endcap_vol);

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

    // Vertex in the whole VESSEL volume except flanges
    if (region == "VESSEL") {
      G4double rand = G4UniformRand();
      if (rand < perc_endcap_vol_) {
	G4VPhysicalVolume *VertexVolume;
	do {
	  vertex = tracking_endcap_gen_->GenerateVertex("VOLUME");  // Tracking endcap
	  // To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	  VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	} while (VertexVolume->GetName() != "VESSEL");
      }
      else if (rand > 1. - perc_endcap_vol_) {
	G4VPhysicalVolume *VertexVolume;
	do {
	  vertex = energy_endcap_gen_->GenerateVertex("VOLUME");  // Energy endcap
	  // To check its volume, one needs to rotate and shift the vertex
	  // because the check is done using global coordinates
	  G4ThreeVector glob_vtx(vertex);
	  // First rotate, then shift
	  glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	  glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	  VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	} while (VertexVolume->GetName() != "VESSEL");
      }
      else
	vertex = body_gen_->GenerateVertex("BODY_VOL");  // Body
    }

    // Vertex in FLANGES
    else if (region == "VESSEL_FLANGES") {
      if (G4UniformRand() < 0.5)
      	vertex = tracking_flange_gen_->GenerateVertex("BODY_VOL");
      else
      	vertex = energy_flange_gen_->GenerateVertex("BODY_VOL");
    }

    // Vertex in TRACKING ENDCAP
    else if (region == "VESSEL_TRACKING_ENDCAP") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = tracking_endcap_gen_->GenerateVertex("VOLUME");  // Tracking endcap
	// To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "VESSEL");
    }

    // Vertex in ENERGY ENDCAP
    else if (region == "VESSEL_ENERGY_ENDCAP") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = energy_endcap_gen_->GenerateVertex("VOLUME");  // Energy endcap
	// To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "VESSEL");
    }
     else {
      G4Exception("[Next100Vessel]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }


} //end namespace nexus
