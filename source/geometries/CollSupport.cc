// ----------------------------------------------------------------------------
// nexus | CollSupport.cc
//
// Support for the collimator used with calibration sources at LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "CollSupport.h"
#include "Visibilities.h"

#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4SubtractionSolid.hh>

namespace nexus {
  using namespace CLHEP;

  CollSupport::CollSupport() : axis_centre_(0.*mm), y_displ_(16.25*mm)
  {
  }

  CollSupport::~CollSupport()
  {
  }

  void CollSupport::Construct()
  {
    G4double alum_thick = 15. * mm;
    G4double horizontal_size = 165 * mm; // same as axial flange diameter
    G4double vertical_size = 132.5*mm;
    G4Box* support_full_solid =
      new G4Box("SOURCE_SUPPORT", horizontal_size/2., vertical_size/2.,  alum_thick/2.);

    G4double diam = 12*mm;
    G4Tubs* feedthrough_solid =
       	new G4Tubs("FEEDTHROUGH", 0., diam/2., (alum_thick + 1.*mm)/2., 0., twopi);
    G4SubtractionSolid* support_solid =
	new G4SubtractionSolid("SOURCE_SUPPORT",
                               support_full_solid, feedthrough_solid,
			       0 , G4ThreeVector(0., y_displ_, 0.));
    G4LogicalVolume* support_logic =
      new G4LogicalVolume(support_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "SOURCE_SUPPORT");
    this->SetLogicalVolume(support_logic);

    axis_centre_ = alum_thick/2.;

    G4VisAttributes green_col = nexus::DarkGreen();
    green_col.SetForceSolid(true);
    support_logic->SetVisAttributes(green_col);
  }

  G4double CollSupport::GetAxisCentre()
  {
    return axis_centre_;
  }

  G4double CollSupport::GetYDisplacement()
  {
    return y_displ_;
  }
}
