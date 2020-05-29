// ----------------------------------------------------------------------------
// nexus | SurroundingAir.cc
//
// Volume of air to be placed inside the LSC lead castle.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SurroundingAir.h"

#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {
  
  using namespace CLHEP;

  SurroundingAir::SurroundingAir()
  {
  }

  SurroundingAir::~SurroundingAir()
  {
  }

 void SurroundingAir::Construct()
  {
    G4double size = 4. * m;
    G4Box* air_solid = 
      new G4Box("AIR", size/2., size/2., size/2.);
    
    G4LogicalVolume* air_logic = new G4LogicalVolume(air_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "AIR");
    
    air_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Set this volume as the wrapper for the whole geometry 
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(air_logic);
  }
  
}
