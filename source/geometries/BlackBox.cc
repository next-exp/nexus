// ----------------------------------------------------------------------------
// nexus | BlackBox.cc
//
// Sapphire disk and DICE board with configurable mask thickness parallelly
// placed in a black box.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "BlackBox.h"
#include "BlackBoxSiPMBoard.h"
#include "FactoryBase.h"

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
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4RotationMatrix.hh>
#include <G4SystemOfUnits.hh>

namespace nexus {

  REGISTER_CLASS(BlackBox, GeometryBase)

  BlackBox::BlackBox():
    GeometryBase(),
    world_z_ (3. * m),
    world_xy_ (2. *m),
    box_z_ (2. * m),
    box_xy_ (1. *m),
    sapphire_(false),
    pedot_coating_(false),
    specific_vertex_(0,0,0),
    dice_board_pos_(0,0,-50.*cm),
    sapphire_pos_(0,0,-49.*cm),
    rotation_(0),
    mask_thickn_(0),
    hole_diameter_(1),
    hole_x_(1),
    hole_y_(1)
  {
    msg_ = new G4GenericMessenger(this, "/Geometry/BlackBox/",
				  "Control commands of BlackBox.");

    // Boolean-type properties (true or false)
    msg_->DeclareProperty("sapphire", sapphire_,
        "True if the sapphire window is going to included in the configuration.");
    msg_->DeclareProperty("pedot_coating", pedot_coating_,
        "True if the sapphire window is coated with PEDOT.");

    msg_->DeclarePropertyWithUnit("specific_vertex", "mm",  specific_vertex_,
      "Set generation vertex.");

    msg_->DeclarePropertyWithUnit("dice_board_pos", "cm",  dice_board_pos_,
      "Position of the DICE.");
    msg_->DeclarePropertyWithUnit("sapphire_pos", "cm",  sapphire_pos_,
      "Position of the sapphire window.");

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

    G4GenericMessenger::Command&  hole_diameter_cmd =
      msg_->DeclareProperty("hole_diameter", hole_diameter_,
                            "Hole Diameter");
    hole_diameter_cmd.SetParameterName("hole_diameter", true);
    hole_diameter_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  hole_x_cmd =
      msg_->DeclareProperty("hole_x", hole_x_,
                            "Hole X");
    hole_x_cmd.SetParameterName("hole_x", true);
    hole_x_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  hole_y_cmd =
      msg_->DeclareProperty("hole_y", hole_y_,
                            "Hole Y");
    hole_y_cmd.SetParameterName("hole_y", true);
    hole_y_cmd.SetUnitCategory("Length");

    dice_ = new BlackBoxSiPMBoard();
  }

  BlackBox::~BlackBox()
  {
    delete msg_;
  }

  void BlackBox::Construct()
  {

  // WORLD /////////////////////////////////////////////////

  G4String world_name = "WORLD";

  G4Material* world_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  world_mat->SetMaterialPropertiesTable(opticalprops::Vacuum());

  G4Box* world_solid_vol =
    new G4Box(world_name, world_xy_/2., world_xy_/2., world_z_/2.);

  G4LogicalVolume* world_logic_vol =
    new G4LogicalVolume(world_solid_vol, world_mat, world_name);
  world_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
  GeometryBase::SetLogicalVolume(world_logic_vol);

  // BLACK BOX /////////////////////////////////////////////////

  G4String box_name = "BLACK BOX";

  G4Box* box_solid_vol =
    new G4Box(box_name, box_xy_/2., box_xy_/2., box_z_/2.);

  G4LogicalVolume* box_logic_vol =
    new G4LogicalVolume(box_solid_vol, world_mat, box_name);
  box_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  G4VPhysicalVolume* box_phys_vol =
   new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),
                     box_logic_vol, box_name, world_logic_vol,
                     false, 0, false);

  // DICE BOARD //////////////////////////////////////////////

  dice_->SetMotherPhysicalVolume(box_phys_vol);
  dice_->SetMaskThickness    (mask_thickn_);
  dice_->SetHoleDiameter     (hole_diameter_);
  dice_->SetHoleX            (hole_x_);
  dice_->SetHoleY            (hole_y_);
  dice_->Construct();
  G4LogicalVolume* dice_board_logic = dice_->GetLogicalVolume();

  ////Dice Board placement
  G4RotationMatrix* rot = new G4RotationMatrix();
  rot -> rotateY(rotation_);
  new G4PVPlacement(rot, dice_board_pos_, dice_board_logic,
            	      dice_board_logic->GetName(), box_logic_vol, false, 1, false);

  // SAPPHIRE //////////////////////////////////////////////////

  if (sapphire_) {

    G4String sapphire_name = "SAPPHIRE";

    G4Material* sapphire_mat = materials::Sapphire();

    sapphire_mat->SetMaterialPropertiesTable(opticalprops::Sapphire());

    G4double pedot_thickn  = 150. *nm;
    G4double window_thickn  = 6. *mm;
    G4double sapphire_diam = 20 *cm;

    if (pedot_coating_) {
    window_thickn  += pedot_thickn;}

    G4Tubs* sapphire_solid_vol =
      new G4Tubs(sapphire_name, 0, sapphire_diam/2., window_thickn/2., 0, twopi);

    G4LogicalVolume* sapphire_logic_vol =
      new G4LogicalVolume(sapphire_solid_vol, sapphire_mat, sapphire_name);

    G4VisAttributes sapphire_col = nexus::LightBlue();
    sapphire_logic_vol->SetVisAttributes(sapphire_col);

    new G4PVPlacement(rot, sapphire_pos_,
                      sapphire_logic_vol, sapphire_name, box_logic_vol,
                      false, 0, false);

    //PEDOT ////////////////////////////////////////////////////////
    if (pedot_coating_) {
      G4String pedot_name = "PEDOT";

      G4Material* pedot_mat = materials::PEDOT();

      pedot_mat->SetMaterialPropertiesTable(opticalprops::PEDOT());

      G4Tubs* pedot_solid_vol =
        new G4Tubs(pedot_name, 0, sapphire_diam/2., pedot_thickn/2., 0, twopi);

        G4LogicalVolume* pedot_logic_vol =
        new G4LogicalVolume(pedot_solid_vol, pedot_mat, pedot_name);

        G4VisAttributes pedot_col = nexus::CopperBrown();
        pedot_logic_vol->SetVisAttributes(pedot_col);

        G4ThreeVector pedot_pos(0,0,window_thickn/2.- pedot_thickn/2.);

        new G4PVPlacement(0, pedot_pos,
                          pedot_logic_vol, pedot_name, sapphire_logic_vol,
                          false, 0, false);
    }
  }
  }

    G4ThreeVector BlackBox::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // WORLD
    if (region == "CENTER") {
      return vertex;
    }
    else if (region == "AD_HOC") {
      return specific_vertex_;
    }
    else {
      G4Exception("[BlackBox]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }
    return vertex;
  }

} // end namespace nexus
