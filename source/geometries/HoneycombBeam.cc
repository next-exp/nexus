// ----------------------------------------------------------------------------
// nexus | HoneycombBeam.cc
//
// Beam of the support structure to the EP copper plate.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "HoneycombBeam.h"
#include "MaterialsList.h"

#include <G4Box.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>


namespace nexus {
  
  using namespace CLHEP;
  
  HoneycombBeam::HoneycombBeam(G4double length, G4double height,
                               G4double thickn):
    GeometryBase(), length_(length), height_(height), thickn_(thickn)
  {
  }
  
  HoneycombBeam::~HoneycombBeam()
  {
  }

  void HoneycombBeam::Construct()
  {
    beam_solid_ =
      new G4Box("HONEYCOMB_BEAM", thickn_/2., height_/2., length_/2.);
    G4Material* steel = materials::Steel();
    G4LogicalVolume* beam_logic =
      new G4LogicalVolume(beam_solid_, steel, "HONEYCOMB_BEAM");

    this->SetLogicalVolume(beam_logic);
  }
  
}
