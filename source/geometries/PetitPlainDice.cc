#include "PetitPlainDice.h"
#include "Visibilities.h"
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

  PetitPlainDice::PetitPlainDice():
    BaseGeometry(),
    rows_(8),
    columns_(8),
    visibility_ (1),
    xy_dim_(52*mm),
    refl_(0.)
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetitPlainDice/", "Control commands of geometry Petit.");
    msg_->DeclareProperty("reflectivity", refl_, "Reflectivity of dice boards");
    msg_->DeclareProperty("visibility", visibility_, "Dice Boards Visibility");
  }

  PetitPlainDice::~PetitPlainDice()
  {
  }

  // void PetitPlainDice::SetSize(G4double xsize, G4double ysize)
  // {
  // xsize_ = xsize;
  // ysize_ = ysize;
  // }

  void PetitPlainDice::Construct()
  {

    const G4double db_x =  xy_dim_;
    const G4double db_y =  xy_dim_;
    const G4double board_thickness = 0.3 * mm;

    SetDimensions(G4ThreeVector(db_x, db_y, board_thickness));

    // KAPTON BOARD /////////////////////////////////////////////////

    G4Box* board_solid = new G4Box("DICE_BOARD", db_x/2., db_y/2., board_thickness/2.);

    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
    //    G4Material* teflon =
    //G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

    G4LogicalVolume* board_logic =
      new G4LogicalVolume(board_solid, kapton, "DICE_BOARD");
    this->SetLogicalVolume(board_logic);

    // OPTICAL SURFACE FOR REFLECTION
    G4OpticalSurface* db_opsur = new G4OpticalSurface("DICE_BOARD");
    db_opsur->SetType(dielectric_metal);
    db_opsur->SetModel(unified);
    db_opsur->SetFinish(ground);
    db_opsur->SetSigmaAlpha(0.1);
    db_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::ReflectantSurface(refl_));
    new G4LogicalSkinSurface("DICE_BOARD", board_logic, db_opsur);

    // SETTING VISIBILITIES   //////////
    if (visibility_) {

      G4VisAttributes dice_col = nexus::CopperBrown();
      dice_col.SetForceSolid(true);
      board_logic->SetVisAttributes(dice_col);

    }
  }



} // end namespace nexus
