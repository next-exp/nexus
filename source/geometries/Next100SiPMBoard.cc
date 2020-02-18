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

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

using namespace nexus;


Next100SiPMBoard::Next100SiPMBoard():
  BaseGeometry(),
  size_     (123.80*mm),
  thickness_(  3.  *mm),
  pitch_    ( 15.60*mm),
  margin_   (  7.30*mm)
{
}


Next100SiPMBoard::~Next100SiPMBoard()
{
}


void Next100SiPMBoard::Construct()
{
  // BOARD ///////////////////////////////////////////////////////////

  G4String name = "SIPM_BOARD";

  G4Box* board_solid_vol = new G4Box(name, size_/2., size_/2., thickness_/2.);

  G4LogicalVolume* board_logic_vol =
    new G4LogicalVolume(board_solid_vol,
                        G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON"),
                        name);

  BaseGeometry::SetLogicalVolume(board_logic_vol);

  // -------------------------------------------------------
  // TODO: Review this section once we confirm the
  //       correctness of the new optical model.
  // -------------------------------------------------------

  G4OpticalSurface* board_opsurf =
    new G4OpticalSurface("SIPM_BOARD_OPSURF", unified, ground, dielectric_metal);
  board_opsurf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
  new G4LogicalSkinSurface("SIPM_BOARD_OPSURF", board_logic_vol, board_opsurf);

  // WLS COATING /////////////////////////////////////////////////////

  name = "SIPM_BOARD_WLS";
  G4double wls_thickness = 2.*micrometer; // Optimal thickness for TPB

  G4Box* wls_solid_vol = new G4Box(name, size_/2., size_/2., wls_thickness/2.);

  MaterialsList::TPB()->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

  G4LogicalVolume* wls_logic_vol =
    new G4LogicalVolume(wls_solid_vol, MaterialsList::TPB(), name);

  G4double zpos = thickness_/2. - wls_thickness/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,zpos), wls_logic_vol,
                    name, board_logic_vol, false, 0, true);

  // -------------------------------------------------------
  // TODO: Review this section once we confirm the
  //       correctness of the new optical model.
  // -------------------------------------------------------

  // SILICON PHOTOMULTIPLIERS ////////////////////////////////////////

  // We use for now the generic photosensor until the exact characteristics
  // of the new Hamamatsu SiPMs are known
  GenericPhotosensor sipm_geom(1.3*mm);
  sipm_geom.Construct();

  zpos = thickness_/2. - wls_thickness - 1.*mm;

  for (unsigned int i=0; i<8; i++) {

    G4double xpos = -size_/2. + margin_ + i * pitch_;

    for (unsigned int j=0; j<8; j++) {

      G4double ypos = -size_/2. + margin_ + j * pitch_;

      new G4PVPlacement(nullptr, G4ThreeVector(xpos, ypos, zpos),
                        sipm_geom.GetLogicalVolume(),
                        sipm_geom.GetLogicalVolume()->GetName(),
                        board_logic_vol, false, i*10+j, true);
    }
  }
}


G4ThreeVector Next100SiPMBoard::GenerateVertex(const G4String&) const
{
  return G4ThreeVector(0., 0., 0.);
}
