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

  CollSupport::CollSupport() : _axis_centre(0.*mm), _y_displ(16.25*mm)
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
      new G4Box("SOURCE_SUPPORT", 100.*mm/2., 100.*mm/2.,  alum_thick/2.);

    G4double diam = 12*mm;
    G4Tubs* feedthrough_solid = 
       	new G4Tubs("FEEDTHROUGH", 0., diam/2., (alum_thick + 1.*mm)/2., 0., twopi);
    G4SubtractionSolid* support_solid =
	new G4SubtractionSolid("SOURCE_SUPPORT",
                               support_full_solid, feedthrough_solid, 
			       0 , G4ThreeVector(0., _y_displ, 0.));
    G4LogicalVolume* support_logic =
      new G4LogicalVolume(support_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_Al"), "SOURCE_SUPPORT");
    this->SetLogicalVolume(support_logic);

    _axis_centre = alum_thick/2.;

    G4VisAttributes green_col = nexus::DarkGreen();
    green_col.SetForceSolid(true);
    support_logic->SetVisAttributes(green_col);
  }

  G4double CollSupport::GetAxisCentre()
  {
    return _axis_centre;
  }

  G4double CollSupport::GetYDisplacement()
  {
    return _y_displ;
  }
}
