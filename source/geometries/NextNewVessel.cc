// -----------------------------------------------------------------------------
// nexus | NextNewVessel.cc
//
// Vessel of the NEXT-WHITE detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextNewVessel.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "CalibrationSource.h"
#include "CylinderPointSamplerLegacy.h"
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
#include <G4RotationMatrix.hh>
#include <G4UnitsTable.hh>
#include <G4Transform3D.hh>
#include <G4SubtractionSolid.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  NextNewVessel::NextNewVessel():

    GeometryBase(),

    // Body dimensions
    vessel_in_diam_ (64.0  * cm),
    vessel_body_length_ (87. * cm),//body cylinder(without flanges)
    vessel_tube_length_ (122.256 * cm),  //  870 body + 2. *( (50-10)bodyflange + (50-10)endcapflange + 96.28 endcap)
    vessel_thickness_ (1.2 * cm),

    // Flange dimensions (one cylinder = 1 on the vessel_body_ side + 1 on the endcap_ side)
    flange_out_diam_ (82.0 * cm),//Flange inner radius = vessel inner radius + vessel thickness
    flange_length_ (10.0 * cm), //5for body + 5for endcap
    flange_z_pos_ (vessel_body_length_/2.),//435

    // Endcaps dimensions
    endcap_in_rad_ (53.2 * cm), //=(66.52/2 - 1.26)
    endcap_theta_ (37.2 * deg), //(38.7 * deg)- 1.5º visfit
    endcap_thickness_ (1.26 * cm),
    endcap_in_z_width_ (9.448 * cm),// inner sphere to flange (96,28)-visfit (1.8mm)

    // Nozzle dimensions
    lat_nozzle_in_diam_  (30.*mm),
    lat_nozzle_high_ (51.*mm),
    // lat_nozzle_thickness_ (6.1*mm),
    lat_nozzle_thickness_ (3.*mm),
    lat_nozzle_flange_diam_ (75.*mm),
    lat_nozzle_flange_high_ (15.*mm), // This is half the total thickness of the flange+cover

    up_nozzle_in_diam_ (60.*mm),
    up_nozzle_high_ (54.3*mm),
    up_nozzle_thickness_ (6.51*mm),
    up_nozzle_flange_diam_ (130*mm),
    // up_nozzle_flange_high_ (16.7*mm), // This is half the total thickness of the flange+cover
    up_nozzle_flange_high_ (19.85*mm), // This is half the total thickness of the flange+cover when the cover is the one with the source port //16.7 + 23

    endcap_nozzle_in_diam_ (88.*mm),
    endcap_nozzle_high_ (86.*mm),
    endcap_nozzle_thickness_ (6.8*mm),
    endcap_nozzle_flange_diam_ (165.*mm),
    endcap_nozzle_flange_high_ (22.*mm),

    lat_nozzle_z_pos_ (120*mm),
    //   lat_nozzle_x_pos_ (vessel_in_diam_/2. +lat_nozzle_high_),
    //  up_nozzle_y_pos_ (vessel_in_diam_/2. +up_nozzle_high_),
    up_nozzle_z_pos_ ( 257. *mm),//anode_nozzle_zpos = -cathode_nozzle_zpos
    endcap_nozzle_z_pos_ (vessel_tube_length_/2. + endcap_in_z_width_), //bodylegth/2 + 300mm????

    port_tube_diam_ (10. * mm),
    port_tube_thickness_ (1. * mm),
    port_tube_window_thickn_ (2. * mm),
    //lat_port_tube_length_ (53. * mm),
    lat_port_tube_length_ (99. * mm),
    lat_port_tube_out_ (30. * mm),
    // up_port_tube_length_ (58. * mm),
    up_port_tube_length_ (111. * mm),
    up_port_tube_out_ (30. * mm),
    // axial_port_tube_length_ (727. * mm),
    axial_port_tube_length_ (777. * mm), // whole tube, according to drawings
    axial_port_tube_out_ (30.*mm), // part of the tube which sticks out of the last flange
    axial_port_flange_ (20.*mm), // refers to the last flange
    axial_distance_flange_endcap_(51.*cm), // distance from the end of the endcap and the beginning of the last flange

    // Vessel gas
    pressure_(1. * bar),
    temperature_ (300 * kelvin),
    visibility_(1),
    sc_yield_(25510. * 1/MeV),
    e_lifetime_(1000. * ms),
    gas_("naturalXe"),
    xe_perc_(100.),
    helium_mass_num_(4),
    //   source_(false),
    source_distance_(0.*mm),
    calib_port_("")

  {
    /// README
    /// This is a quite complicated geometry. In order not to get crazy looking at it every few months,
    /// keep the following in mind:
    /// 1) LATERAL, UPPER and AXIAL ports: to avoid volume subtractions, we first make the union of nozzle + (flange+cover) + external part of source tube, then we put together this volume with the vessel. After that, we put inside the vessel logical volume the volume of air which corresponds to the thickness of flange+cover+external part of source port. After that, we put inside the vessel gas volume the internal part of the source port and, inside it, the corresponding part of air.
    /// 2) All the XXXX_nozzle_flange_high and XXXX_nozzle_high are half the thickness of the solid volume. In the solid union between vessel and nozzle, the centre of the body of the nozzle is placed in the edge of the vessel, thus half of the body length is lost in the union and the real length of the nozzle is XXXX_nozzle_high.
    /// 3) Bear in mind that visualizing this geometry could take to a crash of OpenGL, because of its complexity. Don't worry, geant4 tracking is being done correctly.
    /// 4) The source that fits inside the tube with a screw is a piece of aluminum with a disk of 2 mm thickness, 6 mm diameter placed at 0.5 mm from the bottom of the piece

    // Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    msg_->DeclareProperty("vessel_vis", visibility_, "Vessel Visibility");

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

    msg_->DeclareProperty("gas", gas_, "Gas being used");
    msg_->DeclareProperty("XePercentage", xe_perc_,
			  "Percentage of xenon used in mixtures");
    msg_->DeclareProperty("helium_A", helium_mass_num_,
			  "Mass number for helium used, 3 or 4");

    // msg_->DeclareProperty("source", source_, "Radioactive source being used");
    msg_->DeclareProperty("internal_calib_port", calib_port_, "Calibration port being used");

    G4GenericMessenger::Command& source_dist_cmd =
      msg_->DeclareProperty("source_distance", source_distance_,
			    "Distance of the bottom of the 'screw' source from the bottom of the lateral/axial/upper port tube");
    source_dist_cmd.SetUnitCategory("Length");
    source_dist_cmd.SetParameterName("source_distance", false);
    source_dist_cmd.SetRange("source_distance>=0.");

    cal_ = new CalibrationSource();
    cal_->Construct();

  }



