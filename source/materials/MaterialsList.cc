// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 27 Mar 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "MaterialsList.h"
#include "XenonGasProperties.h"
#include "ArgonGasProperties.h"

#include <G4Material.hh>
#include <G4Element.hh>
#include <G4NistManager.hh>

using namespace nexus;
  
  
G4Material* MaterialsList::GXe(G4double pressure, G4double temperature)
{
  G4String name = "GXe";
  
  G4Material* mat = G4Material::GetMaterial(name, false);
    
  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();
      
    mat = new G4Material(name, XenonGasProperties::Density(pressure), 1,
			 kStateGas, temperature, pressure);
    
    G4Element* Xe = nist->FindOrBuildElement("Xe");
  
    mat->AddElement(Xe,1);
  }

  return mat;
}

G4Material* MaterialsList::GXeEnriched(G4double pressure, G4double temperature)
{
  G4String name = "GXeEnriched";
  
  G4Material* mat = G4Material::GetMaterial(name, false);
    
  if (mat == 0) {
      
    mat = new G4Material(name, XenonGasProperties::Density(pressure), 1,
			 kStateGas, temperature, pressure);
    
    G4Element* Xe = new G4Element("GXeEnriched", "Xe", 6);
    
    G4Isotope* Xe129 = new G4Isotope("Xe129", 54, 129, XenonGasProperties::MassPerMole(129));
    G4Isotope* Xe130 = new G4Isotope("Xe130", 54, 130, XenonGasProperties::MassPerMole(130));
    G4Isotope* Xe131 = new G4Isotope("Xe131", 54, 131, XenonGasProperties::MassPerMole(131));
    G4Isotope* Xe132 = new G4Isotope("Xe132", 54, 132, XenonGasProperties::MassPerMole(132));
    G4Isotope* Xe134 = new G4Isotope("Xe134", 54, 134, XenonGasProperties::MassPerMole(134));
    G4Isotope* Xe136 = new G4Isotope("Xe136", 54, 136, XenonGasProperties::MassPerMole(136));

    Xe->AddIsotope(Xe129, 0.0656392*perCent);
    Xe->AddIsotope(Xe130, 0.0656392*perCent);
    Xe->AddIsotope(Xe131, 0.234361*perCent);
    Xe->AddIsotope(Xe132, 0.708251*perCent);
    Xe->AddIsotope(Xe134, 8.6645*perCent);
    Xe->AddIsotope(Xe136, 90.2616*perCent);
   
    
    // G4cout << Xe->GetNumberOfIsotopes() << G4endl;
    // G4cout << Xe->GetIsotopeVector()->size() << G4endl;
    // for (G4int i=0; i< Xe->GetNumberOfIsotopes(); ++i) {
    //   G4cout << Xe->GetIsotope(i)->GetName() << G4endl;
    // }
    
    mat->AddElement(Xe,1);
  }

  return mat;
}

G4Material* MaterialsList::GXeDepleted(G4double pressure, G4double temperature)
{
  G4String name = "GXeDepleted";
  
  G4Material* mat = G4Material::GetMaterial(name, false);
    
  if (mat == 0) {
      
    mat = new G4Material(name, XenonGasProperties::Density(pressure), 1,
			 kStateGas, temperature, pressure);

    
    G4Element* Xe = new G4Element("GXeDepleted", "Xe", 5);
   
    G4Isotope* Xe129 = new G4Isotope("Xe129", 54, 129, XenonGasProperties::MassPerMole(129));
    G4Isotope* Xe131 = new G4Isotope("Xe131", 54, 131, XenonGasProperties::MassPerMole(131));
    G4Isotope* Xe132 = new G4Isotope("Xe132", 54, 132, XenonGasProperties::MassPerMole(132));
    G4Isotope* Xe134 = new G4Isotope("Xe134", 54, 134, XenonGasProperties::MassPerMole(134));
    G4Isotope* Xe136 = new G4Isotope("Xe136", 54, 136, XenonGasProperties::MassPerMole(136));

    // Bottle number 9056852
    // Xe->AddIsotope(Xe129, 27.11*perCent);
    // Xe->AddIsotope(Xe131, 27.07*perCent);
    // Xe->AddIsotope(Xe132, 28.18*perCent);
    // Xe->AddIsotope(Xe134, 8.59*perCent);
    // Xe->AddIsotope(Xe136, 2.87*perCent);

    // Bottle number 9056842
    Xe->AddIsotope(Xe129, 27.29*perCent);
    Xe->AddIsotope(Xe131, 27.07*perCent);
    Xe->AddIsotope(Xe132, 28.31*perCent);
    Xe->AddIsotope(Xe134, 8.61*perCent);
    Xe->AddIsotope(Xe136, 2.55*perCent);
   
    
    // G4cout << Xe->GetNumberOfIsotopes() << G4endl;
    // G4cout << Xe->GetIsotopeVector()->size() << G4endl;
    // for (G4int i=0; i< Xe->GetNumberOfIsotopes(); ++i) {
    //   G4cout << Xe->GetIsotope(i)->GetName() << G4endl;
    // }
    
    mat->AddElement(Xe,1);
  }

  return mat;
}

