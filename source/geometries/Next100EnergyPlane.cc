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
    _energy_plane_posz (-70.435 * cm),  // It is CathodeGrid_posz (-63.625 cm) - 6.81 cm (from drawings)

    // Carrier Plate dimensions
    _carrier_plate_thickness (10.0 * cm),         // To be checked
    _carrier_plate_diam (106.9 * cm),            // It must be consistent with ACTIVE diameter
    _carrier_plate_central_hole_diam (10. * cm), // It is nozzle_external_diam + 1 cm   (equal to the ICS hole)

    // Enclosures dimensions
    _enclosure_length (18.434 * cm),
    _enclosure_diam (9.6 * cm),
    _enclosure_flange_length (19.5 * mm),
    _enclosure_window_thickness (7.1 * mm),
    _enclosure_window_diam (85. * mm), 
    _enclosure_pad_thickness (1.0 * mm),         
    _pmt_base_diam (47. *mm),
    _pmt_base_thickness (5. *mm),
    _tpb_thickness (1.*micrometer),
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
    _gas = _mother_logic->GetMaterial();
    _pressure =    _gas->GetPressure();
    _temperature = _gas->GetTemperature();
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
		      "CARRIER_PLATE", _mother_logic, false, 0, false);
    

    ////////////////////////
    /////   Enclosures   ///

    /// Assign optical properties to materials ///
    G4Material* sapphire = MaterialsList::Sapphire();
    sapphire->SetMaterialPropertiesTable(OpticalMaterialProperties::Sapphire());
    G4Material* vacuum = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    vacuum->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());
    G4Material* optical_coupler = MaterialsList::OpticalSilicone();
    optical_coupler->SetMaterialPropertiesTable(OpticalMaterialProperties::OptCoupler());

    // // A "pseudo-enclosure"of vacuum is constructed to hold the elements that are replicated
    G4Tubs* enclosure_gas_solid = new G4Tubs("ENCLOSURE_GAS", 0., _enclosure_diam/2., _enclosure_length/2., 0., twopi);
    G4LogicalVolume* enclosure_gas_logic = 
      new G4LogicalVolume(enclosure_gas_solid, vacuum, "ENCLOSURE_GAS");
    
    G4Tubs* enclosure_flange_solid = 
      new G4Tubs("ENCLOSURE_FLANGE", _enclosure_window_diam/2., _enclosure_diam/2., _enclosure_flange_length/2., 0., twopi);

    G4LogicalVolume* enclosure_flange_logic =
      new G4LogicalVolume(enclosure_flange_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
			  "ENCLOSURE_FLANGE");
    
    G4ThreeVector flange_pos(0., 0., _enclosure_length/2. - _enclosure_flange_length/2.);
    new G4PVPlacement(0, flange_pos, enclosure_flange_logic,
      		      "ENCLOSURE_FLANGE", enclosure_gas_logic, false, 0, false);
  
   
    // Adding the sapphire window
    G4Tubs* enclosure_window_solid = 
      new G4Tubs("ENCLOSURE_WINDOW", 0.,_enclosure_window_diam/2., _enclosure_window_thickness/2., 0., twopi);

    G4LogicalVolume* enclosure_window_logic = 
      new G4LogicalVolume(enclosure_window_solid, sapphire, "ENCLOSURE_WINDOW");

    G4double window_posz =  _enclosure_length/2.-_enclosure_window_thickness/2. ;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,window_posz), enclosure_window_logic,
		      "ENCLOSURE_WINDOW", enclosure_gas_logic, false, 0, false);
    
    // Adding the optical pad
    G4Tubs* enclosure_pad_solid = 
      new G4Tubs("ENCLOSURE_PAD", 0., _enclosure_window_diam/2., _enclosure_pad_thickness/2., 0., twopi);

    // G4LogicalVolume* enclosure_pad_logic =
    //   new G4LogicalVolume(enclosure_pad_solid, MaterialsList::OpticalSilicone(),
    // 							       "ENCLOSURE_PAD");
    G4LogicalVolume* enclosure_pad_logic =
      new G4LogicalVolume(enclosure_pad_solid, optical_coupler, "ENCLOSURE_PAD");

    G4double pad_posz = window_posz - _enclosure_window_thickness/2. - _enclosure_pad_thickness/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,pad_posz), enclosure_pad_logic,
		      "ENCLOSURE_PAD", enclosure_gas_logic, false, 0, false);

    /// TPB coating on sapphire window
    G4Material* tpb = MaterialsList::TPB();
    tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB(_pressure, _temperature));
    // G4cout << "P and T on sapphire windows TPB: " << _pressure << 
    //   ", " << _temperature << G4endl;
    G4Tubs* tpb_solid = new G4Tubs("ENCLOSURE_TPB", 0., _enclosure_window_diam/2, 
				   _tpb_thickness/2., 0., twopi);
    G4LogicalVolume* tpb_logic = 
      new G4LogicalVolume(tpb_solid, tpb, "ENCLOSURE_TPB");

      

    // Adding the PMT
    _pmt->Construct();
    G4LogicalVolume* pmt_logic = _pmt->GetLogicalVolume();
    G4double pmt_rel_posz = _pmt->GetRelPosition().z();
    _pmt_zpos = pad_posz - _enclosure_pad_thickness/2. - pmt_rel_posz;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,_pmt_zpos), pmt_logic,
		      "PMT", enclosure_gas_logic, false, 0, false);
    
    // Adding the PMT base
    G4Tubs* pmt_base_solid = 
      new G4Tubs("PMT_BASE", 0., _pmt_base_diam/2., _pmt_base_thickness, 0.,twopi);
    G4LogicalVolume* pmt_base_logic = 
      new G4LogicalVolume(pmt_base_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"),
			  "PMT_BASE"); 
   
    G4double pmt_base_pos = -_enclosure_length/2. + _pmt_base_thickness; //to be checked
    new G4PVPlacement(0, G4ThreeVector(0.,0., pmt_base_pos),
		      pmt_base_logic, "PMT_BASE", enclosure_gas_logic, false, 0, false);

   
    
    // Placing the "pseudo-enclosures" with all internal components in place + TPB in front
    G4double enclosure_posz =  _energy_plane_posz - _enclosure_length/2.;
    // G4cout << "Enclosure ends at z = " << enclosure_posz + _enclosure_length/2. << G4endl;
    // G4cout << "TPB ends at z = " << _energy_plane_posz + _tpb_thickness << G4endl;
    G4ThreeVector pos;
    G4ThreeVector tpb_pos;
    for (int i=0; i<_num_PMTs; i++) {
      pos = _pmt_positions[i];
      pos.setZ(enclosure_posz);
      tpb_pos = _pmt_positions[i];
      tpb_pos.setZ(_energy_plane_posz + _tpb_thickness/2.);      
      new G4PVPlacement(0, pos, enclosure_gas_logic,
			"PSEUDO-ENCLOSURE", _mother_logic, false, i, false);
      new G4PVPlacement(0, tpb_pos, tpb_logic, "ENCLOSURE_TPB", _mother_logic, 
			false, i, false);
    }

    // G4PVPlacement* enclosure_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), enclosure_logic,
    // 						     "ENCLOSURE", _mother_logic, false, 0);


    /////////////////////////////////////
    //  SETTING VISIBILITIES   //////////
    enclosure_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);
    if (_visibility) {
      G4VisAttributes gas = nexus::Red();
      enclosure_gas_logic->SetVisAttributes(gas);
      G4VisAttributes copper_col = CopperBrown();
      // copper_col.SetForceSolid(true);
      carrier_plate_logic->SetVisAttributes(copper_col);
      G4VisAttributes enclosure_col = nexus::Brown();
      // enclosure_col.SetForceSolid(true);
      enclosure_flange_logic->SetVisAttributes(enclosure_col); 
      G4VisAttributes sapphire_col = nexus::Lilla();
      sapphire_col.SetForceSolid(true);
      enclosure_window_logic->SetVisAttributes(sapphire_col);
      G4VisAttributes pad_col = nexus::LightGreen();
      pad_col.SetForceSolid(true);
      enclosure_pad_logic->SetVisAttributes(pad_col);
      G4VisAttributes base_col = nexus::Yellow();
      base_col.SetForceSolid(true);
      pmt_base_logic->SetVisAttributes(base_col);
      G4VisAttributes tpb_col = nexus::LightBlue();
      tpb_col.SetForceSolid(true);
      tpb_logic->SetVisAttributes(tpb_col);     
    } else {
      carrier_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_flange_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_window_logic->SetVisAttributes(G4VisAttributes::Invisible);
      enclosure_pad_logic->SetVisAttributes(G4VisAttributes::Invisible);
      pmt_base_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }



    // VERTEX GENERATORS   //////////
    _carrier_gen = new CylinderPointSampler(_carrier_plate_diam/2., _carrier_plate_thickness, 0., 0.,
					    G4ThreeVector (0., 0., carrier_plate_posz));

    _enclosure_flange_gen = new CylinderPointSampler(_enclosure_window_diam/2., _enclosure_flange_length/2.,
						  _enclosure_diam/2.-_enclosure_window_diam/2., 0.,
     						   G4ThreeVector (0., 0., _energy_plane_posz - _enclosure_flange_length/2.));

    _enclosure_window_gen = new CylinderPointSampler(_enclosure_window_diam/2., _enclosure_window_thickness, 0., 0.,
						     G4ThreeVector (0., 0., enclosure_posz + _enclosure_length/2. - _enclosure_window_thickness/2.));
  
    _enclosure_pad_gen = new CylinderPointSampler(_enclosure_window_diam/2., _enclosure_pad_thickness, 0., 0.,
						     G4ThreeVector (0., 0., enclosure_posz + _enclosure_length/2. - _enclosure_window_thickness - _enclosure_pad_thickness/2. ));

    _pmt_base_gen = new CylinderPointSampler(_pmt_base_diam/2., _pmt_base_thickness, 0., 0., G4ThreeVector(0.,0., enclosure_posz -_enclosure_length/2. + _pmt_base_thickness ));
 

  }


  Next100EnergyPlane::~Next100EnergyPlane()
  {
    delete _carrier_gen;
    delete _enclosure_flange_gen;
    delete _enclosure_window_gen;
    delete _enclosure_pad_gen;
    delete _pmt_base_gen;
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
	// To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "CARRIER_PLATE");
    }

    // Enclosure flange
    else if (region == "ENCLOSURE") {
      vertex = _enclosure_flange_gen->GenerateVertex("WHOLE_VOL");
      // Translating the vertex to a random enclosure
      G4double rand2 = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand2)];
      vertex += enclosure_pos;
      //      G4cout << vertex<<G4endl;
    }

    // Enclosures windows
    else if (region == "ENCLOSURE_WINDOW") {
      vertex = _enclosure_window_gen->GenerateVertex("INSIDE");
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
      vertex += enclosure_pos;
    }

    // Enclosures pads
    else if (region == "ENCLOSURE_PAD") {
      vertex = _enclosure_pad_gen->GenerateVertex("INSIDE");
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
      vertex += enclosure_pos;
    }

    // PMTs bodies
    else if ((region == "PMT_BODY")||(region == "PMT") ) {
      G4ThreeVector ini_vertex = _pmt->GenerateVertex(region);
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector pmt_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + pmt_pos;
      G4double z_translation = _energy_plane_posz - _enclosure_length/2. + _pmt_zpos;
      vertex.setZ(vertex.z() + z_translation);
    }

    //PMT base 
    else if (region=="PMT_BASE"){
      vertex =_pmt_base_gen->GenerateVertex("INSIDE");
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
      vertex += enclosure_pos;
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


