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
#include <G4Tubs.hh>
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
    _visibility (1),
    _teflon_masks(true)
  {
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/KDB/", "Control commands of kapton dice board geometry.");
    _msg->DeclareProperty("visibility", _visibility, "Kapton Dice Boards Visibility");
    _msg->DeclareProperty("teflon_masks", _teflon_masks, "True if teflon masks are placed in front of dices");
    _sipm = new SiPMSensl;
  }



  NextNewKDB::~NextNewKDB()
  {
    delete _msg;
    delete _dice_gen;
  }
  

  void NextNewKDB::SetMotherLogicalVolume(G4LogicalVolume* mother_logic) {
    _mother_logic = mother_logic;
  }


  void NextNewKDB::Construct()
  {
    _sipm->Construct();
   
    /// DIMENSIONS ///

    const G4double sipm_pitch           = 10. * mm;
    const G4double board_thickness      = 0.3 * mm; // this is the real DB thickness
    const G4double board_side_reduction = 0.5 * mm;
    const G4double offset               = sipm_pitch/2. - board_side_reduction;

    const G4double db_x = _columns * sipm_pitch - 2. * board_side_reduction ;  
    const G4double db_y =    _rows * sipm_pitch - 2. * board_side_reduction ;
    const G4double db_z = board_thickness;
    
    //G4cout << "* Kapton Dice board dimensions: "  << db_x << " , "  
    //       << db_y << " , "  << db_z << G4endl;

    const G4double mask_x = db_x;
    const G4double mask_y = db_y;
    const G4double mask_z = 2. * mm;

    const G4double db_case_x = db_x;
    const G4double db_case_y = db_y;
    const G4double db_case_z = db_z + mask_z;

    _dimensions.setX(db_case_x);
    _dimensions.setY(db_case_y);
    _dimensions.setZ(db_case_z);


    /// KAPTON BOARD ///  This is the volume that contains everything
    G4Box* board_solid = new G4Box("DICE_BOARD", db_case_x/2., db_case_y/2., db_case_z/2.);
 
    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
 
    G4LogicalVolume* board_logic = 
      new G4LogicalVolume(board_solid, kapton, "DICE_BOARD");

    this->SetLogicalVolume(board_logic);


    /// TEFLON MASK ///
    // It is always built despite the "_teflon_masks" parameter
    G4double mask_pos_z = - db_case_z / 2. + mask_z / 2.;

    G4Box* mask_solid = new G4Box("DICE_MASK", mask_x/2., mask_y/2., mask_z/2.);
 
    G4Material* teflon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

    G4LogicalVolume* mask_logic = 
      new G4LogicalVolume(mask_solid, teflon, "DICE_MASK");

    // Adding the optical surface
    G4OpticalSurface* mask_optSurf = 
      new G4OpticalSurface("DICE_MASK_OptSurf", unified, ground, dielectric_metal);
    mask_optSurf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
    new G4LogicalSkinSurface("DICE_MASK_OptSurf", mask_logic, mask_optSurf);


    /// Adding the SiPMs to the Dice Board. ///
    // They are placed in a gas cylinder emulating the mask holes
    G4String hole_name   = "SiPM_hole";
    G4double hole_diam   = 3.5 * mm;
    G4double hole_length = mask_z;

    G4Tubs* hole_solid =
      new G4Tubs(hole_name, 0., hole_diam/2., hole_length/2., 0, twopi);

    G4LogicalVolume* hole_logic = 
      new G4LogicalVolume(hole_solid, _mother_logic->GetMaterial(), hole_name);

    // Placing the SiPMs inside the hole.
    G4LogicalVolume* sipm_logic = _sipm->GetLogicalVolume();
    G4double         sipm_pos_z = hole_length/2. - _sipm->GetDimensions().z() / 2.;

    new G4PVPlacement(0, G4ThreeVector(0., 0., sipm_pos_z), sipm_logic,
                      "SIPMSensl", hole_logic, false, 0, false);

    G4int sipm_no = 0;
    for (G4int i=0; i<_columns; i++) {
      G4double pos_x = db_x/2 - offset - i * sipm_pitch;
      for (G4int j=0; j<_rows; j++) {
        G4double pos_y = -db_y/2. + offset + j * sipm_pitch;

        new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, 0), hole_logic,
                          hole_name, mask_logic, false, sipm_no, false);

        std::pair<int, G4ThreeVector> mypos;
        mypos.first = sipm_no;
        mypos.second = G4ThreeVector(pos_x, pos_y, 0.);
        _positions.push_back(mypos);
        //G4cout << "  SiPM " << sipm_no << " : (" << pos_x << ", " << pos_y 
        //       << ", " << pos_z << " )" << G4endl;
        sipm_no++;
      }
    }

    // Placing the Dice Teflon Mask
    new G4PVPlacement(0, G4ThreeVector(0, 0., mask_pos_z), mask_logic,
                      "DICE_MASK", board_logic, false, 0, false);


    // SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes board_col = nexus::Yellow();
      board_logic->SetVisAttributes(board_col);
      mask_logic->SetVisAttributes(G4VisAttributes::Invisible);
      hole_logic->SetVisAttributes(G4VisAttributes::Invisible);

    } else {
      board_logic->SetVisAttributes(G4VisAttributes::Invisible);
      mask_logic->SetVisAttributes(G4VisAttributes::Invisible);
      hole_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    // VERTEX GENERATOR
    double vertex_displ = db_case_z/2. - board_thickness/2.;
    _dice_gen = new BoxPointSampler(db_x, db_y, db_z, 0.,
                                    G4ThreeVector(0., 0., vertex_displ), 0);
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
