// ----------------------------------------------------------------------------
// nexus | ArgonGasProperties.cc
//
// Relevant physical properties of gaseous argon.
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


  G4double ArgonDensity(G4double pressure)
  {
    // Computes Ar (gas) density at T = 293 K
    // values taken from https://webbook.nist.gov/chemistry/fluid).
    // We assume a linear interpolation between any pair of values in the database.

    G4double density;

    const G4int n_pressures = 14;
    G4double data[n_pressures][2] = {{ 1.0 * bar,  1.641 * kg/m3},
                                    { 2.0  * bar,  3.284 * kg/m3},
                                    { 3.0  * bar,  4.929 * kg/m3},
                                    { 4.0  * bar,  7.402 * kg/m3},
                                    { 5.0  * bar, 8.2268 * kg/m3},
                                    { 6.0  * bar, 9.8788 * kg/m3},
                                    { 7.0  * bar, 11.533 * kg/m3},
                                    { 8.0  * bar, 13.189 * kg/m3},
                                    { 9.0  * bar, 14.848 * kg/m3},
                                    { 9.0  * bar, 14.848 * kg/m3},
                                    { 10.0 * bar, 16.508 * kg/m3},
                                    { 15.0 * bar, 24.843 * kg/m3},
                                    { 20.0 * bar, 33.231 * kg/m3},
                                    { 30.0 * bar, 50.155 * kg/m3}};
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
      }
      else {
        throw "Unknown argon density for this pressure!";
      }
    }


    return density;
  }

  G4double ArgonELLightYield(G4double field_strength, G4double pressure)
  {
    // Empirical formula taken from
    // C.M.B. Monteiro thesis https://estudogeral.sib.uc.pt/bitstream/10316/14635/3/Tese_Doutoramento_Cristina%20Monteiro.pdf.

    // Y/x = (a E/p + b) p,
    // where Y/x is the number of photons per unit lenght (cm),
    // E is the electric field strength, p is the pressure, and a and b
    // are empirically determined constants:
    const G4double a = 81. / kilovolt;
    const G4double b = 47. / (bar*cm);

    G4double yield = (a * field_strength/pressure - b) * pressure;
    if (yield < 0.) yield = 0.;

    return yield;
  }


} // end namespace nexus
