// -----------------------------------------------------------------------------
//  nexus | OpticalMaterialProperties.cc
//
//  * Info:         : NEXT-Flex Field Cage geometry for performance studies.
//  * Author        : <jmunoz@ific.uv.es>
//  *                 <paola.ferrario@ific.uv.es>
//  *                 <justo.martin-albo@ific.uv.es>
//  * Creation date : February 2020
//  * Description   : Optical properties of most used materials.
// -----------------------------------------------------------------------------

#include "OpticalMaterialProperties.h"
#include "XenonGasProperties.h"
#include "XenonGasProperties.h"
#include "SellmeierEquation.h"

#include <G4MaterialPropertiesTable.hh>

#include <assert.h>

using namespace nexus;
using namespace CLHEP;


/// Vacuum ///
G4MaterialPropertiesTable* OpticalMaterialProperties::Vacuum()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
  
  G4double photEnergy[] = {optPhotMinE_, optPhotMaxE_};
  G4double nEntries = sizeof(photEnergy) / sizeof(G4double);

  // REFRACTIVE INDEX
  G4double rIndex[] = {1., 1.};
  assert(sizeof(rIndex) == sizeof(photEnergy));
  mpt->AddProperty("RINDEX", photEnergy, rIndex, nEntries);
  
  // ABSORPTION LENGTH
  G4double absLength[] = {noAbsLength_, noAbsLength_};
  assert(sizeof(absLength) == sizeof(photEnergy));
  mpt->AddProperty("ABSLENGTH", photEnergy, absLength, nEntries);
  
  return mpt;
}



/// Fused Silica ///
G4MaterialPropertiesTable* OpticalMaterialProperties::FusedSilica()
{
  // Optical properties of Suprasil 311/312(c) synthetic fused silica.
  // Obtained from http://heraeus-quarzglas.com

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  // The range is chosen to be up to ~10.7 eV because Sellmeier's equation 
  // for fused silica is valid only in that range
  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = optPhotMinE_ + i * eWidth;
  }

  // The following values for the refractive index have been calculated
  // using Sellmeier's equation:
  //    n^2 - 1 = B_1 * \lambda^2 / (\lambda^2 - C_1) +
  //            + B_2 * \lambda^2 / (\lambda^2 - C_2) +
  //            + B_3 * \lambda^2 / (\lambda^2 - C_3),
  // with wavelength \lambda in micrometers and 
  //    B_1 = 4.73E-1, B_2 = 6.31E-1, B_3 = 9.06E-1
  //    C_1 = 1.30E-2, C_2 = 4.13E-3, C_3 = 9.88E+1.

  G4double B_1 = 4.73e-1;
  G4double B_2 = 6.31e-1;
  G4double B_3 = 9.06e-1;
  G4double C_1 = 1.30e-2;
  G4double C_2 = 4.13e-3;
  G4double C_3 = 9.88e+1;

  G4double rIndex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1) 
      + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
      + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
    rIndex[i] = sqrt(n2);
    // G4cout << "* FusedSilica rIndex:  " << std::setw(5) << ri_energy[i]/eV
    //       << " eV -> " << rIndex[i] << G4endl;
  }
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy, rIndex, ri_entries);

  // ABSORPTION LENGTH
  G4double abs_energy[] = {
    optPhotMinE_,  6.46499 * eV, 
    6.54000 * eV,  6.59490 * eV,  6.64000 * eV,  6.72714 * eV,
    6.73828 * eV,  6.75000 * eV,  6.82104 * eV,  6.86000 * eV, 
    6.88000 * eV,  6.89000 * eV,  7.00000 * eV,  7.01000 * eV,
    7.01797 * eV,  7.05000 * eV,  7.08000 * eV,  7.08482 * eV,
    7.30000 * eV,  7.36000 * eV,  7.40000 * eV,  7.48000 * eV,
    7.52000 * eV,  7.58000 * eV,  7.67440 * eV,  7.76000 * eV,
    7.89000 * eV,  7.93000 * eV,  8.00000 * eV,
    optPhotMaxE_
  };
  const G4int abs_entries = sizeof(abs_energy) / sizeof(G4double);

  G4double absLength[] = {
    noAbsLength_, noAbsLength_,
    200.0 * cm,   200.0 * cm,  90.0 * cm,  45.0 * cm,
     45.0 * cm,    30.0 * cm,  24.0 * cm,  21.0 * cm, 
     20.0 * cm,    19.0 * cm,  16.0 * cm,  14.0 * cm,
     13.0 * cm,     8.5 * cm,   8.0 * cm,   6.0 * cm,
      1.5 * cm,     1.2 * cm,   1.0 * cm,   .65 * cm,
       .4 * cm,     .37 * cm,   .32 * cm,   .28 * cm,
      .22 * cm,    .215 * cm,  .00005*cm,
    .00005* cm
  };

  assert(sizeof(absLength) == sizeof(abs_energy));
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, abs_entries);

  return mpt;
}



/// Fake Fused Silica ///
G4MaterialPropertiesTable* OpticalMaterialProperties::FakeFusedSilica(G4double transparency,
                                                                      G4double thickness)
{
  // Optical properties of Suprasil 311/312(c) synthetic fused silica.
  // Obtained from http://heraeus-quarzglas.com

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  // The range is chosen to be up to ~10.7 eV because Sellmeier's equation 
  // for fused silica is valid only in that range
  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = optPhotMinE_ + i * eWidth;
  }

  // The following values for the refractive index have been calculated
  // using Sellmeier's equation:
  //    n^2 - 1 = B_1 * \lambda^2 / (\lambda^2 - C_1) +
  //            + B_2 * \lambda^2 / (\lambda^2 - C_2) +
  //            + B_3 * \lambda^2 / (\lambda^2 - C_3),
  // with wavelength \lambda in micrometers and 
  //    B_1 = 4.73E-1, B_2 = 6.31E-1, B_3 = 9.06E-1
  //    C_1 = 1.30E-2, C_2 = 4.13E-3, C_3 = 9.88E+1.

  G4double B_1 = 4.73e-1;
  G4double B_2 = 6.31e-1;
  G4double B_3 = 9.06e-1;
  G4double C_1 = 1.30e-2;
  G4double C_2 = 4.13e-3;
  G4double C_3 = 9.88e+1;

  G4double rIndex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1) 
      + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
      + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
    rIndex[i] = sqrt(n2);
    //G4cout << "* FakeFusedSilica rIndex:  " << std::setw(5) 
    //       << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
  }
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy, rIndex, ri_entries);

  // ABSORPTION LENGTH (Set to match the transparency)
  G4double abs_length     = -thickness / log(transparency);
  const G4int NUMENTRIES  = 2;
  G4double abs_energy[NUMENTRIES] = {optPhotMinE_, optPhotMaxE_};
  G4double ABSL[NUMENTRIES]       = {abs_length, abs_length};
  mpt->AddProperty("ABSLENGTH", abs_energy, ABSL, NUMENTRIES);

  return mpt;
}



