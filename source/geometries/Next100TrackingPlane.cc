// -----------------------------------------------------------------------------
//  nexus | Next100TrackingPlane.cc
//
//  * Author: <justo.martin-albo@ific.uv.es>
//  * Creation date: 22 January 2020
// -----------------------------------------------------------------------------

#include "Next100TrackingPlane.h"

#include "Next100SiPMBoard.h"

#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>

using namespace nexus;


Next100TrackingPlane::Next100TrackingPlane(G4LogicalVolume* mother_volume):
  BaseGeometry(),
  copper_plate_diameter_       (1340.*mm),
  copper_plate_thickness_      ( 120.*mm),
  distance_gate_tracking_plane_(  25.*mm),
  distance_board_board_        (   1.*mm),
  mother_volume_(mother_volume)
{
  // Make sure that the pointer to the mother volume is actually defined
  if (!mother_volume)
    G4Exception("[Next100TrackingPlane]", "Next100TrackingPlane()",
                FatalException, "Mother volume is a nullptr.");
}


Next100TrackingPlane::~Next100TrackingPlane()
{
}


void Next100TrackingPlane::Construct()
{
  // SIPM BOARDS /////////////////////////////////////////////////////

  Next100SiPMBoard sipm_board_geom;
  sipm_board_geom.Construct();
  G4LogicalVolume* sipm_board_logic_vol = sipm_board_geom.GetLogicalVolume();

  G4double zpos = - distance_gate_tracking_plane_ - sipm_board_geom.GetThickness()/2.;

  // -------------------------------------------------------
  // TODO. Review sensor numbering scheme once HDF5 output
  //       tables are fixed.
  // -------------------------------------------------------

  // Central block of 4x8 boards.

  for (auto i=0; i<4; i++) {
    G4double xpos = - 1.5*sipm_board_geom.GetSize() - 1.5*distance_board_board_
                    + i*(sipm_board_geom.GetSize() + distance_board_board_);

    for (auto j=0; j<8; j++) {
      G4double ypos = - 3.5*sipm_board_geom.GetSize() - 3.5*distance_board_board_
                      + j*(sipm_board_geom.GetSize() + distance_board_board_);

      new G4PVPlacement(nullptr, G4ThreeVector(xpos,ypos,zpos), sipm_board_logic_vol,
                        sipm_board_logic_vol->GetName(), mother_volume_, false, i+j, true);
    }
  }

  // Outer columns of 7 boards

  for (auto j=0; j<7; j++) {
    G4double xpos = - 2.5*sipm_board_geom.GetSize() - 2.5*distance_board_board_;
    G4double ypos = - 3.0*sipm_board_geom.GetSize() - 3.0*distance_board_board_
                    + j*(sipm_board_geom.GetSize() + distance_board_board_);

    new G4PVPlacement(nullptr, G4ThreeVector(xpos,ypos,zpos), sipm_board_logic_vol,
                      sipm_board_logic_vol->GetName(), mother_volume_, false, j+32, true);
    new G4PVPlacement(nullptr, G4ThreeVector(-xpos,ypos,zpos), sipm_board_logic_vol,
                      sipm_board_logic_vol->GetName(), mother_volume_, false, j+39, true);
  }

  // Outer columns of 5 boards

  for (auto j=0; j<5; j++) {
    G4double xpos = - 3.5*sipm_board_geom.GetSize() - 3.5*distance_board_board_;
    G4double ypos = - 2.0*sipm_board_geom.GetSize() - 2.0*distance_board_board_
                    + j*(sipm_board_geom.GetSize() + distance_board_board_);

    new G4PVPlacement(nullptr, G4ThreeVector(xpos,ypos,zpos), sipm_board_logic_vol,
                      sipm_board_logic_vol->GetName(), mother_volume_, false, j+46, true);
    new G4PVPlacement(nullptr, G4ThreeVector(-xpos,ypos,zpos), sipm_board_logic_vol,
                      sipm_board_logic_vol->GetName(), mother_volume_, false, j+51, true);
  }

  // TRACKING COPPER PLATE ///////////////////////////////////////////

  G4String copper_plate_name = "TRACKING_COPPER_PLATE";

  G4Tubs* copper_plate_solid_vol = new G4Tubs(copper_plate_name,
                                             0., copper_plate_diameter_/2.,
                                             copper_plate_thickness_/2.,
                                             0., twopi);

  G4LogicalVolume* copper_plate_logic_vol =
    new G4LogicalVolume(copper_plate_solid_vol,
                        G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
                        copper_plate_name);

  zpos = - distance_gate_tracking_plane_ - sipm_board_geom.GetThickness()
         - copper_plate_thickness_/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,zpos), copper_plate_logic_vol,
                   copper_plate_name, mother_volume_, false, 0, true);
}


G4ThreeVector Next100TrackingPlane::GenerateVertex(const G4String&) const
{
  return G4ThreeVector();
}
