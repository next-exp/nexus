// ----------------------------------------------------------------------------
// nexus | NextDemoTrackingPlane.cc
//
// Tracking plane of the Demo++ geometry.
// It implements 2 different TP implementations:
// "run5": the one used at the beginning with passing-holes masks
// "run7" and "run8": the ones used with coated masks and membranes.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextDemoTrackingPlane.h"

#include "MaterialsList.h"
#include "NextDemoSiPMBoard.h"
#include "BoxPointSampler.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4TransportationManager.hh>
#include <G4RotationMatrix.hh>
#include <G4Box.hh>
#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <Randomize.hh>


using namespace nexus;



NextDemoTrackingPlane::NextDemoTrackingPlane():
  BaseGeometry(),
  verbosity_       (false),
  visibility_      (false),
  config_          (""),
  plate_side_      (16. * cm),
  plate_thickn_    (12. * mm),
  plate_hole_side_ (49. * mm),
  num_boards_      (4),
  sipm_board_      (new NextDemoSiPMBoard()),
  plate_gen_       (nullptr),
  mother_phys_     (nullptr),
  msg_             (nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/",
                                "Control commands of the NextDemo geometry.");

  msg_->DeclareProperty("tracking_plane_verbosity", verbosity_,
                        "Tracking Plane verbosity");

  msg_->DeclareProperty("tracking_plane_vis", visibility_,
                        "Tracking Plane visibility");

  // Initializing the geometry navigator
  geom_navigator_ =
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
}



NextDemoTrackingPlane::~NextDemoTrackingPlane()
{
  delete msg_;
  delete sipm_board_;
  delete plate_gen_;
}



void NextDemoTrackingPlane::Construct()
{
  /// Verbosity
  if(verbosity_) G4cout << G4endl << "*** NEXT Demo Tracking Plane ";

  /// Check that the configuration has been set
  if (config_ == "")
    G4Exception("[NextDemoTrackingPlane]", "Construct()", FatalException,
                "NextDemoTrackingPlane configuration has not been set.");

  /// Defining Tracking Plane parameters
  G4double gate_board_dist = 0.; // Distance from GATE to SiPM Board kapton surface
  G4double mask_thickn     = 0.;
  G4double membrane_thickn = 0.;
  G4double coating_thickn  = 0.;
  G4double hole_diameter   = 0.;

  if (config_ == "run5") {
    if(verbosity_) G4cout << "run5 ..." << G4endl;
    gate_board_dist = 16.76 * mm;
    mask_thickn     = 2.0 * mm;
    membrane_thickn = 0.0 * mm;
    coating_thickn  = 0.0 * mm;
    hole_diameter   = 3.5 * mm;
  }
  else if (config_ == "run7") {
    if(verbosity_) G4cout << "run7 ..." << G4endl;
    gate_board_dist = 9.16 * mm;
    mask_thickn     = 2.0  * mm;
    membrane_thickn = 0.25 * mm;
    coating_thickn  = 2.0  * micrometer;
    hole_diameter   = 3.5  * mm;
  }
  else if (config_ == "run8") {
    if(verbosity_) G4cout << "run8 ..." << G4endl;
    gate_board_dist = 19.66 * mm;
    mask_thickn     = 5.0  * mm;
    membrane_thickn = 0.2  * mm;
    coating_thickn  = 2.0  * micrometer;
    hole_diameter   = 4.0  * mm;
  }

  /// Make sure the pointer to the mother volume is actually defined
  if (!mother_phys_)
    G4Exception("[NextDemoTrackingPlane]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  G4LogicalVolume* mother_logic = mother_phys_->GetLogicalVolume();


  /// SiPM BOARDs
  G4String board_name = "SiPM_BOARD";
  sipm_board_->SetMotherPhysicalVolume(mother_phys_);
  sipm_board_->SetMaskThickness    (mask_thickn);
  sipm_board_->SetMembraneThickness(membrane_thickn);
  sipm_board_->SetCoatingThickness (coating_thickn);
  sipm_board_->SetHoleDiameter     (hole_diameter);
  sipm_board_->Construct();
  G4LogicalVolume* board_logic = sipm_board_->GetLogicalVolume();
  board_size_                  = sipm_board_->GetBoardSize();
  G4double kapton_thickn       = sipm_board_->GetKaptonThickness();

  // Placing the boards
  G4double board_posz      = GetELzCoord() - gate_board_dist + board_size_.z()/2. - kapton_thickn;
  GenerateBoardPositions(board_posz);
  for (G4int i=0; i<num_boards_; i++)
    new G4PVPlacement(nullptr, board_pos_[i], board_logic, board_name,
                      mother_logic, false, i+14, true);


  /// SUPPORT PLATE
  G4String plate_name = "TP_PLATE";

  G4Box* plate_nh_solid   = new G4Box(plate_name, plate_side_/2.,
                                      plate_side_/2., plate_thickn_/2.);

  G4Box* plate_hole_solid = new G4Box("TP_PLATE_HOLE", plate_hole_side_/2.,
                                      plate_hole_side_/2., plate_thickn_/2. + 2. * mm);

  // Making the holes
  G4SubtractionSolid* plate_solid =
    new G4SubtractionSolid(plate_name, plate_nh_solid, plate_hole_solid, nullptr,
                           G4ThreeVector(board_pos_[0].x(),board_pos_[0].y(), 0));

  for (G4int i=1; i<num_boards_; i++) {
    G4ThreeVector hole_pos = board_pos_[i];
    hole_pos.setZ(0.);
    plate_solid = new G4SubtractionSolid(plate_name, plate_solid, plate_hole_solid,
                                         nullptr, hole_pos);
  }

  G4LogicalVolume* plate_logic =
    new G4LogicalVolume(plate_solid, MaterialsList::Steel(), plate_name);

  // We arbitrarily place the TP_PLATE 10 mm away from the SiPMBoards
  G4double plate_posz = board_posz - 10. * mm - plate_thickn_/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., plate_posz), plate_logic,
                    plate_name, mother_logic, false, 0, true);


  /// GENERATORS
  plate_gen_ = new BoxPointSampler(plate_side_, plate_side_, plate_thickn_, 0.,
                                   G4ThreeVector(0., 0., plate_posz), nullptr);

  /// VISIBILITIES
  if (visibility_) {
    plate_logic->SetVisAttributes(CopperBrown());
    board_logic->SetVisAttributes(LightBlue());
  } else {
    plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
    board_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }
}



