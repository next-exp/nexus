// NEXUS: visibilities.cc
//    -- Last modification: 24th May 2007, jmalbos
//

#include <nexus/geometry.h>

#include "G4Colour.hh"


namespace nexus {

  void geometry::set_visibilities()
  {  
    // Defining colors (may look different depending on the system)
    invis_VisAtt  = new G4VisAttributes();
    invis_VisAtt ->SetVisibility(false);
    grey_VisAtt   = new G4VisAttributes(G4Colour(0.5,0.5,0.5));
    blue_VisAtt   = new G4VisAttributes(G4Colour(0.5,0.8,1.0));
    brown_VisAtt  = new G4VisAttributes(G4Colour(1.0,0.8,0.5));
    green_VisAtt  = new G4VisAttributes(G4Colour(0.5,1.0,0.8));
    purple_VisAtt = new G4VisAttributes(G4Colour(0.8,0.3,1.0));
    pink_VisAtt   = new G4VisAttributes(G4Colour(0.6,0.4,0.4));

    // Setting color of the volumes
    logic_WORLD    ->SetVisAttributes(invis_VisAtt);
    logic_OVESSEL  ->SetVisAttributes(brown_VisAtt);
    logic_BUFFER   ->SetVisAttributes(brown_VisAtt);
    logic_IVESSEL  ->SetVisAttributes(green_VisAtt);
    logic_ACTIVE   ->SetVisAttributes(green_VisAtt);
  }

} // namespace nexus
