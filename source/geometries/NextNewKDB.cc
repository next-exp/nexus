// ---------------------------------------------------------------------------
//  $Id: 
//  Author:  <miquel.nebot@ific.uv.es>
//  Created: 18 Sept 2013
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------

#include "NextNewKDB.h"

#include "PmtSD.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"

#include <G4Box.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

#include <sstream>

namespace nexus{

  NextNewKDB::NextNewKDB(G4int rows, G4int columns):
    BaseGeometry(),
    _rows(rows),
    _columns(columns)
  {
    //The SiPM
    _siPM = new SiPM11();
  }
  NextNewKDB::~NextNewKDB()
  {
    delete _siPM;
  }

  void NextNewKDB::Construct()
  {
    ///KAPTON DICE BOARD (KDB)//////
    const G4double sipm_pitch = 10. * mm;
    const G4double coating_thickness = 0.1 * micrometer;
    const G4double board_thickness = 0.3 * mm;
    const G4double board_side_reduction = .5 * mm;    
    const G4double db_x = _columns * sipm_pitch - 2. * board_side_reduction ;  
    const G4double db_y =    _rows * sipm_pitch - 2. * board_side_reduction ;
    const G4double db_z = board_thickness;
    _dimensions.setX(db_x);
    _dimensions.setY(db_y);
    _dimensions.setZ(db_z);

    G4Box* board_solid = new G4Box("DICE_BOARD", db_x/2., db_y/2., db_z/2.);
    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
    G4LogicalVolume* board_logic = 
      new G4LogicalVolume(board_solid, kapton, "DICE_BOARD");
    this->SetLogicalVolume(board_logic);

    /// WLS COATING ////////////////
    G4Box* coating_solid = 
      new G4Box("DB_WLS_COATING", db_x/2., db_y/2., coating_thickness/2.);
    G4Material* TPB = MaterialsList::TPB();
    TPB->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
    G4LogicalVolume* coating_logic =
      new G4LogicalVolume(coating_solid, TPB, "DB_WLS_COATING");
    G4double pos_z = (-db_z + coating_thickness) / 2.;//???????????????
    new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z), coating_logic,
		      "DB_WLS_COATING", board_logic, false, 0, false);

    // SILICON PMs /////////////////?????????????
    _siPM->Construct();
    G4LogicalVolume* sipm_logic = _siPM->GetLogicalVolume();
    pos_z = -db_z/2 + coating_thickness + (_siPM->GetDimensions().z())/2.;
    G4double offset = sipm_pitch/2. - board_side_reduction;
    G4int sipm_no = 0;
    for (G4int i=0; i<_rows; i++) {
      G4double pos_y = db_y/2. - offset - i*sipm_pitch;
      for (G4int j=0; j<_columns; j++) {
	G4double pos_x = -db_x/2 + offset + j*sipm_pitch;
	new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z), sipm_logic,
			  "SIPM11", board_logic, false, sipm_no, false);
	std::pair<int, G4ThreeVector> mypos;
	mypos.first = sipm_no;
	mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
	_positions.push_back(mypos);
	sipm_no++;
      }
    }

    /// Optical surfaces ?????????
    G4OpticalSurface* dboard_opsur = new G4OpticalSurface("DB");
    dboard_opsur->SetType(dielectric_metal);
    dboard_opsur->SetModel(unified);
    dboard_opsur->SetFinish(ground);
    dboard_opsur->SetSigmaAlpha(0.1);
    dboard_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE_with_TPB());
    new G4LogicalSkinSurface("DB", board_logic, dboard_opsur);


    // Visibilities
    //coating_logic->SetVisAttributes(G4VisAttributes::Invisible);
    //sipm_logic->SetVisAttributes(G4VisAttributes::Invisible);
    // G4VisAttributes * vis = new G4VisAttributes;
    // vis->SetColor(1., 0., 0.);
    // coating_logic->SetVisAttributes(vis);
    // vis->SetColor(0., 1., 0.);
    // sipm_logic->SetVisAttributes(vis);

  }

  G4ThreeVector NextNewKDB::GetDimensions()
  {
    return _dimensions;
  }
  
  std::vector<std::pair<int, G4ThreeVector> > NextNewKDB::GetPositions()
  {
    return _positions;
  }
} // end namespace nexus
