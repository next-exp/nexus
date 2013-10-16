// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>, <jmunoz@ific.uv.es>, <justo.martin-albo@ific.uv.es>
//  Created: 9 Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewVessel.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include <G4GenericMessenger.hh>

#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Colour.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>
#include <G4RotationMatrix.hh>

namespace nexus {
  
  NextNewVessel::NextNewVessel():

    BaseGeometry(),
   
    // Body dimensions
    _vessel_in_diam (64.0  * cm),
    _vessel_body_length (87. * cm),//body cylinder(without flanges)
    _vessel_tube_length (122.256 * cm),  //  870 body + 2. *( (50-10)bodyflange + (50-10)endcapflange + 96.28 endcap)
    _vessel_thickness (2.4  * cm),

    // Flange dimensions (one cylinder = 1 on the _vessel_body side + 1 on the _endcap side)
    _flange_out_diam (82.0 * cm),//Flange inner radius = vessel inner radius + vessel thickness
    _flange_length (5.0 * cm),
    _flange_z_pos (_vessel_body_length/2.),

    // Endcaps dimensions
    _endcap_in_rad (53.2 * cm), //=(66.52/2 - 1.26)
    _endcap_theta (38.7 * deg), //2*(38.7 * deg),// 
    _endcap_thickness (1.26 * cm),
    _endcap_in_z_width (9.628 * cm),  // inner spherical part till flange

    // Nozzle dimensions
    _lat_nozzle_in_diam  (30.*mm),//lat_nozzle_in_diam;
    _lat_nozzle_high (51.*mm),
    _lat_nozzle_thickness (6.1*mm),
    _lat_nozzle_flange_diam (75.*mm),
    _lat_nozzle_flange_high (15.*mm),

    _up_nozzle_in_diam (60.*mm), //up_nozzle_in_diam;
    _up_nozzle_high (54.3*mm),
    _up_nozzle_thickness (6.51*mm),
    _up_nozzle_flange_diam (130*mm),
    _up_nozzle_flange_high (16.7*mm),
   
    _endcap_nozzle_in_diam (88.*mm),
    _endcap_nozzle_high (86.*mm),
    _endcap_nozzle_thickness (6.8*mm),
    _endcap_nozzle_flange_diam (165.*mm),
    _endcap_nozzle_flange_high (22.*mm),

    _lat_nozzle_z_pos (120*mm),//xpos = lat_anode_nozzle_xpos;
    _lat_nozzle_x_pos (_vessel_in_diam/2. +_lat_nozzle_high/2.),// lat_cathode_nozzle_xpos;
    _up_nozzle_y_pos (_vessel_in_diam/2. +_up_nozzle_high/2.),
    _up_nozzle_z_pos ( 277. *mm),//anode_nozzle_zpos;
    _endcap_nozzle_z_pos (_vessel_tube_length/2. + _endcap_in_z_width), //bodylegth/2 + 300mm???? 


    // Vessel gas
    _temperature (303 * kelvin)

