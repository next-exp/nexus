// ----------------------------------------------------------------------------
// nexus | Th228Source.cc
//
// Th-228 calibration specific source with plastic support used at LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Th228Source.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include <G4Tubs.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  Th228Source::Th228Source():
    DiskSource()
  {
    source_diam_ = 5.*mm;
    source_thick_ = .1*mm;
    support_diam_ = 12.7*mm;
    support_thick_ = 6.35*mm;
  }

  Th228Source::~Th228Source()
  {

  }

  void Th228Source::Construct()
  {

    ///Plastic support
    G4Tubs* support_solid =
      new G4Tubs("SUPPORT", 0., support_diam_/2., support_thick_/2., 0., twopi);

    G4Material* plastic = MaterialsList::PS();
    G4LogicalVolume* support_logic =
      new G4LogicalVolume(support_solid, plastic, "TH228_SOURCE_SUPPORT");

    this->SetLogicalVolume(support_logic);

    // G4double source_thick = .1*mm;
    // G4double source_diam = 3.*mm;
    G4Tubs* source_solid =
      new G4Tubs("SOURCE", 0., source_diam_/2., source_thick_/2., 0., twopi);
    G4Material* thorium_mat =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Th");
    G4LogicalVolume* source_logic =
      new G4LogicalVolume(source_solid, thorium_mat, "TH228");

    new G4PVPlacement(0, G4ThreeVector(0., 0., support_thick_/2. - source_thick_/2.),
		      source_logic, "TH228",
		      support_logic, false, 0, false);

    G4VisAttributes source_col = nexus::DarkGreen();
    source_col.SetForceSolid(true);
    source_logic->SetVisAttributes(source_col);

    return;

  }

  // G4double Th228Source::GetSourceDiameter()
  // {
  //   return source_diam_;
  // }

  // G4double Th228Source::GetSourceThickness()
  // {
  //   return source_thick_;
  // }

  //  G4double Th228Source::GetSupportDiameter()
  // {
  //   return support_diam_;
  // }

  // G4double Th228Source::GetSupportThickness()
  // {
  //   return support_thick_;
  // }

}
