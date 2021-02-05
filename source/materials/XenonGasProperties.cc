// ----------------------------------------------------------------------------
//  nexus | XenonGasProperties.cc
//
//  This class collects the relevant physical properties of gaseous xenon.
//
//  The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "XenonGasProperties.h"
#include "Interpolation.h"

#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>
#include <G4AnalyticalPolSolver.hh>
#include <G4MaterialPropertiesTable.hh>

using namespace nexus;


XenonGasProperties::XenonGasProperties(G4double pressure, G4double temperature):
  pressure_(pressure),
  //temperature_(temperature),
  data_()
{
}


XenonGasProperties::XenonGasProperties():
  pressure_(),
  //temperature_(),
  data_()
{
}


XenonGasProperties::~XenonGasProperties()
{
}


G4double XenonGasProperties::Density(G4double pressure)
{
  G4double density = 5.324 * kg/m3;

  // These values are taken from O. Sifner and J. Klomfar, "Thermodynamic
  // Properties of Xenon from the Triple Point to 800 K with Pressures up to
  // 350 MPa", J. Phys. Chem. Ref. Data, Vol. 23, No. 1, 1994
  // We assume T = 300 K and perform a linear interpolation between any pair
  // of values.
  const G4int n_pressures = 6;
  G4double data[n_pressures][2] = {{  1.0 * bar,   5.29 * kg/m3},
                                   {  5.0 * bar,  27.01 * kg/m3},
                                   { 10.0 * bar,  55.55 * kg/m3},
                                   { 20.0 * bar, 118.36 * kg/m3},
                                   { 30.0 * bar, 191.51 * kg/m3},
                                   { 40.0 * bar, 280.40 * kg/m3}};
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
      throw "Unknown xenon density for this pressure!";
    }
  }

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
  // XXX Check if there is some newest results.

  G4double pressure = pressure_ / atmosphere;

  G4double Wavelength_peak  = (0.05 * pressure + 169.45) * nm;

  G4double Wavelength_sigma = 0.;
  if (pressure < 4.)
  Wavelength_sigma = 14.3 * nm;
  else
  Wavelength_sigma = (-0.117 * pressure + 15.16) * nm / (2.*sqrt(2*log(2)));

  G4double Energy_peak  = (h_Planck * c_light / Wavelength_peak);
  G4double Energy_sigma = (h_Planck * c_light * Wavelength_sigma / pow(Wavelength_peak,2));

  G4double intensity = exp(-pow(Energy_peak/eV-energy/eV,2) /
  (2*pow(Energy_sigma/eV, 2))) /
  (Energy_sigma/eV*sqrt(pi*2.));

  return intensity;
}


void XenonGasProperties::Scintillation(G4int entries, G4double* energy, G4double* intensity)
{
  for (G4int i=0; i<entries; i++)
  intensity[i] = Scintillation(energy[i]);
}


G4double XenonGasProperties::ELLightYield(G4double field_strength) const
{
  // Empirical formula taken from
  // C.M.B. Monteiro et al., JINST 2 (2007) P05001.

  // Y/x = (a E/p - b) p,
  // where Y/x is the number of photons per unit lenght (cm),
  // E is the electric field strength, p is the pressure
  // a and b are empirically determined constants depending on pressure.
  // Values for different pressures are found in: Freitas-2010
  // Physics Letters B 684 (2010) 205–210

  const G4double b = 116. / (bar*cm);
  G4double a = 140. / kilovolt;

  // Updating the slope
  if (pressure_ >= 2. * bar) a = 141. / kilovolt;
  if (pressure_ >= 4. * bar) a = 142. / kilovolt;
  if (pressure_ >= 5. * bar) a = 151. / kilovolt;
  if (pressure_ >= 6. * bar) a = 161. / kilovolt;
  if (pressure_ >= 8. * bar) a = 170. / kilovolt;

  // Getting the yield
  G4double yield = (a * field_strength/pressure_ - b) * pressure_;
  if (yield < 0.) yield = 0.;

  return yield;
}


void XenonGasProperties::MakeDataTable()
{
  // Fills the data_ vector with temperature, pressure, and density data
  // Assumes the data file goes up in pressure then temperature
  // with the format: Temperature Pressure Density

  // Open file
  G4String path(std::getenv("NEXUSDIR"));
  G4String filename = path + "/data/gxe_density_table.txt";

  std::ifstream inFile;
  inFile.open(filename);
  if (!inFile) {
    throw "File could not be opened";
  }

  std::vector<std::vector<G4double>> data;
  // Read lines in file
  G4String thisline;
  getline(inFile, thisline); // don't use first line
  G4int npressures = 0;
  G4int ntemps = 0;
  G4int count = 0;
  G4double thistemp = 0;
  G4double temp, press, dens;
  char comma;

  while (inFile>>temp>>comma>>press>>comma>>dens){
    std::vector<G4double> thisdata {temp*kelvin, press*bar, dens*(kg/m3)};
    data_.push_back(thisdata);

    // Figure out how many temperature and pressures we have
    if (temp == thistemp){
      npressures++;
    }
    else {
      thistemp = temp;
      ntemps++;
      npressures = 1;
    }
    count++;
  }

  npressures_ = npressures-1;
  ntemps_ = ntemps;

  inFile.close();
  return;
}


G4double XenonGasProperties::GetDensity(G4double pressure, G4double temperature)
{
  // Interpolate to calculate the density
  // at a given pressure and temperature
  G4double density = 5.324 * kg/m3;
  MakeDataTable();

  // Find correct interval and use bilinear interpolation
  G4bool found = false;
  G4int tcount = 0;
  G4int pcount = 0;
  G4int count = 0;
  G4double t1, t2, p1, p2, d11, d12, d21, d22, d1, d2;

  while (tcount < ntemps_-1) {
    t1 = data_[count][0];
    t2 = data_[count+npressures_][0];

    if (temperature >= t1 && temperature < t2) {
      while (pcount < npressures_-1) {
        p1 = data_[count][1];
        p2 = data_[count+1][1];

        if (pressure >= p1 && pressure < p2) {
          d11 = data_[count][2];
          d12 = data_[count+1][2];
          d21 = data_[count+npressures_][2];
          d22 = data_[count+npressures_+1][2];

          density = BilinearInterpolation(temperature, t1, t2,
            pressure, p1, p2,
            d11, d12, d21, d22);
            found = true;
            break;
        }
        pcount++;
        count++;
      }

      if (!found) {
        if (pressure == data_[count][1]) {
          d1 = data_[count][2];
          d2 = data_[count+npressures_][2];
          density = LinearInterpolation(temperature, t1, t2, d1, d2);
          found = true;
        } else {
          throw "Unknown xenon density for this pressure!";
        }
      }

      break;
    }
    tcount++;
    pcount = 0;
    count += npressures_;
  }

  if (!found) {
    if (temperature == data_[count][0]) {
      while (pcount< npressures_-1) {
        p1 = data_[count][1];
        p2 = data_[count+1][1];

        if (pressure >= p1 && pressure < p2){
          d1 = data_[count][2];
          d2 = data_[count+1][2];

          density = LinearInterpolation(pressure, p1, p2, d1, d2);
          found = true;
          break;
        }
        pcount++;
        count++;
      }
      if (!found) {
        if (pressure == data_[count][1]) {
          density = data_[count][2];
          found = true;
        } else {
          throw "Unknown xenon density for this pressure!";
        }
      }
    } else {
        throw "Unknown xenon density for this temperature";
    }
  }
  return density;
}