/// Glass Epoxy ///
G4MaterialPropertiesTable* OpticalMaterialProperties::GlassEpoxy()
{
  // Optical properties of Optorez 1330 glass epoxy.
  // Obtained from http://refractiveindex.info and 
  // https://www.zeonex.com/Optics.aspx.html#glass-like

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  // The range is chosen to be up to ~10.7 eV because Sellmeier's equation 
  // for fused silica is valid only in that range
  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = optPhotMinE_ + i * eWidth;
  }
 
  G4double rIndex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    G4double n2 = 2.291142 - 3.311944E-2*pow(lambda,2) - 1.630099E-2*pow(lambda,-2) + 
                  7.265983E-3*pow(lambda,-4) - 6.806145E-4*pow(lambda,-6) + 
                  1.960732E-5*pow(lambda,-8);
    rIndex[i] = sqrt(n2);
    // G4cout << "* GlassEpoxy rIndex:  " << std::setw(5)
    //        << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
  }
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy, rIndex, ri_entries);

  // ABSORPTION LENGTH
  G4double abs_energy[] = {
    optPhotMinE_, 2.000 * eV,
    2.132 * eV,   2.735 * eV,  2.908 * eV,  3.119 * eV, 
    3.320 * eV,   3.476 * eV,  3.588 * eV,  3.749 * eV,
    3.869 * eV,   3.973 * eV,  4.120 * eV,  4.224 * eV,
    4.320 * eV,   4.420 * eV,  5.018 * eV
  };
  const G4int abs_entries = sizeof(abs_energy) / sizeof(G4double);

  G4double absLength[] = {
    noAbsLength_, noAbsLength_,
    326.00 * mm,  117.68 * mm,  85.89 * mm,  50.93 * mm, 
     31.25 * mm,   17.19 * mm,  10.46 * mm,   5.26 * mm,
      3.77 * mm,    2.69 * mm,   1.94 * mm,   1.33 * mm,
      0.73 * mm,    0.32 * mm,   0.10 * mm
  };
  assert(sizeof(absLength) == sizeof(abs_energy));
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, abs_entries);

  return mpt;
}



/// Sapphire ///
G4MaterialPropertiesTable* OpticalMaterialProperties::Sapphire()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  G4double um2 = micrometer*micrometer;
  G4double B[3] = {1.4313493, 0.65054713, 5.3414021};
  G4double C[3] = {0.0052799261 * um2, 0.0142382647 * um2, 325.017834 * um2};
  SellmeierEquation seq(B, C);

  const G4int ri_entries = 100;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = optPhotMinE_ + i * eWidth;
  }
 
  G4double rIndex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    rIndex[i] = seq.RefractiveIndex(h_Planck*c_light/ri_energy[i]);
    //G4cout << "* Sapphire rIndex:  " << std::setw(5)
    //       << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
  }
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy, rIndex, ri_entries);

  // ABSORPTION LENGTH
  G4double abs_energy[] = {
    optPhotMinE_, 0.900 * eV,
    1.000 * eV,   1.296 * eV,  1.683 * eV,  2.075 * eV,
    2.585 * eV,   3.088 * eV,  3.709 * eV,  4.385 * eV,
    4.972 * eV,   5.608 * eV,  6.066 * eV,  6.426 * eV,
    6.806 * eV,   7.135 * eV,  7.401 * eV,  7.637 * eV,
    7.880 * eV,   8.217 * eV
  };
  const G4int abs_entries = sizeof(abs_energy) / sizeof(G4double);

  G4double absLength[] = {
    noAbsLength_, noAbsLength_,
    3455.0  * mm,  3455.0  * mm,  3455.0  * mm,  3455.0  * mm,
    3455.0  * mm,  3140.98 * mm,  2283.30 * mm,  1742.11 * mm,
     437.06 * mm,   219.24 * mm,  117.773 * mm,   80.560 * mm,
     48.071 * mm,   28.805 * mm,   17.880 * mm,   11.567 * mm,
      7.718 * mm,    4.995 * mm
  };
  assert(sizeof(absLength) == sizeof(abs_energy)); 
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, abs_entries); 

  return mpt;
}



/// Optical Coupler ///
// gel NyoGel OCK-451
G4MaterialPropertiesTable* OpticalMaterialProperties::OptCoupler()
{
  // For comparison of optical coupling materials.
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  G4double um2 = micrometer*micrometer;

  G4double constTerm  = 1.4954;
  G4double squareTerm = 0.008022 * um2;
  G4double quadTerm   = 0.;

  const G4int ri_entries = 100;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = optPhotMinE_ + i * eWidth;
  }
 
  G4double rIndex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    G4double wl = h_Planck * c_light / ri_energy[i];
    rIndex[i] = constTerm + squareTerm/(wl*wl) + quadTerm/pow(wl,4);
    //G4cout << "* OptCoupler rIndex:  " << std::setw(5)
    //       << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
  }
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy, rIndex, ri_entries);

  // ABSORPTION LENGTH
  // Values estimated from printed plot (to be improved).
  G4double abs_energy[] = {
    optPhotMinE_,  1.70 * eV,
    1.77 * eV,     2.07 * eV,  2.48 * eV,  2.76 * eV,
    2.92 * eV,     3.10 * eV,  3.31 * eV,  3.54 * eV,
    3.81 * eV,     4.13 * eV
  };
  const G4int abs_entries = sizeof(abs_energy) / sizeof(G4double);

  G4double absLength[] = {
    noAbsLength_, noAbsLength_,
    1332.8 * mm,  1332.8 * mm,  1332.8 * mm,  666.17 * mm,
     499.5 * mm,   399.5 * mm,   199.5 * mm,  132.83 * mm,
      99.5 * mm,     4.5 * mm
  };
  assert(sizeof(absLength) == sizeof(abs_energy)); 
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, abs_entries); 

  return mpt;
}



