// ----------------------------------------------------------------------------
// nexus | Next100TrackingPlane.cc
//
// Tracking plane of the NEXT-100 geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100TrackingPlane.h"

#include "Next100SiPMBoard.h"
#include "MaterialsList.h"
#include "CylinderPointSampler.h"
#include "BoxPointSampler.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <Randomize.hh>
#include <G4VisAttributes.hh>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

using namespace nexus;


Next100TrackingPlane::Next100TrackingPlane():
  GeometryBase(),
  copper_plate_diameter_  (1340.*mm),
  copper_plate_thickness_ ( 145.*mm),
  distance_board_board_   (   1.*mm),

  visibility_(true),
  sipm_board_geom_(new Next100SiPMBoard),
  copper_plate_gen_(nullptr),
  mpv_(nullptr),
  msg_(nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
                                "Control commands of the NEXT-100 geometry.");

  msg_->DeclareProperty("tracking_plane_vis", visibility_,
                        "Visibility of the tracking plane volumes.");

  geom_navigator_ =
                  G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
}


Next100TrackingPlane::~Next100TrackingPlane()
{
  delete msg_;
  delete sipm_board_geom_;
  delete copper_plate_gen_;
  delete plug_gen_;
}


void Next100TrackingPlane::Construct()
{
  // Make sure the pointer to the mother volume is actually defined
  if (!mpv_)
    G4Exception("[Next100TrackingPlane]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  // COPPER PLATE ////////////////////////////////////////////////////

  G4String copper_plate_name = "TP_COPPER_PLATE";

  G4Tubs* copper_plate_solid =
    new G4Tubs(copper_plate_name, 0., copper_plate_diameter_/2.,
               copper_plate_thickness_/2., 0., 360.*deg);

  G4Material* copper = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
  // In Geant4 11.0.0, a bug in treating the OpBoundaryProcess produced in the
  // surface makes the code fail. This is avoided by setting an empty
  // G4MaterialPropertiesTable of the G4Material.
  copper->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4LogicalVolume* copper_plate_logic =
    new G4LogicalVolume(copper_plate_solid, copper, copper_plate_name);

  G4double copper_plate_zpos =
    GetCoordOrigin().z() - gate_tp_dist_ - copper_plate_thickness_/2.;

  G4VPhysicalVolume* copper_plate_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,copper_plate_zpos),
                      copper_plate_logic, copper_plate_name,
                      mpv_->GetLogicalVolume(), false, 0, false);

  copper_plate_gen_ = new CylinderPointSampler(copper_plate_phys);


  // SIPM BOARDS /////////////////////////////////////////////////////

  sipm_board_geom_->SetMotherPhysicalVolume(mpv_);
  sipm_board_geom_->Construct();
  G4LogicalVolume* sipm_board_logic = sipm_board_geom_->GetLogicalVolume();

  G4double zpos = GetCoordOrigin().z() - gate_tp_dist_ + sipm_board_geom_->GetThickness()/2.;

  // SiPM boards are positioned bottom (negative Y) to top (positive Y)
  // and left (negative X) to right (positive X).
  G4int board_index = 1;

  // Column on the far left has 5 boards.
  // It is located 3.5 boards away from the center.
  PlaceSiPMBoardColumns(5, -3.5, zpos, board_index, sipm_board_logic);

  // Second column from the left has 7 boards.
  // It is located 2.5 boards away from the center.
  PlaceSiPMBoardColumns(7, -2.5, zpos, board_index, sipm_board_logic);

  // Central block of 4 columns with 8 boards each
  PlaceSiPMBoardColumns(8, -1.5, zpos, board_index, sipm_board_logic);
  PlaceSiPMBoardColumns(8, -0.5, zpos, board_index, sipm_board_logic);
  PlaceSiPMBoardColumns(8,  0.5, zpos, board_index, sipm_board_logic);
  PlaceSiPMBoardColumns(8,  1.5, zpos, board_index, sipm_board_logic);

  // Second column from the right has 7 boards and
  // it's located 2.5 boards away from the center.
  PlaceSiPMBoardColumns(7,  2.5, zpos, board_index, sipm_board_logic);

  // Column on the far right has 5 boards.
  // It is located 3.5 boards away from the center.
  PlaceSiPMBoardColumns(5,  3.5, zpos, board_index, sipm_board_logic);

  ///// DB PLUGS
  G4double plug_x = 40. * mm;
  G4double plug_y =  5. * mm;
  G4double plug_z =  2. * mm;
  G4double plug_y_displacement = 90.5 * mm;

  G4Box* plug_solid = new G4Box("DB_PLUG", plug_x/2., plug_y/2., plug_z/2.);
  G4LogicalVolume* plug_logic =
    new G4LogicalVolume(plug_solid,  materials::PEEK(), "DB_PLUG");
  G4double plug_posz = copper_plate_zpos - copper_plate_thickness_/2. - plug_z/2.;

  G4ThreeVector pos;
  for (int i=0; i<int(board_pos_.size()); i++) {
    pos = board_pos_[i];
    pos.setY(pos.getY()-plug_y_displacement);
    pos.setZ(plug_posz);
    plug_pos_.push_back(pos);
    new G4PVPlacement(0, pos, plug_logic, "DB_PLUG", mpv_->GetLogicalVolume(), false, i, false);
  }

  plug_gen_ = new BoxPointSampler(plug_x/2., plug_y/2., plug_z/2., 0.);


  // VISIBILITIES //////////////////////////////////////////
  plug_logic        ->SetVisAttributes(G4VisAttributes::GetInvisible());
  if (visibility_) {
    G4VisAttributes copper_brown = CopperBrownAlpha();
    copper_brown.SetForceSolid(true);
    copper_plate_logic->SetVisAttributes(copper_brown);
  } else {
    copper_plate_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    sipm_board_logic  ->SetVisAttributes(G4VisAttributes::GetInvisible());
  }

}


