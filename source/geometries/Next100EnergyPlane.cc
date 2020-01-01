// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <jmunoz@ific.uv.es>, <paola.ferrario@dipc.org>
//  Created: 25 Apr 2012
//
//  Copyright (c) 2012-2020 NEXT Collaboration
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
#include <G4UnionSolid.hh>
//#include <G4OpticalSurface.hh>
//#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>
#include <G4VPhysicalVolume.hh>
#include <G4TransportationManager.hh>
#include <Randomize.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

namespace nexus {

  using namespace CLHEP;

  Next100EnergyPlane::Next100EnergyPlane():

    _num_PMTs (60),
    _end_of_sapphire_posz (1000 * mm), // Place holder - to be changed to real value
    // Copper Plate dimensions
    _copper_plate_thickness (120 * mm),
    _copper_plate_diam (1340. * mm),
    _copper_plate_central_hole_diam (12. * mm),

    // Enclosures dimensions
    _hut_thickn (5. * mm), // to be checked
    _hut_vacuum_length (35. * mm), // to be checked
    _hut_length_short (70. * mm),
    _hut_length_medium (100. * mm),
    _hut_length_long (120. * mm),
    //_enclosure_length (18.434 * cm),
    _hole_diam (84. * mm), // to be checked with Javi, for now it's the sapphire wndw diam
    //_enclosure_diam (9.6 * cm),
    //_enclosure_flange_length (19.5 * mm),
    _sapphire_window_thickness (10. * mm),
    _sapphire_window_diam (84. * mm),
    _optical_pad_thickness (1.0 * mm),
    //_pmt_base_diam (47. *mm),
    //_pmt_base_thickness (5. *mm),
    _tpb_thickness (1.*micrometer),
    //   _pmts_pitch (11.0 * cm),
    _visibility(0)
  {

    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator =
      G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/",
				  "Control commands of geometry Next100.");
    _msg->DeclareProperty("energy_plane_vis", _visibility, "Energy Plane Visibility");

