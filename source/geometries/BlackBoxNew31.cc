// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  Miryam Martínez Vara <Miryam.Martinez@ific.uv.es>    
//  Created: 2 Oct 2020
//  
//  Copyright (c) 2020 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "BlackBoxNew31.h"
#include "NextNewKDB.h"

#include "BaseGeometry.h"
#include "SpherePointSampler.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4GenericMessenger.hh>
#include <G4Orb.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <Randomize.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4OpticalSurface.hh>
#include <G4RotationMatrix.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {
  
  using namespace CLHEP;
  
  BlackBoxNew31::BlackBoxNew31():
    _world_z (2. * m),
    _world_xy (1. *m),
    // SiPMs per Dice Board
    SiPM_rows_ (8),
    SiPM_columns_ (8),

    _visibility(0)
  {
    _msg = new G4GenericMessenger(this, "/Geometry/BlackBoxNew31/",
				  "Control commands of BlackBoxNew.");
    _msg->DeclareProperty("visibility", _visibility, "Giant detectors visibility");

    G4GenericMessenger::Command&  specific_vertex_X_cmd =
      _msg->DeclareProperty("specific_vertex_X", _specific_vertex_X,
                            "If region is AD_HOC, x coord of primary particles");
    specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
    specific_vertex_X_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Y_cmd =
      _msg->DeclareProperty("specific_vertex_Y", _specific_vertex_Y,
                            "If region is AD_HOC, y coord of primary particles");
    specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
    specific_vertex_Y_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Z_cmd =
      _msg->DeclareProperty("specific_vertex_Z", _specific_vertex_Z,
                            "If region is AD_HOC, z coord of primary particles");
    specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
    specific_vertex_Z_cmd.SetUnitCategory("Length");

    dice_ = new NextNewKDB(SiPM_rows_, SiPM_columns_);
  }
  
  
  
  BlackBoxNew31::~BlackBoxNew31()
  {
    delete _msg;
  }
    
  
  
  void BlackBoxNew31::Construct()
  {
  // WORLD /////////////////////////////////////////////////

  G4String world_name = "WORLD";
  
  G4Material* world_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  world_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

  G4Box* world_solid_vol =
    new G4Box(world_name, _world_xy/2., _world_xy/2., _world_z/2.);

  G4LogicalVolume* world_logic_vol =
    new G4LogicalVolume(world_solid_vol, world_mat, world_name);
  world_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
  BaseGeometry::SetLogicalVolume(world_logic_vol);
  
  // DB //////////////////////////////////////////////

  dice_->SetMotherLogicalVolume(world_logic_vol);
  dice_->Construct();
  kdb_dimensions_ = dice_->GetDimensions();
  G4LogicalVolume* dice_board_logic = dice_->GetLogicalVolume();
  G4double db_thickness =kdb_dimensions_.z();
  ////Dice Boards placement
  dice_board_x_pos_ = 0 * cm;  
  dice_board_y_pos_ = 0 * cm;
  //dice_board_z_pos_ = -80* cm;
  dice_board_z_pos_ = -31.* cm;
  G4ThreeVector post(dice_board_x_pos_,dice_board_y_pos_,dice_board_z_pos_);  
  G4RotationMatrix* rot = new G4RotationMatrix();
  rot -> rotateY(180*deg);

  //new G4PVPlacement(0, post, dice_board_logic,
	//            "DICE_BOARD", world_logic_vol, false, 0, false);
  new G4PVPlacement(rot, post, dice_board_logic,
	            "DICE_BOARD", world_logic_vol, false, 0, false);

  // VISIBILITIES ///////////////////////////////////////////////////
    
    //if (_visibility) {
      //detector_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
    //} 
  }

    G4ThreeVector BlackBoxNew31::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // WORLD
    if (region == "WORLD") {
 
      vertex = G4ThreeVector(0.,0.,0.*mm);
 
    }
    else if (region == "AD_HOC") {
      // AD_HOC does not need to be shifted because it is passed by the user
      vertex = G4ThreeVector(_specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z);
      return vertex;
    }
    else {
      G4Exception("[BlackBoxNew31]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    G4ThreeVector displacement = G4ThreeVector(0., 0., 0.); 
    vertex = vertex + displacement;

    return vertex;
  }




  G4OpticalSurface* BlackBoxNew31::GetPhotOptSurf() 
  {
	
  }
  

} // end namespace nexus
