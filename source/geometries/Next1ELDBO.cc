// ----------------------------------------------------------------------------
// nexus | Next1ELDBO.cc
//
// Dice boards of the NEXT-DEMO detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next1ELDBO.h"

#include "SiPM11.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"

#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

#include <sstream>


namespace nexus {

  using namespace CLHEP;

  Next1ELDBO::Next1ELDBO(G4int rows, G4int columns): GeometryBase()
  {
    DefineGeometry(rows, columns);
  }



  Next1ELDBO::~Next1ELDBO()
  {
  }



  void Next1ELDBO::DefineGeometry(G4int rows, G4int columns)
  {
    // DAUGHTER BOARD (DBO) //////////////////////////////////////////

    const G4double sipm_pitch = 10. * mm;
    const G4double coating_thickn = 0.1 * micrometer;
    const G4double board_thickn = 5. * mm;
    const G4double board_side_reduction = 1. * mm;

    const G4double dbo_x = columns * sipm_pitch - 2. * board_side_reduction;
    const G4double dbo_y =    rows * sipm_pitch - 2. * board_side_reduction;
    const G4double dbo_z = board_thickn + coating_thickn;

    dimensions_.setX(dbo_x);
    dimensions_.setY(dbo_y);
    dimensions_.setZ(dbo_z);

    G4Box* board_solid = new G4Box("DBO", dbo_x/2., dbo_y/2., dbo_z/2.);

    G4Material* teflon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
    teflon->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* board_logic =
      new G4LogicalVolume(board_solid, teflon, "DBO");

    this->SetLogicalVolume(board_logic);


    // WLS COATING ///////////////////////////////////////////////////

    G4Box* coating_solid =
      new G4Box("WLS_COATING", dbo_x/2., dbo_y/2., coating_thickn/2.);

    G4Material* TPB = materials::TPB();
    TPB->SetMaterialPropertiesTable(opticalprops::TPB());

    G4LogicalVolume* coating_logic =
      new G4LogicalVolume(coating_solid, TPB, "WLS_COATING");

    G4double pos_z = (dbo_z - coating_thickn) / 2.;

    new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z), coating_logic,
		      "WLS_COATING", board_logic, false, 0);


    // SILICON PMs ///////////////////////////////////////////////////
    SiPM11 sipm_geom;

    G4LogicalVolume* sipm_logic = sipm_geom.GetLogicalVolume();

    pos_z = dbo_z/2 - coating_thickn - (sipm_geom.GetDimensions().z())/2.;

    G4double offset = sipm_pitch/2. - board_side_reduction;

    G4int sipm_no = 0;
    for (G4int i=0; i<rows; i++) {
      G4double pos_y = dbo_y/2. - offset - i*sipm_pitch;
      for (G4int j=0; j<columns; j++) {
	G4double pos_x = -dbo_x/2 + offset + j*sipm_pitch;
	new G4PVPlacement(0, G4ThreeVector(pos_x, pos_y, pos_z), sipm_logic,
			  "SIPM11", board_logic, false, sipm_no);
	std::pair<int, G4ThreeVector> mypos;
	mypos.first = sipm_no;
	mypos.second = G4ThreeVector(pos_x, pos_y, pos_z);
	positions_.push_back(mypos);
	sipm_no++;
      }
    }

    /// Optical surfaces
    G4OpticalSurface* dboard_opsur = new G4OpticalSurface("DBO");
    dboard_opsur->SetType(dielectric_metal);
    dboard_opsur->SetModel(unified);
    dboard_opsur->SetFinish(ground);
    dboard_opsur->SetSigmaAlpha(0.1);
    dboard_opsur->SetMaterialPropertiesTable(opticalprops::PTFE());

    new G4LogicalSkinSurface("DBO", board_logic, dboard_opsur);

  }

  G4ThreeVector Next1ELDBO::GetDimensions()
  {
    return dimensions_;
  }

  std::vector<std::pair<int, G4ThreeVector> > Next1ELDBO::GetPositions()
  {
    return positions_;
  }

} // end namespace nexus
