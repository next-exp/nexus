// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 27 Oct 2009
//  
//  Copyright (c) 2009 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "LeadShield.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>


namespace nexus {


  LeadShield::LeadShield(G4double X, G4double Y, G4double Z, G4double thickn): 
    _width(X), _height(Y), _length(Z), _thickness(thickn), BaseGeometry()
  {
    DefineGeometry();
  }


  
  void LeadShield::DefineGeometry()
  {
    G4Box* shield_solid = new G4Box("SHIELD", _width/2., _height/2., _length/2.);

    G4LogicalVolume* shield_logic = 
      new G4LogicalVolume(shield_solid, 
			  G4Material::GetMaterial("G4_Pb"), "SHIELD");

    SetLogicalVolume(shield_logic);
    
    G4double in_width  = _width  - 2.*_thickness;
    G4double in_height = _height - 2.*_thickness;
    G4double in_length = _length - 2.*_thickness;

    G4Box* gap_solid = 
      new G4Box("AIR_GAP", in_width/2., in_height/2., in_length/2.);

    G4LogicalVolume* gap_logic = 
      new G4LogicalVolume(gap_solid, G4Material::GetMaterial("G4_AIR"),
			  "AIR_GAP");
    
    G4PVPlacement* gap_physi = 
      new G4PVPlacement(0, G4ThreeVector(0,0,0), gap_logic, "AIR_GAP",
			gap_logic, false, 0);
  }



  G4ThreeVector LeadShield::GenerateVertex(const G4String& region) const
  {
    if (region == "BULK") {
      // FIXME. Proper implementation of vertex location in shield
      return G4ThreeVector(0,0,0);
    }
  }

  
} // end namespace nexus
