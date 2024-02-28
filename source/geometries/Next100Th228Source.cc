// ----------------------------------------------------------------------------
// nexus | Next100Th228Source.cc
//
// Th-228 calibration specific source used at LSC with NEXT-100.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------


#include "Next100Th228Source.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include <G4Tubs.hh>
#include <G4Orb.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4SubtractionSolid.hh>


namespace nexus {

  using namespace CLHEP;

  Next100Th228Source::Next100Th228Source(): GeometryBase(),
                                            source_diam_ (1. * mm),
                                            source_offset_ (0.2 * mm),
                                            support_ext_diam_ (7. * mm),
                                            support_int_diam_ (5. *mm),
                                            support_length_ (16. * mm),
                                            support_screw_length_ (7. * mm)
  {
  }

  Next100Th228Source::~Next100Th228Source()
  {
  }

  void Next100Th228Source::Construct()
  {

    /// Support
    G4Tubs* support_full_solid =
      new G4Tubs("TH228_SOURCE_SUPPORT", 0., support_ext_diam_/2.,
                 support_length_/2., 0., twopi);

    G4double offset = 1. * mm;
    G4Tubs* screw_hole_solid =
      new G4Tubs("SCREW_HOLE", 0, support_int_diam_/2.,
                 (support_screw_length_ + offset)/2., 0., twopi);

    G4SubtractionSolid*  support_solid =
      new G4SubtractionSolid("TH228_SOURCE_SUPPORT", support_full_solid,
                             screw_hole_solid, 0,
                             G4ThreeVector(0., 0., -support_length_/2. +
                                           (support_screw_length_-offset)/2.));

    G4Material* steel =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    G4LogicalVolume* support_logic =
      new G4LogicalVolume(support_solid, steel, "TH228_SOURCE_SUPPORT");

    this->SetLogicalVolume(support_logic);


    /// Active source
    G4Orb* source_solid = new G4Orb("TH228", source_diam_/2.);
    G4Material* thorium_mat =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Th");
    G4LogicalVolume* source_logic =
      new G4LogicalVolume(source_solid, thorium_mat, "TH228");

    G4ThreeVector source_pos =
      G4ThreeVector(0., 0., support_length_/2. - source_diam_/2. -source_offset_);
    new G4PVPlacement(0, source_pos, source_logic, "TH228", support_logic,
                      false, 0, false);

    G4VisAttributes source_col = nexus::Red();
    source_col.SetForceSolid(true);
    source_logic->SetVisAttributes(source_col);

    G4VisAttributes support_col = nexus::DarkGreen();
    support_logic->SetVisAttributes(support_col);

    return;
  }

  G4double Next100Th228Source::GetSupportLength() const
  {
    return support_length_;
  }

  G4double Next100Th228Source::GetDiffThZPosEnd() const
  {
    return  source_diam_/2. + source_offset_;
  }

  G4double Next100Th228Source::GetThRadius() const
  {
    return source_diam_/2.;
  }

}
