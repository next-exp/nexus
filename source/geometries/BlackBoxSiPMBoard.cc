// -----------------------------------------------------------------------------
// nexus | BlackBoxSiPMBoard.cc
//
// SiPM Board used in BlackBox.
// -----------------------------------------------------------------------------

#include "BlackBoxSiPMBoard.h"

#include "MaterialsList.h"
#include "SiPMSensl.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4VisAttributes.hh>
#include <G4LogicalVolume.hh>
#include <G4SubtractionSolid.hh>
#include <G4PVPlacement.hh>
#include <G4RotationMatrix.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4SystemOfUnits.hh>

using namespace nexus;


BlackBoxSiPMBoard::BlackBoxSiPMBoard():
  GeometryBase     (),
  verbosity_       (true),
  num_columns_     (8),
  num_rows_        (8),
  num_sipms_       (num_rows_ * num_columns_),
  sipm_pitch_      (10.  * mm),
  side_reduction_  (0.5  * mm),
  kapton_thickn_   (0.3  * mm),
  mask_thickn_     (0),
  hole_diam_       (0),
  hole_x_          (0),
  hole_y_          (0),
  mother_phys_     (nullptr),
  kapton_gen_      (nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/BlackBox/",
                                "Control commands of the BlackBox geometry.");

  msg_->DeclareProperty("sipm_board_verbosity", verbosity_,
                        "BlackBoxSiPMBoard verbosity");

  sipm_ = new SiPMSensl;
}


BlackBoxSiPMBoard::~BlackBoxSiPMBoard()
{
  delete msg_;
  delete kapton_gen_;
  delete sipm_;
}


