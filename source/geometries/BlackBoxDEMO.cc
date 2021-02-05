// ----------------------------------------------------------------------------
//  nexus | BlackBoxSimple.cc
//  SensL KDB (no mask) in a black box.
//  Author:  Miryam Martínez Vara
// ----------------------------------------------------------------------------

#include "BlackBoxDEMO.h"
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
    world_z_ (3. * m),
    world_xy_ (2. *m),
    box_z_ (2. * m),
    box_xy_ (1. *m),

    visibility_(0)
  {
    msg_ = new G4GenericMessenger(this, "/Geometry/BlackBoxDEMO/",
				  "Control commands of BlackBoxDEMO.");
    msg_->DeclareProperty("visibility", visibility_, "Giant detectors visibility");

    G4GenericMessenger::Command&  specific_vertex_X_cmd =
      msg_->DeclareProperty("specific_vertex_X", specific_vertex_X_,
                            "If region is AD_HOC, x coord of primary particles");
    specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
    specific_vertex_X_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Y_cmd =
      msg_->DeclareProperty("specific_vertex_Y", specific_vertex_Y_,
                            "If region is AD_HOC, y coord of primary particles");
    specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
    specific_vertex_Y_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Z_cmd =
      msg_->DeclareProperty("specific_vertex_Z", specific_vertex_Z_,
                            "If region is AD_HOC, z coord of primary particles");
    specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
    specific_vertex_Z_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  dice_board_z_pos_cmd =
      msg_->DeclareProperty("dice_board_z_pos", dice_board_z_pos_,
                            "Distance between dice and photon source");
    dice_board_z_pos_cmd.SetParameterName("dice_board_z_pos", true);
    dice_board_z_pos_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  rotation_cmd =
      msg_->DeclareProperty("rotation", rotation_,
                            "Angle of rotation of the DICE");
    rotation_cmd.SetParameterName("rotation", true);
    rotation_cmd.SetUnitCategory("Angle");

    G4GenericMessenger::Command&  mask_thickn_cmd =
      msg_->DeclareProperty("mask_thickn", mask_thickn_,
                            "Mask Thickness");
    mask_thickn_cmd.SetParameterName("mask_thickn", true);
    mask_thickn_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  membrane_thickn_cmd =
      msg_->DeclareProperty("membrane_thickn", membrane_thickn_,
                            "Membrane Thickness");
    membrane_thickn_cmd.SetParameterName("membrane_thickn", true);
    membrane_thickn_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  coating_thickn_cmd =
      msg_->DeclareProperty("coating_thickn", coating_thickn_,
                            "Coating Thickness");
    coating_thickn_cmd.SetParameterName("coating_thickn", true);
    coating_thickn_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  hole_thickn_cmd =
      msg_->DeclareProperty("hole_thickn", hole_thickn_,
                            "Hole Thickness");
    hole_thickn_cmd.SetParameterName("hole_thickn", true);
    hole_thickn_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  hole_diameter_cmd =
      msg_->DeclareProperty("hole_diameter", hole_diameter_,
                            "Hole Diameter");
    hole_diameter_cmd.SetParameterName("hole_diameter", true);
    hole_diameter_cmd.SetUnitCategory("Length");
  }

  BlackBoxDEMO::~BlackBoxDEMO()
  {
    delete msg_;
  }

  void BlackBoxDEMO::Construct()
  {

  dice_ = new BlackBoxSiPMBoard();

  // WORLD /////////////////////////////////////////////////

  G4String world_name = "WORLD";

  G4Material* world_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  world_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

  G4Box* world_solid_vol =
    new G4Box(world_name, world_xy_/2., world_xy_/2., world_z_/2.);

  G4LogicalVolume* world_logic_vol =
    new G4LogicalVolume(world_solid_vol, world_mat, world_name);
  world_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
  BaseGeometry::SetLogicalVolume(world_logic_vol);

  // BLACK BOX /////////////////////////////////////////////////

  G4String box_name = "BLACK BOX";

  G4Material* box_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  box_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

  G4Box* box_solid_vol =
    new G4Box(box_name, box_xy_/2., box_xy_/2., box_z_/2.);

  G4LogicalVolume* box_logic_vol =
    new G4LogicalVolume(box_solid_vol, box_mat, box_name);
  box_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  G4VPhysicalVolume* box_phys_vol =
   new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),
                     box_logic_vol, box_name, world_logic_vol,
                     false, 0, false);

  // DICE BOARD //////////////////////////////////////////////

  G4String board_name = "DICE_BOARD";
  dice_->SetMotherPhysicalVolume(box_phys_vol);
  dice_->SetMaskThickness    (mask_thickn_);
  dice_->SetMembraneThickness(membrane_thickn_);
  dice_->SetCoatingThickness (coating_thickn_);
  dice_->SetHoleThickness    (hole_thickn_);
  dice_->SetHoleDiameter     (hole_diameter_);
  dice_->Construct();
  G4LogicalVolume* dice_board_logic = dice_->GetLogicalVolume();

  ////Dice Board placement
  dice_board_x_pos_ = 0 * cm;
  dice_board_y_pos_ = 0 * cm;
  G4ThreeVector post(dice_board_x_pos_,dice_board_y_pos_,dice_board_z_pos_);
  G4RotationMatrix* rot = new G4RotationMatrix();
  rot -> rotateY(rotation_);
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
      vertex = G4ThreeVector(specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_);
      return vertex;
    }
    else {
      G4Exception("[BlackBoxDEMO]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    G4ThreeVector displacement = G4ThreeVector(0., 0., 0.);
    vertex = vertex + displacement;

    return vertex;
  }

} // end namespace nexus
