// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 13 Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewEnergyPlane.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include <G4GenericMessenger.hh>

#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>

#include <G4VPhysicalVolume.hh>
#include <G4TransportationManager.hh>

#include <Randomize.hh>


namespace nexus {

  NextNewEnergyPlane::NextNewEnergyPlane():
   
    _num_PMTs (12),
    _energy_plane_z_pos (34.7 * cm),//center to EP surface  //middle_nozzle(43.5)-(right_nozzle(15.8)-EP_to_rigth_nozzle(7) (from drawings)
    // Carrier Plate dimensions
    _carrier_plate_thickness (12.0 * cm), 
    _carrier_plate_diam (63.0 * cm), // It must be consistent with ACTIVE diameter???
   
    // Enclosures dimensions
    _enclosure_length (20.0 * cm),
    _enclosure_in_diam (7.95 * cm), //without endcap flange TT
    _enclosure_thickness (0.6 * mm),
    _enclosure_endcap_diam (8.5 * mm), ///?????
    _enclosure_endcap_thickness (1.0 * mm), 
    _enclosure_window_thickness (0.5 * mm), //???
    _enclosure_pad_thickness (0.1 * mm)//  ??????

  {
    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNewEnergyPlane.");
    _msg->DeclareProperty("energy_plane_vis", _visibility, "Energy Plane Visibility");
    /// The PMT
    _pmt = new PmtR11410(); 
  }
  void NextNewEnergyPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
    
  }

  void NextNewEnergyPlane::Construct()
  {
    GeneratePositions();
    ////  CARRIER PLATE  ////
    G4Tubs* carrier_plate_nh_solid = 
      new G4Tubs("CARRIER_NH_PLATE", 0., _carrier_plate_diam/2., _carrier_plate_thickness/2., 0., twopi);
    //substract holes for XeGas flow???
    
    // Making PMT holes
    G4Tubs* carrier_plate_pmt_hole_solid = 
      new G4Tubs("CARRIER_PLATE_PMT_HOLE", 0., _enclosure_in_diam/2.+ _enclosure_thickness, (_carrier_plate_thickness+1.*cm)/2., 0., twopi);
    G4SubtractionSolid* carrier_plate_solid = new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_nh_solid,carrier_plate_pmt_hole_solid,0,_pmt_positions[0] );
    for (int i=1; i<_num_PMTs; i++) {
      carrier_plate_solid = new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_solid,
						   carrier_plate_pmt_hole_solid, 0, _pmt_positions[i]);
    }

    G4LogicalVolume* carrier_plate_logic = new G4LogicalVolume(carrier_plate_solid,
							       G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
							       "CARRIER_PLATE");

    ///Placement
     G4double carrier_plate_z_pos = _energy_plane_z_pos + _carrier_plate_thickness/2.;
     G4PVPlacement* _carrier_plate_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,carrier_plate_z_pos), carrier_plate_logic,
							    "CARRIER_PLATE", _mother_logic, false, 0);

     /////   ENCLOSURES  /////
     G4Tubs* enclosure_solid = 
      new G4Tubs("ENCLOSURE", 0.,  _enclosure_in_diam/2.+ _enclosure_thickness, _enclosure_length/2., 0., twopi);

    G4LogicalVolume* enclosure_logic = new G4LogicalVolume(enclosure_solid,
							   G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
							   "ENCLOSURE");

    // Filling the enclosure with vacuum
    G4double gas_diam = _enclosure_in_diam ;
    G4double gas_length = _enclosure_length-_enclosure_endcap_thickness-_enclosure_window_thickness;
    G4Tubs* enclosure_gas_solid = new G4Tubs("ENCLOSURE_GAS", 0., gas_diam/2., gas_length/2., 0., twopi);
    G4LogicalVolume* enclosure_gas_logic = new G4LogicalVolume(enclosure_gas_solid,
							       G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"),
							       "ENCLOSURE_GAS");
    G4ThreeVector gas_pos(0., 0.,-(_enclosure_endcap_thickness+_enclosure_window_thickness )/2.);
    G4PVPlacement* enclosure_gas_physi = new G4PVPlacement(0, gas_pos, enclosure_gas_logic,
							   "ENCLOSURE_GAS", enclosure_logic, false, 0);
    // Adding the sapphire window
    G4double window_diam = gas_diam;
    G4Tubs* enclosure_window_solid = new G4Tubs("ENCLOSURE_WINDOW", 0., window_diam/2., _enclosure_window_thickness/2., 0., twopi);
    G4LogicalVolume* enclosure_window_logic = new G4LogicalVolume(enclosure_window_solid, MaterialsList::Sapphire(),
								  "ENCLOSURE_WINDOW");
    G4double window_z_pos = gas_length/2. + _enclosure_window_thickness/2.;
    G4PVPlacement* enclosure_window_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,-window_z_pos), enclosure_window_logic,
							      "ENCLOSURE_WINDOW", enclosure_gas_logic, false, 0);

    // Adding the optical pad?????
    G4double pad_z_pos = window_z_pos - _enclosure_window_thickness/2. - _enclosure_pad_thickness/2.;
    // Adding the PMT
    _pmt->Construct();
    G4LogicalVolume* pmt_logic = _pmt->GetLogicalVolume();
    G4double pmt_rel_z_pos = _pmt->GetRelPosition().z();
    _pmt_z_pos = pad_z_pos - _enclosure_pad_thickness/2. - pmt_rel_z_pos;
    G4PVPlacement* pmt_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,_pmt_z_pos), pmt_logic,
						 "PMT", enclosure_gas_logic, false, 0);
    // Placing the enclosures
    G4double enclosure_z_pos =  _energy_plane_z_pos + _enclosure_length/2.;
    G4PVPlacement* enclosure_physi;
    G4ThreeVector pos;
    for (int i=0; i<_num_PMTs; i++) {
      pos = _pmt_positions[i];
      pos.setZ(enclosure_z_pos);
      enclosure_physi = new G4PVPlacement(0, pos, enclosure_logic,
					  "ENCLOSURE", _mother_logic, false, i);
    }
    /////  SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes copper_col(G4Colour(.72, .45, .20));
      copper_col.SetForceSolid(true);
      carrier_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);

      G4VisAttributes copper_col2(G4Colour(.58, .36, .16));
      //copper_col2.SetForceSolid(true);
      enclosure_logic->SetVisAttributes(copper_col2);

      enclosure_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_window_logic->SetVisAttributes(G4VisAttributes::Invisible);
      //enclosure_pad_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    else {
      carrier_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_window_logic->SetVisAttributes(G4VisAttributes::Invisible);
      //enclosure_pad_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    
    // VERTEX GENERATORS   //////////
    _carrier_gen = new CylinderPointSampler(_carrier_plate_diam/2., _carrier_plate_thickness, 0., 0.,
					    G4ThreeVector (0., 0., carrier_plate_z_pos));
    _enclosure_body_gen = new CylinderPointSampler(gas_diam/2., _enclosure_length, _enclosure_thickness, 0.,
						   G4ThreeVector (0., 0., enclosure_z_pos));
    _enclosure_cap_gen = new CylinderPointSampler(gas_diam/2., _enclosure_thickness, 0., 0.,
						  G4ThreeVector (0., 0., enclosure_z_pos + _enclosure_length/2. + _enclosure_endcap_thickness/2.));
    _enclosure_window_gen = new CylinderPointSampler(gas_diam/2., _enclosure_window_thickness, 0., 0.,
						     G4ThreeVector (0., 0., enclosure_z_pos - _enclosure_length/2. - _enclosure_window_thickness/2.));

    // Getting the enclosure body volume over total
    G4double body_vol = _enclosure_length * pi * ( ((_enclosure_in_diam+_enclosure_thickness)/2.)*((_enclosure_in_diam+_enclosure_thickness)/2.) - (gas_diam/2.)*(gas_diam/2.) );
    G4double cap_vol = _enclosure_endcap_thickness * pi * (_enclosure_endcap_diam/2.) * (_enclosure_endcap_diam/2.);
  _enclosure_body_perc = body_vol / (body_vol + cap_vol);////???????
  
  }

  NextNewEnergyPlane::~NextNewEnergyPlane()
  {
    delete _carrier_gen;
    delete _enclosure_body_gen;
    delete _enclosure_cap_gen;
    delete _enclosure_window_gen;
  }