/// Gaseous Argon ///
G4MaterialPropertiesTable* OpticalMaterialProperties::GAr(G4double sc_yield)
{
  // An argon gas proportional scintillation counter with UV avalanche photodiode scintillation
  // readout C.M.B. Monteiro, J.A.M. Lopes, P.C.P.S. Simoes, J.M.F. dos Santos, C.A.N. Conde

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = optPhotMinE_ + i * eWidth;
  }

  G4double rIndex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    G4double wl = h_Planck * c_light / ri_energy[i] * 1000; // in micron
     // From refractiveindex.info
    rIndex[i] = 1 + 0.012055*(0.2075*pow(wl,2)/(91.012*pow(wl,2)-1) + 
                0.0415*pow(wl,2)/(87.892*pow(wl,2)-1) + 4.3330*pow(wl,2)/(214.02*pow(wl,2)-1));
    //G4cout << "* GAr rIndex:  " << std::setw(5) << ri_energy[i]/eV 
    //       << " eV -> " << rIndex[i] << G4endl;
  }
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy, rIndex, ri_entries);

  // ABSORPTION LENGTH
  G4double abs_energy[] = {optPhotMinE_, optPhotMaxE_};
  G4double absLength[]  = {noAbsLength_, noAbsLength_};
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, 2);

  // EMISSION SPECTRUM
  G4double Wavelength_peak  = 128.000 * nm;
  G4double Wavelength_sigma =   2.929 * nm;
  G4double Energy_peak  = (h_Planck*c_light / Wavelength_peak);
  G4double Energy_sigma = (h_Planck*c_light * Wavelength_sigma / pow(Wavelength_peak,2));
  //G4cout << "*** GAr Energy_peak: " << Energy_peak/eV << " eV   Energy_sigma: "
  //       << Energy_sigma/eV << " eV" << G4endl;

  // Sampling from ~110 nm to 150 nm <----> from ~11.236 eV to 8.240 eV
  const G4int sc_entries = 380;
  G4double sc_energy[sc_entries];
  G4double intensity[sc_entries];
  for (int i=0; i<sc_entries; i++){
    sc_energy[i] = 8.240*eV + 0.008*i*eV;
    intensity[i] = exp(-pow(Energy_peak/eV-sc_energy[i]/eV,2) / 
                   (2*pow(Energy_sigma/eV, 2)))/(Energy_sigma/eV*sqrt(pi*2.));
    //G4cout << "* GAr energy: " << std::setw(6) << sc_energy[i]/eV << " eV  ->  "
    //       << std::setw(6) << intensity[i] << G4endl;
  }
  mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  mpt->AddProperty("ELSPECTRUM",    sc_energy, intensity, sc_entries);

  // CONST PROPERTIES
  mpt->AddConstProperty("SCINTILLATIONYIELD", sc_yield);
  mpt->AddConstProperty("FASTTIMECONSTANT",   6.*ns);
  mpt->AddConstProperty("SLOWTIMECONSTANT",   37.*ns);
  mpt->AddConstProperty("YIELDRATIO",         .52);
  mpt->AddConstProperty("RESOLUTIONSCALE",    1.0);
  //mpt->AddConstProperty("ELTIMECONSTANT",     1260.*ns);
  mpt->AddConstProperty("ATTACHMENT",         1000.*ms);
  
  return mpt; 
}



/// Gaseous Xenon ///
G4MaterialPropertiesTable* OpticalMaterialProperties::GXe(G4double pressure,
                                                          G4double temperature,
                                                          G4int    sc_yield)
{
  XenonGasProperties GXe_prop(pressure, temperature);
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = optPhotMinE_ + i * eWidth;
  }

  G4double rIndex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    rIndex[i] = GXe_prop.RefractiveIndex(ri_energy[i]);
    // G4cout << "* GXe rIndex:  " << std::setw(7)
    //        << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
  }
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy, rIndex, ri_entries);

  // ABSORPTION LENGTH
  G4double abs_energy[] = {optPhotMinE_, optPhotMaxE_};
  G4double absLength[]  = {noAbsLength_, noAbsLength_};
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, 2);

  // EMISSION SPECTRUM
  // Sampling from ~150 nm to 200 nm <----> from 6.20625 eV to 8.20625 eV
  const G4int sc_entries = 200;
  G4double sc_energy[sc_entries];
  for (int i=0; i<sc_entries; i++){
    sc_energy[i] = 6.20625 * eV + 0.01 * i * eV;
  }
  G4double intensity[sc_entries];
  GXe_prop.Scintillation(sc_entries, sc_energy, intensity);
  //for (int i=0; i<sc_entries; i++) {
  //  G4cout << "* GXe Scint:  " << std::setw(7) << sc_energy[i]/eV
  //         << " eV -> " << intensity[i] << G4endl;
  //}
  mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  mpt->AddProperty("ELSPECTRUM",    sc_energy, intensity, sc_entries);
  mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);

  // CONST PROPERTIES
  mpt->AddConstProperty("SCINTILLATIONYIELD", sc_yield);
  mpt->AddConstProperty("RESOLUTIONSCALE",    1.0);
  mpt->AddConstProperty("FASTTIMECONSTANT",   4.5  * ns);
  mpt->AddConstProperty("SLOWTIMECONSTANT",   100. * ns);
  //mpt->AddConstProperty("ELTIMECONSTANT",     50.  * ns);
  mpt->AddConstProperty("YIELDRATIO",         .1);
  mpt->AddConstProperty("ATTACHMENT",         1000. * ms);
  
  return mpt;
}



/// Fake Grid ///
G4MaterialPropertiesTable* OpticalMaterialProperties::FakeGrid(G4double pressure,
                                                               G4double temperature,
                                                               G4double transparency,
                                                               G4double thickness,
                                                               G4int    sc_yield)
{
  XenonGasProperties GXe_prop(pressure, temperature);
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = optPhotMinE_ + i * eWidth;
  }

  G4double rIndex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    rIndex[i] = GXe_prop.RefractiveIndex(ri_energy[i]);
    // G4cout << "* FakeGrid rIndex:  " << std::setw(7)
    //        << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
  }
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy, rIndex, ri_entries);

  // ABSORPTION LENGTH
  G4double abs_length   = -thickness/log(transparency);
  G4double abs_energy[] = {optPhotMinE_, optPhotMaxE_};
  G4double absLength[]  = {abs_length, abs_length};
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, 2);

  // EMISSION SPECTRUM
  // Sampling from ~150 nm to 200 nm <----> from 6.20625 eV to 8.20625 eV
  const G4int sc_entries = 200;
  G4double sc_energy[sc_entries];
  for (int i=0; i<sc_entries; i++){
    sc_energy[i] = 6.20625 * eV + 0.01 * i * eV;
  }
  G4double intensity[sc_entries];
  GXe_prop.Scintillation(sc_entries, sc_energy, intensity);
  //for (int i=0; i<sc_entries; i++)
  //  G4cout << "* FakeGrid Scint:  " << std::setw(7) << sc_energy[i]/eV
  //         << " eV -> " << intensity[i] << G4endl;
  mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);

  // CONST PROPERTIES
  mpt->AddConstProperty("SCINTILLATIONYIELD", sc_yield); 
  mpt->AddConstProperty("RESOLUTIONSCALE",    1.0);
  mpt->AddConstProperty("FASTTIMECONSTANT",   4.5  * ns);
  mpt->AddConstProperty("SLOWTIMECONSTANT",   100. * ns);
  mpt->AddConstProperty("YIELDRATIO",         .1);
  mpt->AddConstProperty("ATTACHMENT",         1000. * ms);

  return mpt;
}