G4Material* MaterialsList::GAr(G4double pressure, G4double temperature)
{
  G4String name = "GAr";
  
  G4Material* mat = G4Material::GetMaterial(name, false);
    
  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();
      
    mat = new G4Material(name, ArgonGasProperties::Density(pressure), 1,
			 kStateGas, temperature, pressure);
    
    G4Element* Ar = nist->FindOrBuildElement("Ar");
  
    mat->AddElement(Ar,1);
  }

  return mat;
}

G4Material* MaterialsList::GXeAr(G4double pressure, G4double temperature, G4double percXe)
{
  G4String name = "GXeAr";
  
  G4Material* mat = G4Material::GetMaterial(name, false);
    
  if (mat == 0) {
    
    mat = new G4Material(name,
			 (1-(percXe/100.))*ArgonGasProperties::Density(pressure) +
			 percXe/100.*XenonGasProperties::Density(pressure),
			 2, kStateGas, temperature, pressure);
    
    G4Element* NaturalXe = new G4Element("GXeNatural", "Xe", 9);

    G4Isotope* Xe124 = new G4Isotope("Xe124", 54, 124, XenonGasProperties::MassPerMole(124));
    G4Isotope* Xe126 = new G4Isotope("Xe126", 54, 126, XenonGasProperties::MassPerMole(126));
    G4Isotope* Xe128 = new G4Isotope("Xe128", 54, 128, XenonGasProperties::MassPerMole(128));
    G4Isotope* Xe129 = new G4Isotope("Xe129", 54, 129, XenonGasProperties::MassPerMole(129));
    G4Isotope* Xe130 = new G4Isotope("Xe130", 54, 130, XenonGasProperties::MassPerMole(130));
    G4Isotope* Xe131 = new G4Isotope("Xe131", 54, 131, XenonGasProperties::MassPerMole(131));
    G4Isotope* Xe132 = new G4Isotope("Xe132", 54, 132, XenonGasProperties::MassPerMole(132));
    G4Isotope* Xe134 = new G4Isotope("Xe134", 54, 134, XenonGasProperties::MassPerMole(134));
    G4Isotope* Xe136 = new G4Isotope("Xe136", 54, 136, XenonGasProperties::MassPerMole(136));

    NaturalXe->AddIsotope(Xe124, 0.0952*perCent);
    NaturalXe->AddIsotope(Xe126, 0.089*perCent);
    NaturalXe->AddIsotope(Xe128, 1.9102*perCent);
    NaturalXe->AddIsotope(Xe129, 26.4006*perCent);
    NaturalXe->AddIsotope(Xe130, 4.071*perCent);
    NaturalXe->AddIsotope(Xe131, 21.2324*perCent);
    NaturalXe->AddIsotope(Xe132, 26.9086*perCent);
    NaturalXe->AddIsotope(Xe134, 10.4357*perCent);
    NaturalXe->AddIsotope(Xe136, 8.8573*perCent);

    G4Element* NaturalAr  = new G4Element("Argon", "Ar", 18, 39.962383123*g/mole);
       
    mat->AddElement(NaturalAr, (100-percXe)*perCent);
    mat->AddElement(NaturalXe, percXe*perCent);
  }
  
    return mat;
}

  
  
  
G4Material* MaterialsList::Steel()
{
  // Composition ranges correspond to stainless steel grade 304L
  
  G4String name = "Steel";

  G4Material* mat = G4Material::GetMaterial(name, false);
    
  if (mat == 0) {
    
    mat = new G4Material(name, 8000*kg/m3, 4);

    G4NistManager* nist = G4NistManager::Instance();
    
    G4Element* Fe = nist->FindOrBuildElement("Fe");
    mat->AddElement(Fe, 0.66);

    G4Element* Cr = nist->FindOrBuildElement("Cr");
    mat->AddElement(Cr, 0.20);

    G4Element* Mn = nist->FindOrBuildElement("Mn");
    mat->AddElement(Mn, 0.02);
    
    G4Element* Ni = nist->FindOrBuildElement("Ni");
    mat->AddElement(Ni, 0.12);
  }
    
  return mat;
}