G4ThreeVector NextNewEnergyPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    /// Carrier Plate   // As it is full of holes, let's get sure vertexes are in the right volume
    if (region == "CARRIER_PLATE") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _carrier_gen->GenerateVertex(INSIDE);
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      } while (VertexVolume->GetName() != "CARRIER_PLATE");
    }
    /// Enclosures bodies
    else if (region == "ENCLOSURE_BODY") {
      G4double rand1 = G4UniformRand();
      // Generating in the cilindric part of the enclosure
      if (rand1 < _enclosure_body_perc) {
	vertex = _enclosure_body_gen->GenerateVertex(VOLUME);
      }
      // Generating in the rear cap of the enclosure
      else {
	vertex = _enclosure_cap_gen->GenerateVertex(INSIDE);
      }
      // Translating the vertex to a random enclosure
      G4double rand2 = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand2)];
      vertex += enclosure_pos;
    }
    /// Enclosures windows
    else if (region == "ENCLOSURE_WINDOW") {
      vertex = _enclosure_window_gen->GenerateVertex(INSIDE);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
      vertex += enclosure_pos;
    }
    /// PMTs bodies
    else if (region == "PMT_BODY") {
      G4ThreeVector ini_vertex = _pmt->GenerateVertex(region);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector pmt_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + pmt_pos;
      G4double z_translation = _energy_plane_z_pos + _enclosure_length/2. + _pmt_z_pos - _enclosure_endcap_thickness/2.;
      vertex.setZ(vertex.z() + z_translation);
    }

    return vertex;
  }