  {
    /// Needed External variables


    // Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");

    _msg->DeclareProperty("vessel_vis", _visibility, "Vessel Visibility");

    G4GenericMessenger::Command& pressure_cmd = _msg->DeclareProperty("pressure", _pressure, "Xenon pressure");
    pressure_cmd.SetUnitCategory("Pressure");
    pressure_cmd.SetParameterName("pressure", false);
    pressure_cmd.SetRange("pressure>0.");
  }



void NextNewVessel::Construct()
  {
    // Body solid
    G4double vessel_out_diam = _vessel_in_diam + 2*_vessel_thickness;
    G4Tubs* vessel_tube_solid = new G4Tubs("VESSEL_BODY", 0., vessel_out_diam/2.,
					   _vessel_tube_length/2.,0., twopi);
    G4Tubs* vessel_gas_tube_solid = new G4Tubs("VESSEL_GAS_BODY", 0., _vessel_in_diam/2.,
					       _vessel_tube_length/2.,0., twopi);

    // Endcaps solids
    G4double endcap_out_rad = _endcap_in_rad + _endcap_thickness;
    G4Sphere* vessel_tracking_endcap_solid = new G4Sphere("VESSEL_TRACKING_ENDCAP",
                                                          0. * cm,  endcap_out_rad,   //radius
                                                          0. * deg, 360. * deg,       // phi
                                                          0. * deg, _endcap_theta);   // theta
    G4Sphere* vessel_gas_tracking_endcap_solid = new G4Sphere("VESSEL_GAS_TRACKING_ENDCAP",
                                                               0. * cm,  _endcap_in_rad,   //radius
                                                               0. * deg, 360. * deg,       // phi
                                                               0. * deg, _endcap_theta);   // theta
     G4Sphere* vessel_energy_endcap_solid = new G4Sphere("VESSEL_ENERGY_ENDCAP",
                                                         0. * cm,  endcap_out_rad,     //radius
                                                         0. * deg, 360. * deg,        // phi
                                                         180. * deg - _endcap_theta, _endcap_theta); // theta
     G4Sphere* vessel_gas_energy_endcap_solid = new G4Sphere("VESSEL_GAS_ENERGY_ENDCAP",
                                                             0. * cm,  _endcap_in_rad,    //radius
                                                             0. * deg, 360. * deg,        // phi
                                                             180. * deg - _endcap_theta, _endcap_theta); // theta
    
    // Flange solid
    G4Tubs* vessel_flange_solid = new G4Tubs("VESSEL_FLANGE", vessel_out_diam/2., _flange_out_diam/2.,
					     _flange_length, 0., twopi);
    
    // Nozzle solids
    G4Tubs* lateral_nozzle_tube_solid = 
      new G4Tubs("LAT_TUBE_NOZZLE", 0., _lat_nozzle_in_diam/2. +_lat_nozzle_thickness, _lat_nozzle_high, 0., twopi);
    G4Tubs* lateral_nozzle_flange_solid = 
      new G4Tubs("LAT_NOZZLE_FLANGE", 0., _lat_nozzle_flange_diam/2., _lat_nozzle_flange_high, 0., twopi);
    G4Tubs* lateral_nozzle_gas_solid = 
      new G4Tubs("LAT_NOZZLE_GAS", 0., _lat_nozzle_in_diam/2., _lat_nozzle_high, 0., twopi);

    G4Tubs* up_nozzle_tube_solid =
      new G4Tubs("UP_TUBE_NOZZLE", 0., _up_nozzle_in_diam/2.+_up_nozzle_thickness,_up_nozzle_high, 0., twopi);
    G4Tubs* up_nozzle_flange_solid =
      new G4Tubs("UP_NOZZLE_FLANGE", 0., _up_nozzle_flange_diam/2.,_up_nozzle_flange_high, 0., twopi);
    G4Tubs* up_nozzle_gas_solid =
      new G4Tubs("UP_NOZZLE_GAS", 0., _up_nozzle_in_diam/2.,_up_nozzle_high, 0., twopi);
    
    G4Tubs* endcap_nozzle_tube_solid =
      new G4Tubs("ENDCAP_TUBE_NOZZLE", 0., _endcap_nozzle_in_diam/2.+_endcap_nozzle_thickness,_endcap_nozzle_high, 0., twopi);
    G4Tubs* endcap_nozzle_flange_solid =
      new G4Tubs("ENDCAP_NOZZLE_FLANGE", 0., _endcap_nozzle_flange_diam/2.,_endcap_nozzle_flange_high, 0., twopi);
    G4Tubs* endcap_nozzle_gas_solid =
      new G4Tubs("ENDCAP_NOZZLE_GAS", 0., _endcap_nozzle_in_diam/2.,_endcap_nozzle_high, 0., twopi);
  

    //// UNIONS ///////
    G4double endcap_z_pos = (_vessel_tube_length/2.)- (_endcap_in_rad -_endcap_in_z_width);
    G4ThreeVector tracking_endcap_pos(0, 0, endcap_z_pos);
    G4ThreeVector energy_endcap_pos(0, 0, -1. * endcap_z_pos);
    G4ThreeVector tracking_flange_pos(0, 0, _flange_z_pos);
    G4ThreeVector energy_flange_pos(0, 0, -1. * _flange_z_pos);

    // Body + Tracking endcap
    G4UnionSolid* vessel_solid = new G4UnionSolid("VESSEL", vessel_tube_solid, vessel_tracking_endcap_solid,
                                                  0, tracking_endcap_pos);
    // Body + Tracking endcap + Energy endcap
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, vessel_energy_endcap_solid,
                                    0, energy_endcap_pos);
    // Body + Tracking endcap + Energy endcap + Tracking flange
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, vessel_flange_solid,
				    0, tracking_flange_pos);
    // Body + Tracking endcap + Energy endcap + Tracking flange + Energy flange
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid,
                                    vessel_flange_solid, 0, energy_flange_pos);

    // building nozzles
    G4UnionSolid* lateral_nozzle_solid = new G4UnionSolid("LAT_NOZZLE", lateral_nozzle_tube_solid,
							  lateral_nozzle_flange_solid, 0,
							  G4ThreeVector(0.,0.,_lat_nozzle_high));

    G4UnionSolid* up_nozzle_solid = new G4UnionSolid("UP_NOZZLE", up_nozzle_tube_solid,
							  up_nozzle_flange_solid, 0,
							  G4ThreeVector(0.,0.,_up_nozzle_high));    
    G4UnionSolid* endcap_nozzle_solid = new G4UnionSolid("ENDCAP_NOZZLE", endcap_nozzle_tube_solid,
							  endcap_nozzle_flange_solid, 0,
							  G4ThreeVector(0.,0.,_endcap_nozzle_high));    
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
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, lateral_nozzle_solid,
				    rot_lat, G4ThreeVector(_lat_nozzle_x_pos, 0.,_lat_nozzle_z_pos));
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, lateral_nozzle_solid,
				    rot_lat, G4ThreeVector(_lat_nozzle_x_pos, 0.,-_lat_nozzle_z_pos));
    
    // Body + Tracking endcap + Energy endcap + Tracking flange + Energy flange + Lateral nozzles + Upper nozzles
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, up_nozzle_solid,
				    rot_up, G4ThreeVector(0.,_up_nozzle_y_pos,_up_nozzle_z_pos));
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, up_nozzle_solid,
				    rot_up, G4ThreeVector(0.,_up_nozzle_y_pos,0.));
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, up_nozzle_solid,
				    rot_up, G4ThreeVector(0.,_up_nozzle_y_pos, -_up_nozzle_z_pos));
   
    // Body + Tracking endcap + Energy endcap + Tracking flange + Energy flange + Lateral nozzles + Upper nozzles + Endcap nozzles
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, endcap_nozzle_solid,
				    0, G4ThreeVector(0.,0.,_endcap_nozzle_z_pos));
    vessel_solid = new G4UnionSolid("VESSEL", vessel_solid, endcap_nozzle_solid,
				    rot_endcap, G4ThreeVector(0.,0.,-_endcap_nozzle_z_pos));
   

    ////GAS SOLID //////
    // Body gas + Tracking endcap gas
    G4UnionSolid* vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_tube_solid,
                                                      vessel_gas_tracking_endcap_solid, 0, tracking_endcap_pos);
    // Body gas + Tracking endcap gas + Energy endcap gas
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid,
                                        vessel_gas_energy_endcap_solid, 0, energy_endcap_pos);
    // Body gas + Tracking endcap gas + Energy endcap gas + nozzles gas
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, lateral_nozzle_gas_solid, 
					rot_lat, G4ThreeVector(_lat_nozzle_x_pos, 0.,_lat_nozzle_z_pos));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, lateral_nozzle_gas_solid, 
					rot_lat, G4ThreeVector(_lat_nozzle_x_pos, 0.,-_lat_nozzle_z_pos));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, up_nozzle_gas_solid, 
					rot_up, G4ThreeVector(0.,_up_nozzle_y_pos,_up_nozzle_z_pos));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, up_nozzle_gas_solid, 
					rot_up, G4ThreeVector(0.,_up_nozzle_y_pos,0.));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, up_nozzle_gas_solid, 
					rot_up, G4ThreeVector(0.,_up_nozzle_y_pos,-_up_nozzle_z_pos));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, endcap_nozzle_gas_solid, 
					0, G4ThreeVector(0.,0.,_endcap_nozzle_z_pos));
    vessel_gas_solid = new G4UnionSolid("VESSEL_GAS", vessel_gas_solid, endcap_nozzle_gas_solid, 
					rot_endcap, G4ThreeVector(0.,0.,-_endcap_nozzle_z_pos));
    

    //// LOGICS //////
    G4LogicalVolume* vessel_logic = new G4LogicalVolume(vessel_solid,
                                                        MaterialsList::Steel316Ti(),
                                                        "VESSEL");
    this->SetLogicalVolume(vessel_logic);
    G4Material* vessel_gas_mat =  MaterialsList::GXe(_pressure, _temperature);
    vessel_gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, _temperature));
    G4LogicalVolume* vessel_gas_logic = new G4LogicalVolume(vessel_gas_solid, vessel_gas_mat,
							    "VESSEL_GAS");
    _internal_logic_vol = vessel_gas_logic;
    G4PVPlacement* vessel_gas_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), vessel_gas_logic,
                                                        "VESSEL_GAS", vessel_logic, false, 0);

						   	   
    // SETTING VISIBILITIES   //////////
    //if (_visibility) {
    G4VisAttributes titanium_col(G4Colour(.71, .69, .66));
    // titanium_col.SetForceSolid(true);
    vessel_logic->SetVisAttributes(titanium_col);
    // G4VisAttributes gas_col(G4Colour(1., 1., 1.));
    // gas_col.SetForceSolid(true);
    // vessel_gas_logic->SetVisAttributes(gas_col);
    vessel_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);
    // }
      //else {
      //vessel_logic->SetVisAttributes(G4VisAttributes::Invisible);
    // }


    //// VERTEX GENERATORS   //
    // _body_gen  = new CylinderPointSampler(_vessel_in_rad, _vessel_length, _vessel_thickness, 0.);
    // _tracking_endcap_gen = new SpherePointSampler(_endcap_in_rad, _endcap_thickness, tracking_endcap_pos, 0,
    //                                                0., twopi, 0., _endcap_theta);
    // _energy_endcap_gen = new SpherePointSampler(_endcap_in_rad, _endcap_thickness, energy_endcap_pos, 0,
    //                                              0., twopi, 180.*deg - _endcap_theta, _endcap_theta);
    // _tracking_flange_gen  = new CylinderPointSampler(vessel_out_rad, _flange_length,
    //                                                  _flange_out_rad-vessel_out_rad, 0., tracking_flange_pos);
    // _energy_flange_gen  = new CylinderPointSampler(vessel_out_rad, _flange_length,
    //                                                _flange_out_rad-vessel_out_rad, 0., energy_flange_pos);


    /// Calculating some prob ?????????????????
    // G4double body_vol = vessel_body_solid->GetCubicVolume() - vessel_gas_body_solid->GetCubicVolume();
    // G4double endcap_vol =  vessel_tracking_endcap_solid->GetCubicVolume() - vessel_gas_tracking_endcap_solid->GetCubicVolume();
    // _perc_endcap_vol = endcap_vol / (body_vol + 2. * endcap_vol);

   }
   
   NextNewVessel::~NextNewVessel()
   {
     delete _body_gen;
     delete _tracking_endcap_gen;
     delete _energy_endcap_gen;
     delete _tracking_flange_gen;
     delete _energy_flange_gen;
   }