    /// The PMT
    _pmt = new PmtR11410();

  }



  void Next100EnergyPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
    _gas          = _mother_logic->GetMaterial();
    _pressure     = _gas->GetPressure();
    _temperature  = _gas->GetTemperature();
  }



  void Next100EnergyPlane::Construct()
  {
    GeneratePositions();

    ///////////////////////////
    /////   Copper Plate   ////
    ///////////////////////////

    G4Tubs* copper_plate_origin_solid =
      new G4Tubs("COPPER_PLATE_ORIGIN", 0., _copper_plate_diam/2.,
		 _copper_plate_thickness/2., 0., twopi);

    G4double offset = 1. * cm;

    // Making central hole
    G4Tubs* copper_plate_central_hole_solid =
      new G4Tubs("COPPER_PLATE_CENTRAL_HOLE", 0., _copper_plate_central_hole_diam/2.,
		 (_copper_plate_thickness+offset)/2., 0., twopi);

    G4SubtractionSolid* copper_plate_hole_solid =
      new G4SubtractionSolid("COPPER_PLATE", copper_plate_origin_solid,
    			     copper_plate_central_hole_solid, 0, G4ThreeVector(0., 0., 0.) );

    /// Start gluing together the different kinds of huts to the copper plate
    G4double hut_diam   = _hole_diam + 2 * _hut_thickn;
    G4ThreeVector hut_pos;

    G4double hut_length = _hut_vacuum_length + _hut_length_short;
    G4double transl_z = _copper_plate_thickness/2. + hut_length/2 - offset/2;
    G4Tubs* short_hut_solid =
      new G4Tubs("SHORT_HUT", 0., hut_diam/2., (hut_length + offset)/2., 0., twopi);
    hut_pos = _short_hut_pos[0];
    hut_pos.setZ(transl_z);
    G4UnionSolid* copper_plate_solid =
      	new G4UnionSolid("COPPER_PLATE", copper_plate_hole_solid, short_hut_solid,
      			 0, hut_pos);
    for (unsigned int i=1; i<_short_hut_pos.size(); i++) {
      hut_pos = _short_hut_pos[i];
      hut_pos.setZ(transl_z);
      copper_plate_solid =
      	new G4UnionSolid("COPPER_PLATE", copper_plate_solid, short_hut_solid,
      			 0, hut_pos);
    }

    hut_length = _hut_vacuum_length + _hut_length_medium;
    transl_z = _copper_plate_thickness/2. + hut_length/2 - offset/2;
    G4Tubs* medium_hut_solid =
      new G4Tubs("MEDIUM_HUT", 0., hut_diam/2., (hut_length + offset)/2., 0., twopi);
    for (unsigned int i=0; i<_medium_hut_pos.size(); i++) {
      hut_pos = _medium_hut_pos[i];
      hut_pos.setZ(transl_z);
      copper_plate_solid =
      	new G4UnionSolid("COPPER_PLATE", copper_plate_solid, medium_hut_solid,
      			 0, hut_pos);
    }

    hut_length = _hut_vacuum_length + _hut_length_long;
    transl_z = _copper_plate_thickness/2. + hut_length/2 - offset/2;
    G4Tubs* long_hut_solid =
      new G4Tubs("LONG_HUT", 0., hut_diam/2., (hut_length + offset)/2., 0., twopi);
    for (unsigned int i=0; i<_long_hut_pos.size(); i++) {
      hut_pos = _long_hut_pos[i];
      hut_pos.setZ(transl_z);
      copper_plate_solid =
      	new G4UnionSolid("COPPER_PLATE", copper_plate_solid, long_hut_solid,
      			 0, hut_pos);
    }

    G4LogicalVolume* copper_plate_logic =
      new G4LogicalVolume(copper_plate_solid,
			  G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
			  "COPPER_PLATE");

    G4double copper_plate_posz = _end_of_sapphire_posz + _copper_plate_thickness/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,copper_plate_posz), copper_plate_logic,
		      "COPPER_PLATE", _mother_logic, false, 0, false);


    ////////////////////////
    ///   Encapsulation  ///
    ////////////////////////

    /// Assign optical properties to materials ///
    G4Material* sapphire = MaterialsList::Sapphire();
    sapphire->SetMaterialPropertiesTable(OpticalMaterialProperties::Sapphire());
    G4Material* tpb = MaterialsList::TPB();
    // ADD CORRECT MAT PROPERTIES
    // tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
    G4Material* vacuum =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    vacuum->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());
    G4Material* optical_coupler = MaterialsList::OpticalSilicone();
    optical_coupler->SetMaterialPropertiesTable(OpticalMaterialProperties::OptCoupler());

    // A cylnder of vacuum is constructed to hold the elements that are replicated
    _vacuum_length = _copper_plate_thickness + _hut_vacuum_length;
    G4Tubs* vacuum_solid =
      new G4Tubs("VACUUM", 0., _hole_diam/2., _vacuum_length/2., 0., twopi);
    G4LogicalVolume* vacuum_logic =
      new G4LogicalVolume(vacuum_solid, vacuum, "VACUUM");

    // Adding the sapphire window
    G4Tubs* sapphire_window_solid =
      new G4Tubs("SAPPHIRE_WINDOW", 0.,_hole_diam/2.,
		 _sapphire_window_thickness/2., 0., twopi);

    G4LogicalVolume* sapphire_window_logic =
      new G4LogicalVolume(sapphire_window_solid, sapphire, "SAPPHIRE_WINDOW");

    G4double window_posz= - _vacuum_length/2. + _sapphire_window_thickness/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., window_posz), sapphire_window_logic,
     		      "SAPPHIRE_WINDOW", vacuum_logic, false, 0, true);


    // Adding TPB coating on sapphire window
    G4Tubs* tpb_solid = new G4Tubs("SAPPHIRE_WNDW_TPB", 0., _sapphire_window_diam/2,
     				   _tpb_thickness/2., 0., twopi);
    G4LogicalVolume* tpb_logic =
      new G4LogicalVolume(tpb_solid, tpb, "SAPPHIRE_WNDW_TPB");

    G4double tpb_posz = - _sapphire_window_thickness/2. + _tpb_thickness/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., tpb_posz), tpb_logic,
     		      "SAPPHIRE_WNDW_TPB", sapphire_window_logic, false, 0, true);

    // Adding the optical pad
    G4Tubs* optical_pad_solid =
      new G4Tubs("OPTICAL_PAD", 0., _hole_diam/2., _optical_pad_thickness/2., 0., twopi);

    G4LogicalVolume* optical_pad_logic =
      new G4LogicalVolume(optical_pad_solid, optical_coupler, "OPTICAL_PAD");

    G4double pad_posz = window_posz + _sapphire_window_thickness/2. +
      _optical_pad_thickness/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., pad_posz), optical_pad_logic,
     		      "OPTICAL_PAD", vacuum_logic, false, 0, true);

    // Adding the PMT
    _pmt->Construct();
    G4LogicalVolume* pmt_logic = _pmt->GetLogicalVolume();
    G4double pmt_rel_posz = _pmt->GetRelPosition().z();
    _pmt_zpos = pad_posz + _optical_pad_thickness/2. + pmt_rel_posz;
    G4ThreeVector pmt_pos =  G4ThreeVector(0., 0., _pmt_zpos);

    _pmt_rot = new G4RotationMatrix();
    _rot_angle = pi;
    _pmt_rot->rotateY(_rot_angle);
    new G4PVPlacement(G4Transform3D(*_pmt_rot, pmt_pos), pmt_logic,
		      "PMT", vacuum_logic, false, 0, true);

    // Placing the encapsulating volume with all internal components in place
    G4double vacuum_posz =  - _copper_plate_thickness/2  + _vacuum_length/2.;
    G4ThreeVector pos;
    for (int i=0; i<_num_PMTs; i++) {
      pos = _pmt_positions[i];
      pos.setZ(vacuum_posz);
      new G4PVPlacement(0, pos, vacuum_logic, "VACUUM", copper_plate_logic, false, i, true);
    }

    //////////////////////////////
    ///  SETTING VISIBILITIES  ///
    //////////////////////////////

    vacuum_logic->SetVisAttributes(G4VisAttributes::Invisible);
    if (_visibility) {
      G4VisAttributes copper_col = CopperBrown();
      //copper_col.SetForceSolid(true);
      copper_plate_logic->SetVisAttributes(copper_col);
      G4VisAttributes sapphire_col = nexus::Lilla();
      sapphire_col.SetForceSolid(true);
      sapphire_window_logic->SetVisAttributes(sapphire_col);
      G4VisAttributes pad_col = nexus::LightGreen();
      pad_col.SetForceSolid(true);
      optical_pad_logic->SetVisAttributes(pad_col);
      G4VisAttributes tpb_col = nexus::LightBlue();
      tpb_col.SetForceSolid(true);
      tpb_logic->SetVisAttributes(tpb_col);
    }  else {
      copper_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
      sapphire_window_logic->SetVisAttributes(G4VisAttributes::Invisible);
      optical_pad_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    //////////////////////////
    /// VERTEX GENERATORS  ///
    //////////////////////////

    G4double full_copper_length =
      _copper_plate_thickness + _hut_vacuum_length + _hut_length_long;
    _copper_gen =
      new CylinderPointSampler(_copper_plate_diam/2., full_copper_length, 0., 0.,
			       G4ThreeVector (0., 0., copper_plate_posz + _copper_plate_thickness/2. + _hut_vacuum_length + _hut_length_long - full_copper_length/2.));

    //   _enclosure_flange_gen =
    //new CylinderPointSampler(_enclosure_window_diam/2., _enclosure_flange_length/2.,
    //						  _enclosure_diam/2.-_enclosure_window_diam/2., 0.,
    //						   G4ThreeVector (0., 0., _energy_plane_posz - _enclosure_flange_length/2.));

    _sapphire_window_gen =
      new CylinderPointSampler(_sapphire_window_diam/2., _sapphire_window_thickness, 0., 0.,
			       G4ThreeVector (0., 0., _end_of_sapphire_posz + _sapphire_window_thickness/2.));
  
    _optical_pad_gen =
      new CylinderPointSampler(_sapphire_window_diam/2., _optical_pad_thickness, 0., 0.,
			       G4ThreeVector (0., 0., _end_of_sapphire_posz + _sapphire_window_thickness + _optical_pad_thickness/2.));

  }


  Next100EnergyPlane::~Next100EnergyPlane()
  {
    delete _copper_gen;
    //delete _enclosure_flange_gen;
    delete _sapphire_window_gen;
    delete _optical_pad_gen;
  }



  G4ThreeVector Next100EnergyPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Copper Plate
    // As it is full of holes, let's get sure vertices are in the right volume
    if (region == "COPPER_PLATE") {
      G4VPhysicalVolume *VertexVolume;
      do {
    	vertex = _copper_gen->GenerateVertex("INSIDE");
    	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      } while (VertexVolume->GetName() != "COPPER_PLATE");
    }

    // // Enclosure flange
    // else if (region == "ENCLOSURE") {
    //   vertex = _enclosure_flange_gen->GenerateVertex("WHOLE_VOL");
    //   // Translating the vertex to a random enclosure
    //   G4double rand2 = _num_PMTs * G4UniformRand();
    //   G4ThreeVector enclosure_pos = _pmt_positions[int(rand2)];
    //   vertex += enclosure_pos;
    //   //      G4cout << vertex<<G4endl;
    // }

    // Sapphire windows
    else if (region == "SAPPHIRE_WINDOW") {
      vertex = _sapphire_window_gen->GenerateVertex("INSIDE");
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector sapphire_pos = _pmt_positions[int(rand)];
      vertex += sapphire_pos;
    }

    // Optical pads
    else if (region == "OPTICAL_PAD") {
      vertex = _optical_pad_gen->GenerateVertex("INSIDE");
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector optical_pad_pos = _pmt_positions[int(rand)];
      vertex += optical_pad_pos;
    }

    // PMTs
    else  if (region == "PMT" ) {
      G4ThreeVector ini_vertex = _pmt->GenerateVertex(region);
      ini_vertex.rotate(_rot_angle, G4ThreeVector(0., 1., 0.));
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector pmt_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + pmt_pos;
      G4double z_translation = _end_of_sapphire_posz + _vacuum_length/2. + _pmt_zpos;
      vertex.setZ(vertex.z() + z_translation);
    }

    // //PMT base
    // else if (region=="PMT_BASE"){
    //   vertex =_pmt_base_gen->GenerateVertex("INSIDE");
    //   G4double rand = _num_PMTs * G4UniformRand();
    //   G4ThreeVector enclosure_pos = _pmt_positions[int(rand)];
    //   vertex += enclosure_pos;
    // }

    else {
      G4Exception("[Next100EnergyPlane]", "GenerateVertex()", FatalException,
     		  "Unknown vertex generation region!");
    }

    return vertex;
  }



  void Next100EnergyPlane::GeneratePositions()
  {
    /// Function that computes and stores the XY positions of PMTs in the copper plate

    G4int num_conc_circles = 4;
    G4int num_inner_pmts = 6;
    G4double x_pitch = 125 * mm;
    G4double y_pitch = 108.3 * mm;
    G4int total_positions = 0;
    G4ThreeVector position(0.,0.,0.);


    for (G4int circle=1; circle<=num_conc_circles; circle++) {
      //G4cout << "*** Circle " << circle << " *****" << G4endl;
      G4double rad     = circle * x_pitch;
      G4double step    = 360.0/num_inner_pmts;
      for (G4int place=0; place<num_inner_pmts; place++) {
	G4double angle = place * step;
	position.setX(rad * cos(angle*deg));
	position.setY(rad * sin(angle*deg));
	_pmt_positions.push_back(position);
	G4cout << position << G4endl;
	total_positions++;
      }

      for (G4int i=1; i<circle; i++) {
	G4double start_x = (circle-(i*0.5))*x_pitch;
	G4double start_y = i*y_pitch;
	rad  = std::sqrt(std::pow(start_x, 2) + std::pow(start_y, 2));
	G4double start_angle = std::atan2(start_y, start_x)/deg;
	//G4double start_step = 360.0/((circle-i+1)*num_inner_pmts);
	//G4cout <<  "(x, y) = " << start_x << ", " << start_y  << G4endl;
	for (G4int place=0; place<num_inner_pmts; place++) {
	  G4double angle = start_angle + place * step;
	  //G4cout << "Place: " << place << G4endl;
	  //G4cout << rad << ", " << angle << G4endl;
	  position.setX(rad * cos(angle*deg));
	  position.setY(rad * sin(angle*deg));
	  _pmt_positions.push_back(position);
	  G4cout << position << G4endl;
	  total_positions++;
	}
      }

    }

    _long_hut_pos =
      std::vector<G4ThreeVector>(_pmt_positions.begin(), _pmt_positions.begin()+18);
    _medium_hut_pos =
      std::vector<G4ThreeVector>(_pmt_positions.begin()+18, _pmt_positions.begin()+36);
    _short_hut_pos =
      std::vector<G4ThreeVector>(_pmt_positions.begin()+36, _pmt_positions.end());


    //_medium_hut_pos = _pmt_positions[std::slice(18, 18, 1)];
    //_long_hut_pos = _pmt_positions[std::slice(36, 24, 1)];

    // Checking
    if (total_positions != _num_PMTs) {
      G4cout << "\nERROR: Number of PMTs doesn't match with number of positions calculated\n";
      G4cout << "Number of PMTs = " << _num_PMTs << ", number of positions = "
	     << total_positions << G4endl;
      exit(0);
    }

  }


}
