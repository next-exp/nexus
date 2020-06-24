// ----------------------------------------------------------------------------
// nexus | PetPlainDice.cc
//
// Kapton non-instrumented dice board.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PetPlainDice.h"
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
#include <G4Material.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

#include <sstream>

namespace nexus {

  using namespace CLHEP;

  PetPlainDice::PetPlainDice():
    BaseGeometry(),
    rows_(8),
    columns_(8),
    visibility_ (1),
    ysize_(5.*cm),
    xsize_(5.*cm),
    xy_size_(5. * cm),
    refl_(0.95)
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetPlainDice/", "Control commands of geometry Pet.");
    // xy size
    G4GenericMessenger::Command& xysize_cmd =
      msg_->DeclareProperty("xy_size", xy_size_, "xy dimension");
    xysize_cmd.SetUnitCategory("Length");
    xysize_cmd.SetParameterName("xy_size", false);
    xysize_cmd.SetRange("xy_size>0.");

    msg_->DeclareProperty("reflectivity", refl_, "Reflectivity of dice boards");
    msg_->DeclareProperty("visibility", visibility_, "Dice Boards Visibility");
  }

  PetPlainDice::~PetPlainDice()
  {
  }

  void PetPlainDice::SetSize(G4double xsize, G4double ysize)
  {
  xsize_ = xsize;
  ysize_ = ysize;
  }

  void PetPlainDice::Construct()
  {
    xsize_ = xy_size_;
    ysize_ = xy_size_;

    //   const G4double sipm_pitch = 6.2 * mm;
    const G4double board_thickness = 0.3 * mm;
    /*
     G4double sipm_pitch = ysize_/rows_;
    const G4double coating_thickness = 0.1 * micrometer;

    //const G4double board_side_reduction = .5 * mm;
    const G4double board_side_reduction = 0. * mm;
    // const G4double db_x = columns_ * sipm_pitch - 2. * board_side_reduction ;
    // const G4double db_y =    rows_ * sipm_pitch - 2. * board_side_reduction ;
    */
     const G4double db_x = xsize_ ;
     //    const G4double db_y =    rows_ * sipm_pitch - 2. * board_side_reduction ;
     const G4double db_y = ysize_;
     const G4double db_z = board_thickness;

    // Outer element volume  ///////////////////////////////////////////////////
     //  const G4double border = .5*mm;
    const G4double out_x = db_x ;
    const G4double out_y = db_y;
    const G4double out_z = db_z;

    dimensions_.setX(out_x);
    dimensions_.setY(out_y);
    dimensions_.setZ(out_z);

    // G4Material* out_material = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");

    // G4Box* out_solid = new G4Box("LXE_DICE", out_x/2., out_y/2., out_z/2.);
    // G4LogicalVolume* out_logic =
    //   new G4LogicalVolume(out_solid, out_material,  "LXE_DICE");



    // KAPTON BOARD /////////////////////////////////////////////////

    G4Box* board_solid = new G4Box("DICE_BOARD", db_x/2., db_y/2., db_z/2.);

    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
    //    G4Material* teflon =
    //G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

    G4LogicalVolume* board_logic =
      new G4LogicalVolume(board_solid, kapton, "DICE_BOARD");
    this->SetLogicalVolume(board_logic);
    // new G4PVPlacement(0, G4ThreeVector(0.,0., -border), board_logic,
    // 			"DICE_BOARD", out_logic, false, 0, true);

    // OPTICAL SURFACE FOR REFLECTION
    G4OpticalSurface* db_opsur = new G4OpticalSurface("DICE_BOARD");
    db_opsur->SetType(dielectric_metal);
    db_opsur->SetModel(unified);
    db_opsur->SetFinish(ground);
    db_opsur->SetSigmaAlpha(0.1);

    // G4cout << refl_ << G4endl;
    // db_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE_with_TPB());
    db_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE_LXe(refl_));

    new G4LogicalSkinSurface("DICE_BOARD", board_logic, db_opsur);


    // WLS COATING //////////////////////////////////////////////////
    /*
    G4Box* coating_solid =
      new G4Box("DB_WLS_COATING", db_x/2., db_y/2., coating_thickness/2.);

    G4Material* TPB = MaterialsList::TPB();
    TPB->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB_LXe());

    G4LogicalVolume* coating_logic =
      new G4LogicalVolume(coating_solid, TPB, "DB_WLS_COATING");

    G4double pos_z = db_z/2. - coating_thickness / 2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., pos_z), coating_logic,
    		      "DB_WLS_COATING", board_logic, false, 0, true);
    */



    // SETTING VISIBILITIES   //////////
    if (visibility_) {

      G4VisAttributes dice_col = nexus::CopperBrown();
      dice_col.SetForceSolid(true);
      board_logic->SetVisAttributes(dice_col);

    }
    else {
      //   coating_logic->SetVisAttributes(G4VisAttributes::Invisible);
      //     sipm_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  G4ThreeVector PetPlainDice::GetDimensions() const
  {
    return dimensions_;
  }

  const std::vector<std::pair<int, G4ThreeVector> >& PetPlainDice::GetPositions()
  {
    return positions_;
  }



} // end namespace nexus
