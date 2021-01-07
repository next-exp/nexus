// -----------------------------------------------------------------------------
// KDB_SensL.cc
//
// Dice board. It consists of an 8x8 array of
// SensL SiPMs on a kapton board.
//
// Miryam Martínez Vara
// -----------------------------------------------------------------------------

#include "KDB_Sensl.h"

#include "SiPMSensl.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"
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
#include <G4RotationMatrix.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  KDB_Sensl::KDB_Sensl(G4int rows, G4int columns):
    BaseGeometry (),
    rows_        (rows),
    columns_     (columns),
    visibility_  (true)
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/KDB/", "Control commands of kapton dice board geometry.");
    msg_->DeclareProperty("visibility", visibility_, "Kapton Dice Boards Visibility");
    sipm_ = new SiPMSensl;
  }

  KDB_Sensl::~KDB_Sensl()
  {
    delete msg_;
    delete sipm_;
    delete dice_gen_;
  }

  void KDB_Sensl::SetMotherLogicalVolume(G4LogicalVolume* mother_logic) {
    mother_logic_ = mother_logic;
  }

  void KDB_Sensl::Construct()
  {
    /// Constructing the SiPM ///
    sipm_->SetSensorDepth(3);
    sipm_->SetMotherDepth(2);
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

    const G4double db_case_x = db_x;
    const G4double db_case_y = db_y;
    const G4double db_case_z = db_z + sipm_->GetDimensions().z();

    dimensions_.setX(db_case_x);
    dimensions_.setY(db_case_y);
    dimensions_.setZ(db_case_z);

    /// AIR VOLUME ///  This is the volume that contains everything
    G4Box* air_solid = new G4Box("DICE_BOARD", db_case_x/2., db_case_y/2., db_case_z/2.);

    G4Material* air =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

    air->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

    G4LogicalVolume* air_logic =
      new G4LogicalVolume(air_solid, air, "DICE_BOARD");

    this->SetLogicalVolume(air_logic);

    /// KAPTON BOARD ///  This is the volume that contains everything
    G4Box* board_solid = new G4Box("KAPTON", db_x/2., db_y/2., db_z/2.);

    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");

    G4LogicalVolume* board_logic =
      new G4LogicalVolume(board_solid, kapton, "KAPTON");

    // Placing the Kapton
    new G4PVPlacement(0, G4ThreeVector(0, 0., -db_case_z /2. + db_z / 2.), board_logic,
                      "KAPTON", air_logic, false, 0, false);

    // Placing the SiPMs inside the hole.
    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();
    G4double         sipm_pos_z = -db_case_z /2. + db_z / 2. + db_z/2. + sipm_->GetDimensions().z() / 2.;

    G4RotationMatrix* rot = new G4RotationMatrix();
    rot -> rotateY(180*deg);

    G4int sipm_no = 0;
    for (G4int i=0; i<columns_; i++) {
      G4double pos_x = db_x/2 - offset - i * sipm_pitch;
      for (G4int j=0; j<rows_; j++) {
        G4double pos_y = -db_y/2. + offset + j * sipm_pitch;

        new G4PVPlacement(rot, G4ThreeVector(pos_x, pos_y, sipm_pos_z), sipm_logic,
                          "SiPM", air_logic, false, sipm_no, false);

        std::pair<int, G4ThreeVector> mypos;
        mypos.first = sipm_no;
        mypos.second = G4ThreeVector(pos_x, pos_y, 0.);
        positions_.push_back(mypos);
        //G4cout << "  SiPM " << sipm_no << " : (" << pos_x << ", " << pos_y
          //     << " )" << G4endl;
        sipm_no++;
      }
    }

    /// SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes board_col = nexus::Yellow();
      board_logic->SetVisAttributes(board_col);

    } else {
      board_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    // VERTEX GENERATOR
    double vertex_displ = db_case_z/2. - board_thickness/2.;
    dice_gen_ = new BoxPointSampler(db_x, db_y, db_z, 0.,
                                    G4ThreeVector(0., 0., vertex_displ), 0);
  }



  G4ThreeVector KDB_Sensl::GetDimensions() const
  {
    return dimensions_;
  }



  const std::vector<std::pair<int, G4ThreeVector> >& KDB_Sensl::GetPositions()
  {
    return positions_;
  }



  G4ThreeVector KDB_Sensl::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "DICE_BOARD") {
      vertex =dice_gen_->GenerateVertex("INSIDE");
    } else {
      G4Exception("[KDB_Sensl]", "GenerateVertex()", FatalException,
                  "Unknown vertex generation region!");
    }
    return vertex;
  }

} // end namespace nexus
