// ---------------------------------------------------------------------------
//  $Id$
// 
//  Author:  <miquel.nebot@ific.uv.es>
//  Created: 18 Sept 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------

#include "PetKDB.h"

#include "SiPMpet.h"
#include "PmtSD.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"

#include <G4Box.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
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

  PetKDB::PetKDB(G4int rows, G4int columns): 
    BaseGeometry(),
    _rows(rows),
    _columns(columns),
    _visibility (0)
  {
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/PetalX/", "Control commands of geometry Pet.");
    _msg->DeclareProperty("kdb_vis", _visibility, "Kapton Dice Boards Visibility");
  }

  PetKDB::~PetKDB()
  {
  }

  void PetKDB::Construct()
  {
   

    const G4double sipm_pitch = 10. * mm;
    const G4double coating_thickness = 0.1 * micrometer;
    const G4double board_thickness = 0.3 * mm;
    const G4double board_side_reduction = .5 * mm;    
    const G4double db_x = _columns * sipm_pitch - 2. * board_side_reduction ;  
    const G4double db_y =    _rows * sipm_pitch - 2. * board_side_reduction ;
    const G4double db_z = board_thickness;

    // Outer element volume  /////////////////////////////////////////////////// 
    const G4double border = .5*mm;
    const G4double out_x = db_x + 2.*border;
    const G4double out_y = db_y + 2.*border;
    const G4double out_z = db_z + 2.*border;

    _dimensions.setX(out_x);
    _dimensions.setY(out_y);
    _dimensions.setZ(out_z);

    G4cout << "DB whole volume = " << out_z << G4endl;

    G4Material* out_material = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");

    G4Box* out_solid = new G4Box("LXE_DICE", out_x/2., out_y/2., out_z/2.);
    G4LogicalVolume* out_logic = 
      new G4LogicalVolume(out_solid, out_material,  "LXE_DICE");
    this->SetLogicalVolume(out_logic);


    // KAPTON BOARD /////////////////////////////////////////////////

    G4Box* board_solid = new G4Box("DICE_BOARD", db_x/2., db_y/2., db_z/2.);
 
    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
 
    G4LogicalVolume* board_logic = 
      new G4LogicalVolume(board_solid, kapton, "DICE_BOARD");
    new G4PVPlacement(0, G4ThreeVector(0.,0., -border), board_logic,
			"DICE_BOARD", out_logic, false, 0, true);

   
    // WLS COATING //////////////////////////////////////////////////

    // G4Box* coating_solid = 
    //   new G4Box("DB_WLS_COATING", db_x/2., db_y/2., coating_thickness/2.);

    // G4Material* TPB = MaterialsList::TPB();
    // TPB->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());

    // G4LogicalVolume* coating_logic =
    //   new G4LogicalVolume(coating_solid, TPB, "DB_WLS_COATING");

    // G4double pos_z = -db_z/2. + coating_thickness / 2.;
    // new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z), coating_logic,
    // 		      "DB_WLS_COATING", board_logic, false, 0, true);


    // SILICON PMs //////////////////////////////////////////////////

    SiPMpet sipm;
    sipm.Construct();
    G4LogicalVolume* sipm_logic = sipm.GetLogicalVolume();

    G4double pos_z = db_z/2. - border+ (sipm.GetDimensions().z())/2.;
    G4double offset = sipm_pitch/2. - board_side_reduction;
    G4int sipm_no = 0;

    for (G4int i=0; i<_rows; i++) {

      G4double pos_y = db_y/2. - offset - i*sipm_pitch;

      for (G4int j=0; j<_columns; j++) {

        G4double pos_x = -db_x/2 + offset + j*sipm_pitch;

	G4cout << pos_x << ", " << pos_y << ", " << pos_z << G4endl;

        new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z), 
          sipm_logic, "SIPMpet", out_logic, false, sipm_no, true);

        std::pair<int, G4ThreeVector> mypos;
        mypos.first = sipm_no;
        mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
        _positions.push_back(mypos);
        sipm_no++;
      }
    }

    /// OPTICAL SURFACES ////////////////////////////////////////////

    // G4OpticalSurface* dboard_opsur = new G4OpticalSurface("DB");
    // dboard_opsur->SetType(dielectric_metal);
    // dboard_opsur->SetModel(unified);
    // dboard_opsur->SetFinish(ground);
    // dboard_opsur->SetSigmaAlpha(0.1);
    // dboard_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE_with_TPB());
    
    // new G4LogicalSkinSurface("DB", board_logic, dboard_opsur);

    // SETTING VISIBILITIES   //////////
    // _visibility  = true;
    //  if (_visibility) {
      G4VisAttributes silicon_col(G4Colour(1., 1., 0.));
      silicon_col.SetForceSolid(true);
      sipm_logic->SetVisAttributes(silicon_col);
      G4VisAttributes tpb_col(G4Colour(1., 1., 1.));
      tpb_col.SetForceSolid(true);
      //     coating_logic->SetVisAttributes(tpb_col);
      
    // }
    // else {
    //   coating_logic->SetVisAttributes(G4VisAttributes::Invisible);
    //   sipm_logic->SetVisAttributes(G4VisAttributes::Invisible);
    // }
  }

  G4ThreeVector PetKDB::GetDimensions() const
  {
    return _dimensions;
  }
  
  const std::vector<std::pair<int, G4ThreeVector> >& PetKDB::GetPositions()
  {
    return _positions;
  }

  // void PetKDB::SetMaterial(G4Material& mat)
  // {
  //   _out_mat = mat;
  // }


} // end namespace nexus