G4LogicalVolume* NextNewVessel::GetInternalLogicalVolume()
  {
    return _internal_logic_vol;
  }


G4ThreeVector NextNewVessel::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // // Vertex in the whole VESSEL volume except flanges
    // if (region == "VESSEL") {
    //   G4double rand = G4UniformRand();
    //   if (rand < _perc_endcap_vol) {
    //     G4VPhysicalVolume *VertexVolume;
    //     do {
    //       vertex = _tracking_endcap_gen->GenerateVertex("VOLUME");  // Tracking endcap
    //       VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
    //     } while (VertexVolume->GetName() != "VESSEL");
    //   }
    //   else if (rand > 1. - _perc_endcap_vol) {
    //     G4VPhysicalVolume *VertexVolume;
    //     do {
    //       vertex = _energy_endcap_gen->GenerateVertex("VOLUME");  // Energy endcap
    //       VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
    //     } while (VertexVolume->GetName() != "VESSEL");
    //   }
    //   else
    //     vertex = _body_gen->GenerateVertex(TUBE_VOLUME);  // Body
    // }

    // // Vertex in FLANGES
    // else if (region == "VESSEL_FLANGES") {
    //   if (G4UniformRand() < 0.5)
    //     vertex = _tracking_flange_gen->GenerateVertex(TUBE_VOLUME);
    //   else
    //     vertex = _energy_flange_gen->GenerateVertex(TUBE_VOLUME);
    // }

    // // Vertex in TRACKING ENDCAP
    // else if (region == "VESSEL_TRACKING_ENDCAP") {
    //   G4VPhysicalVolume *VertexVolume;
    //   do {
    //     vertex = _tracking_endcap_gen->GenerateVertex("VOLUME");  // Tracking endcap
    //     VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
    //   } while (VertexVolume->GetName() != "VESSEL");
    // }

    // // Vertex in ENERGY ENDCAP
    // else if (region == "VESSEL_ENERGY_ENDCAP") {
    //   G4VPhysicalVolume *VertexVolume;
    //   do {
    //     vertex = _energy_endcap_gen->GenerateVertex("VOLUME");  // Energy endcap
    //     VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
    //   } while (VertexVolume->GetName() != "VESSEL");
    // }

    return vertex;
  }


} //end namespace nexus
