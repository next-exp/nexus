// ----------------------------------------------------------------------------
// nexus | MaterialsList.cc
//
// Definition of materials of common use.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "MaterialsList.h"
#include "XenonGasProperties.h"
#include "ArgonGasProperties.h"
#include "HeliumGasProperties.h"

#include <G4Material.hh>
#include <G4Element.hh>
#include <G4NistManager.hh>

using namespace nexus;


G4Material* MaterialsList::GXe(G4double pressure, G4double temperature)
{
  G4Material* mat = GXe_bydensity(XenonGasProperties::Density(pressure),
				  temperature, pressure);

  return mat;
}


G4Material* MaterialsList::GXe_bydensity(G4double density,
					 G4double temperature,
					 G4double pressure)
{
  G4String name = "GXe";

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();

    mat = new G4Material(name, density, 1,
			 kStateGas, temperature, pressure);

    G4Element* Xe = nist->FindOrBuildElement("Xe");

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


G4Material* MaterialsList::GXeHe(G4double pressure,
				 G4double temperature,
				 G4double percXe, G4int mass_num)
{
  G4String name = "GXeHe";

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {

    G4double prop_xe = percXe * perCent;
    G4double prop_he = 1. - prop_xe;

    mat = new G4Material(name,
			 prop_xe * XenonGasProperties::Density(pressure)
			 + prop_he * HeliumGasProperties::Density(pressure),
			 2, kStateGas, temperature, pressure);


    G4Element* enrichedXe = new G4Element("GXeEnriched", "enrichedXe", 6);
    G4Isotope* Xe129 = new G4Isotope("Xe129", 54, 129,
				     XenonGasProperties::MassPerMole(129));
    G4Isotope* Xe130 = new G4Isotope("Xe130", 54, 130,
				     XenonGasProperties::MassPerMole(130));
    G4Isotope* Xe131 = new G4Isotope("Xe131", 54, 131,
				     XenonGasProperties::MassPerMole(131));
    G4Isotope* Xe132 = new G4Isotope("Xe132", 54, 132,
				     XenonGasProperties::MassPerMole(132));
    G4Isotope* Xe134 = new G4Isotope("Xe134", 54, 134,
				     XenonGasProperties::MassPerMole(134));
    G4Isotope* Xe136 = new G4Isotope("Xe136", 54, 136,
				     XenonGasProperties::MassPerMole(136));

    enrichedXe->AddIsotope(Xe129, 0.0656392*perCent);
    enrichedXe->AddIsotope(Xe130, 0.0656392*perCent);
    enrichedXe->AddIsotope(Xe131, 0.234361*perCent);
    enrichedXe->AddIsotope(Xe132, 0.708251*perCent);
    enrichedXe->AddIsotope(Xe134, 8.6645*perCent);
    enrichedXe->AddIsotope(Xe136, 90.2616*perCent);

    G4Element * Helium = new G4Element("Helium", "Helium", 1);
    G4Isotope * He     = new G4Isotope("He", 2, mass_num,
				       HeliumGasProperties::MassPerMole(mass_num));
    Helium->AddIsotope(He, 100 * perCent);

    mat->AddElement(Helium, prop_he);
    mat->AddElement(enrichedXe, prop_xe);


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


G4Material* MaterialsList::CarbonFiber()
{
  G4String name = "CarbonFiber";

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();

    G4Element* C = nist->FindOrBuildElement("C");

    mat = new G4Material(name, 1.6*g/cm3, 1);
    mat->AddElement(C, 1);
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


G4Material* MaterialsList::LYSO()
{
  G4String name = "LYSO";

  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {
    G4NistManager* nist = G4NistManager::Instance();

    G4Element* Si = nist->FindOrBuildElement("Si");
    G4Element* O = nist->FindOrBuildElement("O");
    G4Element* Y = nist->FindOrBuildElement("Y");
    G4Element* Lu = nist->FindOrBuildElement("Lu");

    // Saint Gobain LYSO cristals
    mat = new G4Material(name, 7.1*g/cm3, 4, kStateSolid);
    mat->AddElement(Si, 0.064);
    mat->AddElement(O, 0.182);
    mat->AddElement(Y, 0.040);
    mat->AddElement(Lu, 0.714);
  }

  return mat;
}


G4Material* MaterialsList::FR4()
{
  // Material widely used for printed circuit boards

  G4String name = "FR4";
  G4Material* mat = G4Material::GetMaterial(name, false);

  if (mat == 0) {

    G4NistManager* nist = G4NistManager::Instance();
    G4double density = 1.850*g/cm3;

    mat = new G4Material(name, density, 3, kStateSolid);

    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");
    G4Element* O = nist->FindOrBuildElement("O");

    mat->AddElement(H, 12);
    mat->AddElement(C, 18);
    mat->AddElement(O,  3);
  }

  return mat;
}


G4Material* MaterialsList::CopyMaterial(G4Material* original, G4String newname)
{
  G4Material* newmat = G4Material::GetMaterial(newname, false);

  if (newmat == 0) {
    G4double density = original->GetDensity();
    G4State state = original->GetState();
    G4double temperature = original->GetTemperature();
    G4double pressure = original->GetPressure();

    G4int n_elem = original->GetNumberOfElements();
    if (n_elem == 1){
      G4double z = original->GetZ();
      G4double a = original->GetA();
      newmat =
	new G4Material(newname, z, a, density, state, temperature, pressure);
    } else {
      // Gas mixture
      const G4double * fractions = original->GetFractionVector();
      newmat =
	new G4Material(newname, density, n_elem, state, temperature, pressure);
      for (G4int i = 0; i < n_elem; ++i)
	newmat->AddElement(new G4Element(original->GetElement(i)->GetName(),
					 original->GetElement(i)->GetSymbol(),
					 original->GetElement(i)->GetZ(),
					 original->GetElement(i)->GetA()),
			   fractions[i]);
    }
  }

  return newmat;
}


G4Material* MaterialsList::FakeDielectric(G4Material* model_mat, G4String name)
{
  return CopyMaterial(model_mat, name);
}
