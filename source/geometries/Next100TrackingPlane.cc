// -----------------------------------------------------------------------------
//  nexus | Next100TrackingPlane.cc
//
//  * Author: <justo.martin-albo@ific.uv.es>
//  * Creation date: 22 January 2020
// -----------------------------------------------------------------------------

#include "Next100TrackingPlane.h"

#include "Next100SiPMBoard.h"
#include "CylinderPointSampler2020.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <Randomize.hh>
#include <G4VisAttributes.hh>

using namespace nexus;


Next100TrackingPlane::Next100TrackingPlane(G4double origin_z_coord):
  BaseGeometry(),
  z0_(origin_z_coord), // distance between gate and first face of tracking plane
  copper_plate_diameter_       (1340.*mm),
  copper_plate_thickness_      ( 120.*mm),
  distance_board_board_        (   1.*mm),
  visibility_(false),
  sipm_board_geom_(new Next100SiPMBoard),
  copper_plate_gen_(nullptr),
  mpv_(nullptr),
  msg_(nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
                                "Control commands of the NEXT-100 geometry.");

  msg_->DeclareProperty("tracking_plane_vis", visibility_,
                        "Visibility of the tracking plane volumes.");
}


Next100TrackingPlane::~Next100TrackingPlane()
{
  delete sipm_board_geom_;
  delete copper_plate_gen_;
}


void Next100TrackingPlane::Construct()
{
  // Make sure that the pointer to the mother volume is actually defined
  if (!mpv_)
    G4Exception("[Next100TrackingPlane]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  // SIPM BOARDS /////////////////////////////////////////////////////

  sipm_board_geom_->SetMotherPhysicalVolume(mpv_);
  sipm_board_geom_->Construct();
  G4LogicalVolume* sipm_board_logic_vol = sipm_board_geom_->GetLogicalVolume();

  G4double zpos = GetELzCoord() - z0_ - sipm_board_geom_->GetThickness()/2.;

  // -------------------------------------------------------
  // TODO. Review sensor numbering scheme.
  // -------------------------------------------------------

  // Central block of 4x8 boards.

  G4int counter = 1;

  for (auto i=0; i<4; i++) {
    G4double xpos = - 1.5*sipm_board_geom_->GetSize() - 1.5*distance_board_board_
                    + i*(sipm_board_geom_->GetSize() + distance_board_board_);

    for (auto j=0; j<8; j++) {
      G4double ypos = - 3.5*sipm_board_geom_->GetSize() - 3.5*distance_board_board_
                      + j*(sipm_board_geom_->GetSize() + distance_board_board_);

      G4ThreeVector position(xpos, ypos, zpos); board_pos_.push_back(position);

      new G4PVPlacement(nullptr, position,
                        sipm_board_logic_vol,
                        sipm_board_logic_vol->GetName(),
                        mpv_->GetLogicalVolume(),
                        false, counter, false);
      counter++;
    }
  }

  // Outer columns of 7 boards

  for (auto j=0; j<7; j++) {
    G4double xpos = - 2.5*sipm_board_geom_->GetSize() - 2.5*distance_board_board_;
    G4double ypos = - 3.0*sipm_board_geom_->GetSize() - 3.0*distance_board_board_
                    + j*(sipm_board_geom_->GetSize() + distance_board_board_);


    G4ThreeVector position(xpos, ypos, zpos); board_pos_.push_back(position);

    new G4PVPlacement(nullptr, position, sipm_board_logic_vol,
                      sipm_board_logic_vol->GetName(), mpv_->GetLogicalVolume(),
                      false, counter, false);

    counter++;

    position.setX(-xpos); board_pos_.push_back(position);

    new G4PVPlacement(nullptr, position, sipm_board_logic_vol,
                      sipm_board_logic_vol->GetName(), mpv_->GetLogicalVolume(),
                      false, counter, false);

    counter++;
  }

  // Outer columns of 5 boards

  for (auto j=0; j<5; j++) {
    G4double xpos = - 3.5*sipm_board_geom_->GetSize() - 3.5*distance_board_board_;
    G4double ypos = - 2.0*sipm_board_geom_->GetSize() - 2.0*distance_board_board_
                    + j*(sipm_board_geom_->GetSize() + distance_board_board_);

    G4ThreeVector position(xpos, ypos, zpos); board_pos_.push_back(position);

    new G4PVPlacement(nullptr, position, sipm_board_logic_vol,
                      sipm_board_logic_vol->GetName(), mpv_->GetLogicalVolume(),
                      false, counter, false);

    counter++;

    position.setX(-xpos); board_pos_.push_back(position);

    new G4PVPlacement(nullptr, position, sipm_board_logic_vol,
                      sipm_board_logic_vol->GetName(), mpv_->GetLogicalVolume(),
                      false, counter, false);

    counter++;
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

  zpos = GetELzCoord() - z0_ - sipm_board_geom_->GetThickness() - copper_plate_thickness_/2.;

  G4VPhysicalVolume* copper_plate_phys_vol =
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,zpos),
                      copper_plate_logic_vol, copper_plate_name, mpv_->GetLogicalVolume(),
                      false, 0, false);

  copper_plate_gen_ = new CylinderPointSampler2020(copper_plate_phys_vol);

  // VISIBILITIES //////////////////////////////////////////

  if (visibility_) {
    G4VisAttributes copper_brown = CopperBrown();
    copper_plate_logic_vol->SetVisAttributes(copper_brown);
  } else {
    copper_plate_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
    sipm_board_logic_vol  ->SetVisAttributes(G4VisAttributes::Invisible);
  }

}


void Next100TrackingPlane::PrintSiPMPositions() const
{
  auto sipm_positions = sipm_board_geom_->GetSiPMPositions();

  for (unsigned int i=0; i<board_pos_.size(); ++i) {
    for (unsigned int j=0; j<sipm_positions.size(); ++j) {
      G4int id = 1000 * (i+1) + j;
      G4ThreeVector pos = sipm_positions[j] + board_pos_[i];
      G4cout << id << "  " << pos << G4endl;
    }
  }
}


G4ThreeVector Next100TrackingPlane::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "SIPM_BOARD") {
    vertex = sipm_board_geom_->GenerateVertex("");
    G4int board_num = G4RandFlat::shootInt((long) 0, board_pos_.size());
    vertex += board_pos_[board_num];
  }
  else if (region == "TRK_SUPPORT") {
    vertex = copper_plate_gen_->GenerateVertex("VOLUME");
  }

  return vertex;
}
