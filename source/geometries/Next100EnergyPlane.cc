// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <jmunoz@ific.uv.es>
//  Created: 25 Apr 2012
//  
//  Copyright (c) 2012 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next100EnergyPlane.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

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

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  Next100EnergyPlane::Next100EnergyPlane():

    _num_PMTs (60),
    _energy_plane_posz (-70.435 * cm),            // It is CathodeGrid_posz (-63.625 cm) - 6.81 cm (from drawings)

    // Carrier Plate dimensions
    _carrier_plate_thickness (10.0 * cm),         // To be checked
    _carrier_plate_diam (106.9 * cm),             // It must be consistent with ACTIVE diameter
    _carrier_plate_central_hole_diam (10. * cm),  // It is nozzle_external_diam + 1 cm   (equal to the ICS hole)

    // Enclosures dimensions
    _enclosure_length (18.434 * cm),
    _enclosure_diam (9.6 * cm),
    _enclosure_thickness (8.5 * mm),              // Equal to ext diam (96 mm) - Int. diam (79 mm) / 2.
    _enclosure_window_thickness (7.1 * mm),
    _enclosure_pad_thickness (2.0 * mm),          // To be checked

    //   _pmts_pitch (11.0 * cm),
    _visibility(0)
  {

    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/", "Control commands of geometry Next100.");
    _msg->DeclareProperty("energy_plane_vis", _visibility, "Energy Plane Visibility");

    /// The PMT
    _pmt = new PmtR11410(); 

  }



  void Next100EnergyPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }



  void Next100EnergyPlane::Construct()
  {
    GeneratePositions();

    ///////////////////////////
    /////   Carrier Plate   ///

    G4Tubs* carrier_plate_nh_solid = 
      new G4Tubs("CARRIER_PLATE_NH", 0., _carrier_plate_diam/2., _carrier_plate_thickness/2., 0., twopi);

    // Making central hole for vacuum manifold
    G4Tubs* carrier_plate_central_hole_solid = 
      new G4Tubs("CARRIER_PLATE_CENTRAL_HOLE", 0., _carrier_plate_central_hole_diam/2., (_carrier_plate_thickness+1.*cm)/2., 0., twopi);
  
    G4SubtractionSolid* carrier_plate_solid = new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_nh_solid,
								     carrier_plate_central_hole_solid, 0, G4ThreeVector(0. , 0., 0.) );

    // Making PMT holes
    G4Tubs* carrier_plate_pmt_hole_solid = 
      new G4Tubs("CARRIER_PLATE_PMT_HOLE", 0., _enclosure_diam/2., (_carrier_plate_thickness+1.*cm)/2., 0., twopi);

    for (int i=0; i<_num_PMTs; i++) {
      carrier_plate_solid = new G4SubtractionSolid("CARRIER_PLATE", carrier_plate_solid,
						   carrier_plate_pmt_hole_solid, 0, _pmt_positions[i]);
    }

    G4LogicalVolume* carrier_plate_logic = new G4LogicalVolume(carrier_plate_solid,
							       G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
							       "CARRIER_PLATE");

    G4double carrier_plate_posz = _energy_plane_posz - _carrier_plate_thickness/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,carrier_plate_posz), carrier_plate_logic,
		      "CARRIER_PLATE", _mother_logic, false, 0);
    

    ////////////////////////
    /////   Enclosures   ///

    /// Assign optical properties to materials ///
    G4Material* sapphire = MaterialsList::Sapphire();
    sapphire->SetMaterialPropertiesTable(OpticalMaterialProperties::Sapphire());
    G4Material* vacuum = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    vacuum->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

    G4Tubs* enclosure_solid = 
      new G4Tubs("ENCLOSURE", 0., _enclosure_diam/2., _enclosure_length/2., 0., twopi);

    G4LogicalVolume* enclosure_logic = new G4LogicalVolume(enclosure_solid,
							   G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
							   "ENCLOSURE");

    // Filling the enclosure with vacuum
    G4double gas_diam = _enclosure_diam - 2 * _enclosure_thickness;
    G4double gas_length = _enclosure_length - _enclosure_thickness;
    G4Tubs* enclosure_gas_solid = new G4Tubs("ENCLOSURE_GAS", 0., gas_diam/2., gas_length/2., 0., twopi);

    G4LogicalVolume* enclosure_gas_logic = 
      new G4LogicalVolume(enclosure_gas_solid, vacuum, "ENCLOSURE_GAS");
    
    G4ThreeVector gas_pos(0., 0., _enclosure_thickness/2.);
    new G4PVPlacement(0, gas_pos, enclosure_gas_logic,
		      "ENCLOSURE_GAS", enclosure_logic, false, 0);

    // Adding the sapphire window
    G4double window_diam = gas_diam;
    G4Tubs* enclosure_window_solid = 
      new G4Tubs("ENCLOSURE_WINDOW", 0., window_diam/2., _enclosure_window_thickness/2., 0., twopi);

    G4LogicalVolume* enclosure_window_logic = 
      new G4LogicalVolume(enclosure_window_solid, sapphire, "ENCLOSURE_WINDOW");

    G4double window_posz = gas_length/2. - _enclosure_window_thickness/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,window_posz), enclosure_window_logic,
		      "ENCLOSURE_WINDOW", enclosure_gas_logic, false, 0);
    
    // Adding the optical pad
    G4double pad_diam = gas_diam;
    G4Tubs* enclosure_pad_solid = 
      new G4Tubs("ENCLOSURE_PAD", 0., pad_diam/2., _enclosure_pad_thickness/2., 0., twopi);

    // G4LogicalVolume* enclosure_pad_logic =
    //   new G4LogicalVolume(enclosure_pad_solid, MaterialsList::OpticalSilicone(),
    // 							       "ENCLOSURE_PAD");
    G4LogicalVolume* enclosure_pad_logic =
      new G4LogicalVolume(enclosure_pad_solid, vacuum, "ENCLOSURE_PAD");

    // *************** Add optical properties of the pad   TO BE DONE !!    ********************************

    G4double pad_posz = window_posz - _enclosure_window_thickness/2. - _enclosure_pad_thickness/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,pad_posz), enclosure_pad_logic,
		      "ENCLOSURE_PAD", enclosure_gas_logic, false, 0);
    

    // Adding the PMT
    _pmt->Construct();
    G4LogicalVolume* pmt_logic = _pmt->GetLogicalVolume();
    G4double pmt_rel_posz = _pmt->GetRelPosition().z();
    _pmt_zpos = pad_posz - _enclosure_pad_thickness/2. - pmt_rel_posz;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,_pmt_zpos), pmt_logic,
		      "PMT", enclosure_gas_logic, false, 0);
    

    // Placing the enclosures
    G4double enclosure_posz =  _energy_plane_posz - _enclosure_length/2.;
    G4PVPlacement* enclosure_physi;
    G4ThreeVector pos;
    for (int i=0; i<_num_PMTs; i++) {
      pos = _pmt_positions[i];
      pos.setZ(enclosure_posz);
      enclosure_physi = new G4PVPlacement(0, pos, enclosure_logic,
					  "ENCLOSURE", _mother_logic, false, i);
    }

    // G4PVPlacement* enclosure_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), enclosure_logic,
    // 						     "ENCLOSURE", _mother_logic, false, 0);


    /////////////////////////////////////
    //  SETTING VISIBILITIES   //////////
    enclosure_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);
    enclosure_window_logic->SetVisAttributes(G4VisAttributes::Invisible);
    enclosure_pad_logic->SetVisAttributes(G4VisAttributes::Invisible);
    if (_visibility) {
      G4VisAttributes copper_col = CopperBrown();
      copper_col.SetForceSolid(true);
      carrier_plate_logic->SetVisAttributes(copper_col);

      G4VisAttributes enclosure_col = nexus::Brown();
      //copper_col2.SetForceSolid(true);
      enclosure_logic->SetVisAttributes(enclosure_col);      
    } else {
      carrier_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }



    // VERTEX GENERATORS   //////////
    _carrier_gen = new CylinderPointSampler(_carrier_plate_diam/2., _carrier_plate_thickness, 0., 0.,
					    G4ThreeVector (0., 0., carrier_plate_posz));

    _enclosure_body_gen = new CylinderPointSampler(gas_diam/2., _enclosure_length, _enclosure_thickness, 0.,
						   G4ThreeVector (0., 0., enclosure_posz));

    _enclosure_cap_gen = new CylinderPointSampler(gas_diam/2., _enclosure_thickness, 0., 0.,
						  G4ThreeVector (0., 0., enclosure_posz - _enclosure_length/2. + _enclosure_thickness/2.));

    _enclosure_window_gen = new CylinderPointSampler(gas_diam/2., _enclosure_window_thickness, 0., 0.,
						     G4ThreeVector (0., 0., enclosure_posz + _enclosure_length/2. - _enclosure_window_thickness/2.));

    // Getting the enclosure body volume over total
    G4double body_vol = _enclosure_length * pi * ( (_enclosure_diam/2.)*(_enclosure_diam/2.) -
						   (gas_diam/2.)*(gas_diam/2.) );
    G4double cap_vol = _enclosure_thickness * pi * (gas_diam/2.) * (gas_diam/2.);
    _enclosure_body_perc = body_vol / (body_vol + cap_vol);

  }




  Next100EnergyPlane::~Next100EnergyPlane()
  {
    delete _carrier_gen;
    delete _enclosure_body_gen;
    delete _enclosure_cap_gen;
    delete _enclosure_window_gen;
  }



  G4ThreeVector Next100EnergyPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Carrier Plate
    // As it is full of holes, let's get sure vertexes are in the right volume
    if (region == "CARRIER_PLATE") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _carrier_gen->GenerateVertex("INSIDE");
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      } while (VertexVolume->GetName() != "CARRIER_PLATE");
    }

    // Enclosures bodies
    else if (region == "ENCLOSURE_BODY") {
      G4double rand1 = G4UniformRand();
      // Generating in the cilindric part of the enclosure
      if (rand1 < _enclosure_body_perc) {
	vertex = _enclosure_body_gen->GenerateVertex("WHOLE_VOL");
      }
      // Generating in the rear cap of the enclosure
      else {
	vertex = _enclosure_cap_gen->GenerateVertex("INSIDE");
      }
      // Translating the vertex to a random enclosure
      G4double rand2 = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand2)];
      vertex += enclosure_pos;
    }

    // Enclosures windows
    else if (region == "ENCLOSURE_WINDOW") {
      vertex = _enclosure_window_gen->GenerateVertex("INSIDE");
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
      vertex += enclosure_pos;
    }

    // PMTs bodies
    else if (region == "PMT_BODY") {
      G4ThreeVector ini_vertex = _pmt->GenerateVertex(region);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector pmt_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + pmt_pos;
      G4double z_translation = _energy_plane_posz - _enclosure_length/2. + _pmt_zpos + _enclosure_thickness/2.;
      vertex.setZ(vertex.z() + z_translation);
    }
    else {
      G4Exception("[Next100EnergyPlane]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");     
    }
    
    return vertex;
  }



  void Next100EnergyPlane::GeneratePositions()
  {
    /// Function that computes and stores the XY positions of PMTs in the carrier plate

    G4int num_conc_circles = 4;
    G4int num_inner_pmts = 6;
    G4double pitch = 11. * cm;   // To be checked
    G4int total_positions = 0;
    G4ThreeVector position(0.,0.,0.);


    for (G4int circle=1; circle <= num_conc_circles; circle++) {
      G4double rad = circle * pitch;
      G4int num_places = circle * num_inner_pmts;
      G4int step_deg = 360.0 / num_places;
      for (G4int place=0; place<num_places; place++) {
	G4double angle = place * step_deg;
	position.setX(rad * sin(angle * deg));
	position.setY(rad * cos(angle * deg));
	_pmt_positions.push_back(position);
	total_positions++;
      }

    }

    // Checking
    if (total_positions != _num_PMTs) {
      G4cout << "\n\nERROR: Number of PMTs doesn't match with number of positions calculated\n";
      exit(0);
    }

  }


}


