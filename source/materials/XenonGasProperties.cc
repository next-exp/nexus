// ----------------------------------------------------------------------------
// nexus | XenonGasProperties.cc
//
// This class collects the relevant physical properties of gaseous xenon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "XenonGasProperties.h"

#include <G4AnalyticalPolSolver.hh>
#include <G4MaterialPropertiesTable.hh>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <stdexcept>


namespace nexus {

  using namespace CLHEP;

  XenonGasProperties::XenonGasProperties(G4double pressure,
					 G4double /*temperature*/):
    pressure_(pressure)
    //, _temperature(temperature)
  {
    //Density();
  }



  XenonGasProperties::~XenonGasProperties()
  {
  }



  G4double XenonGasProperties::Density(G4double pressure)
  {
    G4double density = 5.324*kg/m3;
    const G4int n_pressures = 6;

    // These values are taken from O. Sifner and J. Klomfar, "Thermodynamic Properties of Xenon from the Triple Point to 800 K with Pressures up to 350 MPa", J. Phys. Chem. Ref. Data, Vol. 23, No. 1, 1994
    // We assume T = 300 K and perform a linear interpolation between any pair of values.

    G4double data[n_pressures][2] =
      {{ 1.0*bar,  5.290*kg/m3},
       { 5.0*bar, 27.01*kg/m3},
       { 10.0*bar, 55.55*kg/m3},
       { 20.0*bar, 118.36*kg/m3},
       { 30.0*bar, 191.51*kg/m3},
       { 40.0*bar, 280.40*kg/m3}
      };

    G4bool found = false;

    for (G4int i=0; i<n_pressures-1; ++i) {
      if  (pressure >= data[i][0] && pressure < data[i+1][0]) {
	G4double x1 = data[i][0];
	G4double x2 = data[i+1][0];
	G4double y1 = data[i][1];
	G4double y2 = data[i+1][1];
	density = y1 + (y2-y1)*(pressure-x1)/(x2-x1);
	found = true;
	break;
      }
    }

    if (!found) {
      if (pressure == data[n_pressures-1][0]) {
	density = data[n_pressures-1][1];
      } else {
	G4Exception("[XenonGaseousProperties]", "Density()", FatalException,
		    "Unknown xenon density for this pressure!");
      }
    }


    // Ideal gas state equation P*V = n*R*T
    // assuming atmosphere = bar

    // values for R
    //  8.3145 J mol-1 K-1
    //  0.0831451 L bar K-1 mol-1
    //  82.058 cm3 atm mol-1 K-1
    // value for molar mass: 131.29 g/mol

    // Warning: this is a dimensionless value.
    // _density =
    //   (_pressure/atmosphere)*131.29/(_temperature*82.058);



    // if (pressure/bar > 0.9 && pressure/bar < 1.1)
    //   density = 5.324*kg/m3;
    // else if (pressure/bar > 1.9 && pressure/bar < 2.1)
    //   density = 10.7*kg/m3;
    // else if (pressure/bar > 4.9 && pressure/bar < 5.1)
    //   density = 27.2*kg/m3;
    // else if (pressure/bar > 6.9 && pressure/bar < 7.1)
    //   density = 38.5548*kg/m3;
    // else if (pressure/bar > 9.9 && pressure/bar < 10.1)
    //   density = 55.587*kg/m3;
    // else if (pressure/bar > 14.9 && pressure/bar < 15.1)
    //   density = 85.95 *kg/m3;
    // else if (pressure/bar > 19.9 && pressure/bar < 20.1)
    //   density = 118.4*kg/m3;
    // else if (pressure/bar > 29.9 && pressure/bar < 30.1)
    //   density = 193.6*kg/m3;
    // else if (pressure/bar > 39.9 && pressure/bar < 40.1)
    //   density = 284.3*kg/m3;
    // else
    //   G4Exception("[XenonGaseousProperties]", "Density()", FatalException,
    //               "Unknown xenon density for this pressure!");


    return density;
  }

  G4double XenonGasProperties::MassPerMole(G4int a)
  {
    // Isotopic mass per mole taken from
    // http://rushim.ru/books/spravochniki/handbook-chemistry-and-physics.pdf

    G4double mass_per_mole = 135.907220*g/mole;

    if (a == 124) {
      mass_per_mole = 123.9058958*g/mole;
    } else if (a == 126) {
      mass_per_mole = 125.904269*g/mole;
    } else if (a == 128) {
      mass_per_mole = 127.9035304*g/mole;
    } else if (a == 129) {
      mass_per_mole = 128.9047795*g/mole;
    } else if (a == 130) {
      mass_per_mole = 129.9035079*g/mole;
    } else if (a == 131) {
      mass_per_mole = 130.9050819*g/mole;
    } else if (a == 132) {
      mass_per_mole = 131.9041545*g/mole;
    } else if (a == 134) {
      mass_per_mole = 133.9053945*g/mole;
    } else if (a == 136) {
      mass_per_mole = 135.907220*g/mole;
    }
    else {
      G4Exception("[XenonGaseousProperties]", "MassPerMole()", FatalException,
                  "Unknown mass per mole for this isotope!");
    }

    return mass_per_mole;
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

    // Need to use g/cm3
    G4double density = Density(pressure_) / g * cm3;

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

    G4double pressure = pressure_/atmosphere;

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

    G4double yield = (a * field_strength/pressure_ - b) * pressure_;
    if (yield < 0.) yield = 0.;

    return yield;
  }


} // end namespace nexus