G4Material* MaterialsList::Steel316Ti()
{
  G4String name = "Steel316Ti";

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (!mat) {
    mat = new G4Material(name, 8000*kg/m3, 6);
    
    G4NistManager* nist = G4NistManager::Instance();

    G4Element* Fe = nist->FindOrBuildElement("Fe");
    mat->AddElement(Fe, 0.636);

    G4Element* Cr = nist->FindOrBuildElement("Cr");
    mat->AddElement(Cr, 0.18);

    G4Element* Ni = nist->FindOrBuildElement("Ni");
    mat->AddElement(Ni, 0.14);

    G4Element* Mo = nist->FindOrBuildElement("Mo");
    mat->AddElement(Mo, 0.03);
    
    G4Element* Si = nist->FindOrBuildElement("Si");
    mat->AddElement(Si, 0.007);
    
    G4Element* Ti = nist->FindOrBuildElement("Ti");
    mat->AddElement(Ti, 0.007);
  }
  
  return mat;
}



G4Material* MaterialsList::Epoxy()
{
  G4String name = "Epoxy";

  G4Material* mat = G4Material::GetMaterial(name, false);
    
  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();

    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");
    G4Element* O = nist->FindOrBuildElement("O");

    mat = new G4Material(name, 1.3*g/cm3, 3);
    mat->AddElement(H, 44);
    mat->AddElement(C, 15);
    mat->AddElement(O, 7);
  }

  return mat;
}



G4Material* MaterialsList::Kovar()
{
  G4String name = "Kovar";

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();

    G4Element* Fe = nist->FindOrBuildElement("Fe");
    G4Element* Ni = nist->FindOrBuildElement("Ni");
    G4Element* Co = nist->FindOrBuildElement("Co");

    mat = new G4Material(name, 8.35*g/cm3, 3);
    mat->AddElement(Fe, 54);
    mat->AddElement(Ni, 29);
    mat->AddElement(Co, 17);
  }
  
  return mat;
}

  
  
G4Material* MaterialsList::PEEK()
{
  G4String name = "PEEK";

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();

    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");
    G4Element* O = nist->FindOrBuildElement("O");

    mat = new G4Material(name, 1.3*g/cm3, 3, kStateSolid);
    mat->AddElement(H, 12);
    mat->AddElement(C, 18);
    mat->AddElement(O,  3);
  }

  return mat;
}



G4Material* MaterialsList::Sapphire()
{
  G4Material* mat = 
    G4NistManager::Instance()->FindOrBuildMaterial("G4_ALUMINUM_OXIDE");

  return mat;
}



G4Material* MaterialsList::FusedSilica()
{
  G4Material* mat =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  return mat;
}

  
  
G4Material* MaterialsList::PS() // polystyrene
{
  G4String name = "PS";

  // Check whether material exists already in the materials table
  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();

    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");
    //      G4Element* O = nist->FindOrBuildElement("O");

    //      mat = new G4Material(name, 1320*kg/m3, 3, kStateSolid);
    mat = new G4Material(name, 1.05*g/cm3, 2, kStateSolid);
    mat->AddElement(H, 8);
    mat->AddElement(C, 8);
    //      mat->AddElement(O,  3);
  }

  return mat;
}
  
  
  