/// PTFE ///
G4MaterialPropertiesTable* OpticalMaterialProperties::PTFE()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFLECTIVITY
  const G4int REFL_NUMENTRIES = 7;
  G4double ENERGIES[REFL_NUMENTRIES] = {
    optPhotMinE_,  2.8 * eV,  3.5 * eV,  4. * eV,
    6. * eV,       7.2 * eV,  optPhotMaxE_
  };
  G4double REFLECTIVITY[REFL_NUMENTRIES] = {
    .98,  .98,  .98,  .98,
    .72,  .72,  .72
  };
  mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, REFL_NUMENTRIES);

  // REFLEXION BEHAVIOR
  const G4int NUMENTRIES = 2;
  G4double ENERGIES_2[NUMENTRIES]    = {optPhotMinE_, optPhotMaxE_};
  // Specular reflection about the normal to a microfacet.
  // Such a vector is chosen according to a gaussian distribution with 
  // sigma = SigmaAlhpa (in rad) and centered in the average normal.
  G4double specularlobe[NUMENTRIES]  = {0., 0.};
  // specular reflection about the average normal 
  G4double specularspike[NUMENTRIES] = {0., 0.}; 
  // 180 degrees reflection.
  G4double backscatter[NUMENTRIES]   = {0., 0.};
  // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

  mpt->AddProperty("SPECULARLOBECONSTANT", ENERGIES_2, specularlobe,  NUMENTRIES);
  mpt->AddProperty("SPECULARSPIKECONSTANT",ENERGIES_2, specularspike, NUMENTRIES);
  mpt->AddProperty("BACKSCATTERCONSTANT",  ENERGIES_2, backscatter,   NUMENTRIES);

  // REFRACTIVE INDEX
  G4double rIndex[] = {1.41, 1.41};
  mpt->AddProperty("RINDEX", ENERGIES_2, rIndex, NUMENTRIES);

  return mpt;
}



/// TPB (tetraphenyl butadiene) ///
G4MaterialPropertiesTable* OpticalMaterialProperties::TPB()
{
  // Data from https://doi.org/10.1140/epjc/s10052-018-5807-z
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
  
  // REFRACTIVE INDEX
  const G4int rIndex_numEntries = 2;
  G4double rIndex_energies[rIndex_numEntries] = {optPhotMinE_, optPhotMaxE_};
  G4double TPB_rIndex[rIndex_numEntries]      = {1.67    , 1.67};
  mpt->AddProperty("RINDEX", rIndex_energies,
                   TPB_rIndex, rIndex_numEntries);

  // ABSORPTION LENGTH
  // Assuming no absorption except WLS
  G4double abs_energy[] = {optPhotMinE_, optPhotMaxE_};
  G4double absLength[]  = {noAbsLength_, noAbsLength_};
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, 2);

  // WLS ABSORPTION LENGTH
  // This is a combination of figure 11 (for wavelength > 270 nm) and
  // figure 20 (for 50 nm < wavelength < 250 nm).
  // XXX There is a mismatch in the border of the figures that anyway, we implement.
  // Fig 20 -> WLS_absLength = 400 nm for wavelength = 250 nm
  // Fig 11 -> WLS_absLength = 100 nm for wavelength = 270 nm
  G4double WLS_abs_energy[] = {
    optPhotMinE_,
    nm_to_eV_ / 480.,  nm_to_eV_ / 470.,
    nm_to_eV_ / 450.,  nm_to_eV_ / 430.,
    nm_to_eV_ / 420.,  nm_to_eV_ / 400.,
    nm_to_eV_ / 380.,  nm_to_eV_ / 360.,
    nm_to_eV_ / 330.,  nm_to_eV_ / 320.,
    nm_to_eV_ / 300.,  nm_to_eV_ / 270.,
    nm_to_eV_ / 250.,  nm_to_eV_ / 230.,
    nm_to_eV_ / 210.,  nm_to_eV_ / 190.,
    nm_to_eV_ / 170.,  nm_to_eV_ / 150.,
    nm_to_eV_ / 100.,  nm_to_eV_ /  70.,
    nm_to_eV_ /  50.,  nm_to_eV_ /  40.,
    optPhotMaxE_
  };
  const G4int WLS_abs_entries = sizeof(WLS_abs_energy) / sizeof(G4double);


  G4double WLS_absLength[] = {
    noAbsLength_,
    noAbsLength_,  1.e6 * nm,
    1.e5 * nm,     1.e4 * nm,
    5.e3 * nm,     1.e3 * nm,
    100. * nm,      30. * nm,
     30. * nm,      50. * nm,
    100. * nm,     100. * nm,
    400. * nm,     400. * nm,
    350. * nm,     250. * nm,
    350. * nm,     400. * nm,
    400. * nm,     350. * nm,
    400. * nm,     noAbsLength_,
    noAbsLength_
  };
  assert(sizeof(WLS_absLength) == sizeof(WLS_abs_energy));
  mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy,
                   WLS_absLength,  WLS_abs_entries); 

  // WLS EMISSION SPECTRUM
  // Implemented with formula (7), with parameter values in table (3)
  // Sampling from ~380 nm to 600 nm <--> from 2.06 to 3.26 eV
  const G4int WLS_emi_entries = 120;
  G4double WLS_emi_energy[WLS_emi_entries];
  for (int i=0; i<WLS_emi_entries; i++)
    WLS_emi_energy[i] = 2.06 * eV + 0.01 * i * eV;

  G4double WLS_emiSpectrum[WLS_emi_entries];
  G4double A      = 0.782;
  G4double alpha  = 3.7e-2;
  G4double sigma1 = 15.43;
  G4double mu1    = 418.10;
  G4double sigma2 = 9.72;
  G4double mu2    = 411.2;

  for (int i=0; i<WLS_emi_entries; i++) {
    G4double wl = h_Planck * c_light / WLS_emi_energy[i] / nm;
    WLS_emiSpectrum[i] = A * (alpha/2.) * exp((alpha/2.) *
                         (2*mu1 + alpha*pow(sigma1,2) - 2*wl)) *
                         erfc((mu1 + alpha*pow(sigma1,2) - wl) / (sqrt(2)*sigma1)) +
                         (1-A) * (1 / sqrt(2*pow(sigma2,2)*3.1416)) *                         
                         exp((-pow(wl-mu2,2)) / (2*pow(sigma2,2)));
    // G4cout << "* newTPB WLSemi:  " << std::setw(4)
    //        << wl << " nm -> " << WLS_emiSpectrum[i] << G4endl;
  };
  assert(sizeof(WLS_emiSpectrum) == sizeof(WLS_emi_energy));
  mpt->AddProperty("WLSCOMPONENT", WLS_emi_energy,
                   WLS_emiSpectrum, WLS_emi_entries);

  // WLS Delay
  mpt->AddConstProperty("WLSTIMECONSTANT", 1.2 * ns);

  // WLS Quantum Efficiency
  // According to the paper, the QE of TPB depends on the incident wavelength.
  // As Geant4 doesn't allow this possibility, it is set to the value corresponding
  // to Xe scintillation spectrum peak.
  mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.65);

  return mpt;
}



