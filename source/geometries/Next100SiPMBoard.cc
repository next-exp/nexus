// -----------------------------------------------------------------------------
//  nexus | Next100SiPMBoard.cc
//
//  * Author: <justo.martin-albo@ific.uv.es>
//  * Creation date: 7 January 2020
// -----------------------------------------------------------------------------

#include "Next100SiPMBoard.h"

#include "MaterialsList.h"
#include "GenericPhotosensor.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>

using namespace nexus;


Next100SiPMBoard::Next100SiPMBoard():
  BaseGeometry(),
  size_     (123.80*mm),
  thickness_(  3.  *mm),
  pitch_    ( 15.60*mm),
  margin_   (  7.30*mm),
  mpv_   (nullptr),
  vtxgen_(nullptr)
{
}


Next100SiPMBoard::~Next100SiPMBoard()
{
  delete mpv_;
}


void Next100SiPMBoard::Construct()
{
  if (!mpv_)
    G4Exception("[Next100SiPMBoard]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  // BOARD ///////////////////////////////////////////////////////////

  G4String board_name = "SIPM_BOARD";

  G4Box* board_solid_vol = new G4Box(board_name, size_/2., size_/2., thickness_/2.);

  G4LogicalVolume* board_logic_vol =
    new G4LogicalVolume(board_solid_vol,
                        G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
                        board_name);

  BaseGeometry::SetLogicalVolume(board_logic_vol);

  G4OpticalSurface* board_opsurf =
    new G4OpticalSurface("SIPM_BOARD_OPSURF", unified, ground, dielectric_metal);
  board_opsurf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
  new G4LogicalSkinSurface("SIPM_BOARD_OPSURF", board_logic_vol, board_opsurf);

  // WLS COATING /////////////////////////////////////////////////////

  G4String wls_name = "SIPM_BOARD_WLS";
  G4double wls_thickness = 1.*micrometer; // Optimal thickness for TPB

  G4Box* wls_solid_vol = new G4Box(wls_name, size_/2., size_/2., wls_thickness/2.);

  G4Material* tpb = MaterialsList::TPB();
  tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());

  G4LogicalVolume* wls_logic_vol =
    new G4LogicalVolume(wls_solid_vol, tpb, wls_name);

  G4double zpos = thickness_/2. - wls_thickness/2.;

  G4VPhysicalVolume* wls_phys_vol =
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,zpos),
                      wls_logic_vol, wls_name, board_logic_vol,
                      false, 0, true);

  G4OpticalSurface* wls_opsurf =
    new G4OpticalSurface("SIPM_BOARD_WLS_OPSURF",
                         glisur, ground, dielectric_dielectric, .01);
  new G4LogicalBorderSurface("SIPM_BOARD_WLS_OPSURF", wls_phys_vol, mpv_, wls_opsurf);
  new G4LogicalBorderSurface("SIPM_BOARD_WLS_OPSURF", mpv_, wls_phys_vol, wls_opsurf);

  // SILICON PHOTOMULTIPLIERS ////////////////////////////////////////

  // We use for now the generic photosensor until the exact characteristics
  // of the new Hamamatsu SiPMs are known.
  // The refractive index of the sensor's window is matched to that of TPB,
  // its surrounding medium.
  GenericPhotosensor sipm_geom(1.3*mm);
  sipm_geom.SetRefractiveIndex(tpb->GetMaterialPropertiesTable()->GetProperty("RINDEX"));
  sipm_geom.Construct();

  zpos = thickness_/2. - wls_thickness - 1.*mm;

  for (unsigned int i=0; i<8; i++) {

    G4double xpos = -size_/2. + margin_ + i * pitch_;

    for (unsigned int j=0; j<8; j++) {

      G4double ypos = -size_/2. + margin_ + j * pitch_;

      G4ThreeVector position(xpos, ypos, zpos);
      sipm_positions.push_back(position);

      new G4PVPlacement(nullptr, position,
                        sipm_geom.GetLogicalVolume(),
                        sipm_geom.GetLogicalVolume()->GetName(),
                        board_logic_vol,
                        false, i*10+j, true);
    }
  }

  // VERTEX GENERATOR //////////////////////////////////////

  vtxgen_ = new BoxPointSampler(size_, size_, thickness_, 0.);
}


G4ThreeVector Next100SiPMBoard::GenerateVertex(const G4String&) const
{
  // Only one generation region available at the moment
  return vtxgen_->GenerateVertex("INSIDE");
}
