// ----------------------------------------------------------------------------
// nexus | NextDemoTrackingPlane1.cc
//
// Tracking plane 1 of the Demo++ geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextDemoTrackingPlane1.h"

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



NextDemoTrackingPlane1::NextDemoTrackingPlane1():
  BaseGeometry(),
  verbosity_       (false),
  visibility_      (false),
  plate_side_      (16. * cm),
  plate_thickness_ (12. * mm),
  plate_hole_side_ (49. * mm),
  num_boards_      (4),
  sipm_board_      (new NextDemoSiPMBoard()),
  plate_gen_(nullptr),
  mother_phys_(nullptr),
  msg_(nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/",
                                "Control commands of the NextDemo geometry.");

  msg_->DeclareProperty("tracking_plane_verbosity", verbosity_,
                        "Tracking Plane verbosity");

  msg_->DeclareProperty("tracking_plane_vis", visibility_,
                        "Visibility of the tracking plane volumes.");

  // Initializing the geometry navigator
  geom_navigator_ =
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
}



NextDemoTrackingPlane1::~NextDemoTrackingPlane1()
{
  delete msg_;
  delete sipm_board_;
  delete plate_gen_;
}



void NextDemoTrackingPlane1::Construct()
{
  // Make sure the pointer to the mother volume is actually defined
  if (!mother_phys_)
    G4Exception("[NextDemoTrackingPlane1]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  G4LogicalVolume* mother_logic = mother_phys_->GetLogicalVolume();

  /// Verbosity
  if(verbosity_) {
    G4cout << G4endl << "*** NEXT Demo Tracking Plane ..." << G4endl;
  }

  /// SiPM BOARDs
  G4String board_name = "SiPM_BOARD";
  sipm_board_->SetMotherLogicalVolume(mother_logic);
  sipm_board_->Construct();
  G4LogicalVolume* board_logic = sipm_board_->GetLogicalVolume();
  board_dimensions_            = sipm_board_->GetDimensions();
  if (verbosity_)
    G4cout << "* SiPM board dimensions: " << board_dimensions_ << G4endl; 

  // Placing the boards
  G4double gate_board_dist = 16.76 * mm;
  G4double board_thickness =  0.25 * mm;
  G4double board_posz      = GetELzCoord() - gate_board_dist - board_thickness/2.;

  GenerateBoardPositions(board_posz);

  G4RotationMatrix* board_rot = new G4RotationMatrix();
  board_rot->rotateY(pi);

  G4ThreeVector pos;
  for (G4int i=0; i<num_boards_; i++)
    new G4PVPlacement(G4Transform3D(*board_rot, board_pos_[i]), board_logic, board_name,
                      mother_logic, false, i+1, true);

  /// SUPPORT PLATE
  G4String plate_name = "TP_PLATE";

  G4Box* plate_nh_solid = new G4Box(plate_name, plate_side_/2.,
                                    plate_side_/2., plate_thickness_/2.);

  G4Box* plate_hole_solid = new G4Box("TP_PLATE_HOLE", plate_hole_side_/2.,
                                      plate_hole_side_/2., plate_thickness_/2. + 2. * mm);

  // Making the holes
  G4SubtractionSolid* plate_solid =
    new G4SubtractionSolid(plate_name, plate_nh_solid, plate_hole_solid, nullptr,
                           G4ThreeVector(board_pos_[0].getX(),board_pos_[0].getY(), 0));
  for (G4int i=1; i<num_boards_; i++) {
    G4ThreeVector hole_pos = board_pos_[i];
    hole_pos.setZ(0.);
    plate_solid = new G4SubtractionSolid(plate_name, plate_solid, plate_hole_solid,
                                         nullptr, hole_pos);
  }

  G4LogicalVolume* plate_logic =
    new G4LogicalVolume(plate_solid, MaterialsList::Steel(), plate_name);

  // We arbitrarily place the TP_PLATE 10 mm away from the SiPMBoards
  G4double plate_posz = board_posz - 10. * mm - plate_thickness_/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., plate_posz), plate_logic,
                    plate_name, mother_logic, false, 0, true);


  /// GENERATORS
  plate_gen_ = new BoxPointSampler(plate_side_/2., plate_side_/2., plate_thickness_/2., 0.,
                                   G4ThreeVector(0., 0., plate_posz), nullptr);

  /// VISIBILITIES
  if (visibility_) {
    //G4VisAttributes brown = CopperBrown();
    plate_logic->SetVisAttributes(CopperBrown());
    board_logic->SetVisAttributes(LightBlue());
  } else {
    plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
    board_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }
}



void NextDemoTrackingPlane1::GenerateBoardPositions(G4double board_posz)
{
  /// Function that computes and stores the XY positions of Dice Boards
  //  From NextNewTrackingPlane: & From Drawing "0000-00 ASSEMBLY NEXT-DEMO++.pdf"

  G4double boards_gap = 1. * mm;
  G4double pos_x = (board_dimensions_.getX() + boards_gap) / 2.;
  G4double pos_y = (board_dimensions_.getY() + boards_gap) / 2.;

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



G4ThreeVector NextDemoTrackingPlane1::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "SIPM_BOARD") {
    vertex         = sipm_board_->GenerateVertex("DICE_BOARD");
    G4double rand  = num_boards_ * G4UniformRand();
    vertex        += board_pos_[int(rand)];
    //vertex        -= G4ThreeVector(0., 0., board_dimensions_.getZ()/2.);
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