/// TPH ///
G4MaterialPropertiesTable* OpticalMaterialProperties::TPH()
{
  // from http://omlc.ogi.edu/spectra/PhotochemCAD/html/p-terphenyl.html
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
    
  // REFRACTIVE INDEX
  G4double ri_energy[]  = {optPhotMinE_, optPhotMaxE_};
  G4double rIndex[]     = {1.65, 1.65};
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy, rIndex, 2);
  
  // ABSORPTION LENGTH
  G4double abs_energy[]  = {optPhotMinE_, optPhotMaxE_};
  G4double absLength[]   = {noAbsLength_, noAbsLength_};
  assert(sizeof(absLength) == sizeof(abs_energy));
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, 2);
  
  // WLS EMISSION SPECTRUM
  G4double WLS_emi_energy[] = {
    optPhotMinE_, 
    2.5 * eV,  3.0 * eV,  3.4 * eV,  3.8 * eV,
    3.9 * eV,  4.0 * eV,  5.0 * eV, 
    optPhotMaxE_
  };
  const G4int WLS_emi_entries = sizeof(WLS_emi_energy) / sizeof(G4double);

  G4double WLS_emiSpectrum[] = {
    0.00,
    0.00,  0.00,  0.00,  1.00,
    0.00,  0.00,  0.00, 
    0.00
  };
  // XXX TPH WLS emission spectrum set flat between 3.7 and 3.9 eV (To be fixed)
  assert(sizeof(WLS_emiSpectrum) == sizeof(WLS_emi_energy));
  mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy,
                   WLS_emiSpectrum, WLS_emi_entries);

  // WLS ABSORPTION LENGTH
  G4double WLS_abs_energy[] = {
    optPhotMinE_, 2.0 * eV,  3.9 * eV,  4.5 * eV,
    5.5 * eV,     8.0 * eV,  optPhotMaxE_
  };
  const G4int WLS_abs_entries = sizeof(WLS_abs_energy) / sizeof(G4double);

  G4double WLS_absLength[] = {
    noAbsLength_,  noAbsLength_,  noAbsLength_,  0.0177 * cm,
    noAbsLength_,  noAbsLength_,  noAbsLength_
  };
  // XXX TPH WLS AbsLength modelled being a triangle centered at 4.5 eV -> 
  // No absorption for Xe scint light (To be fixed.)
  assert(sizeof(WLS_absLength) == sizeof(WLS_abs_energy));
  mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy,
                   WLS_absLength,  WLS_abs_entries); 

  // CONST PROPERTIES
  mpt->AddConstProperty("WLSTIMECONSTANT", 0.5 * ns);

  return mpt;
}



