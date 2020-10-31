// -----------------------------------------------------------------------------
// nexus | NextDemoVessel.cc
//
// Geometry of the pressure vessel of the NEXT-DEMO++ detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextDemoVessel.h"

#include "MaterialsList.h"

#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>

using namespace nexus;


NextDemoVessel::NextDemoVessel():
  BaseGeometry(),
  vessel_diam_  (298.8*mm), // Internal diameter
  vessel_length_(600.0*mm), // Internal length
  vessel_thickn_(  3.0*mm)
{
}


NextDemoVessel::~NextDemoVessel()
{
}


void NextDemoVessel::Construct()
{
  G4String vessel_name = "VESSEL";

  G4Tubs* vessel_solid_vol = new G4Tubs(vessel_name,
                                        0., (vessel_diam_/2.+vessel_thickn_),
                                        vessel_length_/2., 0, twopi);

  G4LogicalVolume* vessel_logic_vol =
    new G4LogicalVolume(vessel_solid_vol, MaterialsList::Steel(), vessel_name);

  BaseGeometry::SetLogicalVolume(vessel_logic_vol);
}


G4ThreeVector NextDemoVessel::GenerateVertex(const G4String& region) const
{
  return G4ThreeVector();
}
