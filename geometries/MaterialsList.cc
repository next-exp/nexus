// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J Martin-Albo <jmalbos@ific.uv.es>
//  Created: 27 Mar 2009
//
//  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "MaterialsList.h"

#include <G4Material.hh>
#include <G4Element.hh>
#include <G4NistManager.hh>


namespace nexus {

  
  G4Material* MaterialsList::GXe(G4double pressure, G4double temperature)
  {
    G4String name = "GXe";

    G4Material* mat = G4Material::GetMaterial(name, false);
    
    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();
      mat = nist->ConstructNewGasMaterial(name,"G4_Xe",temperature,pressure);
    }
    
    return mat;
  }
  
  
  
  G4Material* MaterialsList::Steel()
  {
    G4String name = "Steel";

    G4Material* mat = G4Material::GetMaterial(name, false);
    
    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* Cr = nist->FindOrBuildElement("Cr");
      G4Element* Ni = nist->FindOrBuildElement("Ni");
      G4Element* Mn = nist->FindOrBuildElement("Mn");
      G4Element* Fe = nist->FindOrBuildElement("Fe");

      mat = new G4Material(name, 8000*kg/m3, 4);
      mat->AddElement(Cr, 0.18);
      mat->AddElement(Ni, 0.14);
      mat->AddElement(Mn, 0.02);
      mat->AddElement(Fe, 0.66);
    }
    
    return mat;
  }



  G4Material* MaterialsList::Epoxy()
  {
    G4String name = "Epoxy";

    // Check whether material exists already in Geant4 material table
    G4Material* mat = G4Material::GetMaterial(name, false);
    
    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* H = nist->FindOrBuildElement("H");
      G4Element* C = nist->FindOrBuildElement("C");
      G4Element* O = nist->FindOrBuildElement("O");

      mat = new G4Material(name, 1.3*g/cm3, 3);
      mat->AddElement(H, 44);
      mat->AddElement(C, 15);
      mat->AddElement(H, 7);
    }

    return mat;
  }
  
  
} // namespace nexus