/// EJ-280 ///
G4MaterialPropertiesTable* OpticalMaterialProperties::EJ280()
{
  // https://eljentechnology.com/products/wavelength-shifting-plastics/ej-280-ej-282-ej-284-ej-286
  // and data sheets from the provider.
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  G4double ri_energy[] = {
    optPhotMinE_,
    nm_to_eV_ / 609.,  nm_to_eV_ / 589.26,
    nm_to_eV_ / 550.,  nm_to_eV_ / 530.  ,
    nm_to_eV_ / 500.,  nm_to_eV_ / 490.  ,
    nm_to_eV_ / 481.,  nm_to_eV_ / 460.  ,
    nm_to_eV_ / 435.,  nm_to_eV_ / 425.  ,
    optPhotMaxE_
  };
  const G4int ri_entries = sizeof(ri_energy) / sizeof(G4double);

  G4double rIndex[] = {
    1.5780,
    1.5780,  1.5800,   // 609 , 589.26 nm
    1.5845,  1.5870,   // 550 , 530 nm
    1.5913,  1.5929,   // 500 , 490 nm
    1.5945,  1.5986,   // 481 , 460 nm
    1.6050,  1.6080,   // 435 , 425 nm
    1.608
  };
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy,
                   rIndex,   ri_entries);

  // ABSORPTION LENGTH
  G4double abs_energy[]  = {
    optPhotMinE_,      nm_to_eV_ / 610.,  nm_to_eV_ / 605.,
    nm_to_eV_ / 350.,  nm_to_eV_ / 345.,  optPhotMaxE_
  };
  G4double absLength[]   = {
    noAbsLength_,  3. * m,  3. * m,
    3. *  m,       3. * m,  noAbsLength_
  };
  assert(sizeof(absLength) == sizeof(abs_energy));
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, 2);

  // WLS ABSORPTION LENGTH
  G4double WLS_abs_energy[] = {
    optPhotMinE_,      nm_to_eV_ / 500.,
    nm_to_eV_ / 495.,  nm_to_eV_ / 490.,
    nm_to_eV_ / 485.,  nm_to_eV_ / 480.,
    nm_to_eV_ / 475.,  nm_to_eV_ / 470.,
    nm_to_eV_ / 465.,  nm_to_eV_ / 460.,
    nm_to_eV_ / 455.,  nm_to_eV_ / 450.,
    nm_to_eV_ / 445.,  nm_to_eV_ / 440.,
    nm_to_eV_ / 435.,  nm_to_eV_ / 430.,
    nm_to_eV_ / 425.,  nm_to_eV_ / 420.,
    nm_to_eV_ / 415.,  nm_to_eV_ / 410.,
    nm_to_eV_ / 405.,  nm_to_eV_ / 400.,
    nm_to_eV_ / 395.,  nm_to_eV_ / 390.,
    nm_to_eV_ / 385.,  nm_to_eV_ / 380.,
    nm_to_eV_ / 375.,  nm_to_eV_ / 370.,
    nm_to_eV_ / 365.,  nm_to_eV_ / 360.,
    nm_to_eV_ / 355.,  nm_to_eV_ / 350.,
    nm_to_eV_ / 345.,  optPhotMaxE_
  };
  const G4int WLS_abs_entries = sizeof(WLS_abs_energy) / sizeof(G4double);

  G4double WLS_absLength[] = {
    noAbsLength_,        noAbsLength_,
    (1. / 0.0080) * cm,  (1. / 0.0165) * cm,    // 495 , 490 nm
    (1. / 0.0325) * cm,  (1. / 0.0815) * cm,    // 485 , 480 nm
    (1. / 0.2940) * cm,  (1. / 0.9640) * cm,    // 475 , 470 nm
    (1. / 2.8600) * cm,  (1. / 6.3900) * cm,    // 465 , 460 nm
    (1. / 9.9700) * cm,  (1. / 11.0645)* cm,    // 455 , 450 nm
    (1. / 10.198) * cm,  (1. / 9.4465) * cm,    // 445 , 440 nm
    (1. / 10.2145)* cm,  (1. / 12.240) * cm,    // 435 , 430 nm
    (1. / 12.519) * cm,  (1. / 10.867) * cm,    // 425 , 420 nm
    (1. / 9.0710) * cm,  (1. / 8.0895) * cm,    // 415 , 410 nm
    (1. / 7.6650) * cm,  (1. / 6.7170) * cm,    // 405 , 400 nm
    (1. / 5.2460) * cm,  (1. / 4.1185) * cm,    // 395 , 390 nm
    (1. / 3.3175) * cm,  (1. / 2.6800) * cm,    // 385 , 380 nm
    (1. / 1.9610) * cm,  (1. / 1.4220) * cm,    // 375 , 370 nm
    (1. / 1.0295) * cm,  (1. / 0.7680) * cm,    // 365 , 360 nm    
    (1. / 0.6865) * cm,  (1. / 0.5885) * cm,    // 355 , 350 nm
    noAbsLength_,        noAbsLength_
  };
  assert(sizeof(WLS_absLength) == sizeof(WLS_abs_energy));
  mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy,
                   WLS_absLength,  WLS_abs_entries); 

  // WLS EMISSION SPECTRUM
  G4double WLS_emi_energy[] = {
    optPhotMinE_,      nm_to_eV_ / 610.,
    nm_to_eV_ / 605.,  nm_to_eV_ / 600.,
    nm_to_eV_ / 595.,  nm_to_eV_ / 590.,
    nm_to_eV_ / 585.,  nm_to_eV_ / 580.,
    nm_to_eV_ / 575.,  nm_to_eV_ / 570.,
    nm_to_eV_ / 565.,  nm_to_eV_ / 560.,
    nm_to_eV_ / 555.,  nm_to_eV_ / 550.,
    nm_to_eV_ / 545.,  nm_to_eV_ / 540.,
    nm_to_eV_ / 535.,  nm_to_eV_ / 530.,
    nm_to_eV_ / 525.,  nm_to_eV_ / 520.,
    nm_to_eV_ / 515.,  nm_to_eV_ / 510.,
    nm_to_eV_ / 505.,  nm_to_eV_ / 500.,
    nm_to_eV_ / 495.,  nm_to_eV_ / 490.,
    nm_to_eV_ / 485.,  nm_to_eV_ / 480.,
    nm_to_eV_ / 475.,  nm_to_eV_ / 470.,
    nm_to_eV_ / 465.,  nm_to_eV_ / 460.,
    nm_to_eV_ / 455.,  nm_to_eV_ / 450.,
    nm_to_eV_ / 445.,  nm_to_eV_ / 440.,
    nm_to_eV_ / 435.,  optPhotMaxE_
  };
  const G4int WLS_emi_entries = sizeof(WLS_emi_energy) / sizeof(G4double);

  G4double WLS_emiSpectrum[] = {
    0.000,    0.000,   //     , 610 nm
    0.003,    0.006,   // 605 , 600 nm
    0.007,    0.009,   // 595 , 590 nm
    0.014,    0.017,   // 585 , 580 nm
    0.024,    0.033,   // 575 , 570 nm
    0.042,    0.051,   // 565 , 560 nm
    0.063,    0.081,   // 555 , 550 nm
    0.112,    0.157,   // 545 , 540 nm
    0.211,    0.274,   // 535 , 530 nm
    0.329,    0.341,   // 525 , 520 nm
    0.325,    0.346,   // 515 , 510 nm
    0.433,    0.578,   // 505 , 500 nm
    0.792,    1.000,   // 495 , 490 nm
    0.966,    0.718,   // 485 , 480 nm
    0.604,    0.681,   // 475 , 470 nm
    0.708,    0.525,   // 465 , 460 nm
    0.242,    0.046,   // 455 , 450 nm
    0.012,    0.003,   // 445 , 440 nm
    0.000,    0.000    // 435 ,     nm
  };
  assert(sizeof(WLS_emiSpectrum) == sizeof(WLS_emi_energy));
  mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy,
                   WLS_emiSpectrum, WLS_emi_entries);

  // WLS Delay
  mpt->AddConstProperty("WLSTIMECONSTANT", 8.5 * ns);

  // WLS Quantum Efficiency
  mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.86);

  return mpt;
}



