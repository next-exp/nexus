// ----------------------------------------------------------------------------
// nexus | MaterialsList.cc
//
// Definition of materials of common use.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "MaterialsList.h"
#include "XenonProperties.h"
#include "ArgonGasProperties.h"
#include "HeliumGasProperties.h"

#include <G4Material.hh>
#include <G4Element.hh>
#include <G4NistManager.hh>

#include <vector>
#include <utility> 

using namespace nexus;
using namespace CLHEP;

namespace materials {
    // Calculation of Compressibilty factor from https://next.ific.uv.es/DocDB/0011/001183/001/GasDensity_2021-04-23.pdf
    G4double CompressibilityFactor(G4double pressure, G4double temperature) {
        double T_r = temperature/(289.733*kelvin);
        double P_r = pressure/ (58.420*bar);
        double B0 = 0.083 - (0.422 / pow(T_r, 1.6));   //Second virial coefficient    
        double z = 1 + B0*(P_r/T_r);  //Compressibility factor(in G4unit)
        return z;       
    }

    // Function to calculate gas density based on Xe isotopic composition
    G4double CalculateGasDensityFromIsotopicComposition(G4double pressure, G4double temperature,
     const std::vector<std::pair<int, double>>& isotopicComposition)   //pressure in G4unit , temperature in G4unit
      {
        const double R = 8.314*joule/(mole*kelvin); // Ideal gas constant in G4unit
        double average_molar_mass = 0.0; 

        for (const auto& iso : isotopicComposition) {
            int mass_number = iso.first;
            double percentage = iso.second;
            double molar_mass = XenonMassPerMole(mass_number);
            average_molar_mass += molar_mass * percentage;
        }


        double z = CompressibilityFactor(pressure, temperature);
        double density = (pressure * average_molar_mass) / (z * R * temperature); 
        return density; //in G4unit
    }

  G4Material* GXe(G4double pressure, G4double temperature) {
    std::vector<std::pair<int, double>> isotopicComposition = {
      {124, 0.095*perCent}, {126, 0.089*perCent}, {128, 1.910*perCent},
      {129, 26.401*perCent}, {130, 4.071*perCent}, {131, 21.232*perCent}, 
      {132, 26.909*perCent}, {134,10.436*perCent}, {136, 8.857*perCent}
    };

    G4double gas_density = CalculateGasDensityFromIsotopicComposition(pressure , temperature, isotopicComposition);
    G4Material* mat = GXe_bydensity(gas_density, temperature, pressure);
    return mat;
  }

  G4Material* GXe_bydensity(G4double density, G4double temperature, G4double pressure) {
    G4String name = "GXe";
    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();
      mat = new G4Material(name, density, 1, kStateGas, temperature, pressure);
      G4Element* Xe = nist->FindOrBuildElement("Xe");
      mat->AddElement(Xe, 1);
    }

