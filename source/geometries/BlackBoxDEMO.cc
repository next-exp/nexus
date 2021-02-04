// ----------------------------------------------------------------------------
//  nexus | BlackBoxSimple.cc
//  SensL KDB (no mask) in a black box.
//  Author:  Miryam Martínez Vara
// ----------------------------------------------------------------------------

#include "BlackBoxDEMO.h"
//#include "KDB_Sensl.h"
#include "BlackBoxSiPMBoard.h"

#include "BaseGeometry.h"
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

  BlackBoxDEMO::BlackBoxDEMO():
    _world_z (3. * m),
    _world_xy (2. *m),
    _box_z (2. * m),
    _box_xy (1. *m),
    // SiPMs per Dice Board
    //SiPM_rows_ (8),
    //SiPM_columns_ (8),
    //mother_phys_     (nullptr),

    _visibility(0)
  {
    _msg = new G4GenericMessenger(this, "/Geometry/BlackBoxDEMO/",
				  "Control commands of BlackBoxDEMO.");
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

    G4GenericMessenger::Command&  dice_board_z_pos_cmd =
      _msg->DeclareProperty("dice_board_z_pos", _dice_board_z_pos,
                            "Distance between dice and photon source");
    dice_board_z_pos_cmd.SetParameterName("dice_board_z_pos", true);
    dice_board_z_pos_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  rotation_cmd =
      _msg->DeclareProperty("rotation", _rotation,
                            "Angle of rotation of the DICE");
    rotation_cmd.SetParameterName("rotation", true);
    rotation_cmd.SetUnitCategory("Angle");

    //dice_ = new KDB_Sensl(SiPM_rows_, SiPM_columns_);
  }

  BlackBoxDEMO::~BlackBoxDEMO()
  {
    delete _msg;
  }

  void BlackBoxDEMO::Construct()
  {
  dice_ = new BlackBoxSiPMBoard();
  //G4double mask_thickn     = 0.;
  //G4double membrane_thickn = 0.;
  //G4double coating_thickn  = 0.;
  //G4double hole_diameter   = 0.;
  G4double mask_thickn     = 2.0 * mm;
  G4double membrane_thickn = 0.0 * mm;
  G4double coating_thickn  = 0.0 * mm;
  G4double hole_diameter   = 3.5 * mm;
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

  // BLACK BOX /////////////////////////////////////////////////

  G4String box_name = "BLACK BOX";

  G4Material* box_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  box_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

  G4Box* box_solid_vol =
    new G4Box(box_name, _box_xy/2., _box_xy/2., _box_z/2.);

  G4LogicalVolume* box_logic_vol =
    new G4LogicalVolume(box_solid_vol, box_mat, box_name);
  box_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  G4VPhysicalVolume* box_phys_vol =
   new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),
                     box_logic_vol, box_name, world_logic_vol,
                     false, 0, false);

  // DB //////////////////////////////////////////////

  /// SiPM BOARDs
  G4String board_name = "DICE_BOARD";
  dice_->SetMotherPhysicalVolume(box_phys_vol);
  dice_->SetMaskThickness    (mask_thickn);
  dice_->SetMembraneThickness(membrane_thickn);
  dice_->SetCoatingThickness (coating_thickn);
  dice_->SetHoleDiameter     (hole_diameter);
  dice_->Construct();
  G4LogicalVolume* dice_board_logic = dice_->GetLogicalVolume();
  //board_size_                  = dice_->GetBoardSize();
  //G4double kapton_thickn       = dice_->GetKaptonThickness();

  // Placing the boards
  //G4double board_posz      = GetELzCoord() - gate_board_dist + board_size_.z()/2. - kapton_thickn;
  //GenerateBoardPositions(board_posz);
  //for (G4int i=0; i<num_boards_; i++)
  //  new G4PVPlacement(nullptr, board_pos_[i], board_logic, board_name,
    //                  mother_logic, false, i+14, true);
  /////////////////////////////////////////////////////

  //dice_->SetMotherLogicalVolume(world_logic_vol);
  //dice_->Construct();
  //kdb_dimensions_ = dice_->GetDimensions();
  //G4LogicalVolume* dice_board_logic = dice_->GetLogicalVolume();
  ////Dice Boards placement
  dice_board_x_pos_ = 0 * cm;
  dice_board_y_pos_ = 0 * cm;
  G4ThreeVector post(dice_board_x_pos_,dice_board_y_pos_,_dice_board_z_pos);
  G4RotationMatrix* rot = new G4RotationMatrix();
  rot -> rotateY(_rotation);
  //new G4PVPlacement(rot, post, dice_board_logic,
	  //          "DICE_BOARD", world_logic_vol, false, 0, false);
  new G4PVPlacement(rot, post, dice_board_logic,
            	      board_name, box_logic_vol, false, 0, false);
  /////////////////////////////////////////////////////////

  }

    G4ThreeVector BlackBoxDEMO::GenerateVertex(const G4String& region) const
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
      G4Exception("[BlackBoxSimple]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    G4ThreeVector displacement = G4ThreeVector(0., 0., 0.);
    vertex = vertex + displacement;

    return vertex;
  }

} // end namespace nexus