/// EJ-286 ///
G4MaterialPropertiesTable* OpticalMaterialProperties::EJ286()
{
  // https://eljentechnology.com/products/wavelength-shifting-plastics/ej-280-ej-282-ej-284-ej-286
  // and data sheets from the provider.
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  G4double ri_energy[] = {
    optPhotMinE_,
    nm_to_eV_ / 609.,    nm_to_eV_ / 589.26,
    nm_to_eV_ / 550.,    nm_to_eV_ / 530.  ,
    nm_to_eV_ / 500.,    nm_to_eV_ / 490.  ,
    nm_to_eV_ / 481.,    nm_to_eV_ / 460.  ,
    nm_to_eV_ / 435.,    nm_to_eV_ / 425.  ,
    optPhotMaxE_
  };
  const G4int ri_entries = sizeof(ri_energy) / sizeof(G4double);

  G4double rIndex[] = {
    1.5780,
    1.5780,  1.5800,   // 609 , 589.26 nm
    1.5845,  1.5870,   // 550 , 530 nm
    1.5913,  1.5929,   // 500 , 490 nm
    1.5945,  1.5986,   // 481 , 460 nm
    1.6050,  1.6080,   // 435 , 425 nm
    1.6080
  };
  assert(sizeof(rIndex) == sizeof(ri_energy));
  mpt->AddProperty("RINDEX", ri_energy,
                   rIndex,   ri_entries);

  // ABSORPTION LENGTH
  G4double abs_energy[]  = {
    optPhotMinE_,     nm_to_eV_ / 535.,  nm_to_eV_ / 530.,
    nm_to_eV_ / 285., nm_to_eV_ / 280.,  optPhotMaxE_
  };
  G4double absLength[]   = {
    noAbsLength_,  3. * m,  3. * m,
    3. *  m,       3. * m,  noAbsLength_
  };
  assert(sizeof(absLength) == sizeof(abs_energy));
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, 2);

  // WLS ABSORPTION LENGTH
  G4double WLS_abs_energy[] = {
    optPhotMinE_,      nm_to_eV_ / 445.,
                       nm_to_eV_ / 440.,
    nm_to_eV_ / 435.,  nm_to_eV_ / 430.,
    nm_to_eV_ / 425.,  nm_to_eV_ / 420.,
    nm_to_eV_ / 415.,  nm_to_eV_ / 410.,
    nm_to_eV_ / 405.,  nm_to_eV_ / 400.,
    nm_to_eV_ / 395.,  nm_to_eV_ / 390.,
    nm_to_eV_ / 385.,  nm_to_eV_ / 380.,
    nm_to_eV_ / 375.,  nm_to_eV_ / 370.,
    nm_to_eV_ / 365.,  nm_to_eV_ / 360.,
    nm_to_eV_ / 355.,  nm_to_eV_ / 350.,
    nm_to_eV_ / 345.,  nm_to_eV_ / 340.,
    nm_to_eV_ / 335.,  nm_to_eV_ / 330.,
    nm_to_eV_ / 325.,  nm_to_eV_ / 320.,
    nm_to_eV_ / 315.,  nm_to_eV_ / 310.,
    nm_to_eV_ / 305.,  nm_to_eV_ / 300.,
    nm_to_eV_ / 295.,  nm_to_eV_ / 290.,
    nm_to_eV_ / 285.,  nm_to_eV_ / 280.,
    nm_to_eV_ / 275.,  optPhotMaxE_
  };
  const G4int WLS_abs_entries = sizeof(WLS_abs_energy) / sizeof(G4double);

  G4double WLS_absLength[] = {
    noAbsLength_, noAbsLength_,
                          (1. / 0.00007) * cm,    // 440 nm
    (1. /  0.0003) * cm,  (1. / 0.00104) * cm,    // 435 , 430 nm
    (1. / 0.00223) * cm,  (1. / 0.00408) * cm,    // 425 , 420 nm
    (1. /  0.0104) * cm,  (1. / 0.18544) * cm,    // 415 , 410 nm
    (1. /  1.4094) * cm,  (1. /  3.7088) * cm,    // 405 , 400 nm
    (1. /  7.4176) * cm,  (1. / 11.8682) * cm,    // 395 , 390 nm
    (1. / 16.6155) * cm,  (1. / 22.2529) * cm,    // 385 , 380 nm
    (1. / 27.8162) * cm,  (1. / 33.3794) * cm,    // 375 , 370 nm
    (1. / 37.8671) * cm,  (1. / 40.4262) * cm,    // 365 , 360 nm
    (1. / 41.5388) * cm,  (1. / 41.1679) * cm,    // 355 , 350 nm
    (1. / 38.9426) * cm,  (1. / 35.0113) * cm,    // 345 , 340 nm
    (1. / 31.1541) * cm,  (1. / 27.4453) * cm,    // 335 , 330 nm
    (1. / 23.4398) * cm,  (1. / 20.0276) * cm,    // 325 , 320 nm
    (1. / 16.3188) * cm,  (1. / 13.3518) * cm,    // 315 , 310 nm
    (1. / 10.5331) * cm,  (1. /  8.1594) * cm,    // 305 , 300 nm
    (1. /  6.1196) * cm,  (1. /  4.6731) * cm,    // 295 , 290 nm
    (1. /  3.6346) * cm,  (1. /  3.0412) * cm,    // 285 , 280 nm
    noAbsLength_, noAbsLength_
  };
  // XXX We are assuming that EJ286 doesn't absorb wave lengths shorter than 280 nm
  // although the spectrum continues ...
  assert(sizeof(WLS_absLength) == sizeof(WLS_abs_energy));
  mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy,
                   WLS_absLength,  WLS_abs_entries); 

  // WLS EMISSION SPECTRUM
  G4double WLS_emi_energy[] = {
    optPhotMinE_,
    nm_to_eV_ / 535.,  nm_to_eV_ / 530.,
    nm_to_eV_ / 525.,  nm_to_eV_ / 520.,
    nm_to_eV_ / 515.,  nm_to_eV_ / 510.,
    nm_to_eV_ / 505.,  nm_to_eV_ / 500.,
    nm_to_eV_ / 495.,  nm_to_eV_ / 490.,
    nm_to_eV_ / 485.,  nm_to_eV_ / 480.,
    nm_to_eV_ / 475.,  nm_to_eV_ / 470.,
    nm_to_eV_ / 465.,  nm_to_eV_ / 460.,
    nm_to_eV_ / 455.,  nm_to_eV_ / 450.,
    nm_to_eV_ / 445.,  nm_to_eV_ / 440.,
    nm_to_eV_ / 435.,  nm_to_eV_ / 430.,
    nm_to_eV_ / 425.,  nm_to_eV_ / 420.,
    nm_to_eV_ / 415.,  nm_to_eV_ / 410.,
    nm_to_eV_ / 405.,  nm_to_eV_ / 400.,
    nm_to_eV_ / 395.,  nm_to_eV_ / 390.,
    nm_to_eV_ / 385.,  nm_to_eV_ / 380.,
    nm_to_eV_ / 375.,  nm_to_eV_ / 370.,
    nm_to_eV_ / 365.,  nm_to_eV_ / 360.,
    nm_to_eV_ / 355.,  optPhotMaxE_
  };
  const G4int WLS_emi_entries = sizeof(WLS_emi_energy) / sizeof(G4double);

  G4double WLS_emiSpectrum[] = {
    0.0000,
    0.0000,  0.0089,   // 535 , 530 nm
    0.0100,  0.0181,   // 525 , 520 nm
    0.0210,  0.0270,   // 515 , 510 nm
    0.0380,  0.0496,   // 505 , 500 nm
    0.0600,  0.0721,   // 495 , 490 nm
    0.0900,  0.1125,   // 485 , 480 nm
    0.1500,  0.1848,   // 475 , 470 nm
    0.2100,  0.2388,   // 465 , 460 nm
    0.2800,  0.3289,   // 455 , 450 nm
    0.4000,  0.4956,   // 445 , 440 nm
    0.5700,  0.6230,   // 435 , 430 nm
    0.6450,  0.6667,   // 425 , 420 nm
    0.8000,  0.9800,   // 415 , 410 nm
    0.9900,  0.8559,   // 405 , 400 nm
    0.7118,  0.7400,   // 395 , 390 nm
    0.8000,  0.6702,   // 385 , 380 nm
    0.3800,  0.1082,   // 375 , 370 nm
    0.0400,  0.0089,   // 365 , 360 nm
    0.0000,  0.0000    // 355 ,     nm
  };
  assert(sizeof(WLS_emiSpectrum) == sizeof(WLS_emi_energy));
  mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy,
                   WLS_emiSpectrum, WLS_emi_entries);

  // WLS Delay
  mpt->AddConstProperty("WLSTIMECONSTANT", 1.2 * ns);

  // WLS Quantum Efficiency
  mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.92);

  return mpt;
}