void Next100TrackingPlane::PlaceSiPMBoardColumns(G4int num_boards,
                                                 G4double distance_from_center,
                                                 G4double zpos,
                                                 G4int& board_index,
                                                 G4LogicalVolume* logic_vol)
{
  G4double size = sipm_board_geom_->GetSize() + distance_board_board_;

  G4double xpos = distance_from_center * size;

  for (auto i=0; i<num_boards; i++) {
    G4double ypos = (- 0.5 * (num_boards - 1) + i ) * size;
    G4ThreeVector position(xpos, ypos, zpos);
    board_pos_.push_back(position);

    new G4PVPlacement(nullptr, position, logic_vol, logic_vol->GetName(),
                      mpv_->GetLogicalVolume(), false, board_index, false);
    board_index++;
  }
}


void Next100TrackingPlane::PrintSiPMPosInGas() const
{
  auto sipm_positions = sipm_board_geom_->GetSiPMPositions();

  for (unsigned int i=0; i<board_pos_.size(); ++i) {
    for (unsigned int j=0; j<sipm_positions.size(); ++j) {
      G4int id = 1000 * (i+1) + j;
      G4ThreeVector pos = sipm_positions[j] + board_pos_[i];
      G4cout << "SiPM " << id << "  " << pos << G4endl;
    }
  }
}

void Next100TrackingPlane::GetSiPMPosInGas(std::vector<G4ThreeVector>& sipm_pos) const
{
  auto sipm_positions = sipm_board_geom_->GetSiPMPositions();
  for (unsigned int i=0; i<board_pos_.size(); ++i) {
    for (unsigned int j=0; j<sipm_positions.size(); ++j) {
      G4ThreeVector pos = sipm_positions[j] + board_pos_[i];
      sipm_pos.push_back(pos);
    }
  }
}



G4ThreeVector Next100TrackingPlane::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex(0., 0., 0.);
  G4VPhysicalVolume *VertexVolume;

  if (region == "SIPM_BOARD") {
    do {
        vertex = sipm_board_geom_->GenerateVertex("");
        G4int board_num = G4RandFlat::shootInt((long) 0, board_pos_.size());
        vertex += board_pos_[board_num];
        G4ThreeVector glob_vtx(vertex);
        glob_vtx = glob_vtx - GetCoordOrigin();
        VertexVolume =
          geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);

      } while ((VertexVolume->GetName() == "SIPM_BOARD_MASK_HOLE")  ||
              (VertexVolume->GetName() == "SIPM_BOARD_MASK_WLS_HOLE"));

  }
  else if (region == "DB_PLUG") {
    vertex = plug_gen_->GenerateVertex(INSIDE);
    G4int plug_num = G4RandFlat::shootInt((long) 0, plug_pos_.size());
    vertex += plug_pos_[plug_num];
  }
  else if (region == "TP_COPPER_PLATE") {
    vertex = copper_plate_gen_->GenerateVertex(VOLUME);
  }

  return vertex;
}
