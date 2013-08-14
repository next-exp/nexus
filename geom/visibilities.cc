// NEXT simulation: visibilities.cc
//

#include <next/geometry.hh>


void geometry::set_visibilities()
{  
  // create a generic store
  bhep::gstore store;

  // Defining a reader for GLOBAL group
  bhep::sreader reader(store);
  reader.file("next.par");
  reader.info_level(bhep::NORMAL);
  reader.group("GLOBAL");
  reader.read();
  G4int DISPLAY_ALL = store.fetch_istore("DISPLAY_ALL");
  
  // Defining colors (may look different depending on the system)
  invis_VisAtt  = new G4VisAttributes();
  invis_VisAtt ->SetVisibility(false);
  grey_VisAtt   = new G4VisAttributes(G4Colour(0.5,0.5,0.5));
  blue_VisAtt   = new G4VisAttributes(G4Colour(0.5,0.8,1.0));
  brown_VisAtt  = new G4VisAttributes(G4Colour(1.0,0.8,0.5));
  green_VisAtt  = new G4VisAttributes(G4Colour(0.5,1.0,0.8));
  purple_VisAtt = new G4VisAttributes(G4Colour(0.8,0.3,1.0));
  pink_VisAtt   = new G4VisAttributes(G4Colour(0.6,0.4,0.4));

  // Displaying everything
  if (DISPLAY_ALL) {
    logic_WORLD          ->SetVisAttributes(invis_VisAtt);
    logic_CHAMBER        ->SetVisAttributes(green_VisAtt);
    logic_ACTIVE         ->SetVisAttributes(blue_VisAtt);
    logic_DETECTOR       ->SetVisAttributes(brown_VisAtt);
    }
  // Displaying only the shape
  else {
    logic_WORLD          ->SetVisAttributes(invis_VisAtt);
    logic_CHAMBER        ->SetVisAttributes(green_VisAtt);
    logic_DETECTOR       ->SetVisAttributes(brown_VisAtt);
    logic_ACTIVE         ->SetVisAttributes(blue_VisAtt);
  }
}
