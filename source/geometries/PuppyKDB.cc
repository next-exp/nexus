#include "PuppyKDB.h"

#include "SiPMpet9mm2.h"
#include "PmtSD.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

#include <sstream>

namespace nexus {

  using namespace CLHEP;

  PuppyKDB::PuppyKDB():
    BaseGeometry(),
    visibility_ (0),
    xy_dim_(5.2*cm)
  {
    /// Messenger
   msg_ = new G4GenericMessenger(this, "/Geometry/PuppyKDB/", "Control commands of PET geometry.");
   msg_->DeclareProperty("kdb_vis", visibility_, "PET Kapton Dice Boards Visibility");

   sipm_ = new SiPMpet9mm2;

  }

  PuppyKDB::~PuppyKDB()
  {
  }

  // void PuppyKDB::SetPitchSize(G4double pitch)
  // {
  //   sipm_pitch_ = pitch;
  // }

  // void PuppyKDB::SetXYsize(G4double xysize)
  // {
  //   xysize_ = xysize;
  // }

  void PuppyKDB::Construct()
  {
    sipm_->Construct();

    const G4double db_x =  xy_dim_;
    const G4double db_y =  xy_dim_;
    const G4double board_thickness = 0.3 * mm;

    // Outer element volume  ///////////////////////////////////////////////////
    G4double sipm_z_dim = sipm_->GetDimensions().getZ();
    const G4double border = sipm_z_dim/2.;
    const G4double out_x = db_x;
    const G4double out_y = db_y;
    const G4double out_z = board_thickness + 2.*border;

    dimensions_.setX(out_x);
    dimensions_.setY(out_y);
    dimensions_.setZ(out_z);

    G4Material* out_material = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    out_material->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    //std::cout << "Border = " << border << G4endl;
    //std::cout << "LXe_volume, x: " << out_x << ", y: " << out_y << ", z: " << out_z << std::endl;
    G4Box* out_solid = new G4Box("LXE_DICE", out_x/2., out_y/2., out_z/2.);
    G4LogicalVolume* out_logic =
      new G4LogicalVolume(out_solid, out_material,  "LXE_DICE");
    this->SetLogicalVolume(out_logic);

    // KAPTON BOARD /////////////////////////////////////////////////

    //std::cout << "db_x: " << db_x << ", db_y: " << db_y << ", db_z: " << db_z << std::endl;
    G4Box* board_solid = new G4Box("DICE_BOARD", db_x/2., db_y/2., board_thickness/2.);

    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
    //    G4Material* teflon =
    //    G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

    G4LogicalVolume* board_logic =
      new G4LogicalVolume(board_solid, kapton, "DICE_BOARD");
    new G4PVPlacement(0, G4ThreeVector(0.,0., -border), board_logic,
			"DICE_BOARD", out_logic, false, 0, true);

    // OPTICAL SURFACE FOR REFLECTION
    /*
    G4OpticalSurface* db_opsur = new G4OpticalSurface("DICE_BOARD");
    db_opsur->SetType(dielectric_metal);
    db_opsur->SetModel(unified);
    db_opsur->SetFinish(ground);
    db_opsur->SetSigmaAlpha(0.1);

    db_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE_LXe(refl_));

    new G4LogicalSkinSurface("DICE_BOARD", board_logic, db_opsur);
    */

    // SILICON PMs //////////////////////////////////////////////////

    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();
    G4double sipm_xy_dim = sipm_->GetDimensions().getX();

    G4double pos_z = board_thickness/2. - border + sipm_z_dim/2.;
    G4cout << "Pos of SiPM in LXe dice = " << pos_z << G4endl;
    G4double offset = sipm_xy_dim/2.;
    G4double sipm_pitch = sipm_xy_dim;
    G4int sipm_no = 0;

    const G4int rows = 8;
    const G4int columns = 8;

    for (G4int i=0; i<rows; i++) {
      G4double pos_y = db_y/2. - offset - i*sipm_pitch;
      for (G4int j=0; j<columns; j++) {
        G4double pos_x = -db_x/2 + offset + j*sipm_pitch;
	//G4cout << pos_x << ", " << pos_y << ", " << pos_z << G4endl;
        new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z),
          sipm_logic, "SIPMpet", out_logic, false, sipm_no, true);
        std::pair<int, G4ThreeVector> mypos;
        mypos.first = sipm_no;
        mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
        positions_.push_back(mypos);
        sipm_no++;
      }
    }

    //   G4cout << "SiPMs start at " << pos_z - sipm.GetDimensions().z()/2. << " and end at " << pos_z + sipm.GetDimensions().z()/2. << " in the ref system of LXe outer box " << G4endl;


    // SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes board_col = nexus::DirtyWhite;
      board_col.SetForceSolid(true);
      board_logic->SetVisAttributes(board_col);
    }
    else {
      board_logic->SetVisAttributes(G4VisAttributes::Invisible);
      // coating_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  G4ThreeVector PuppyKDB::GetDimensions() const
  {
    return dimensions_;
  }

  const std::vector<std::pair<int, G4ThreeVector> >& PuppyKDB::GetPositions()
  {
    return positions_;
  }


} // end namespace nexus
