// -----------------------------------------------------------------------------
// nexus | Next100SiPMBoard.cc
//
// Geometry of the NEXT-100 SiPM board, consisting of an 8x8 array of
// silicon photomultipliers (1.3x1.3 mm2 of active area) mounted on a Kapton
// board covered with a TPB-coated teflon mask.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "Next100SiPMBoard.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"
#include "Visibilities.h"
#include "Next100SiPM.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UnionSolid.hh>

using namespace nexus;


Next100SiPMBoard::Next100SiPMBoard():
  GeometryBase     (),
  size_            (122.40  * mm),
  pitch_           ( 15.55  * mm),
  margin_          (  6.775 * mm),
  board_thickness_ (  0.2   * mm),
  mask_thickness_  (  6.0   * mm),
  time_binning_    (1. * microsecond),
  visibility_      (true),
  sipm_visibility_ (false),
  mpv_             (nullptr),
  vtxgen_          (nullptr),
  sipm_            (new Next100SiPM())
{
  msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
                                "Control commands of the NEXT-100 geometry.");

  msg_->DeclareProperty("sipm_board_vis", visibility_, "Visibility of Next100SiPMBoard.");

  msg_->DeclareProperty("sipm_vis", sipm_visibility_, "Visibility of Next100 SiPMs.");

  G4GenericMessenger::Command& time_binning_cmd =
  msg_->DeclareProperty("sipm_time_binning", time_binning_,
                        "TP SiPMs time binning.");
  time_binning_cmd.SetParameterName("sipm_time_binning", false);
  time_binning_cmd.SetUnitCategory("Time");
  time_binning_cmd.SetRange("sipm_time_binning>0.");
}


Next100SiPMBoard::~Next100SiPMBoard()
{
  delete msg_;
  delete vtxgen_;
  delete sipm_;
}