void NextNewEnergyPlane::GeneratePositions()
  {
    /// Function that computes and stores the XY positions of PMTs in the carrier plate

    G4int num_conc_circles = 2;
    G4int num_inner_pmts = 3;
    G4int num_outer_pmts = 9;
    G4double rad1 = 7. * cm;   // inner circle radius
    G4double rad2 = 18.5 * cm;   // outer circle radius
    G4double offset_angle = 20.;
    G4int total_positions = 0;
    G4ThreeVector position(0.,0.,0.);
    
    for (G4int circle=1; circle <= num_conc_circles; circle++) {
      if(circle==1){
	G4int step_deg = 360.0 / num_inner_pmts;
	for (G4int place=0; place<num_inner_pmts; place++) {
	  G4double angle = offset_angle + (place * step_deg);
	  position.setX(rad1 * sin(angle * deg));
	  position.setY(rad1 * cos(angle * deg));
	  _pmt_positions.push_back(position);
	  total_positions++;
	}
      }
      else if (circle==2){
	G4int step_deg = 360.0 / num_outer_pmts;
	for (G4int place=0; place<num_outer_pmts; place++) {
	  G4double angle = place * step_deg;
	  position.setX(rad2 * sin(angle * deg));
	  position.setY(rad2 * cos(angle * deg));
	  _pmt_positions.push_back(position);
	  total_positions++;
	}
      }
      else { std::cout<<"Error in PMTs positions generation."<<std::endl; } 
    }
    // Checking
    if (total_positions != _num_PMTs) {
      G4cout << "\n\nERROR: Number of PMTs doesn't match with number of positions calculated\n";
      exit(0);
    }
  }
}//end namespace nexus
