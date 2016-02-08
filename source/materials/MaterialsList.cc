// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 27 Mar 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "MaterialsList.h"

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

    G4double density = 5.5*kg/m3;

    if (pressure/bar > 0.9 && pressure/bar < 1.1)
      density = 5.324*kg/m3;
    else if (pressure/bar > 1.9 && pressure/bar < 2.1)
      density = 10.7*kg/m3;
    else if (pressure/bar > 4.9 && pressure/bar < 5.1)
      density = 27.2*kg/m3;
    else if (pressure/bar > 9.9 && pressure/bar < 10.1)
      density = 55.587*kg/m3;
    else if (pressure/bar > 14.9 && pressure/bar < 15.1)
      density = 85.95 *kg/m3;
    else if (pressure/bar > 19.9 && pressure/bar < 20.1)
      density = 118.4*kg/m3;
    else if (pressure/bar > 29.9 && pressure/bar < 30.1)
      density = 193.6*kg/m3;
    else if (pressure/bar > 39.9 && pressure/bar < 40.1)
      density = 284.3*kg/m3;
    else
      G4cout  << "[MaterialsList] Pressure not recognized! " 
              << pressure/bar << G4endl;
      
    mat = new G4Material(name, density, 1,
			 kStateGas, temperature, pressure);
    
    G4Element* Xe = nist->FindOrBuildElement("Xe");
    mat->AddElement(Xe,1);
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