void Next100SiPMBoard::Construct()
{
  // Make sure the mother physical volume is actually valid
  if (!mpv_)
    G4Exception("[Next100SiPMBoard]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  G4Material* mother_gas = mpv_->GetLogicalVolume()->GetMaterial();


  // KAPTON BOARD ////////////////////////////////////////////////////
  // Wrapper volume that contains all other elements.

  G4String board_name = "SIPM_BOARD";

  G4Box* board_solid_vol =
    new G4Box(board_name, size_/2., size_/2., (board_thickness_ + mask_thickness_)/2.);


  G4Material* kapton = G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
  // In Geant4 11.0.0, a bug in treating the OpBoundaryProcess produced
  // in the surface makes the code fail. This is avoided by setting an
  // empty G4MaterialPropertiesTable of the G4Material.
  kapton->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4LogicalVolume* board_logic_vol =
    new G4LogicalVolume(board_solid_vol, kapton, board_name);

  GeometryBase::SetLogicalVolume(board_logic_vol);


  // TEFLON MASK /////////////////////////////////////////////////////

  G4String mask_name = "SIPM_BOARD_MASK";
  G4double mask_zpos = board_thickness_/2.;

  G4Box* mask_solid_vol =
    new G4Box(mask_name, size_/2., size_/2., mask_thickness_/2.);

  G4Material* teflon = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  // teflon is the material used in the sipm-board masks which are covered by
  // a G4LogicalSkinSurface.
  // In Geant4 11.0.0, a bug in treating the OpBoundaryProcess produced in the
  // surface makes the code fail. This is avoided by setting an empty
  // G4MaterialPropertiesTable of the G4Material.
  teflon->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4LogicalVolume* mask_logic_vol =
      new G4LogicalVolume(mask_solid_vol, teflon, mask_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., mask_zpos),
                    mask_logic_vol, mask_name, board_logic_vol, false, 0, false);

  G4OpticalSurface* mask_opsurf =
    new G4OpticalSurface(mask_name+"_OPSURF", unified, ground, dielectric_metal);
  mask_opsurf->SetMaterialPropertiesTable(opticalprops::PTFE());
  new G4LogicalSkinSurface(mask_name+"_OPSURF", mask_logic_vol, mask_opsurf);


  // WLS COATING /////////////////////////////////////////////////////

  G4String mask_wls_name = "SIPM_BOARD_MASK_WLS";
  G4double wls_thickness = 1. * um;
  G4double mask_wls_zpos = mask_thickness_/2. - wls_thickness/2.;

  G4Box* mask_wls_solid_vol =
    new G4Box(mask_wls_name, size_/2., size_/2., wls_thickness/2.);

  G4Material* tpb = materials::TPB();
  tpb->SetMaterialPropertiesTable(opticalprops::TPB());

  G4LogicalVolume* mask_wls_logic_vol =
    new G4LogicalVolume(mask_wls_solid_vol, tpb, mask_wls_name);

  G4VPhysicalVolume* mask_wls_phys_vol =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., mask_wls_zpos),
                      mask_wls_logic_vol, mask_wls_name, mask_logic_vol, false, 0, false);

  G4OpticalSurface* mask_wls_opsurf =
    new G4OpticalSurface(mask_wls_name+"_OPSURF",
                         glisur, ground, dielectric_dielectric, .01);
  new G4LogicalBorderSurface(mask_wls_name+"_OPSURF",
                             mask_wls_phys_vol, mpv_, mask_wls_opsurf);
  new G4LogicalBorderSurface(mask_wls_name+"_OPSURF",
                             mpv_, mask_wls_phys_vol, mask_wls_opsurf);


  // MASK GAS HOLE ///////////////////////////////////////////////////

  G4String mask_hole_name   = "SIPM_BOARD_MASK_HOLE";
  G4double mask_hole_length = mask_thickness_ - wls_thickness;
  G4double mask_hole_zpos   = - mask_thickness_/2. + mask_hole_length/2.;
  G4double mask_hole_x = 6.0 * mm;
  G4double mask_hole_y = 5.0 * mm;

  G4Box* mask_hole_solid_vol =
    new G4Box(mask_hole_name, mask_hole_x/2., mask_hole_y/2., mask_hole_length/2.);

  G4LogicalVolume* mask_hole_logic_vol =
    new G4LogicalVolume(mask_hole_solid_vol, mother_gas, mask_hole_name);

  // (Placement of this volume below.)

  // HOLE WALLs WLS ///////////////////////////////////////////////////

  G4String mask_wall_wls_name = "SIPM_BOARD_MASK_WALL_WLS";

  G4Box* latwall_wls_solid_vol =
    new G4Box(mask_wall_wls_name, wls_thickness/2., mask_hole_y/2.,
              mask_hole_length/2.);

  G4Box* uppwall_wls_solid_vol =
      new G4Box(mask_wall_wls_name, mask_hole_x/2., wls_thickness/2.,  mask_hole_length/2.);

  G4UnionSolid* wall_wls_solid_vol =
    new G4UnionSolid(mask_wall_wls_name, latwall_wls_solid_vol,
                     uppwall_wls_solid_vol, 0,
                     G4ThreeVector(mask_hole_x/2.-wls_thickness/2.,
                                   -mask_hole_y/2. + wls_thickness/2., 0.));

  wall_wls_solid_vol =
    new G4UnionSolid(mask_wall_wls_name, wall_wls_solid_vol,
                     uppwall_wls_solid_vol, 0,
                     G4ThreeVector(mask_hole_x/2.-wls_thickness/2.,
                                   mask_hole_y/2. - wls_thickness/2., 0.));

  wall_wls_solid_vol =
    new G4UnionSolid(mask_wall_wls_name, wall_wls_solid_vol,
                     latwall_wls_solid_vol, 0,
                     G4ThreeVector(mask_hole_x-wls_thickness, 0., 0.));

  G4LogicalVolume* wall_wls_logic_vol =
    new G4LogicalVolume(wall_wls_solid_vol, tpb, mask_wall_wls_name);

  G4VPhysicalVolume* wall_wls_phys_vol =
    new G4PVPlacement(nullptr, G4ThreeVector(-mask_hole_x/2. + wls_thickness/2., 0., 0.),
                      wall_wls_logic_vol, mask_wall_wls_name, mask_hole_logic_vol, false, 0, false);

  // MASK WLS GAS HOLE ///////////////////////////////////////////////

  G4String mask_wls_hole_name = "SIPM_BOARD_MASK_WLS_HOLE";

  G4Box* mask_wls_hole_solid_vol =
    new G4Box(mask_wls_hole_name, mask_hole_x/2., mask_hole_y/2., wls_thickness/2.);

  G4LogicalVolume* mask_wls_hole_logic_vol =
    new G4LogicalVolume(mask_wls_hole_solid_vol, mother_gas, mask_wls_hole_name);

  // (Placement of this volume below.)

  // SILICON PHOTOMULTIPLIER (SIPM) //////////////////////////////////

  sipm_->SetVisibility(sipm_visibility_);
  sipm_->SetSiPMCoatingThickness(2. * micrometer);
  sipm_->SetTimeBinning(time_binning_);
  sipm_->SetSensorDepth(2);
  sipm_->SetMotherDepth(4);
  sipm_->SetNamingOrder(1000);
  sipm_->Construct();

  G4double sipm_thickn = sipm_->GetDimensions().z();

  G4double sipm_zpos = - mask_hole_length/2. + sipm_thickn/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., sipm_zpos),
                    sipm_->GetLogicalVolume(), sipm_->GetLogicalVolume()->GetName(),
                    mask_hole_logic_vol, false, 0, false);

  ////////////////////////////////////////////////////////////////////

  // Placing now 8x8 replicas of the gas hole and SiPM

  G4double zpos = board_thickness_ + sipm_thickn/2.;
  G4VPhysicalVolume* mask_hole_phys_vol;

  G4int counter = 0;

  for (auto i=0; i<8; i++) {

    G4double xpos = -size_/2. + margin_ + i * pitch_;

    for (auto j=0; j<8; j++) {

      G4double ypos = -size_/2. + margin_ + j * pitch_;

      G4ThreeVector sipm_position(xpos, ypos, zpos);
      sipm_positions_.push_back(sipm_position);

      // Placement of the WLS gas hole
      new G4PVPlacement(nullptr, G4ThreeVector(xpos, ypos, 0.),
                        mask_wls_hole_logic_vol, mask_wls_hole_name,
                        mask_wls_logic_vol, false, counter, false);
      // Placement of the hole+SiPM
      mask_hole_phys_vol =
        new G4PVPlacement(nullptr, G4ThreeVector(xpos, ypos, mask_hole_zpos),
                          mask_hole_logic_vol, mask_hole_name, mask_logic_vol,
                          false, counter, false);

      new G4LogicalBorderSurface(mask_wall_wls_name+"_OPSURF",
                                 mask_hole_phys_vol, wall_wls_phys_vol,
                                 mask_wls_opsurf);
      new G4LogicalBorderSurface(mask_wls_name+"_OPSURF",
                                 wall_wls_phys_vol, mask_hole_phys_vol,
                                 mask_wls_opsurf);

      counter++;
    }
  }

  // VERTEX GENERATOR ////////////////////////////////////////////////

  vtxgen_ = new BoxPointSampler(size_/2., size_/2., (board_thickness_+mask_thickness_)/2.,
                                0., G4ThreeVector(0., 0., 0));

  // VISIBILITIES ////////////////////////////////////////////////////
  if (visibility_) {
    G4VisAttributes light_blue = LightBlue();
    mask_logic_vol  ->SetVisAttributes(light_blue);
  }
  else{
    mask_logic_vol  ->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
  mask_hole_logic_vol    ->SetVisAttributes(G4VisAttributes::GetInvisible());
  mask_wls_logic_vol     ->SetVisAttributes(G4VisAttributes::GetInvisible());
  mask_wls_hole_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
  wall_wls_logic_vol     ->SetVisAttributes(G4VisAttributes::GetInvisible());
  board_logic_vol ->SetVisAttributes(G4VisAttributes::GetInvisible());
}



G4ThreeVector Next100SiPMBoard::GenerateVertex(const G4String&) const
{
  // Only one generation region available at the moment
  return vtxgen_->GenerateVertex(INSIDE);
}
