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
    _copper_plate_thickn (120 * mm),
    _copper_plate_diam (1340. * mm),
    _gas_hole_diam (12. * mm),
    _hole_up_posx (-62.5 * mm),
    _hole_up_posy (515. * mm),
    _hole_lat1_posx (477 * mm),
    _hole_lat1_posy (-203.8 * mm),
    _hole_lat2_posx (-415 * mm),
    _hole_lat2_posy (-311.2 * mm),
    // Hut dimensions
    _hut_int_diam (76. * mm),
    _hut_thickn (5. * mm),
    _hut_hole_length (45. * mm),
    _hut_length_short (70. * mm),
    _hut_length_medium (100. * mm),
    _hut_length_long (120. * mm),
    _hole_diam_front (84. * mm),
    _hole_diam_rear (65. * mm),
    _hole_length_front (41.75 * mm),// average between 37 mm of front and 46.5 mm of front+medium
    _hole_length_rear (78.25 * mm), // the sum of front+rear length must give copper thickness
    _sapphire_window_thickn (6. * mm),
    _optical_pad_thickn (1.0 * mm),
    _tpb_thickn (1.*micrometer),
    _pmt_stand_out (2. * mm), // length that PMTs stand oput of copper, in the front
    _internal_pmt_base_diam (54. * mm),
    _internal_pmt_base_thickn (0.2 * mm),
    _visibility(0),
    _verbosity(0)
  {

    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator =
      G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/",
				  "Control commands of geometry Next100.");
    _msg->DeclareProperty("energy_plane_vis", _visibility, "Energy Plane Visibility");
    _msg->DeclareProperty("verbosity", _verbosity, "Energy Plane verbosity");

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

    /// Copper Plate ///
    G4Tubs* copper_plate_origin_solid =
      new G4Tubs("COPPER_PLATE_ORIGIN", 0., _copper_plate_diam/2.,
		 _copper_plate_thickn/2., 0., twopi);

    G4double offset = 1. * cm;

    /// Holes for gas flow ///
    G4Tubs* copper_plate_gas_hole_solid =
      new G4Tubs("COPPER_PLATE_CENTRAL_HOLE", 0., _gas_hole_diam/2.,
		 (_copper_plate_thickn + offset)/2., 0., twopi);

    G4ThreeVector gas_hole_pos = G4ThreeVector(0., 0., 0.);
    G4SubtractionSolid* copper_plate_hole_solid =
      new G4SubtractionSolid("COPPER_PLATE", copper_plate_origin_solid,
    			     copper_plate_gas_hole_solid, 0, gas_hole_pos);

    gas_hole_pos.setX(_hole_up_posx);
    gas_hole_pos.setY(_hole_up_posy);
    copper_plate_hole_solid =
      new G4SubtractionSolid("COPPER_PLATE", copper_plate_hole_solid,
    			     copper_plate_gas_hole_solid, 0, gas_hole_pos);
    gas_hole_pos.setX(_hole_lat1_posx);
    gas_hole_pos.setY(_hole_lat1_posy);
    copper_plate_hole_solid =
      new G4SubtractionSolid("COPPER_PLATE", copper_plate_hole_solid,
    			     copper_plate_gas_hole_solid, 0, gas_hole_pos);
    gas_hole_pos.setX(_hole_lat2_posx);
    gas_hole_pos.setY(_hole_lat2_posy);
    copper_plate_hole_solid =
      new G4SubtractionSolid("COPPER_PLATE", copper_plate_hole_solid,
    			     copper_plate_gas_hole_solid, 0, gas_hole_pos);


    /// Glue together the different kinds of huts to the copper plate ///
    G4double hut_diam   = _hut_int_diam + 2 * _hut_thickn;
    G4ThreeVector hut_pos;

    G4double hut_length = _hut_hole_length + _hut_length_short;
    G4double transl_z = _copper_plate_thickn/2. + hut_length/2 - offset/2.;
    G4Tubs* short_hut_solid =
      new G4Tubs("SHORT_HUT", 0., hut_diam/2., (hut_length + offset)/2., 0., twopi);
    hut_pos = _short_hut_pos[0];
    hut_pos.setZ(transl_z);
    G4UnionSolid* copper_plate_hut_solid =
      	new G4UnionSolid("COPPER_PLATE", copper_plate_hole_solid, short_hut_solid,
      			 0, hut_pos);
    for (unsigned int i=1; i<_short_hut_pos.size(); i++) {
      hut_pos = _short_hut_pos[i];
      hut_pos.setZ(transl_z);
      copper_plate_hut_solid =
      	new G4UnionSolid("COPPER_PLATE", copper_plate_hut_solid, short_hut_solid,
      			 0, hut_pos);
    }

    hut_length = _hut_hole_length + _hut_length_medium;
    transl_z = _copper_plate_thickn/2. + hut_length/2 - offset/2.;
    G4Tubs* medium_hut_solid =
      new G4Tubs("MEDIUM_HUT", 0., hut_diam/2., (hut_length + offset)/2., 0., twopi);
    for (unsigned int i=0; i<_medium_hut_pos.size(); i++) {
      hut_pos = _medium_hut_pos[i];
      hut_pos.setZ(transl_z);
      copper_plate_hut_solid =
      	new G4UnionSolid("COPPER_PLATE", copper_plate_hut_solid, medium_hut_solid,
      			 0, hut_pos);
    }

    hut_length = _hut_hole_length + _hut_length_long;
    transl_z = _copper_plate_thickn/2. + hut_length/2 - offset/2.;
    G4Tubs* long_hut_solid =
      new G4Tubs("LONG_HUT", 0., hut_diam/2., (hut_length + offset)/2., 0., twopi);
    for (unsigned int i=0; i<_long_hut_pos.size(); i++) {
      hut_pos = _long_hut_pos[i];
      hut_pos.setZ(transl_z);
      copper_plate_hut_solid =
      	new G4UnionSolid("COPPER_PLATE", copper_plate_hut_solid, long_hut_solid,
      			 0, hut_pos);
    }

    /// Holes in copper ///
    G4Tubs* hole_front_solid =
      new G4Tubs("HOLE_FRONT", 0., _hole_diam_front/2., (_hole_length_front + offset)/2.,
		 0., twopi);
    G4Tubs* hole_rear_solid =
      new G4Tubs("HOLE_REAR", 0., _hole_diam_rear/2., (_hole_length_rear + offset)/2.,
		 0., twopi);
    transl_z = (_hole_length_front + offset)/2. + _hole_length_rear/2 - offset/2.;
    G4UnionSolid* hole_solid =
      	new G4UnionSolid("HOLE", hole_front_solid, hole_rear_solid,
			 0, G4ThreeVector(0., 0., transl_z));

    G4Tubs* hole_hut_solid =
      new G4Tubs("HOLE_HUT", 0., _hut_int_diam/2., (_hut_hole_length + offset)/2., 0., twopi);
    transl_z = (_hole_length_front + offset)/2. + _hole_length_rear + _hut_hole_length/2.
      - offset/2.;
    hole_solid =
      	new G4UnionSolid("HOLE", hole_solid, hole_hut_solid,
			 0, G4ThreeVector(0., 0., transl_z));

    G4ThreeVector hole_pos = _pmt_positions[0];
    transl_z = - _copper_plate_thickn/2. + _hole_length_front/2. - offset/2.;
    hole_pos.setZ(transl_z);
    G4SubtractionSolid* copper_plate_solid =
      new G4SubtractionSolid("COPPER_PLATE", copper_plate_hut_solid, hole_solid, 0, hole_pos);;

    for (G4int i=1; i<_num_PMTs; i++) {
      hole_pos = _pmt_positions[i];
      hole_pos.setZ(transl_z);
      copper_plate_solid =
	new G4SubtractionSolid("COPPER_PLATE", copper_plate_solid, hole_solid, 0, hole_pos);
    }

    G4LogicalVolume* copper_plate_logic =
      new G4LogicalVolume(copper_plate_solid,
			  G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
			  "COPPER_PLATE");

    G4double stand_out_length =
      _sapphire_window_thickn + _optical_pad_thickn + _pmt_stand_out;
    _copper_plate_posz = _end_of_sapphire_posz + stand_out_length + _copper_plate_thickn/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., _copper_plate_posz), copper_plate_logic,
		      "COPPER_PLATE", _mother_logic, false, 0, false);



    /// Assign optical properties to materials ///

    G4Material* sapphire = MaterialsList::Sapphire();
    sapphire->SetMaterialPropertiesTable(OpticalMaterialProperties::Sapphire());
    G4Material* tpb = MaterialsList::TPB();
    tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
    G4Material* vacuum =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    vacuum->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());
    G4Material* optical_coupler = MaterialsList::OpticalSilicone();
    optical_coupler->SetMaterialPropertiesTable(OpticalMaterialProperties::OptCoupler());


    /// Vacuum volume that encapsulates all elements related to PMTs. ///
    G4Tubs* vacuum_front_solid =
      new G4Tubs("HOLE_FRONT", 0., _hole_diam_front/2., _hole_length_front/2., 0., twopi);
    transl_z = _hole_length_front/2. + _hole_length_rear/2 - offset/2.;
    G4UnionSolid* vacuum_solid =
      	new G4UnionSolid("HOLE", vacuum_front_solid, hole_rear_solid,
			 0, G4ThreeVector(0., 0., transl_z));
    transl_z = _hole_length_front/2. + _hole_length_rear + _hut_hole_length/2. - offset/2.;
    vacuum_solid =
      	new G4UnionSolid("HOLE", vacuum_solid, hole_hut_solid,
			 0, G4ThreeVector(0., 0., transl_z));

    G4Tubs* stand_out_solid =
      new G4Tubs("STAND_OUT", 0., _hole_diam_front/2., (stand_out_length + offset)/2.,
		 0., twopi);
    transl_z = - _hole_length_front/2. - stand_out_length/2 + offset/2.;
    vacuum_solid =
      new G4UnionSolid("HOLE", vacuum_solid, stand_out_solid, 0,
		       G4ThreeVector(0., 0., transl_z));
    G4LogicalVolume* vacuum_logic = new G4LogicalVolume(vacuum_solid, vacuum, "HOLE");


    /// Sapphire window ///
    G4Tubs* sapphire_window_solid =
      new G4Tubs("SAPPHIRE_WINDOW", 0., _hole_diam_front/2., _sapphire_window_thickn/2.,
		 0., twopi);

    G4LogicalVolume* sapphire_window_logic =
      new G4LogicalVolume(sapphire_window_solid, sapphire, "SAPPHIRE_WINDOW");

    G4double window_posz =
      - _hole_length_front/2. - stand_out_length + _sapphire_window_thickn/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., window_posz), sapphire_window_logic,
     		      "SAPPHIRE_WINDOW", vacuum_logic, false, 0, false);


    /// TPB coating on sapphire window ///
    G4Tubs* tpb_solid = new G4Tubs("SAPPHIRE_WNDW_TPB", 0., _hole_diam_front/2,
     				   _tpb_thickn/2., 0., twopi);
    G4LogicalVolume* tpb_logic =
      new G4LogicalVolume(tpb_solid, tpb, "SAPPHIRE_WNDW_TPB");

    G4double tpb_posz = - _sapphire_window_thickness/2. + _tpb_thickness/2.;

    new G4PVPlacement(0, G4ThreeVector(0., 0., tpb_posz), tpb_logic,
     		      "SAPPHIRE_WNDW_TPB", sapphire_window_logic, false, 0, false);

    /// Optical pad ///
    G4Tubs* optical_pad_solid =
      new G4Tubs("OPTICAL_PAD", 0., _hole_diam_front/2., _optical_pad_thickn/2., 0., twopi);

    G4LogicalVolume* optical_pad_logic =
      new G4LogicalVolume(optical_pad_solid, optical_coupler, "OPTICAL_PAD");

    G4double pad_posz = window_posz + _sapphire_window_thickn/2. +
      _optical_pad_thickn/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., pad_posz), optical_pad_logic,
     		      "OPTICAL_PAD", vacuum_logic, false, 0, false);

    /// PMT ///
    _pmt->Construct();
    G4LogicalVolume* pmt_logic = _pmt->GetLogicalVolume();
    G4double pmt_rel_posz = _pmt->GetRelPosition().z();
    _pmt_zpos = pad_posz + _optical_pad_thickn/2. + pmt_rel_posz;
    G4ThreeVector pmt_pos = G4ThreeVector(0., 0., _pmt_zpos);

    _pmt_rot = new G4RotationMatrix();
    _rot_angle = pi;
    _pmt_rot->rotateY(_rot_angle);
    new G4PVPlacement(G4Transform3D(*_pmt_rot, pmt_pos), pmt_logic,
		      "PMT", vacuum_logic, false, 0, false);

    /// Part of the PMT bases with pins and resistors ///
    G4Tubs* internal_pmt_base_solid =
      new G4Tubs("INT_PMT_BASE", 0., _internal_pmt_base_diam/2.,
		 _internal_pmt_base_thickn/2., 0., twopi);

    G4LogicalVolume* internal_pmt_base_logic =
      new G4LogicalVolume(internal_pmt_base_solid,
			  G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"),
			  "INT_PMT_BASE");
    G4double int_pmt_base_posz = _hole_length_front/2. + _hole_length_rear + _hut_hole_length/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., int_pmt_base_posz), internal_pmt_base_logic,
     		      "INT_PMT_BASE", vacuum_logic, false, 0, false);


    /// Placing the encapsulating volume with all internal components in place ///
    _vacuum_posz = _copper_plate_posz - _copper_plate_thickn/2  + _hole_length_front/2.;
    G4ThreeVector pos;
    for (int i=0; i<_num_PMTs; i++) {
      pos = _pmt_positions[i];
      pos.setZ(_vacuum_posz);
      new G4PVPlacement(0, pos, vacuum_logic, "HOLE", _mother_logic, false, i, false);
    }


    if (_verbosity) {
      G4cout << "Copper plate starts in z = " << _copper_plate_posz - _copper_plate_thickn/2.
	     << " mm and ends in z = " << _copper_plate_posz + _copper_plate_thickn/2.
	     << G4endl;
      G4cout << "Sapphire windows starts in z = "
	     << _vacuum_posz + window_posz - _sapphire_window_thickn/2.
	     << " mm and ends in z = " << _vacuum_posz + window_posz + _sapphire_window_thickn/2.
	     << G4endl;
    }

    //////////////////////
    ///  VISIBILITIES  ///
    //////////////////////

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
      G4VisAttributes pmt_base_col = Yellow();
      pmt_base_col.SetForceSolid(true);
      internal_pmt_base_logic->SetVisAttributes(pmt_base_col);
      //G4VisAttributes vacuum_col = Red();
      //vacuum_col.SetForceSolid(true);
      // vacuum_logic->SetVisAttributes(vacuum_col);
    } else {
      copper_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
      sapphire_window_logic->SetVisAttributes(G4VisAttributes::Invisible);
      optical_pad_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    //////////////////////////
    /// VERTEX GENERATORS  ///
    //////////////////////////

    G4double full_copper_length =
      _copper_plate_thickn + _hut_hole_length + _hut_length_long;
    _copper_gen =
      new CylinderPointSampler(_copper_plate_diam/2., full_copper_length, 0., 0.,
			       G4ThreeVector(0., 0., _copper_plate_posz +
					     _copper_plate_thickn/2. + _hut_hole_length +
					     _hut_length_long - full_copper_length/2.));

    //   _enclosure_flange_gen =
    //new CylinderPointSampler(_enclosure_window_diam/2., _enclosure_flange_length/2.,
    //			       _enclosure_diam/2.-_enclosure_window_diam/2., 0.,
    //		       G4ThreeVector(0., 0., _energy_plane_posz - _enclosure_flange_length/2.));

    _sapphire_window_gen =
      new CylinderPointSampler(_hole_diam_front/2., _sapphire_window_thickn, 0., 0.,
			       G4ThreeVector(0., 0., _vacuum_posz + window_posz));

    _optical_pad_gen =
      new CylinderPointSampler(_hole_diam_front/2., _optical_pad_thickn, 0., 0.,
			       G4ThreeVector(0., 0., _vacuum_posz + pad_posz));

    _internal_pmt_base_gen =
      new CylinderPointSampler(_internal_pmt_base_diam/2., _internal_pmt_base_thickn, 0., 0.,
			       G4ThreeVector(0.,0., _vacuum_posz + int_pmt_base_posz));

  }


  Next100EnergyPlane::~Next100EnergyPlane()
  {
    delete _copper_gen;
    //delete _enclosure_flange_gen;
    delete _sapphire_window_gen;
    delete _optical_pad_gen;
    delete _internal_pmt_base_gen;
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
    else  if (region == "PMT" || region == "PMT_BODY") {
      G4ThreeVector ini_vertex = _pmt->GenerateVertex(region);
      ini_vertex.rotate(_rot_angle, G4ThreeVector(0., 1., 0.));
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector pmt_pos = _pmt_positions[int(rand)];
      vertex = ini_vertex + pmt_pos;
      G4double z_translation = _vacuum_posz + _pmt_zpos;
      vertex.setZ(vertex.z() + z_translation);
    }

    // PMT bases - internal part
    else if (region == "INTERNAL_PMT_BASE") {
      vertex = _internal_pmt_base_gen->GenerateVertex("INSIDE");
      G4double rand = _num_PMTs * G4UniformRand();
      G4ThreeVector pmt_base_pos = _pmt_positions[int(rand)];
      vertex += pmt_base_pos;
    }

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
      G4double rad     = circle * x_pitch;
      G4double step    = 360.0/num_inner_pmts;
      for (G4int place=0; place<num_inner_pmts; place++) {
	G4double angle = place * step;
	position.setX(rad * cos(angle*deg));
	position.setY(rad * sin(angle*deg));
	_pmt_positions.push_back(position);
	//G4cout << position << G4endl;
	total_positions++;
      }

      for (G4int i=1; i<circle; i++) {
	G4double start_x = (circle-(i*0.5))*x_pitch;
	G4double start_y = i*y_pitch;
	rad  = std::sqrt(std::pow(start_x, 2) + std::pow(start_y, 2));
	G4double start_angle = std::atan2(start_y, start_x)/deg;
      	for (G4int place=0; place<num_inner_pmts; place++) {
	  G4double angle = start_angle + place * step;
	  position.setX(rad * cos(angle*deg));
	  position.setY(rad * sin(angle*deg));
	  _pmt_positions.push_back(position);
	  //G4cout << position << G4endl;
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

    // Checking
    if (total_positions != _num_PMTs) {
      G4cout << "\nERROR: Number of PMTs doesn't match with number of positions calculated\n";
      G4cout << "Number of PMTs = " << _num_PMTs << ", number of positions = "
	     << total_positions << G4endl;
      exit(0);
    }

  }


}
