// -----------------------------------------------------------------------------
// nexus | NextElDB.cc
//
// Geometry of the NEXT-DEMO SiPM board.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextElDB.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "SiPM11.h"

#include <G4Box.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  NextElDB::NextElDB(G4int rows, G4int columns):
    GeometryBase(),
    rows_(rows),
    columns_(columns)
  {
    // The SiPM
    siPM_ = new SiPM11();
  }



  NextElDB::~NextElDB()
  {
    delete siPM_;
  }



  void NextElDB::Construct()
  {
    // DAUGHTER BOARD (DBO) //////////////////////////////////////////

    const G4double sipm_pitch = 10. * mm;
    const G4double coating_thickn = 0.1 * micrometer;
    const G4double board_thickn = 5. * mm;
    // const G4double board_side_reduction = 1. * mm;
    const G4double board_side_reduction = .5 * mm;

    const G4double dbo_x = columns_ * sipm_pitch - 2. * board_side_reduction ;
    const G4double dbo_y =    rows_ * sipm_pitch - 2. * board_side_reduction ;
    const G4double dbo_z = board_thickn;

    dimensions_.setX(dbo_x);
    dimensions_.setY(dbo_y);
    dimensions_.setZ(dbo_z);

    G4Box* board_solid = new G4Box("DICE_BOARD", dbo_x/2., dbo_y/2., dbo_z/2.);

    G4Material* teflon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

    teflon->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* board_logic =
      new G4LogicalVolume(board_solid, teflon, "DICE_BOARD");

    this->SetLogicalVolume(board_logic);


    // WLS COATING ///////////////////////////////////////////////////

    G4Box* coating_solid =
      new G4Box("DB_WLS_COATING", dbo_x/2., dbo_y/2., coating_thickn/2.);

    G4Material* TPB = materials::TPB();
    TPB->SetMaterialPropertiesTable(opticalprops::TPB());

    G4LogicalVolume* coating_logic =
      new G4LogicalVolume(coating_solid, TPB, "DB_WLS_COATING");

    G4double pos_z = (-dbo_z + coating_thickn) / 2.;

    new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z), coating_logic,
		      "DB_WLS_COATING", board_logic, false, 0, false);


    // SILICON PMs ///////////////////////////////////////////////////
    siPM_->Construct();
    G4LogicalVolume* sipm_logic = siPM_->GetLogicalVolume();

    pos_z = -dbo_z/2 + coating_thickn + (siPM_->GetDimensions().z())/2.;

    G4double offset = sipm_pitch/2. - board_side_reduction;

    G4int sipm_no = 0;

    for (G4int i=0; i<rows_; i++) {
      G4double pos_y = dbo_y/2. - offset - i*sipm_pitch;
      for (G4int j=0; j<columns_; j++) {
	G4double pos_x = -dbo_x/2 + offset + j*sipm_pitch;
	new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z), sipm_logic,
			  "SIPM11", board_logic, false, sipm_no, false);
	std::pair<int, G4ThreeVector> mypos;
	mypos.first = sipm_no;
	mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
	positions_.push_back(mypos);
	sipm_no++;
      }
    }

    /// Optical surfaces
    G4OpticalSurface* dboard_opsur = new G4OpticalSurface("DB");
    dboard_opsur->SetType(dielectric_metal);
    dboard_opsur->SetModel(unified);
    dboard_opsur->SetFinish(ground);
    dboard_opsur->SetSigmaAlpha(0.1);
    dboard_opsur->SetMaterialPropertiesTable(opticalprops::PTFE());

    new G4LogicalSkinSurface("DB", board_logic, dboard_opsur);


    // Visibilities
    coating_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    sipm_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  }



  G4ThreeVector NextElDB::GetDimensions()
  {
    return dimensions_;
  }



  std::vector<std::pair<int, G4ThreeVector> > NextElDB::GetPositions()
  {
    return positions_;
  }

} // end namespace nexus