    return mat;
  }
  
  G4Material* GXeEnriched(G4double pressure, G4double temperature) {
    std::vector<std::pair<int, double>> isotopicComposition = {
      {129, 0.0656392*perCent}, {130, 0.0656392*perCent}, {131, 0.234361*perCent},
      {132, 0.708251*perCent}, {134, 8.6645*perCent}, {136, 90.2616*perCent}
    };

    G4double gas_density = CalculateGasDensityFromIsotopicComposition(pressure, temperature, isotopicComposition);
    G4Material* mat = GXeEnriched_bydensity(gas_density, temperature, pressure, isotopicComposition);
    return mat;
  }

   G4Material* GXeEnriched_bydensity(G4double density,
              G4double temperature,
              G4double pressure,
              const std::vector<std::pair<int, double>>& isotopicComposition )
  {
    G4String name = "GXeEnriched";

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {

      mat = new G4Material(name, density, 1,
        kStateGas, temperature, pressure);

      G4Element* Xe = new G4Element("GXeEnriched", "Xe", isotopicComposition.size());

        for (const auto& isotopeInfo : isotopicComposition) {
            int massNumber = isotopeInfo.first;
            double abundance = isotopeInfo.second;
            G4String isotopeName = "Xe" + std::to_string(massNumber);
            G4Isotope* isotope = new G4Isotope(isotopeName, 54, massNumber, XenonMassPerMole(massNumber));
            Xe->AddIsotope(isotope, abundance);
        }
      mat->AddElement(Xe,1);
    }

    return mat;
  }

  G4Material* GXeDepleted(G4double pressure, G4double temperature) {
    std::vector<std::pair<int, double>> isotopicComposition = {
      {124, 0.102*perCent}, {126, 0.201*perCent}, {128, 3.065*perCent}, {129, 24.900*perCent}, {130, 5.361*perCent},
      {131, 23.280*perCent}, {132, 30.666*perCent}, {134, 9.822*perCent}, {136, 2.602*perCent}
    };

    G4double gas_density = CalculateGasDensityFromIsotopicComposition(pressure, temperature, isotopicComposition);
    G4Material* mat = GXeDepleted_bydensity(gas_density, temperature, pressure, isotopicComposition);
    return mat;
  }

  G4Material* GXeDepleted_bydensity(G4double density,
              G4double temperature,
              G4double pressure,
              const std::vector<std::pair<int, double>>& isotopicComposition)
  {
    G4String name = "GXeDepleted";

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {

      mat = new G4Material(name, density, 1,
        kStateGas, temperature, pressure);


      G4Element* Xe = new G4Element("GXeDepleted", "Xe",isotopicComposition.size());
      
      for (const auto& isotopeInfo : isotopicComposition) {
            int massNumber = isotopeInfo.first;
            double abundance = isotopeInfo.second;
            G4String isotopeName = "Xe" + std::to_string(massNumber);
            G4Isotope* isotope = new G4Isotope(isotopeName, 54, massNumber, XenonMassPerMole(massNumber));
            // Bottle number 9056842//https://next.ific.uv.es/DocDB/0004/000481/001/IsotopicComposition_20170921.pdf
            Xe->AddIsotope(isotope, abundance); 
        }
      mat->AddElement(Xe,1);
    }

    return mat;
  }

  G4Material* LXe()
  {
    G4String name = "LXe";

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4double density = LXeDensity();
      mat = new G4Material(name, density, 1,
        kStateLiquid, 163.*kelvin, 0.9*bar);

      G4Element* Xe = nist->FindOrBuildElement("Xe");

      mat->AddElement(Xe, 1);
    }

    return mat;
  }

  G4Material* GAr(G4double pressure, G4double temperature)
  {
    G4String name = "GAr";

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      mat = new G4Material(name, ArgonDensity(pressure), 1,
        kStateGas, temperature, pressure);

      G4Element* Ar = nist->FindOrBuildElement("Ar");

      mat->AddElement(Ar,1);
    }

    return mat;
  }

  G4Material* GXeAr(G4double pressure, G4double temperature, G4double percXe) 
  {
    G4String name = "GXeAr";

    G4Material* mat = G4Material::GetMaterial(name, false);

    std::vector<std::pair<int, double>> isotopicComposition = {
      {124, 0.0952*perCent}, {126, 0.089*perCent}, {128, 1.9102*perCent}, {129, 26.4006*perCent},
      {130, 4.071*perCent}, {131, 21.2324*perCent}, {132, 26.9086*perCent}, {134, 10.4357*perCent}, {136, 8.8573*perCent}
    };

    G4double GXeNatural_density = CalculateGasDensityFromIsotopicComposition(pressure, temperature, isotopicComposition);

    if (mat == 0) {

      mat = new G4Material(name,
        (1-(percXe/100.))*ArgonDensity(pressure) +
        percXe/100.*GXeNatural_density,
        2, kStateGas, temperature, pressure);

      G4Element* NaturalXe = new G4Element("GXeNatural", "Xe", isotopicComposition.size());


      for (const auto& isotopeInfo : isotopicComposition) {
            int massNumber = isotopeInfo.first;
            double abundance = isotopeInfo.second;
            G4String isotopeName = "Xe" + std::to_string(massNumber);
            G4Isotope* isotope = new G4Isotope(isotopeName, 54, massNumber, XenonMassPerMole(massNumber));
            NaturalXe->AddIsotope(isotope, abundance); 
        }


      G4Element* NaturalAr  = new G4Element("Argon", "Ar", 18, 39.962383123*g/mole);

      mat->AddElement(NaturalAr, (100-percXe)*perCent);
      mat->AddElement(NaturalXe, percXe*perCent);
    }

      return mat;
  }




  G4Material* GXeHe(G4double pressure,
          G4double temperature,
          G4double percXe, G4int mass_num)
  {
    G4String name = "GXeHe";

    std::vector<std::pair<int, double>> isotopicComposition = {
      {129, 0.0656392*perCent}, {130, 0.0656392*perCent}, {131, 0.234361*perCent},
      {132, 0.708251*perCent}, {134, 8.6645*perCent}, {136, 90.2616*perCent}
    };

    G4double GXeEnriched_density = CalculateGasDensityFromIsotopicComposition(pressure, temperature, isotopicComposition);

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {

      G4double prop_xe = percXe * perCent;
      G4double prop_he = 1. - prop_xe;

      mat = new G4Material(name,
        prop_xe * GXeEnriched_density
        + prop_he * HeliumDensity(pressure),
        2, kStateGas, temperature, pressure);


      G4Element* enrichedXe = new G4Element("GXeEnriched", "enrichedXe", isotopicComposition.size());
      for (const auto& isotopeInfo : isotopicComposition) {
            int massNumber = isotopeInfo.first;
            double abundance = isotopeInfo.second;
            G4String isotopeName = "Xe" + std::to_string(massNumber);
            G4Isotope* isotope = new G4Isotope(isotopeName, 54, massNumber, XenonMassPerMole(massNumber));
            enrichedXe->AddIsotope(isotope, abundance); 
        }
      G4Element * Helium = new G4Element("Helium", "Helium", 1);
      G4Isotope * He     = new G4Isotope("He", 2, mass_num,
                HeliumMassPerMole(mass_num));
      Helium->AddIsotope(He, 100 * perCent);

      mat->AddElement(Helium, prop_he);
      mat->AddElement(enrichedXe, prop_xe);


    }

      return mat;
  }





  G4Material* Steel()
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



  G4Material* Steel316Ti()
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



  G4Material* Epoxy()
  {
    // Definition taken from the Geant4 advanced example "Composite Calorimeter"
    // (Geant4/examples/advanced/composite_calorimeter/dataglobal/material.cms).

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



  G4Material* Kovar()
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



  G4Material* PEEK()
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



  G4Material* Sapphire()
  {
    G4Material* mat =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_ALUMINUM_OXIDE");

    return mat;
  }



  G4Material* FusedSilica()
  {
    G4Material* mat =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    return mat;
  }



  G4Material* PS() // polystyrene
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



  G4Material* TPB()
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


  G4Material* ITO()
  {
    // Density data from  https://doi.org/10.1116/1.1371326
    // Composition data from: https://en.wikipedia.org/wiki/Indium_tin_oxide

    G4String name = "ITO";

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* In = nist->FindOrBuildElement("In");
      G4Element* O  = nist->FindOrBuildElement("O");
      G4Element* Sn = nist->FindOrBuildElement("Sn");

      mat = new G4Material(name, 6.8*g/cm3, 3, kStateSolid);
      mat->AddElement(In, 74. * perCent);
      mat->AddElement(O , 18. * perCent);
      mat->AddElement(Sn,  8. * perCent);
    }
    return mat;
  }


  G4Material* PVT()
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


  G4Material* Kevlar()
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


  G4Material* HDPE()
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


  G4Material* PE1000()
  {
    G4String name = "PE1000";

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* H = nist->FindOrBuildElement("H");
      G4Element* C = nist->FindOrBuildElement("C");

      mat = new G4Material(name, .93*g/cm3, 2, kStateSolid);
      mat->AddElement(H, 4);
      mat->AddElement(C, 2);
    }

    return mat;

  }


  G4Material* PE500()
  {
    G4String name = "PE500";

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* H = nist->FindOrBuildElement("H");
      G4Element* C = nist->FindOrBuildElement("C");

      mat = new G4Material(name, .96*g/cm3, 2, kStateSolid);
      mat->AddElement(H, 4);
      mat->AddElement(C, 2);
    }

    return mat;

  }


  G4Material* OpticalSilicone()
  {
    // Silicone resin with a methyl group
    // (https://en.wikipedia.org/wiki/Silicone_resin)
    G4String name = "OpticalSilicone";

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* H  = nist->FindOrBuildElement("H");
      G4Element* C  = nist->FindOrBuildElement("C");
      G4Element* O  = nist->FindOrBuildElement("O");
      G4Element* Si = nist->FindOrBuildElement("Si");

      mat = new G4Material(name, 1.05*g/cm3, 4, kStateSolid);
      mat->AddElement(H,  3);
      mat->AddElement(C,  1);
      mat->AddElement(Si, 1);
      mat->AddElement(O,  1);
    }

    return mat;
  }


  G4Material* SeF6(G4double pressure, G4double temperature)
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


  G4Material* FR4()
  {
    // FR-4 is a composite material widely used for printed circuits boards.
    // It consists of woven fiberglass cloth with an epoxy resin binder that is
    // flame resistant. Typical proportions are 60% fused silica and 40% epoxy.

    G4String name = "FR4";
    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      mat = new G4Material(name, 1.850*g/cm3, 2, kStateSolid);
      mat->AddMaterial(materials::FusedSilica(), 0.60);
      mat->AddMaterial(materials::Epoxy(),       0.40);
    }

    return mat;
  }


  G4Material* Limestone()
  {
    // Limestone modelled as pure calcium carbonate
    G4String name = "Limestone";

    G4Material* mat = G4Material::GetMaterial(name, false);
    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* Ca = nist->FindOrBuildElement("Ca");
      G4Element* C  = nist->FindOrBuildElement("C");
      G4Element* O  = nist->FindOrBuildElement("O");
      mat = new G4Material(name, 2.711*g/cm3, 3, kStateSolid);
      mat->AddElement(Ca, 1);
      mat->AddElement(C , 1);
      mat->AddElement(O , 1);
    }
    return mat;
  }


  G4Material* CopyMaterial(G4Material* original, const G4String& newname)
  {
    G4Material* newmat = G4Material::GetMaterial(newname, false);

    if (newmat == 0) {

      G4double density     = original->GetDensity();
      G4double temperature = original->GetTemperature();
      G4double pressure    = original->GetPressure();
      G4State  state       = original->GetState();
      G4int    n_elem      = original->GetNumberOfElements();

      if (n_elem == 1) {
        G4double z = original->GetZ();
        G4double a = original->GetA();
        newmat = new G4Material(newname, z, a, density, state, temperature, pressure);
      }
      else {
        const G4double* fractions = original->GetFractionVector();
        newmat = new G4Material(newname, density, n_elem, state, temperature, pressure);
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


  G4Material* FakeDielectric(G4Material* model_mat, G4String name)
  {
    return materials::CopyMaterial(model_mat, name);
  }



  G4Material* TPH()
  {
    G4String name = "TPH"; // p - terphenyl

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* H = nist->FindOrBuildElement("H");
      G4Element* C = nist->FindOrBuildElement("C");

      mat = new G4Material(name, 1.24*g/cm3, 2, kStateSolid);
      mat->AddElement(H, 14);
      mat->AddElement(C, 18);
    }

    return mat;
  }


  // Pethylene (cladding material)
  G4Material* Pethylene()
  {
    G4String name = "Pethylene"; //

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* H = nist->FindOrBuildElement("H");
      G4Element* C = nist->FindOrBuildElement("C");

      mat = new G4Material(name, 1.200 * g/cm3, 2, kStateSolid);
      mat->AddElement(H, 4);
      mat->AddElement(C, 2);
    }

    return mat;
  }


  // FPethylene (fluorinated polyethylene - cladding material)
  G4Material* FPethylene()
  {
    G4String name = "FPethylene"; //

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* H = nist->FindOrBuildElement("H");
      G4Element* C = nist->FindOrBuildElement("C");

      mat = new G4Material(name, 1.400 * g/cm3, 2, kStateSolid);
      mat->AddElement(H, 4);
      mat->AddElement(C, 2);
    }

    return mat;
  }



  // PMMA == PolyMethylmethacrylate (cladding)
  G4Material* PMMA()
  {
    G4String name = "PMMA"; //

    G4Material* mat = G4Material::GetMaterial(name, false);

    if (mat == 0) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* H = nist->FindOrBuildElement("H");
      G4Element* C = nist->FindOrBuildElement("C");
      G4Element* O = nist->FindOrBuildElement("O");

      mat = new G4Material(name, 1.190 * g/cm3, 3, kStateSolid);
      mat->AddElement(H, 8);
      mat->AddElement(C, 5);
      mat->AddElement(O, 2);
    }

    return mat;
  }


  G4Material* PEDOT()
  {
    // Poly(3,4-ethylenedioxythiophene) is a conductive polymer.
    // Linear Formula: (C6H4O2S)n
    // Density of dry coating with poly(styrenesulfonate): 1.011 g/cm3

    G4String name = "PEDOT";
    G4Material* material = G4Material::GetMaterial(name, false);

    if (!material) {
      G4NistManager* nist = G4NistManager::Instance();

      G4Element* H = nist->FindOrBuildElement("H");
      G4Element* C = nist->FindOrBuildElement("C");
      G4Element* O = nist->FindOrBuildElement("O");
      G4Element* S = nist->FindOrBuildElement("S");

      material = new G4Material(name, 1.011*g/cm3, 4, kStateSolid);
      material->AddElement(H,4);
      material->AddElement(C,6);
      material->AddElement(O,2);
      material->AddElement(S,1);
    }

    return material;
  }

}