G4Material* MaterialsList::TPB()
{
  G4String name = "TPB"; // Tetraphenyl butadiene

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();
      
    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");

    mat = new G4Material(name, 1*g/cm3, 2, kStateSolid);
    mat->AddElement(H, 22);
    mat->AddElement(C, 28);
  }
    
  return mat;
}
  
  
G4Material* MaterialsList::ITO()
{
  G4String name = "ITO"; 

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();
      
    G4Element* In = nist->FindOrBuildElement("In");
    G4Element* O = nist->FindOrBuildElement("O");

    mat = new G4Material(name, 7.14*g/cm3, 2, kStateSolid);
    mat->AddElement(In, 2);
    mat->AddElement(O, 3);
  }
    
  return mat;

}


G4Material* MaterialsList::PVT()
{
  G4String name = "PVT"; // 

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();
      
    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");

    mat = new G4Material(name, 1*g/cm3, 2, kStateSolid);
    mat->AddElement(H, 4);
    mat->AddElement(C, 2);
  }
    
  return mat;

}


G4Material* MaterialsList::Kevlar()
{
  G4String name = "Kevlar"; // 

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();
      
    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");
    G4Element* N = nist->FindOrBuildElement("N");
    G4Element* O = nist->FindOrBuildElement("O");

    mat = new G4Material(name, 1.44*g/cm3, 4, kStateSolid);
    mat->AddElement(H, 10);
    mat->AddElement(C, 14);
    mat->AddElement(O, 2);
    mat->AddElement(N, 2);
  }
    
  return mat;

}



G4Material* MaterialsList::HDPE()
{
  G4String name = "HDPE"; 

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();    

    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");

    mat = new G4Material(name, .954*g/cm3, 2, kStateSolid);
    mat->AddElement(H, 4);
    mat->AddElement(C, 2);
  }
    
  return mat;

}



G4Material* MaterialsList::OpticalSilicone()
{
  G4String name = "OpticalSilicone"; 

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();    

    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");

    mat = new G4Material(name, 1.060 *g/cm3, 2, kStateSolid);
    mat->AddElement(H, 6);
    mat->AddElement(C, 2);
  }

  return mat;
}


G4Material* MaterialsList::SeF6(G4double pressure, G4double temperature)
{
  // Composition ranges correspond to Selenium Hexafluoride
  
  G4String name = "SeF6";
  G4Material* mat = G4Material::GetMaterial(name, false);
    
  if (mat == 0) {
    
    G4NistManager* nist = G4NistManager::Instance();
    G4double density = 7.887*kg/m3;

    if (pressure/bar > 9.5 && pressure/bar < 10.5)
      density *= 10.;
    else
      G4cout  << "[MaterialsList] Pressure " << pressure/bar
              << " bar not recognized for SeF6! ... Assuming 1bar. " << G4endl;
    
    mat = new G4Material(name, density, 2, kStateGas, temperature, pressure);
    G4Element* Se = nist->FindOrBuildElement("Se");
    mat->AddElement(Se, 1);
    G4Element* F = nist->FindOrBuildElement("F");
    mat->AddElement(F, 6);
  }    

  return mat;
}




G4Material* MaterialsList::CopyMaterial(G4Material* original, G4String newname)
{
  G4Material* newmat = G4Material::GetMaterial(newname, false);
  
  if (newmat == 0) {
    G4double z = original->GetZ();
    G4double a = original->GetA();
    G4double density = original->GetDensity();
    G4State state = original->GetState();
    G4double temperature = original->GetTemperature();
    G4double pressure = original->GetPressure();
    
    newmat = 
      new G4Material(newname, z, a, density, state, temperature, pressure);
  }

  return newmat;
}



G4Material* MaterialsList::FakeDielectric(G4Material* model_mat, G4String name)
{
  return CopyMaterial(model_mat, name);
}
