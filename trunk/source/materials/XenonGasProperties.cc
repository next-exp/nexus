// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 25 Dec 2010
//
//  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "XenonGasProperties.h"

#include <G4AnalyticalPolSolver.hh>
#include <G4MaterialPropertiesTable.hh>
#include "CLHEP/Units/PhysicalConstants.h"
//#include <bhep/utilities.h>



namespace nexus {
  
  
  XenonGasProperties::XenonGasProperties(G4double pressure,
					 G4double temperature):
    _pressure(pressure), _temperature(temperature)
  {
    //Density();
  }
  
  
  
  XenonGasProperties::~XenonGasProperties()
  {
  }
  

  
  G4double XenonGasProperties::Density()
  {

    // G4double data[50][1] =
    //   {{ 1.0,  5.29074},
    //    { 2.0, 10.63668},
    //    { 3.0, 16.03932},
    //    { 4.0, 21.50023},
    //    { 5.0, 27.02107},
    //   };

    // Ideal gas state equation P*V = n*R*T
    // I'm assuming atmosphere = bar 

    // values for R
    //  8.3145 J mol-1 K-1
    //  0.0831451 L bar K-1 mol-1
    //  82.058 cm3 atm mol-1 K-1
    // value for molar mass: 131.29 g/mol

    G4double _density = (_pressure/atmosphere)*131.29/(_temperature*82.058);

    return _density;
  }
  

  
  G4double XenonGasProperties::RefractiveIndex(G4double energy)
  {
    // Formula for the refractive index taken from
    // A. Baldini et al., "Liquid Xe scintillation calorimetry 
    // and Xe optical properties", arXiv:physics/0401072v1 [physics.ins-det]
    
    // The Lorentz-Lorenz equation (also known as Clausius-Mossotti equation)
    // relates the refractive index of a fluid with its density:
    // (n^2 - 1) / (n^2 + 2) = - A · d_M,     (1)
    // where n is the refractive index, d_M is the molar density and
    // A is the first refractivity viral coefficient:
    // A(E) = \sum_i^3 P_i / (E^2 - E_i^2),   (2)
    // with:
    G4double P[3] = {71.23, 77.75, 1384.89}; // [eV^3 cm3 / mole]
    G4double E[3] = {8.4, 8.81, 13.2};       // [eV]

    // Note.- Equation (1) has, actually, a sign difference with respect 
    // to the one appearing in the reference. Otherwise, it yields values
    // for the refractive index below 1.

    // Let's calculate the virial coefficient.
    // We won't use the implicit system of units of Geant4 because
    // it results in loss of numerical precision.

    energy = energy / eV;
    G4double virial = 0.;

    for (G4int i=0; i<3; i++)
      virial = virial + P[i] / (energy*energy - E[i]*E[i]);
    
    G4double density = 6.E-3;
    G4double mol_density = density / 131.29;
    G4double alpha = virial * mol_density;
    
    // Isolating now the n2 from equation (1) and taking the square root
    G4double n2 = (1. - 2*alpha) / (1. + alpha);

    if (n2 < 1.) {
 //      G4String msg = "Non-physical refractive index for energy "
	// + bhep::to_string(energy) + " eV. Use n=1 instead.";
 //      G4Exception("[XenonGasProperties]", "RefractiveIndex()",
	// 	  JustWarning, msg);
      n2 = 1.;
    }
    
    return sqrt(n2);
  }
  

  
  G4double XenonGasProperties::Scintillation(G4double energy)
  {
    // FWHM and peak of emission extracted from paper: 
    // Physical review A, Volume 9, Number 2, 
    // February 1974. Koehler, Ferderber, Redhead and Ebert.
    // Pressure must be in atm = bar

    G4double pressure = _pressure/atmosphere;

    G4double Wavelength_peak = (0.05 * pressure + 169.45)*nm;
    G4double Wavelength_sigma = 
        2.*sqrt(2*log(2)) * (-0.117 * pressure + 15.42)*nm;

    G4double Energy_peak = (h_Planck*c_light/Wavelength_peak);
    G4double Energy_sigma = (h_Planck*c_light*Wavelength_sigma/pow(Wavelength_peak,2));
    // G4double bin = 6*Energy_sigma/500;

    G4double intensity = 
	  exp(-pow(Energy_peak/eV-energy/eV,2)/(2*pow(Energy_sigma/eV, 2)))/(Energy_sigma/eV*sqrt(pi*2.));

    return intensity;
  }


  
  void XenonGasProperties::Scintillation
  (G4int entries, G4double* energy, G4double* intensity)
  {
    for (G4int i=0; i<entries; i++)
      intensity[i] = Scintillation(energy[i]);
  }
  
  
  
  G4double XenonGasProperties::ELLightYield(G4double field_strength) const
  {
    // Empirical formula taken from
    // C.M.B. Monteiro et al., JINST 2 (2007) P05001.

    // Y/x = (a E/p + b) p,
    // where Y/x is the number of photons per unit lenght (cm),
    // E is the electric field strength, p is the pressure, and a and b
    // are empirically determined constants:
    const G4double a = 140. / kilovolt;
    const G4double b = 116. / (bar*cm);

    G4double yield = (a * field_strength/_pressure - b) * _pressure;
    if (yield < 0.) yield = 0.;

    return yield;
  }
  
  
} // end namespace nexus