void NextDemoTrackingPlane::GenerateBoardPositions(G4double board_posz)
{
  /// Function that computes and stores the XY positions of Dice Boards
  //  From NextNewTrackingPlane: & From Drawing "0000-00 ASSEMBLY NEXT-DEMO++.pdf"

  G4double boards_gap = 1. * mm;
  G4double pos_x = (board_size_.x() + boards_gap) / 2.;
  G4double pos_y = (board_size_.y() + boards_gap) / 2.;

  // Placing board 1
  G4ThreeVector pos1(0., 0., 0.);
  pos1.setX(-pos_x);
  pos1.setY( pos_y);
  pos1.setZ( board_posz);
  board_pos_.push_back(pos1);
  if (verbosity_) G4cout << "* SiPM board 1 position: " << pos1 << G4endl; 

  // Placing board 2
  G4ThreeVector pos2(0., 0., 0.);
  pos2.setX( pos_x);
  pos2.setY( pos_y);
  pos2.setZ( board_posz);
  board_pos_.push_back(pos2);
  if (verbosity_) G4cout << "* SiPM board 2 position: " << pos2 << G4endl; 

  // Placing board 3
  G4ThreeVector pos3(0., 0., 0.);
  pos3.setX(-pos_x);
  pos3.setY(-pos_y);
  pos3.setZ( board_posz);
  board_pos_.push_back(pos3);
  if (verbosity_) G4cout << "* SiPM board 3 position: " << pos3 << G4endl; 

  // Placing board 4
  G4ThreeVector pos4(0., 0., 0.);
  pos4.setX( pos_x);
  pos4.setY(-pos_y);
  pos4.setZ( board_posz);
  board_pos_.push_back(pos4);
  if (verbosity_) G4cout << "* SiPM board 4 position: " << pos4 << G4endl; 
}



G4ThreeVector NextDemoTrackingPlane::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "SIPM_BOARD") {
    vertex         = sipm_board_->GenerateVertex("DICE_BOARD");
    G4double rand  = num_boards_ * G4UniformRand();
    vertex        += board_pos_[int(rand)];
  }

  else if (region == "TP_PLATE") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = plate_gen_->GenerateVertex("INSIDE");
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx + G4ThreeVector(0, 0, -GetELzCoord());
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  return vertex;
}