void NextNewVessel::Construct()
  {
    // Body solid
    G4double vessel_out_diam = vessel_in_diam_ + 2*vessel_thickness_;
    G4Tubs* vessel_tube_solid = new G4Tubs("VESSEL_BODY", 0., vessel_out_diam/2.,
					   vessel_tube_length_/2.,0., twopi);
    G4Tubs* vessel_gas_tube_solid = new G4Tubs("VESSEL_GAS_BODY", 0., vessel_in_diam_/2.,
     					       vessel_tube_length_/2.,0., twopi);

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
							0. * cm,  endcap_out_rad,     //radius
							0. * deg, 360. * deg,        // phi
							180. * deg - endcap_theta_, endcap_theta_); // theta
    G4Sphere* vessel_gas_energy_endcap_solid = new G4Sphere("VESSEL_GAS_ENERGY_ENDCAP",
							    0. * cm,  endcap_in_rad_,    //radius
							    0. * deg, 360. * deg,        // phi
							    180. * deg - endcap_theta_, endcap_theta_); // theta

    // Flange solid
    G4Tubs* vessel_flange_solid = new G4Tubs("VESSEL_FLANGE", vessel_out_diam/2., flange_out_diam_/2.,
    					     flange_length_/2., 0., twopi);

    // Nozzle solids
    G4Tubs* lateral_nozzle_tube_solid =
      new G4Tubs("LAT_TUBE_NOZZLE", 0., lat_nozzle_in_diam_/2. +lat_nozzle_thickness_, lat_nozzle_high_, 0., twopi);
    G4Tubs* lateral_nozzle_flange_solid =
      new G4Tubs("LAT_NOZZLE_FLANGE", 0., lat_nozzle_flange_diam_/2., lat_nozzle_flange_high_, 0., twopi);
    G4Tubs* lateral_nozzle_gas_solid =
      new G4Tubs("LAT_NOZZLE_GAS", 0., lat_nozzle_in_diam_/2., lat_nozzle_high_, 0., twopi);
    G4Tubs* lateral_port_tube_out_solid =
      new G4Tubs("LAT_PORT_TUBE_OUT", 0., port_tube_diam_/2. + port_tube_thickness_, (lat_port_tube_out_+0.01*mm)/2., 0., twopi);

    G4Tubs* up_nozzle_tube_solid =
      new G4Tubs("UP_TUBE_NOZZLE", 0., up_nozzle_in_diam_/2.+up_nozzle_thickness_,up_nozzle_high_, 0., twopi);
    G4Tubs* up_nozzle_flange_solid =
      new G4Tubs("UP_NOZZLE_FLANGE", 0., up_nozzle_flange_diam_/2.,up_nozzle_flange_high_, 0., twopi);
    G4Tubs* up_nozzle_gas_solid =
      new G4Tubs("UP_NOZZLE_GAS", 0., up_nozzle_in_diam_/2.,up_nozzle_high_, 0., twopi);
    G4Tubs* up_port_tube_out_solid =
      new G4Tubs("UP_PORT_TUBE_OUT", 0., (port_tube_diam_ + 2.*port_tube_thickness_)/2., (up_port_tube_out_+0.01*mm)/2., 0., twopi);

    G4Tubs* endcap_nozzle_tube_solid =
      new G4Tubs("ENDCAP_TUBE_NOZZLE", 0., endcap_nozzle_in_diam_/2.+endcap_nozzle_thickness_,endcap_nozzle_high_, 0., twopi);
    G4Tubs* endcap_nozzle_flange_solid =
      new G4Tubs("ENDCAP_NOZZLE_FLANGE_B", 0., endcap_nozzle_flange_diam_/2.,endcap_nozzle_flange_high_, 0., twopi);
    G4Tubs* endcap_nozzle_gas_solid =
      new G4Tubs("ENDCAP_NOZZLE_GAS", 0., endcap_nozzle_in_diam_/2., endcap_nozzle_high_, 0., twopi);
    G4double axial_port_tube_ext =
      axial_port_tube_out_ + 2.*axial_port_flange_ + axial_distance_flange_endcap_ - endcap_nozzle_high_ - endcap_nozzle_flange_high_;
    G4Tubs* endcap_port_tube_out_solid =
      new G4Tubs("AXIAL_PORT_TUBE_OUT", 0., (port_tube_diam_ + 2.*port_tube_thickness_)/2., (axial_port_tube_ext+0.01*mm)/2., 0., twopi);


    /// Making holes in nozzle flanges for source ports
    // G4Tubs* axial_port_hole_solid =
    //   new G4Tubs("AXIAL_PORT_HOLE", 0., port_tube_diam_/2.,
    // 		 endcap_nozzle_flange_high_ + 1.*mm, 0., twopi);
    // G4SubtractionSolid* endcap_nozzle_flange_solid =
    //   new G4SubtractionSolid("ENDCAP_NOZZLE_FLANGE", endcap_nozzle_flange_solid_b, axial_port_hole_solid,
    // 			     0, G4ThreeVector(0., 0., 0.));

    // G4Tubs* lateral_port_hole_solid =
    //   new G4Tubs("LATERAL_PORT_HOLE", 0., port_tube_diam_/2.,
    // 		 lat_nozzle_flange_high_ + 1.*mm, 0., twopi);
    // G4SubtractionSolid* lateral_nozzle_flange_solid =
    //   new G4SubtractionSolid("LATERAL_NOZZLE_FLANGE", lateral_nozzle_flange_solid_b, lateral_port_hole_solid,
    // 			     0, G4ThreeVector(0., 0., 0.));

    // G4Tubs* up_port_hole_solid =
    //   new G4Tubs("UP_PORT_HOLE", 0., port_tube_diam_/2.,
    // 		 up_nozzle_flange_high_ + 1.*mm, 0., twopi);
    // G4SubtractionSolid* up_nozzle_flange_solid =
    //   new G4SubtractionSolid("UP_NOZZLE_FLANGE", up_nozzle_flange_solid_b, up_port_hole_solid,
    // 			     0, G4ThreeVector(0., 0., 0.));



    //// UNIONS ///////
    G4double endcap_z_pos = (vessel_tube_length_/2.)- (endcap_in_rad_ -endcap_in_z_width_);
    G4ThreeVector tracking_endcap_pos(0, 0, endcap_z_pos);
    G4ThreeVector energy_endcap_pos(0, 0, -1. * endcap_z_pos);
    G4ThreeVector tracking_flange_pos(0, 0, flange_z_pos_);
    G4ThreeVector energy_flange_pos(0, 0, -1. * flange_z_pos_);

    // Body + Tracking endcap
    G4UnionSolid* vessel_solid = new G4UnionSolid("VESSEL", vessel_tube_solid, vessel_tracking_endcap_solid,
                                                  0, tracking_endcap_pos);
    // Body + Tracking endcap + Energy endcap
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, vessel_energy_endcap_solid,
                                     0, energy_endcap_pos);
    // // Body + Tracking endcap + Energy endcap + Tracking flange
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, vessel_flange_solid,
    				    0,tracking_flange_pos);
    // // Body + Tracking endcap + Energy endcap + Tracking flange + Energy flange
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, vessel_flange_solid,
      				    0, energy_flange_pos);

    // building nozzles
    G4UnionSolid* lateral_nozzle_solid =
      new G4UnionSolid("LAT_NOZZLE", lateral_nozzle_tube_solid, lateral_nozzle_flange_solid, 0,
		       G4ThreeVector(0.,0.,lat_nozzle_high_));
    G4UnionSolid* lateral_nozzle_solid_with_tube =
      new G4UnionSolid("LAT_NOZZLE", lateral_nozzle_solid, lateral_port_tube_out_solid, 0,
		       G4ThreeVector(0.,0., lat_nozzle_high_ + lat_nozzle_flange_high_ + (lat_port_tube_out_-0.01*mm)/2.));


    G4UnionSolid* up_nozzle_solid =
      new G4UnionSolid("UP_NOZZLE", up_nozzle_tube_solid, up_nozzle_flange_solid, 0,
		       G4ThreeVector(0.,0.,up_nozzle_high_));
    G4UnionSolid* up_nozzle_solid_with_tube =
      new G4UnionSolid("UP_NOZZLE", up_nozzle_solid,
		       up_port_tube_out_solid, 0,
		       G4ThreeVector(0.,0., up_nozzle_high_ + up_nozzle_flange_high_ + (up_port_tube_out_-0.01*mm)/2.));

    G4UnionSolid* endcap_nozzle_solid = new G4UnionSolid("ENDCAP_NOZZLE", endcap_nozzle_tube_solid,
							 endcap_nozzle_flange_solid, 0,
							 G4ThreeVector(0.,0.,endcap_nozzle_high_));
    G4UnionSolid* endcap_nozzle_solid_with_tube =
      new G4UnionSolid("ENDCAP_NOZZLE", endcap_nozzle_solid, endcap_port_tube_out_solid, 0,
		       G4ThreeVector(0.,0., endcap_nozzle_high_ + endcap_nozzle_flange_high_ + (axial_port_tube_ext-0.01*mm)/2.));

    //Rotate the lateral nozzles
    G4RotationMatrix* rot_lat = new G4RotationMatrix();
    rot_lat->rotateY(3*pi/2.);
    //Rotate the upper nozzles to vertical
    G4RotationMatrix* rot_up = new G4RotationMatrix();
    rot_up->rotateX(pi/2.);
    //Rotate the endcap nozzle
    G4RotationMatrix* rot_endcap = new G4RotationMatrix();
    rot_endcap->rotateY(pi);

    // Body + Tracking endcap + Energy endcap + Tracking flange + Energy flange + Lateral nozzles
     vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, lateral_nozzle_solid_with_tube,
      				    rot_lat, G4ThreeVector( vessel_in_diam_/2., 0.,lat_nozzle_z_pos_));
     vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, lateral_nozzle_solid,
     				    rot_lat, G4ThreeVector(vessel_in_diam_/2., 0.,-lat_nozzle_z_pos_));

    // Body + Tracking endcap + Energy endcap + Tracking flange + Energy flange + Lateral nozzles + Upper nozzles

     vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, up_nozzle_solid,
				     rot_up, G4ThreeVector(0., vessel_in_diam_/2., up_nozzle_z_pos_));
     vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, up_nozzle_solid_with_tube,
				     rot_up, G4ThreeVector(0., vessel_in_diam_/2., 0.));
     vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, up_nozzle_solid,
				     rot_up, G4ThreeVector(0.,vessel_in_diam_/2., -up_nozzle_z_pos_));

    // Body + Tracking endcap + Energy endcap + Tracking flange + Energy flange + Lateral nozzles + Upper nozzles + Endcap nozzles
    // vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, endcap_nozzle_solid_with_tube,
    //  				    0, G4ThreeVector(0.,0.,endcap_nozzle_z_pos_));
    // vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, endcap_nozzle_solid,
    //   				    rot_endcap, G4ThreeVector(0.,0.,-endcap_nozzle_z_pos_));
     vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, endcap_nozzle_solid,
                                     0, G4ThreeVector(0.,0.,endcap_nozzle_z_pos_));
     vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, endcap_nozzle_solid_with_tube,
                                     rot_endcap, G4ThreeVector(0.,0.,-endcap_nozzle_z_pos_));


    ////GAS SOLID //////
    // Body gas + Tracking endcap gas
    G4UnionSolid* vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_tube_solid,
      						      vessel_gas_tracking_endcap_solid, 0, tracking_endcap_pos);
    // Body gas + Tracking endcap gas + Energy endcap gas
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid,
      						      vessel_gas_energy_endcap_solid, 0, energy_endcap_pos);
    // Body gas + Tracking endcap gas + Energy endcap gas + nozzles gas
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, lateral_nozzle_gas_solid,
      					rot_lat, G4ThreeVector(vessel_in_diam_/2., 0.,lat_nozzle_z_pos_));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, lateral_nozzle_gas_solid,
      					rot_lat, G4ThreeVector(vessel_in_diam_/2., 0.,-lat_nozzle_z_pos_));

    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, up_nozzle_gas_solid,
       					rot_up, G4ThreeVector(0., vessel_in_diam_/2.,up_nozzle_z_pos_));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, up_nozzle_gas_solid,
     					rot_up, G4ThreeVector(0., vessel_in_diam_/2,0.));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, up_nozzle_gas_solid,
       					rot_up, G4ThreeVector(0., vessel_in_diam_/2,-up_nozzle_z_pos_));

    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, endcap_nozzle_gas_solid,
       					0, G4ThreeVector(0.,0.,endcap_nozzle_z_pos_));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, endcap_nozzle_gas_solid,
      					rot_endcap, G4ThreeVector(0.,0.,-endcap_nozzle_z_pos_));


    //// LOGICS //////
    
    G4Material* steel_316_Ti = materials::Steel316Ti();
    steel_316_Ti->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* vessel_logic =
      new G4LogicalVolume(vessel_solid,	steel_316_Ti, "VESSEL");
    this->SetLogicalVolume(vessel_logic);

    // Place the port air inside the flanges end external tubes for lateral and upper
    G4Tubs* lateral_port_hole_solid =
      new G4Tubs("LATERAL_PORT_AIR_EXT", 0., port_tube_diam_/2.,
    		 (lat_nozzle_flange_high_ + lat_port_tube_out_)/2., 0., twopi);
    
    G4Material* air_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    air_mat->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
    
    G4LogicalVolume* lateral_port_hole_logic =
      new G4LogicalVolume(lateral_port_hole_solid, air_mat,
			  "LATERAL_PORT_AIR_EXT");
    new G4PVPlacement(G4Transform3D(*rot_lat, G4ThreeVector(vessel_in_diam_/2.+lat_nozzle_high_+(lat_nozzle_flange_high_+lat_port_tube_out_)/2., 0., lat_nozzle_z_pos_)),
		      lateral_port_hole_logic, "LATERAL_PORT_AIR_EXT", vessel_logic, false, 0, false);


    G4Tubs* upper_port_hole_solid =
      new G4Tubs("UP_PORT_AIR_EXT", 0., port_tube_diam_/2.,
    		 (up_nozzle_flange_high_ + up_port_tube_out_)/2., 0., twopi);
    G4LogicalVolume* upper_port_hole_logic =
      new G4LogicalVolume(upper_port_hole_solid, air_mat,
			  "UP_PORT_AIR_EXT");
    new G4PVPlacement(G4Transform3D(*rot_up, G4ThreeVector(0., vessel_in_diam_/2.+ up_nozzle_high_ + (up_nozzle_flange_high_ + up_port_tube_out_)/2. , 0.)), upper_port_hole_logic,
    		      "UP_PORT_AIR_EXT", vessel_logic, false, 0, false);

    G4Tubs* axial_port_hole_solid =
      new G4Tubs("AXIAL_PORT_AIR_EXT", 0., port_tube_diam_/2.,
    		 (endcap_nozzle_flange_high_ + axial_port_tube_ext)/2., 0., twopi);
    G4LogicalVolume* axial_port_hole_logic =
      new G4LogicalVolume(axial_port_hole_solid, air_mat,
			  "AXIAL_PORT_AIR_EXT");
    // new G4PVPlacement(G4Transform3D(*rot_endcap, G4ThreeVector(0., 0.,  endcap_nozzle_z_pos_ + endcap_nozzle_high_ + (endcap_nozzle_flange_high_)/2.)), axial_port_hole_logic,
    new G4PVPlacement(G4Transform3D(*rot_endcap, G4ThreeVector(0., 0.,  -endcap_nozzle_z_pos_ - endcap_nozzle_high_ - (endcap_nozzle_flange_high_ + axial_port_tube_ext)/2.)), axial_port_hole_logic,
		      "AXIAL_PORT_AIR_EXT", vessel_logic, false, 0, false);

    G4Material* vessel_gas_mat = nullptr;

    if (gas_ == "naturalXe") {
      vessel_gas_mat = materials::GXe(pressure_, temperature_);
      vessel_gas_mat->SetMaterialPropertiesTable(opticalprops::GXe(pressure_, temperature_, sc_yield_, e_lifetime_));
    } else if (gas_ == "enrichedXe") {
      vessel_gas_mat =  materials::GXeEnriched(pressure_, temperature_);
      vessel_gas_mat->SetMaterialPropertiesTable(opticalprops::GXe(pressure_, temperature_, sc_yield_, e_lifetime_));
    } else if  (gas_ == "depletedXe") {
      vessel_gas_mat =  materials::GXeDepleted(pressure_, temperature_);
      vessel_gas_mat->SetMaterialPropertiesTable(opticalprops::GXe(pressure_, temperature_, sc_yield_, e_lifetime_));
    } else if (gas_ == "Ar") {
      vessel_gas_mat =  materials::GAr(pressure_, temperature_);
      vessel_gas_mat->SetMaterialPropertiesTable(opticalprops::GAr(sc_yield_, e_lifetime_));
    } else if (gas_ == "ArXe") {
      vessel_gas_mat =  materials::GXeAr(pressure_, temperature_, xe_perc_);
      vessel_gas_mat->SetMaterialPropertiesTable(opticalprops::GAr(sc_yield_, e_lifetime_));
    } else if (gas_ == "XeHe") {
      vessel_gas_mat =  materials::GXeHe(pressure_, temperature_,
					     xe_perc_, helium_mass_num_);
      vessel_gas_mat->SetMaterialPropertiesTable(opticalprops::GXe(pressure_, temperature_, sc_yield_, e_lifetime_));
    } else {
      G4Exception("[NextNewVessel]", "Construct()", FatalException,
		  "Unknown kind of gas, valid options are: naturalXe, enrichedXe, depletedXe, Ar, ArXe, XeHe.");
    }

    G4LogicalVolume* vessel_gas_logic = new G4LogicalVolume(vessel_gas_solid, vessel_gas_mat,"VESSEL_GAS");
    internal_logic_vol_ = vessel_gas_logic;

    G4VPhysicalVolume* vessel_gas_phys =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), vessel_gas_logic,
                        "VESSEL_GAS", vessel_logic, false, 0, false);
    internal_phys_vol_ = vessel_gas_phys;


    /// INTERNAL SOURCE TUBES ////
    // Lateral
    G4double simulated_length_lat =
    lat_port_tube_length_ - lat_port_tube_out_ - lat_nozzle_flange_high_;

    G4Tubs* lateral_port_tube_solid = new G4Tubs("LATERAL_PORT", 0., (port_tube_diam_/2.+port_tube_thickness_),
						 simulated_length_lat/2., 0, twopi);

    G4LogicalVolume* lateral_port_tube_logic =
    new G4LogicalVolume(lateral_port_tube_solid, steel_316_Ti, "LATERAL_PORT");

    // G4ThreeVector pos_lateral_port(lat_nozzle_x_pos_  + lat_nozzle_high_/2. - simulated_length_lat/2.,  0., lat_nozzle_z_pos_);
    G4ThreeVector pos_lateral_port(vessel_in_diam_/2.  + lat_nozzle_high_ - simulated_length_lat/2.,  0., lat_nozzle_z_pos_);

    new G4PVPlacement(G4Transform3D(*rot_lat, pos_lateral_port), lateral_port_tube_logic,
		      "LATERAL_PORT", vessel_gas_logic, false, 0, false);

    G4Tubs* lateral_port_tube_air_solid =
      new G4Tubs("LATERAL_PORT_AIR", 0., port_tube_diam_/2.,
		 (simulated_length_lat  - port_tube_window_thickn_)/2.,
		 0, twopi);

  G4LogicalVolume* lateral_port_tube_air_logic =
    new G4LogicalVolume(lateral_port_tube_air_solid,
			air_mat, "LATERAL_PORT_AIR");

  new G4PVPlacement(0,G4ThreeVector(0.,0., -port_tube_window_thickn_/2.),
		    lateral_port_tube_air_logic, "LATERAL_PORT_AIR",
		    lateral_port_tube_logic, false, 0, false);

  // Screwed internal source
  if (calib_port_ == "lateral") {

    G4LogicalVolume* lateral_screw_tube_logic = cal_->GetLogicalVolume();

    G4double piece_length = cal_->GetCapsuleThickness(); //16. *mm;
    G4double source_diam = cal_->GetSourceDiameter(); // 6. * mm;
    G4double source_thickness = cal_->GetSourceThickness(); // 2. * mm;

    G4double z_pos_source = cal_->GetSourceZpos();

    G4ThreeVector pos_screw_source(0., 0., 0.);
    if (source_distance_ >= 0.*mm
	&& source_distance_ <= (simulated_length_lat - piece_length - port_tube_window_thickn_)) {
      pos_screw_source = G4ThreeVector(0., 0., (simulated_length_lat - port_tube_window_thickn_)/2. -  piece_length/2. - source_distance_);
      new G4PVPlacement(0, pos_screw_source, lateral_screw_tube_logic,
			"SCREW_SUPPORT", lateral_port_tube_air_logic, false, 0, false);
    } else if (source_distance_ > (simulated_length_lat - piece_length - port_tube_window_thickn_)
	       && source_distance_ < (simulated_length_lat - port_tube_window_thickn_)) {
      // Put the source in the farthest position, inside the inner part of tube, since it cannot be placed between two volumes
      pos_screw_source = G4ThreeVector(0., 0., -(simulated_length_lat - port_tube_window_thickn_)/2.  + piece_length/2.);
      new G4PVPlacement(0, pos_screw_source, lateral_screw_tube_logic,
			"SCREW_SUPPORT", lateral_port_tube_air_logic, false, 0, false);
    } else if (source_distance_ >= (simulated_length_lat - port_tube_window_thickn_)
	       && source_distance_ <= lat_port_tube_length_ - port_tube_window_thickn_ - piece_length) {
      pos_screw_source = G4ThreeVector(0., 0., -(lat_nozzle_flange_high_ + lat_port_tube_out_)/2. + (lat_port_tube_length_ - port_tube_window_thickn_ - source_distance_) - piece_length/2.);
      new G4PVPlacement(0,  pos_screw_source, lateral_screw_tube_logic,
			"SCREW_SUPPORT", lateral_port_hole_logic , false, 0, false);
    } else {
      G4Exception("[NextNewVessel]", "Construct()", FatalException,
		  "This position of the screw source in lateral port is not permitted, since it is outside the lateral port.");
    }

    G4double gen_pos = 0.;
    if (source_distance_ >= 0.*mm && source_distance_ <= (simulated_length_lat - piece_length - port_tube_window_thickn_)) {
      gen_pos = vessel_in_diam_/2. + lat_nozzle_high_ - simulated_length_lat + port_tube_window_thickn_ + piece_length/2. - z_pos_source + source_distance_;
    } else if (source_distance_ > (simulated_length_lat - piece_length - port_tube_window_thickn_)
	       && source_distance_ < (simulated_length_lat - port_tube_window_thickn_)) {
      gen_pos = vessel_in_diam_/2.  + lat_nozzle_high_ - piece_length/2. - z_pos_source;
    } else if (source_distance_ >= (simulated_length_lat - port_tube_window_thickn_)
	       && source_distance_ <= lat_port_tube_length_ - port_tube_window_thickn_ - piece_length) {
      gen_pos = vessel_in_diam_/2.+ lat_nozzle_high_ + (lat_nozzle_flange_high_ + lat_port_tube_out_) - (lat_port_tube_length_ - port_tube_window_thickn_ - source_distance_) + piece_length/2. - z_pos_source;
    } else {
      G4Exception("[NextNewVessel]", "Construct()", FatalException,
		  "This position of the screw source is not permitted, since it is outside the lateral port.");
    }

    screw_gen_lat_ =
      new CylinderPointSamplerLegacy(0., source_thickness, source_diam/2., 0., G4ThreeVector(gen_pos, 0., lat_nozzle_z_pos_), rot_lat);

  }


  // This position of the source is assumed to be at the bottom of the tube, inside.
  lateral_port_source_pos_.setX(vessel_in_diam_/2.  + lat_nozzle_high_ - simulated_length_lat + port_tube_window_thickn_);
  lateral_port_source_pos_.setY(0.);
  lateral_port_source_pos_.setZ(lat_nozzle_z_pos_);

  // Source placed outside the flange, at the end of the port tube
  lateral_port_source_pos_ext_.setX(vessel_in_diam_/2.  + lat_nozzle_high_ + lat_nozzle_flange_high_ + lat_port_tube_out_);
  lateral_port_source_pos_ext_.setY(0.);
  lateral_port_source_pos_ext_.setZ(lat_nozzle_z_pos_);


  // Upper
  G4double simulated_length_up =
    up_port_tube_length_ - up_port_tube_out_ - up_nozzle_flange_high_;

  G4Tubs* upper_port_tube_solid = new G4Tubs("UPPER_PORT", 0., port_tube_diam_/2.+port_tube_thickness_,
					     simulated_length_up/2., 0, twopi);
  G4LogicalVolume* upper_port_tube_logic =
    new G4LogicalVolume(upper_port_tube_solid, steel_316_Ti, "UPPER_PORT");

  G4ThreeVector pos_upper_port(0., vessel_in_diam_/2. + up_nozzle_high_ - simulated_length_up/2., 0.);

  new G4PVPlacement(G4Transform3D(*rot_up, pos_upper_port), upper_port_tube_logic,
   		    "UPPER_PORT", vessel_gas_logic, false, 0, false);

  G4Tubs* upper_port_tube_air_solid =
    new G4Tubs("UPPER_PORT_AIR", 0., port_tube_diam_/2.,
	       (simulated_length_up - port_tube_window_thickn_)/2.,
	       0, twopi);

  G4LogicalVolume* upper_port_tube_air_logic =
    new G4LogicalVolume(upper_port_tube_air_solid,
			air_mat, "UPPER_PORT_AIR");

  new G4PVPlacement(0,G4ThreeVector(0.,0.,-port_tube_window_thickn_/2.),
		    upper_port_tube_air_logic, "UPPER_PORT_AIR",
		    upper_port_tube_logic, false, 0, false);


   // Screwed internal source
  if (calib_port_ == "upper") {

    G4LogicalVolume* upper_screw_tube_logic = cal_->GetLogicalVolume();

    G4double piece_length = cal_->GetCapsuleThickness(); //16. *mm;
    G4double source_diam = cal_->GetSourceDiameter(); // 6. * mm;
    G4double source_thickness = cal_->GetSourceThickness(); // 2. * mm;

    G4double z_pos_source = cal_->GetSourceZpos();

    G4ThreeVector pos_screw_source(0., 0., 0.);
    if (source_distance_ >= 0.*mm
	&& source_distance_ <= (simulated_length_up - piece_length - port_tube_window_thickn_)) {
      pos_screw_source = G4ThreeVector(0., 0., (simulated_length_up - port_tube_window_thickn_)/2. -  piece_length/2. - source_distance_);
      new G4PVPlacement(0, pos_screw_source, upper_screw_tube_logic,
			"SCREW_SUPPORT", upper_port_tube_air_logic, false, 0, false);
    } else if (source_distance_ > (simulated_length_up - piece_length - port_tube_window_thickn_)
	       && source_distance_ < (simulated_length_up - port_tube_window_thickn_)) {
      // Put the source in the farthest position, inside the inner part of tube, since it cannot be placed between two volumes
      pos_screw_source = G4ThreeVector(0., 0., -(simulated_length_up - port_tube_window_thickn_)/2.  + piece_length/2.);
      new G4PVPlacement(0, pos_screw_source, upper_screw_tube_logic,
			"SCREW_SUPPORT", upper_port_tube_air_logic, false, 0, false);
    } else if (source_distance_ >= (simulated_length_up - port_tube_window_thickn_)
	       && source_distance_ <= up_port_tube_length_ - port_tube_window_thickn_ - piece_length) {
      pos_screw_source = G4ThreeVector(0., 0., -(up_nozzle_flange_high_ + up_port_tube_out_)/2. + (up_port_tube_length_ - port_tube_window_thickn_ - source_distance_) - piece_length/2.);
      new G4PVPlacement(0,  pos_screw_source, upper_screw_tube_logic,
			"SCREW_SUPPORT", upper_port_hole_logic , false, 0, false);
    } else {
      G4Exception("[NextNewVessel]", "Construct()", FatalException,
		  "This position of the screw source in upper port is not permitted, since it is outside the upper port.");
    }

    G4double gen_pos = 0.;
    if (source_distance_ >= 0.*mm && source_distance_ <= (simulated_length_up - piece_length - port_tube_window_thickn_)) {
      gen_pos = vessel_in_diam_/2. + up_nozzle_high_ - simulated_length_up + port_tube_window_thickn_ + piece_length/2. - z_pos_source + source_distance_;
    } else if (source_distance_ > (simulated_length_up - piece_length - port_tube_window_thickn_)
	       && source_distance_ < (simulated_length_up - port_tube_window_thickn_)) {
      gen_pos = vessel_in_diam_/2.  + up_nozzle_high_ - piece_length/2. - z_pos_source;
    } else if (source_distance_ >= (simulated_length_up - port_tube_window_thickn_)
	       && source_distance_ <= up_port_tube_length_ - port_tube_window_thickn_ - piece_length) {
      gen_pos = vessel_in_diam_/2.+ up_nozzle_high_ + (up_nozzle_flange_high_ + up_port_tube_out_) - (up_port_tube_length_ - port_tube_window_thickn_ - source_distance_) + piece_length/2. - z_pos_source;
    } else {
      G4Exception("[NextNewVessel]", "Construct()", FatalException,
		  "This position of the screw source is not permitted, since it is outside the upper port.");
    }

    screw_gen_up_ =
      new CylinderPointSamplerLegacy(0., source_thickness, source_diam/2., 0., G4ThreeVector(0., gen_pos, 0.), rot_up);

  }

  // This position of the source is assumed to be at the bottom of the tube, inside.
  upper_port_source_pos_.setX(0.);
  upper_port_source_pos_.setY(vessel_in_diam_/2.  + up_nozzle_high_ - simulated_length_up + port_tube_window_thickn_);
  upper_port_source_pos_.setZ(0.);

  // Source placed outside the flange, at the end of the port tube
  upper_port_source_pos_ext_.setX(0.);
  upper_port_source_pos_ext_.setY(vessel_in_diam_/2.  + up_nozzle_high_ + up_nozzle_flange_high_ + up_port_tube_out_);
  upper_port_source_pos_ext_.setZ(0.);

  // ENDCAP

  G4double simulated_length =
    axial_port_tube_length_ - axial_port_tube_out_ - 2.*axial_port_flange_ -
    axial_distance_flange_endcap_ + endcap_nozzle_high_;


  G4Tubs* axial_port_tube_solid =
    new G4Tubs("AXIAL_PORT", 0., port_tube_diam_/2.+port_tube_thickness_,
	       simulated_length/2., 0, twopi);
  G4LogicalVolume* axial_port_tube_logic =
    new G4LogicalVolume(axial_port_tube_solid, steel_316_Ti, "AXIAL_PORT");

  G4ThreeVector pos_axial_port(0.,0.,  - endcap_nozzle_z_pos_ - endcap_nozzle_high_ +  simulated_length/2.);

  new G4PVPlacement(0, pos_axial_port, axial_port_tube_logic,
		    "AXIAL_PORT", vessel_gas_logic, false, 0, false);


  G4Tubs* axial_port_tube_air_solid =
    new G4Tubs("AXIAL_PORT_AIR", 0., port_tube_diam_/2.,
	       (simulated_length - port_tube_window_thickn_)/2.,
	       0, twopi);

  G4LogicalVolume* axial_port_tube_air_logic =
    new G4LogicalVolume(axial_port_tube_air_solid,
			air_mat, "AXIAL_PORT_AIR");

  new G4PVPlacement(0,G4ThreeVector(0.,0., -port_tube_window_thickn_/2.),
		    axial_port_tube_air_logic, "AXIAL_PORT_AIR",
		    axial_port_tube_logic, false, 0, false);

  // Screwed internal source
  if (calib_port_ == "axial") {

    G4LogicalVolume* axial_screw_tube_logic = cal_->GetLogicalVolume();

    G4double piece_length = cal_->GetCapsuleThickness(); //16. *mm;
    G4double source_diam = cal_->GetSourceDiameter(); // 6. * mm;
    G4double source_thickness = cal_->GetSourceThickness(); // 2. * mm;

    G4double z_pos_source = cal_->GetSourceZpos();

    G4ThreeVector pos_screw_source(0., 0., 0.);
    if (source_distance_ >= 0.*mm
	&& source_distance_ <= (simulated_length - piece_length - port_tube_window_thickn_)) {
      pos_screw_source = G4ThreeVector(0., 0., (simulated_length - port_tube_window_thickn_)/2. -  piece_length/2. - source_distance_);
      new G4PVPlacement(0, pos_screw_source, axial_screw_tube_logic,
			"SCREW_SUPPORT", axial_port_tube_air_logic, false, 0, false);
    } else if (source_distance_ > (simulated_length - piece_length - port_tube_window_thickn_)
	       && source_distance_ < (simulated_length - port_tube_window_thickn_)) {
      // Put the source in the farthest position, inside the inner part of tube, since it cannot be placed between two volumes
      pos_screw_source = G4ThreeVector(0., 0., -(simulated_length - port_tube_window_thickn_)/2.  + piece_length/2.);
      new G4PVPlacement(0, pos_screw_source, axial_screw_tube_logic,
			"SCREW_SUPPORT", axial_port_tube_air_logic, false, 0, false);
    } else if (source_distance_ >= (simulated_length - port_tube_window_thickn_)
	       && source_distance_ <= axial_port_tube_length_ - port_tube_window_thickn_ - piece_length) {
      pos_screw_source = G4ThreeVector(0., 0., (endcap_nozzle_flange_high_ + axial_port_tube_ext)/2. - (axial_port_tube_length_ - port_tube_window_thickn_ - source_distance_) + piece_length/2.);
      new G4PVPlacement(G4Transform3D(*rot_endcap,  pos_screw_source), axial_screw_tube_logic,
        		"SCREW_SUPPORT", axial_port_hole_logic , false, 0, false);
    } else {
      G4Exception("[NextNewVessel]", "Construct()", FatalException,
		  "This position of the screw source is not permitted, since it is outside the axial port.");
    }

    G4double gen_pos = 0.;
    if (source_distance_ >= 0.*mm && source_distance_ <= (simulated_length - piece_length - port_tube_window_thickn_)) {
      gen_pos = - endcap_nozzle_z_pos_ - endcap_nozzle_high_ + simulated_length - port_tube_window_thickn_ - piece_length/2. + z_pos_source - source_distance_;
    } else if (source_distance_ > (simulated_length - piece_length - port_tube_window_thickn_)
	       && source_distance_ < (simulated_length - port_tube_window_thickn_)) {
      gen_pos = - endcap_nozzle_z_pos_ - endcap_nozzle_high_ + piece_length/2. + z_pos_source;
    } else if (source_distance_ >= (simulated_length - port_tube_window_thickn_)
	       && source_distance_ <= axial_port_tube_length_ - port_tube_window_thickn_ - piece_length) {
      gen_pos = - endcap_nozzle_z_pos_ - endcap_nozzle_high_ -endcap_nozzle_flange_high_ - axial_port_tube_ext + (axial_port_tube_length_ - port_tube_window_thickn_ - source_distance_) - piece_length/2. + z_pos_source;
    } else {
      G4Exception("[NextNewVessel]", "Construct()", FatalException,
		  "This position of the screw source in axial port is not permitted, since it is outside the axial port.");
    }

    screw_gen_axial_ =
      new CylinderPointSamplerLegacy(0., source_thickness, source_diam/2., 0., G4ThreeVector(0., 0., gen_pos), 0);

  }

  /*
    G4Tubs* axial_port_collimator_solid =
    new G4Tubs("AXIAL_PORT_COLLIMATOR", 2.5 * mm, port_tube_diam_/2.,
    (simulated_length - port_tube_window_thickn_)/2.,
    0, twopi);
    G4LogicalVolume* axial_port_collimator_logic =
    new G4LogicalVolume(axial_port_collimator_solid,
    G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "AXIAL_PORT_COLLIMATOR");
    new G4PVPlacement(0,G4ThreeVector(0., 0., 0.),
    axial_port_collimator_logic, "AXIAL_PORT_COLLIMATOR",
    axial_port_tube_air_logic, false, 0, true);
  */

  // This position of the source is assumed to be at the bottom of the tube, inside.
  axial_port_source_pos_.setX(0.);
  axial_port_source_pos_.setY(0.);
  axial_port_source_pos_.setZ(- endcap_nozzle_z_pos_  - endcap_nozzle_high_ + simulated_length - port_tube_window_thickn_);

  // This is the external position of the source.
  axial_port_source_pos_ext_.setX(0.);
  axial_port_source_pos_ext_.setY(0.);
  axial_port_source_pos_ext_.setZ(-endcap_nozzle_z_pos_ -axial_distance_flange_endcap_ - 2.*axial_port_flange_ - axial_port_tube_out_);



  // G4cout << "*** Positions of internal sources ***" << G4endl;
  // G4cout << "Axial: " << axial_port_source_pos_ << G4endl;
  // G4cout << "Lateral: " << lateral_port_source_pos_ << G4endl;
  // G4cout << "Upper: " << upper_port_source_pos_ << G4endl;

  // G4cout << "*** Positions of external sources ***" << G4endl;
  // G4cout << "Axial: " << axial_port_source_pos_ext_ << G4endl;
  // G4cout << "Lateral: " << lateral_port_source_pos_ext_ << G4endl;
  // G4cout << "Upper: " << upper_port_source_pos_ext_ << G4endl;

  // SETTING VISIBILITIES   //////////
  G4VisAttributes titanium_col = nexus::TitaniumGrey();
  titanium_col.SetForceSolid(true);
  lateral_port_tube_logic->SetVisAttributes(titanium_col);
  vessel_gas_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  if (visibility_) {

    vessel_logic->SetVisAttributes(titanium_col);
    lateral_port_tube_logic->SetVisAttributes(titanium_col);
    upper_port_tube_logic->SetVisAttributes(titanium_col);
    G4VisAttributes air_col = nexus::Yellow();
    air_col.SetForceSolid(true);
    lateral_port_tube_air_logic->SetVisAttributes(air_col);
    upper_port_tube_air_logic->SetVisAttributes(air_col);
  } else {
    vessel_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }


  //// VERTEX GENERATORS   //
  body_gen_   = new CylinderPointSamplerLegacy(vessel_in_diam_/2., vessel_tube_length_, vessel_thickness_, 0.);
  flange_gen_ = new CylinderPointSamplerLegacy(vessel_out_diam/2., flange_length_,
					 flange_out_diam_/2.-vessel_out_diam/2., 0., G4ThreeVector(0.,0.,0.));
  //trick to avoid vertex the vessel_gas-vessel interface -1*mm thickness
  tracking_endcap_gen_ = new SpherePointSampler(endcap_in_rad_+1*mm, endcap_in_rad_+endcap_thickness_, 0., twopi, 0., endcap_theta_, tracking_endcap_pos, 0);
  energy_endcap_gen_ =
    new SpherePointSampler(endcap_in_rad_+1*mm, endcap_in_rad_+endcap_thickness_, 0., twopi, 180.*deg - endcap_theta_, endcap_theta_, energy_endcap_pos, 0);



  /// Calculating some prob
  G4double body_vol = vessel_tube_solid->GetCubicVolume() - vessel_gas_tube_solid->GetCubicVolume();
  G4double flange_vol = vessel_flange_solid->GetCubicVolume();
  G4double endcap_vol = vessel_tracking_endcap_solid->GetCubicVolume() - vessel_gas_tracking_endcap_solid->GetCubicVolume();
  G4double vol_tot = body_vol+ 2*flange_vol+ 2*endcap_vol;

  perc_tube_vol_ = body_vol/vol_tot;
  perc_endcap_vol_ = endcap_vol /vol_tot;
  }

  NextNewVessel::~NextNewVessel()
  {
    delete body_gen_;
    delete flange_gen_;
    delete tracking_endcap_gen_;
    delete energy_endcap_gen_;
    delete screw_gen_lat_;
    delete screw_gen_axial_;
  }



  G4LogicalVolume* NextNewVessel::GetInternalLogicalVolume() const
  {
    return internal_logic_vol_;
  }

  G4VPhysicalVolume* NextNewVessel::GetInternalPhysicalVolume() const
  {
    return internal_phys_vol_;
  }


  G4double NextNewVessel::GetUPNozzleZPosition() const
  {
    return up_nozzle_z_pos_;
  }

 G4double NextNewVessel::GetLATNozzleZPosition() const
  {
    return lat_nozzle_z_pos_;
  }

  G4ThreeVector NextNewVessel::GetLatExtSourcePosition() const
  {
    return lateral_port_source_pos_ext_;
  }

  G4ThreeVector NextNewVessel::GetUpExtSourcePosition() const
  {
    return upper_port_source_pos_ext_;
  }

  G4ThreeVector NextNewVessel::GetAxialExtSourcePosition() const
  {
    return axial_port_source_pos_ext_;
  }

  G4double NextNewVessel::GetOuterRadius() const
  {
    return vessel_in_diam_/2. + vessel_thickness_;
  }

   G4double NextNewVessel::GetLength() const
  {
    return vessel_tube_length_;
  }

  G4ThreeVector NextNewVessel::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    // Vertex in the VESSEL volume
    if (region == "VESSEL") {
      G4double rand = G4UniformRand();
      if (rand < perc_tube_vol_) { //VESSEL_TUBE
	G4VPhysicalVolume *VertexVolume;
	do {
	  // std::cout<< "vessel tube \t"<< rand <<"\t"<< perc_tube_vol_ << std::endl;
	  vertex = body_gen_->GenerateVertex("BODY_VOL");
	  // To check its volume, one needs to rotate and shift the vertex
	  // because the check is done using global coordinates
	  G4ThreeVector glob_vtx(vertex);
	  // First rotate, then shift
	  glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	  glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	  VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	  // std::cout<<vertex<<std::endl;
	} while (VertexVolume->GetName() != "VESSEL");
      }
      //Vertex in ENDCAPS
      else if (rand < (perc_tube_vol_+2*perc_endcap_vol_)){
	G4VPhysicalVolume *VertexVolume;
	do {
	  if (G4UniformRand() < 0.5){
	    //std::cout<< "tracking endcap "<< rand <<"\t"<< perc_tube_vol_+2*perc_endcap_vol_<< std::endl;
	    vertex = tracking_endcap_gen_->GenerateVertex(VOLUME);  // Tracking
	  }
	  else {
	    //std::cout<< "energy endcap " << rand <<"\t"<< perc_tube_vol_+2*perc_endcap_vol_<< std::endl;
	    vertex = energy_endcap_gen_->GenerateVertex(VOLUME);  // Energy endcap
	  }
	  // To check its volume, one needs to rotate and shift the vertex
	  // because the check is done using global coordinates
	  G4ThreeVector glob_vtx(vertex);
	  // First rotate, then shift
	  glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	  glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	  VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	  //std::cout<<vertex<<std::endl;
	} while (VertexVolume->GetName() != "VESSEL");
      }
      else { //FLANGES
   	if (G4UniformRand() < 0.5) {
	  vertex = flange_gen_->GenerateVertex("BODY_VOL");
	  vertex.setZ(vertex.z() + flange_z_pos_);
	  //std::cout<< "flange tracking \t"<<vertex.z() <<"\t"<< rand << std::endl;
    	}
    	else {
	  vertex = flange_gen_->GenerateVertex("BODY_VOL");
    	  vertex.setZ(vertex.z() - flange_z_pos_);
	  //std::cout<< "flange energy \t"<<vertex.z() <<"\t"<<rand<< std::endl;
   	}
      }
    }
    /// Vertex inside lateral feedthrough, most internal position
    else if (region =="SOURCE_PORT_ANODE") {
      vertex = lateral_port_source_pos_;
    }
    else if (region =="SOURCE_PORT_UP"){
      vertex = upper_port_source_pos_;
    }
    // else if (region =="SOURCE_PORT_CATHODE"){
    //   vertex = G4ThreeVector(lat_nozzle_x_pos_, 0.,-lat_nozzle_z_pos_);
    // }
    /// Vertex inside axial feedthrough, most internal position
    else if (region =="SOURCE_PORT_AXIAL") {
      vertex = axial_port_source_pos_;
    }
    /// Calibration source in capsule, placed at a variable position inside the lateral feedthrough
    else if (region =="INTERNAL_PORT_ANODE") {
      vertex =  screw_gen_lat_->GenerateVertex("BODY_VOL");
    }
    /// Calibration source in capsule, placed at a variable position inside the upper feedthrough
    else if (region =="INTERNAL_PORT_UPPER") {
      vertex =  screw_gen_up_->GenerateVertex("BODY_VOL");
    }
     /// Calibration source in capsule, placed at a variable position inside the axial feedthrough
    else if (region =="INTERNAL_PORT_AXIAL") {
      vertex =  screw_gen_axial_->GenerateVertex("BODY_VOL");
    }
    else {
      G4Exception("[NextNewVessel]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }

}//end namespace nexus
