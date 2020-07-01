// ----------------------------------------------------------------------------
// nexus | ArgonGasProperties.cc
//
// This class collects the relevant physical properties of gaseous argon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ArgonGasProperties.h"

#include <G4AnalyticalPolSolver.hh>
#include <G4MaterialPropertiesTable.hh>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <stdexcept>


namespace nexus {

  using namespace CLHEP; 
  
  ArgonGasProperties::ArgonGasProperties(G4double pressure,
					 G4double /*temperature*/):
    pressure_(pressure)
  {
    //Density();
  }
  
  
  
  ArgonGasProperties::~ArgonGasProperties()
  {
  }
  

  
  G4double ArgonGasProperties::Density(G4double pressure)
  {
    //These values are for a temperature of 300 K
    // taken from http://www.nist.gov/srd/upload/jpcrd363.pdf
    G4double density = 1.60279*kg/m3;
    
    if (pressure/bar > 0.9 && pressure/bar < 1.1)
      density = 1.60279*kg/m3;
    else if (pressure/bar > 1.9 && pressure/bar < 2.1)
      density = 3.20719*kg/m3;
    else if (pressure/bar > 4.9 && pressure/bar < 5.1)
      density = 8.032*kg/m3;
    else if (pressure/bar > 9.9 && pressure/bar < 10.1)
      density = 16.1118*kg/m3;
    else if (pressure/bar > 14.9 && pressure/bar < 15.1)
      density = 24.2369 *kg/m3;
    else if (pressure/bar > 19.9 && pressure/bar < 20.1)
      density = 32.4066*kg/m3;
    else if (pressure/bar > 29.9 && pressure/bar < 30.1)
      density = 48.8708*kg/m3;
    else if (pressure/bar > 39.9 && pressure/bar < 40.1)
      density = 65.494*kg/m3;
    else
      G4Exception("[ArgonGaseousProperties]", "Density()", FatalException,
                  "Unknown argon density for this pressure!");     
   
    return density;
  }

  
  

  
 //  G4double ArgonGasProperties::RefractiveIndex(G4double energy)
 //  {
 //    // Formula for the refractive index taken from
 //    // A. Baldini et al., "Liquid Xe scintillation calorimetry 
 //    // and Xe optical properties", arXiv:physics/0401072v1 [physics.ins-det]
    
 //    // The Lorentz-Lorenz equation (also known as Clausius-Mossotti equation)
 //    // relates the refractive index of a fluid with its density:
 //    // (n^2 - 1) / (n^2 + 2) = - A · d_M,     (1)
 //    // where n is the refractive index, d_M is the molar density and
 //    // A is the first refractivity viral coefficient:
 //    // A(E) = \sum_i^3 P_i / (E^2 - E_i^2),   (2)
 //    // with:
 //    G4double P[3] = {71.23, 77.75, 1384.89}; // [eV^3 cm3 / mole]
 //    G4double E[3] = {8.4, 8.81, 13.2};       // [eV]

 //    // Note.- Equation (1) has, actually, a sign difference with respect 
 //    // to the one appearing in the reference. Otherwise, it yields values
 //    // for the refractive index below 1.

 //    // Let's calculate the virial coefficient.
 //    // We won't use the implicit system of units of Geant4 because
 //    // it results in loss of numerical precision.

 //    energy = energy / eV;
 //    G4double virial = 0.;

 //    for (G4int i=0; i<3; i++)
 //      virial = virial + P[i] / (energy*energy - E[i]*E[i]);

 //    // Need to use g/cm3    
 //    G4double density = Density(_pressure) / g * cm3;

 //    G4double mol_density = density / 131.29;
 //    G4double alpha = virial * mol_density;
    
 //    // Isolating now the n2 from equation (1) and taking the square root
 //    G4double n2 = (1. - 2*alpha) / (1. + alpha);
 //    if (n2 < 1.) {
 // //      G4String msg = "Non-physical refractive index for energy "
 // 	// + bhep::to_string(energy) + " eV. Use n=1 instead.";
 // //      G4Exception("[ArgonGasProperties]", "RefractiveIndex()",
 // 	// 	  JustWarning, msg);
 //      n2 = 1.;
 //    }
    
 //    return sqrt(n2);
 //  }
  

  
 //  G4double ArgonGasProperties::Scintillation(G4double energy)
 //  {
 //    // FWHM and peak of emission extracted from paper: 
 //    // Physical review A, Volume 9, Number 2, 
 //    // February 1974. Koehler, Ferderber, Redhead and Ebert.
 //    // Pressure must be in atm = bar

 //    G4double pressure = _pressure/atmosphere;

 //    G4double Wavelength_peak = (0.05 * pressure + 169.45)*nm;
 //    G4double Wavelength_sigma = 
 //        2.*sqrt(2*log(2)) * (-0.117 * pressure + 15.42)*nm;

 //    G4double Energy_peak = (h_Planck*c_light/Wavelength_peak);
 //    G4double Energy_sigma = (h_Planck*c_light*Wavelength_sigma/pow(Wavelength_peak,2));
 //    // G4double bin = 6*Energy_sigma/500;

 //    G4double intensity = 
 // 	  exp(-pow(Energy_peak/eV-energy/eV,2)/(2*pow(Energy_sigma/eV, 2)))/(Energy_sigma/eV*sqrt(pi*2.));

 //    return intensity;
 //  }


  
 //  void ArgonGasProperties::Scintillation
 //  (G4int entries, G4double* energy, G4double* intensity)
 //  {
 //    for (G4int i=0; i<entries; i++)
 //      intensity[i] = Scintillation(energy[i]);
 //  }
  
  
  
  G4double ArgonGasProperties::ELLightYield(G4double field_strength) const
  {
    // Empirical formula taken from
    // C.M.B. Monteiro thesis https://estudogeral.sib.uc.pt/bitstream/10316/14635/3/Tese_Doutoramento_Cristina%20Monteiro.pdf.

    // Y/x = (a E/p + b) p,
    // where Y/x is the number of photons per unit lenght (cm),
    // E is the electric field strength, p is the pressure, and a and b
    // are empirically determined constants:
    const G4double a = 81. / kilovolt;
    const G4double b = 47. / (bar*cm);

    G4double yield = (a * field_strength/pressure_ - b) * pressure_;
    if (yield < 0.) yield = 0.;

    return yield;
  }
  
  
} // end namespace nexus
