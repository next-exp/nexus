//
//  MaterialsList.cc
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>
//     Created: 27 Mar 2009
//     Updated:
//
//  Copyright (c) 2009 -- NEXT Collaboration
//

#include "MaterialsList.h"
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Element.hh>


namespace nexus {

  void MaterialsList::DefineMaterials()
  {
    G4NistManager* nist = G4NistManager::Instance();
    
    G4Element* Xe = nist->FindOrBuildElement("Xe");

    G4Material* G4_AIR = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* G4_Galactic = nist->FindOrBuildMaterial("G4_Galactic");
    G4Material* G4_lXe = nist->FindOrBuildMaterial("G4_lXe");
    

    const G4double R = 8.314472 *m3*hep_pascal/(kelvin*mole);
    G4double A = Xe->GetA();
    //G4double A = 131.293 * g/mole;
    G4cout << A << G4endl;
    G4double pressure = 10 * atmosphere;
    G4double temperature = (273.15 + 15.) * kelvin;
    
    G4double density = (pressure * A) / (R * temperature);
    G4Material* GXe = 
      new G4Material("GXe", density, 1, kStateGas, temperature, pressure);
    GXe->AddElement(Xe, 1.0);
    density = IdealGasLaw(1., 1., 1.);
  }
  
  
  
   G4double MaterialsList::IdealGasLaw(G4double pressure, 
 				      G4double temperature,
 				      G4double A)
   {
     /// pV = nRT -> p = nRT/V = mRT/aV = RTd/a -> d = pa/RT
     const G4double R = 8.314472 *m3*hep_pascal/(kelvin*mole);
     G4double density = (pressure * A) / (R * temperature); 
     return density;
   }


} // namespace nexus
