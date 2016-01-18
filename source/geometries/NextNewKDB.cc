// ---------------------------------------------------------------------------
//  $Id$
// 
//  Author:  <miquel.nebot@ific.uv.es>
//  Created: 18 Sept 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------

#include "NextNewKDB.h"

#include "SiPMSensl.h"
#include "PmtSD.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

#include <sstream>

namespace nexus {

  using namespace CLHEP;

  NextNewKDB::NextNewKDB(G4int rows, G4int columns): 
    BaseGeometry(),
    _rows(rows),
    _columns(columns),
    _visibility (1)
  {
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("kdb_vis", _visibility, "Kapton Dice Boards Visibility");

    _sipm = new SiPMSensl;
  }

  NextNewKDB::~NextNewKDB()
  {
  }
  

  void NextNewKDB::Construct()
  {
    // DIMENSIONS ///////////////////////////////////////////////////
    
    _sipm->Construct();
   
    const G4double sipm_pitch = 10. * mm;
    const G4double coating_thickness = 0.1 * micrometer;
    const G4double board_thickness = 0.3 * mm; // this is the real DB thickness
    const G4double board_side_reduction = .5 * mm;    
    const G4double db_x = _columns * sipm_pitch - 2. * board_side_reduction ;  
    const G4double db_y =    _rows * sipm_pitch - 2. * board_side_reduction ;
    //  const G4double db_z = board_thickness;
    const G4double db_z = _sipm->GetDimensions().z() + 1. * mm; // this is in order not to have volumes overlapping
   
    _dimensions.setX(db_x);
    _dimensions.setY(db_y);
    _dimensions.setZ(db_z);

    double vertex_displ = db_z/2. - board_thickness/2.;
    // Vertex generator
    _dice_gen = new BoxPointSampler(db_x, db_y, board_thickness, 0., G4ThreeVector(0., 0., -vertex_displ), 0);

    // KAPTON BOARD /////////////////////////////////////////////////

    G4Box* board_solid = new G4Box("DICE_BOARD", db_x/2., db_y/2., db_z/2.);
 
    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
 
    G4LogicalVolume* board_logic = 
      new G4LogicalVolume(board_solid, kapton, "DICE_BOARD");
    this->SetLogicalVolume(board_logic);

   
    // WLS COATING //////////////////////////////////////////////////

    G4Box* coating_solid = 
      new G4Box("DB_WLS_COATING", db_x/2., db_y/2., coating_thickness/2.);

    G4Material* TPB = MaterialsList::TPB();
    TPB->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());

    G4LogicalVolume* coating_logic =
      new G4LogicalVolume(coating_solid, TPB, "DB_WLS_COATING");

    G4double pos_z = (-db_z + coating_thickness) / 2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z), coating_logic,
		      "DB_WLS_COATING", board_logic, false, 0, false);


    // SILICON PMs //////////////////////////////////////////////////

    G4LogicalVolume* sipm_logic = _sipm->GetLogicalVolume();
    pos_z = -db_z/2. + coating_thickness + (_sipm->GetDimensions().z())/2.;
    G4double offset = sipm_pitch/2. - board_side_reduction;
    G4int sipm_no = 0;

    //    for (G4int i=0; i<_rows; i++) {
    for (G4int i=0; i<_columns; i++) {
      
      // G4double pos_y = db_y/2. - offset - i*sipm_pitch;
      G4double pos_x = db_x/2 - offset - i*sipm_pitch;
      
      //  for (G4int j=0; j<_columns; j++) {
      for (G4int j=0; j<_rows; j++) {
	
	//	G4double pos_x = -db_x/2 + offset + j*sipm_pitch;
	G4double pos_y = -db_y/2. + offset + j*sipm_pitch;

        new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z), 
          sipm_logic, "SIPMSensl", board_logic, false, sipm_no, false);
        std::pair<int, G4ThreeVector> mypos;
        mypos.first = sipm_no;
        mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
        _positions.push_back(mypos);
        sipm_no++;
      }
    }

    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes board_col = nexus::Yellow();
      board_logic->SetVisAttributes(board_col);
      G4VisAttributes tpb_col = nexus::DarkGreen();
      tpb_col.SetForceSolid(true);
      coating_logic->SetVisAttributes(tpb_col);      
    } else {
      board_logic->SetVisAttributes(G4VisAttributes::Invisible);
      coating_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    // VERTEX GENERATOR
  }

  G4ThreeVector NextNewKDB::GetDimensions() const
  {
    return _dimensions;
  }
  
  const std::vector<std::pair<int, G4ThreeVector> >& NextNewKDB::GetPositions()
  {
    return _positions;
  }

  G4ThreeVector NextNewKDB::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "DICE_BOARD") {
      vertex =_dice_gen->GenerateVertex("INSIDE");
    } else {
      G4Exception("[NextNewKDB]", "GenerateVertex()", FatalException,
                  "Unknown vertex generation region!");     
    }
    return vertex;
  }

} // end namespace nexus