void BlackBoxSiPMBoard::Construct()
{

  /// Make sure the mother physical volume is actually valid
  if (!mother_phys_)
    G4Exception("[BlackBoxSiPMBoard]", "Construct()",
                FatalException, "Mother physical volume is a nullptr.");

  G4Material* mother_gas = mother_phys_->GetLogicalVolume()->GetMaterial();


  /// Board configuration checks
  // Masks require holes
  if (mask_thickn_ > 0.)
    if (hole_diam_ == 0. && hole_x_ ==0.)
      G4Exception("[BlackBoxSiPMBoard]", "Construct()", FatalException,
      "Masks require holes");

  sipm_->SetSensorDepth(2);
  sipm_->SetMotherDepth(3);
  sipm_->SetNamingOrder(1000);
  sipm_->Construct();

  /// Board-Wrapper volume that contains all other elements
  G4String board_name = "SIPM_BOARD";

  // Calculating board wrapper dimensions
  G4double board_size_x = num_columns_   * sipm_pitch_ - 2. * side_reduction_;
  G4double board_size_y = num_rows_      * sipm_pitch_ - 2. * side_reduction_;
  G4double board_size_z = kapton_thickn_ + std::max(sipm_->GetDimensions().z(),
                                                    mask_thickn_);

  board_size_ = G4ThreeVector(board_size_x, board_size_y, board_size_z);

  G4Box* board_solid = new G4Box(board_name, board_size_x/2.,
                                 board_size_y/2., board_size_z/2.);

  G4LogicalVolume* board_logic =
    new G4LogicalVolume(board_solid, mother_gas, board_name);

  GeometryBase::SetLogicalVolume(board_logic);

  /// Kapton
  G4String kapton_name = "KAPTON_BOARD";

  G4double kapton_posz = - board_size_z/2. + kapton_thickn_/2.;

  G4Box* kapton_solid = new G4Box(kapton_name, board_size_x/2.,
                                  board_size_y/2., kapton_thickn_/2.);

  G4Material* kapton_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");

  kapton_mat->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4LogicalVolume* kapton_logic =
    new G4LogicalVolume(kapton_solid, kapton_mat, kapton_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., kapton_posz), kapton_logic,
                    kapton_name, board_logic, false, 0, false);

  // Generate SiPM positions
  GenerateSiPMPositions();

  // SiPM placement.
  G4RotationMatrix* sipm_rot = new G4RotationMatrix();
  sipm_rot->rotateY(pi);
  G4double sipm_posz = - board_size_z/2. + kapton_thickn_ + sipm_->GetDimensions().z()/2.;
  G4ThreeVector sipm_pos;

  for (G4int sipm_id=0; sipm_id<num_sipms_; sipm_id++) {
    if (verbosity_){
       sipm_pos = sipm_positions_[sipm_id] + G4ThreeVector(0., 0., sipm_posz);
       G4cout << "SiPM" << sipm_id << ":" << sipm_pos << G4endl;
    }

    new G4PVPlacement(sipm_rot, sipm_pos, sipm_->GetLogicalVolume(),
                      sipm_->GetLogicalVolume()->GetName(), board_logic,
                      false, sipm_id, false);
  }

  /// Teflon Masks
  G4LogicalVolume* mask_logic;

  if (mask_thickn_!=0.){
    /// Create solid mask (no holes for sipms)
    G4String mask_name = "BOARD_MASK";

    G4double mask_posz = - board_size_z/2. + kapton_thickn_ + mask_thickn_/2.;

    G4Box* mask_solid = new G4Box(mask_name, board_size_x/2.,
                                board_size_y/2., mask_thickn_/2.);

    /// Define mask holes.
    G4VSolid* hole_solid = nullptr;

    if (hole_diam_!=0.){
      hole_solid = new G4Tubs("MASK_HOLE", 0., hole_diam_/2., mask_thickn_, 0., 360.*deg);}

    else {
      hole_solid = new G4Box("MASK_HOLE", hole_x_/2., hole_y_/2., mask_thickn_);}


    /// Create mask with holes.
    G4ThreeVector hole_pos = sipm_positions_[0];
    G4SubtractionSolid* mask_with_holes =
                        new G4SubtractionSolid("BOARD_MASK", mask_solid,
                                               hole_solid, 0, hole_pos);
    for (G4int sipm_id=1; sipm_id<num_sipms_; sipm_id++) {
         hole_pos = sipm_positions_[sipm_id];

         mask_with_holes = new G4SubtractionSolid("BOARD_MASK", mask_with_holes,
                                                  hole_solid, 0, hole_pos);
    }
    mask_logic =
      new G4LogicalVolume(mask_with_holes, G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
                          mask_name);

    // Adding the optical surface
    G4OpticalSurface* mask_opsurf =
      new G4OpticalSurface(mask_name, unified, ground, dielectric_metal);
    mask_opsurf->SetMaterialPropertiesTable(opticalprops::PTFE());
    new G4LogicalSkinSurface(mask_name + "_OPSURF", mask_logic, mask_opsurf);

    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., mask_posz), mask_logic,
                      mask_name, board_logic, false, 0, false);

  }


  /// VERTEX GENERATOR
  kapton_gen_ = new BoxPointSampler(board_size_x, board_size_y, kapton_thickn_, 0.,
                                    G4ThreeVector(0., 0., -board_size_z/2. + kapton_thickn_/2.),
                                    nullptr);


  /// VERBOSITY
  if (verbosity_){
    G4cout << "* SiPM board size:    " << board_size_      << G4endl;
    G4cout << "* " << num_sipms_ << " SiPMs from Sensl"    << G4endl;
    G4cout << "* Kapton thickness:   " << kapton_thickn_   << G4endl;
    G4cout << "* Mask thickness:     " << mask_thickn_     << G4endl;
    G4cout << "* Mask hole diameter: " << hole_diam_       << G4endl;}


  /// VISIBILITIES
  board_logic  ->SetVisAttributes(Red());
  kapton_logic ->SetVisAttributes(Blue());
  if (mask_thickn_!=0){
    mask_logic   ->SetVisAttributes(LightBlue());
  }
}




G4ThreeVector BlackBoxSiPMBoard::GenerateVertex(const G4String&) const
{
  // Only one generation region available at the moment
  return kapton_gen_->GenerateVertex("INSIDE");
}

void BlackBoxSiPMBoard::GenerateSiPMPositions()
{
  G4double margin    = sipm_pitch_/2. - side_reduction_;

  for (auto i=0; i<8; i++) {
    G4double sipm_posy = -board_size_.y()/2. + margin + i * sipm_pitch_;

    for (auto j=0; j<8; j++) {
      G4double sipm_posx = +board_size_.x()/2. - margin - j * sipm_pitch_;

      sipm_positions_.push_back(G4ThreeVector(sipm_posx, sipm_posy, 0.));
    }
  }
}
