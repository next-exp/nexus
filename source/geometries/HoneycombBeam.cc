// ----------------------------------------------------------------------------
// nexus | HoneycombBeam.cc
//
// Beam of the support structure to the EP copper plate.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "HoneycombBeam.h"
#include "MaterialsList.h"

#include <G4Trd.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>


namespace nexus {

  using namespace CLHEP;

  HoneycombBeam::HoneycombBeam(G4double thickn, G4double length_b,
                               G4double length_t, G4double height):
    GeometryBase(),
    length_b_(length_b),
    length_t_(length_t),
    height_(height),
    thickn_(thickn)
  {
  }

  HoneycombBeam::~HoneycombBeam()
  {
  }

  void HoneycombBeam::Construct()
  {
    beam_solid_ =
      new G4Trd("HONEYCOMB_BEAM", thickn_/2., thickn_/2., length_b_/2.,
                length_t_/2., height_/2.);
    G4Material* steel = materials::Steel();
    G4LogicalVolume* beam_logic =
      new G4LogicalVolume(beam_solid_, steel, "HONEYCOMB_BEAM");

    this->SetLogicalVolume(beam_logic);
  }

}