/// Pethylene ///
// Fiber cladding material.
G4MaterialPropertiesTable* OpticalMaterialProperties::Pethylene()
{
  // Properties from geant4/examples/extended/optical/wls
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  const G4int rIndex_numEntries = 2;
  G4double rIndex_energies[rIndex_numEntries] = {optPhotMinE_, optPhotMaxE_};
  G4double TPB_rIndex[rIndex_numEntries]      = {1.49, 1.49};
  mpt->AddProperty("RINDEX", rIndex_energies,
                   TPB_rIndex, rIndex_numEntries);

  // ABSORPTION LENGTH
  G4double abs_energy[] = {optPhotMinE_, optPhotMaxE_};
  G4double absLength[]  = {noAbsLength_, noAbsLength_};
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, 2);

  return mpt;
}



/// FPethylene ///
// Fiber cladding material.
G4MaterialPropertiesTable* OpticalMaterialProperties::FPethylene()
{
  // Properties from geant4/examples/extended/optical/wls
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  const G4int rIndex_numEntries = 2;
  G4double rIndex_energies[rIndex_numEntries] = {optPhotMinE_, optPhotMaxE_};
  G4double TPB_rIndex[rIndex_numEntries]      = {1.42, 1.42};
  mpt->AddProperty("RINDEX", rIndex_energies,
                   TPB_rIndex, rIndex_numEntries);

  // ABSORPTION LENGTH
  G4double abs_energy[] = {optPhotMinE_, optPhotMaxE_};
  G4double absLength[]  = {noAbsLength_, noAbsLength_};
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, 2);

  return mpt;
}



/// PMMA == PolyMethylmethacrylate ///
// Fiber cladding material.
G4MaterialPropertiesTable* OpticalMaterialProperties::PMMA()
{
  // Properties from geant4/examples/extended/optical/wls
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  const G4int rIndex_numEntries = 2;
  G4double rIndex_energies[rIndex_numEntries] = {optPhotMinE_, optPhotMaxE_};
  G4double TPB_rIndex[rIndex_numEntries]      = {1.49, 1.49};
  mpt->AddProperty("RINDEX", rIndex_energies,
                   TPB_rIndex, rIndex_numEntries);

  // ABSORPTION LENGTH
  const G4int abs_entries = 16;
  G4double abs_energy[abs_entries] = {
    optPhotMinE_,
    2.722 * eV,  3.047 * eV,  3.097 * eV,  3.136 * eV,  3.168 * eV,  3.229 * eV,  3.291 * eV, 
    3.323 * eV,  3.345 * eV,  3.363 * eV,  3.397 * eV,  3.451 * eV,  3.511 * eV,  3.590 * eV,
    optPhotMaxE_
  };
  G4double abslength[abs_entries] = {
    noAbsLength_,
    noAbsLength_,  4537. * mm,  329.7 * mm,  98.60 * mm,  36.94 * mm,  10.36 * mm,  4.356 * mm, 
    2.563 * mm,    1.765 * mm,  1.474 * mm,  1.153 * mm,  0.922 * mm,  0.765 * mm,  0.671 * mm,
    0.671 * mm
  };
  mpt->AddProperty("ABSLENGTH", abs_energy, abslength, abs_entries);

  return mpt;
}



/// XXX ///
// Playing material properties
G4MaterialPropertiesTable* OpticalMaterialProperties::XXX()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  const G4int rIndex_numEntries = 2;
  G4double rIndex_energies[rIndex_numEntries] = {optPhotMinE_, optPhotMaxE_};
  G4double TPB_rIndex[rIndex_numEntries]      = {1.0    , 1.0};
  mpt->AddProperty("RINDEX", rIndex_energies,
                   TPB_rIndex, rIndex_numEntries);

  // ABSORPTION LENGTH
  G4double abs_energy[] = {optPhotMinE_, optPhotMaxE_};
  G4double absLength[]  = {10.*cm, 10.*cm};
  mpt->AddProperty("ABSLENGTH", abs_energy, absLength, 2);

  // WLS ABSORPTION LENGTH
  G4double WLS_abs_energy[] = {optPhotMinE_, optPhotMaxE_};
  G4double WLS_absLength[]  = {noAbsLength_, noAbsLength_};
  mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy,
                   WLS_absLength,  2); 

  // WLS EMISSION SPECTRUM
  G4double WLS_emi_energy[] = {optPhotMinE_, optPhotMaxE_};
  G4double WLS_emiSpectrum[] = {1.0, 1.0};
  mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy,
                   WLS_emiSpectrum, 2);

  // WLS Delay
  mpt->AddConstProperty("WLSTIMECONSTANT", 1. * ns);

  // WLS Quantum Efficiency
  mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 1.);

  return mpt;
}
