// -----------------------------------------------------------------------------
// nexus | NextNewKDB.cc
//
// Dice board of the NEXT-WHITE detector. It consists of an 8x8 array of
// SensL SiPMs on a kapton board.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextNewKDB.h"

#include "SiPMSensl.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSamplerLegacy.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;


  NextNewKDB::NextNewKDB(G4int rows, G4int columns):
    GeometryBase (),
    rows_            (rows),
    columns_         (columns),
    time_binning_    (1. * microsecond),
    visibility_      (true),
    sipm_visibility_ (true)
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/KDB/", "Control commands of kapton dice board geometry.");
    msg_->DeclareProperty("visibility", visibility_, "Kapton Dice Boards Visibility");

    msg_->DeclareProperty("sipm_visibility", sipm_visibility_, "SiPMs Visibility");

    G4GenericMessenger::Command& time_binning_cmd = msg_->DeclareProperty("sipm_time_binning", time_binning_, "TP SiPMs time binning.");
    time_binning_cmd.SetParameterName("sipm_time_binning", false);
    time_binning_cmd.SetUnitCategory("Time");
    time_binning_cmd.SetRange("sipm_time_binning>0.");

    sipm_ = new SiPMSensl;
  }



  NextNewKDB::~NextNewKDB()
  {
    delete msg_;
    delete sipm_;
    delete dice_gen_;
  }


  void NextNewKDB::SetMotherLogicalVolume(G4LogicalVolume* mother_logic) {
    mother_logic_ = mother_logic;
  }


  void NextNewKDB::Construct()
  {
    /// Constructing the SiPM ///
    sipm_->SetVisibility(sipm_visibility_);
    sipm_->SetTimeBinning(time_binning_);
    sipm_->SetSensorDepth(3);
    sipm_->SetMotherDepth(5);
    sipm_->SetNamingOrder(1000);
    sipm_->Construct();


    /// DIMENSIONS ///
    const G4double sipm_pitch           = 10. * mm;
    const G4double board_thickness      = 0.3 * mm; // this is the real DB thickness
    const G4double board_side_reduction = 0.5 * mm;
    const G4double offset               = sipm_pitch/2. - board_side_reduction;

    const G4double db_x = columns_ * sipm_pitch - 2. * board_side_reduction ;
    const G4double db_y =    rows_ * sipm_pitch - 2. * board_side_reduction ;
    const G4double db_z = board_thickness;

    //G4cout << "* Kapton Dice board dimensions: "  << db_x << " , "
    //       << db_y << " , "  << db_z << G4endl;

    const G4double mask_x = db_x;
    const G4double mask_y = db_y;
    const G4double mask_z = 2. * mm;

    const G4double db_case_x = db_x;
    const G4double db_case_y = db_y;
    const G4double db_case_z = db_z + mask_z;

    dimensions_.setX(db_case_x);
    dimensions_.setY(db_case_y);
    dimensions_.setZ(db_case_z);


    /// KAPTON BOARD ///  This is the volume that contains everything
    G4Box* board_solid = new G4Box("DICE_BOARD", db_case_x/2., db_case_y/2., db_case_z/2.);

    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
    kapton->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* board_logic =
      new G4LogicalVolume(board_solid, kapton, "DICE_BOARD");

    this->SetLogicalVolume(board_logic);


    /// TEFLON MASK ///
    G4double mask_pos_z = - db_case_z / 2. + mask_z / 2.;

    G4Box* mask_solid = new G4Box("DICE_MASK", mask_x/2., mask_y/2., mask_z/2.);

    G4Material* teflon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
    teflon->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* mask_logic =
      new G4LogicalVolume(mask_solid, teflon, "DICE_MASK");

    // Adding the optical surface
    G4OpticalSurface* mask_optSurf =
      new G4OpticalSurface("DICE_MASK_OPSURF", unified, ground, dielectric_metal);
    mask_optSurf->SetMaterialPropertiesTable(opticalprops::PTFE());
    new G4LogicalSkinSurface("DICE_MASK_OPSURF", mask_logic, mask_optSurf);


    /// Adding the SiPMs to the Dice Board. ///
    // They are placed in a gas cylinder emulating the mask holes
    G4String hole_name   = "SiPM_HOLE";
    G4double hole_diam   = 3.5 * mm;
    G4double hole_length = mask_z;

    G4Tubs* hole_solid =
      new G4Tubs(hole_name, 0., hole_diam/2., hole_length/2., 0, twopi);

    G4LogicalVolume* hole_logic =
      new G4LogicalVolume(hole_solid, mother_logic_->GetMaterial(), hole_name);

    // Placing the SiPMs inside the hole.
    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();
    G4double         sipm_pos_z = hole_length/2. - sipm_->GetDimensions().z() / 2.;

    new G4PVPlacement(0, G4ThreeVector(0., 0., sipm_pos_z), sipm_logic,
                      sipm_logic->GetName(), hole_logic, false, 0, false);

    G4int sipm_no = 0;
    for (G4int i=0; i<columns_; i++) {
      G4double pos_x = db_x/2 - offset - i * sipm_pitch;
      for (G4int j=0; j<rows_; j++) {
        G4double pos_y = -db_y/2. + offset + j * sipm_pitch;

        new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, 0), hole_logic,
                          hole_name, mask_logic, false, sipm_no, false);

        std::pair<int, G4ThreeVector> mypos;
        mypos.first = sipm_no;
        mypos.second = G4ThreeVector(pos_x, pos_y, 0.);
        positions_.push_back(mypos);
        //G4cout << "  SiPM " << sipm_no << " : (" << pos_x << ", " << pos_y
        //       << ", " << pos_z << " )" << G4endl;
        sipm_no++;
      }
    }

    // Placing the Dice Teflon Mask
    new G4PVPlacement(0, G4ThreeVector(0, 0., mask_pos_z), mask_logic,
                      "DICE_MASK", board_logic, false, 0, false);


    /// SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes board_col = nexus::Yellow();
      board_logic->SetVisAttributes(board_col);
      mask_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      hole_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

    } else {
      board_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      mask_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      hole_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    // VERTEX GENERATOR
    double vertex_displ = db_case_z/2. - board_thickness/2.;
    dice_gen_ = new BoxPointSamplerLegacy(db_x, db_y, db_z, 0.,
                                          G4ThreeVector(0., 0., vertex_displ), 0);
  }



  G4ThreeVector NextNewKDB::GetDimensions() const
  {
    return dimensions_;
  }



  const std::vector<std::pair<int, G4ThreeVector> >& NextNewKDB::GetPositions()
  {
    return positions_;
  }



  G4ThreeVector NextNewKDB::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "DICE_BOARD") {
      vertex =dice_gen_->GenerateVertex("INSIDE");
    } else {
      G4Exception("[NextNewKDB]", "GenerateVertex()", FatalException,
                  "Unknown vertex generation region!");
    }
    return vertex;
  }

} // end namespace nexus
