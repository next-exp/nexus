// ----------------------------------------------------------------------------
// nexus | OpticalMaterialProperties.cc
//
// Optical properties of relevant materials.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "OpticalMaterialProperties.h"
#include "XenonProperties.h"
#include "SellmeierEquation.h"

#include <G4MaterialPropertiesTable.hh>

#include <assert.h>

using namespace nexus;
using namespace CLHEP;

namespace opticalprops {
  /// Vacuum ///
  G4MaterialPropertiesTable* Vacuum()
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    std::vector<G4double> photEnergy = {optPhotMinE_, optPhotMaxE_};

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex = {1., 1.};
    mpt->AddProperty("RINDEX", photEnergy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> absLength = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", photEnergy, absLength);

    return mpt;
  }



  /// Fused Silica ///
  G4MaterialPropertiesTable* FusedSilica()
  {
    // Optical properties of Suprasil 311/312(c) synthetic fused silica.
    // Obtained from http://heraeus-quarzglas.com

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    // The range is chosen to be up to ~10.7 eV because Sellmeier's equation
    // for fused silica is valid only in that range
    const G4int ri_entries = 200;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
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

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
      G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1)
        + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
        + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
      rIndex.push_back(sqrt(n2));
      // G4cout << "* FusedSilica rIndex:  " << std::setw(5) << ri_energy[i]/eV
      //       << " eV -> " << rIndex[i] << G4endl;
    }
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
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

    std::vector<G4double> absLength = {
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

    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }



  /// Fake Fused Silica ///
  G4MaterialPropertiesTable* FakeFusedSilica(G4double transparency,
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

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
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

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
      G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1)
        + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
        + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
      rIndex.push_back(sqrt(n2));
      //G4cout << "* FakeFusedSilica rIndex:  " << std::setw(5)
      //       << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
    }
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH (Set to match the transparency)
    G4double abs_length     = -thickness / log(transparency);
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> abs_l      = {abs_length, abs_length};
    mpt->AddProperty("ABSLENGTH", abs_energy, abs_l);

    return mpt;
  }



  /// ITO ///
  G4MaterialPropertiesTable* ITO()
  {
    // Input data: complex refraction index obtained from:
    // https://refractiveindex.info/?shelf=other&book=In2O3-SnO2&page=Moerland
    // Only valid in [1000 - 400] nm

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    std::vector<G4double> energies = {
      optPhotMinE_,
      h_Planck * c_light / (1000. * nm),  h_Planck * c_light / (800. * nm),
      h_Planck * c_light / ( 700. * nm),  h_Planck * c_light / (600. * nm),
      h_Planck * c_light / ( 580. * nm),  h_Planck * c_light / (560. * nm),
      h_Planck * c_light / ( 540. * nm),  h_Planck * c_light / (520. * nm),
      h_Planck * c_light / ( 500. * nm),  h_Planck * c_light / (480. * nm),
      h_Planck * c_light / ( 460. * nm),  h_Planck * c_light / (440. * nm),
      h_Planck * c_light / ( 420. * nm),  h_Planck * c_light / (400. * nm),
      optPhotMaxE_ };

    std::vector<G4double> rIndex = {
      1.635,
      1.635, 1.775,
      1.835, 1.894,
      1.906, 1.919,
      1.931, 1.945,
      1.960, 1.975,
      1.993, 2.012,
      2.036, 2.064,
      2.064 };
    mpt->AddProperty("RINDEX", energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_length = {
      (1000. * nm) / (4*pi * 0.0103),
      (1000. * nm) / (4*pi * 0.0103),  (800. * nm) / (4*pi * 0.0049),
      ( 700. * nm) / (4*pi * 0.0033),  (600. * nm) / (4*pi * 0.0023),
      ( 580. * nm) / (4*pi * 0.0022),  (560. * nm) / (4*pi * 0.0022),
      ( 540. * nm) / (4*pi * 0.0022),  (520. * nm) / (4*pi * 0.0023),
      ( 500. * nm) / (4*pi * 0.0026),  (480. * nm) / (4*pi * 0.0031),
      ( 460. * nm) / (4*pi * 0.0039),  (440. * nm) / (4*pi * 0.0053),
      ( 420. * nm) / (4*pi * 0.0080),  (400. * nm) / (4*pi * 0.0125),
      (400. * nm) / (4*pi * 0.0125) };
    mpt->AddProperty("ABSLENGTH", energies, abs_length);

    //G4cout << "*** ITO properties ...  " << G4endl;
    //for (int i=0; i<num_energies; i++) {
    //  G4cout << "* Energy: " << std::setw(5) << energies[i]/eV << " eV  ->  " << std::setw(5)
    //         << (1240./ (energies[i] / eV)) << " nm" << G4endl;
    //  G4cout << "  rIndex    : " << std::setw(5) << rIndex[i]
    //         << "  Abs Length: " << std::setw(5) << abs_length[i] / nm << " nm" << G4endl;
    //}

    return mpt;
  }



  /// PEDOT ///
  G4MaterialPropertiesTable* PEDOT()
  {
    // Input data: complex refraction index obtained from:
    // https://refractiveindex.info/?shelf=other&book=PEDOT-PSS&page=Chen
    // Only valid in [1097 - 302] nm

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    std::vector<G4double> energies = {
      optPhotMinE_,
      h_Planck * c_light / (1097. * nm),  h_Planck * c_light / (1000. * nm),
      h_Planck * c_light / ( 950. * nm),  h_Planck * c_light / ( 900. * nm),
      h_Planck * c_light / ( 800. * nm),  h_Planck * c_light / ( 700. * nm),
      h_Planck * c_light / ( 600. * nm),  h_Planck * c_light / ( 550. * nm),
      h_Planck * c_light / ( 500. * nm),  h_Planck * c_light / ( 450. * nm),
      h_Planck * c_light / ( 420. * nm),  h_Planck * c_light / ( 400. * nm),
      h_Planck * c_light / ( 370. * nm),  h_Planck * c_light / ( 350. * nm),
      h_Planck * c_light / ( 302. * nm),  optPhotMaxE_ };

    std::vector<G4double> rIndex = {
      1.4760,
      1.4760, 1.4662,
      1.4665, 1.4693,
      1.4802, 1.4935,
      1.5080, 1.5155,
      1.5235, 1.5328,
      1.5391, 1.5439,
      1.5522, 1.5587,
      1.5805, 1.5805 };
    mpt->AddProperty("RINDEX", energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_length = {
      (1097. * nm) / (4*pi * 0.1191),
      (1097. * nm) / (4*pi * 0.1191),   (1000. * nm) / (4*pi * 0.0859),
      ( 950. * nm) / (4*pi * 0.0701),   ( 900. * nm) / (4*pi * 0.0561),
      ( 800. * nm) / (4*pi * 0.0340),   ( 700. * nm) / (4*pi * 0.0197),
      ( 600. * nm) / (4*pi * 0.0107),   ( 550. * nm) / (4*pi * 0.0076),
      ( 500. * nm) / (4*pi * 0.0051),   ( 450. * nm) / (4*pi * 0.0035),
      ( 420. * nm) / (4*pi * 0.0025),   ( 400. * nm) / (4*pi * 0.00194),
      ( 370. * nm) / (4*pi * 0.00135),  ( 350. * nm) / (4*pi * 0.00103),
      ( 302. * nm) / (4*pi * 0.0004),   ( 302. * nm) / (4*pi * 0.0004) };
    mpt->AddProperty("ABSLENGTH", energies, abs_length);

    //G4cout << "*** PEDOT properties ...  " << G4endl;
    //for (int i=0; i<num_energies; i++) {
    //  G4cout << "* Energy: " << std::setw(5) << energies[i]/eV << " eV  ->  " << std::setw(5)
    //         << (1240./ (energies[i] / eV)) << " nm" << G4endl;
    //  G4cout << "  rIndex    : " << std::setw(5) << rIndex[i]
    //         << "  Abs Length: " << std::setw(5) << abs_length[i] / nm << " nm" << G4endl;
    //}

    return mpt;
  }



  /// Glass Epoxy ///
  G4MaterialPropertiesTable* GlassEpoxy()
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

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
      G4double n2 = 2.291142 - 3.311944E-2*pow(lambda,2) - 1.630099E-2*pow(lambda,-2) +
                    7.265983E-3*pow(lambda,-4) - 6.806145E-4*pow(lambda,-6) +
                    1.960732E-5*pow(lambda,-8);
      rIndex.push_back(sqrt(n2));
      // G4cout << "* GlassEpoxy rIndex:  " << std::setw(5)
      //        << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
    }
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_, 2.000 * eV,
      2.132 * eV,   2.735 * eV,  2.908 * eV,  3.119 * eV,
      3.320 * eV,   3.476 * eV,  3.588 * eV,  3.749 * eV,
      3.869 * eV,   3.973 * eV,  4.120 * eV,  4.224 * eV,
      4.320 * eV,   4.420 * eV,  5.018 * eV
    };

    std::vector<G4double> absLength = {
      noAbsLength_, noAbsLength_,
      326.00 * mm,  117.68 * mm,  85.89 * mm,  50.93 * mm,
      31.25 * mm,   17.19 * mm,  10.46 * mm,   5.26 * mm,
        3.77 * mm,    2.69 * mm,   1.94 * mm,   1.33 * mm,
        0.73 * mm,    0.32 * mm,   0.10 * mm
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }



  /// Sapphire ///
  G4MaterialPropertiesTable* Sapphire()
  {
    // Input data: Sellmeier equation coeficients extracted from:
    // https://refractiveindex.info/?shelf=3d&book=crystals&page=sapphire
    //C[i] coeficients at line 362 are squared.

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    G4double um2 = micrometer*micrometer;
    G4double B[3] = {1.4313493, 0.65054713, 5.3414021};
    G4double C[3] = {0.0052799261 * um2, 0.0142382647 * um2, 325.017834 * um2};
    SellmeierEquation seq(B, C);

    const G4int ri_entries = 100;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      rIndex.push_back(seq.RefractiveIndex(h_Planck*c_light/ri_energy[i]));
      //G4cout << "* Sapphire rIndex:  " << std::setw(5)
      //       << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
    }
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_, 0.900 * eV,
      1.000 * eV,   1.296 * eV,  1.683 * eV,  2.075 * eV,
      2.585 * eV,   3.088 * eV,  3.709 * eV,  4.385 * eV,
      4.972 * eV,   5.608 * eV,  6.066 * eV,  6.426 * eV,
      6.806 * eV,   7.135 * eV,  7.401 * eV,  7.637 * eV,
      7.880 * eV,   8.217 * eV
    };

    std::vector<G4double> absLength = {
      noAbsLength_, noAbsLength_,
      3455.0  * mm,  3455.0  * mm,  3455.0  * mm,  3455.0  * mm,
      3455.0  * mm,  3140.98 * mm,  2283.30 * mm,  1742.11 * mm,
      437.06 * mm,   219.24 * mm,  117.773 * mm,   80.560 * mm,
      48.071 * mm,   28.805 * mm,   17.880 * mm,   11.567 * mm,
        7.718 * mm,    4.995 * mm
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }



  /// Optical Coupler ///
  G4MaterialPropertiesTable* OptCoupler()
  {
    // gel NyoGel OCK-451
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    G4double um2 = micrometer*micrometer;

    G4double constTerm  = 1.4954;
    G4double squareTerm = 0.008022 * um2;
    G4double quadTerm   = 0.;

    const G4int ri_entries = 100;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      G4double wl = h_Planck * c_light / ri_energy[i];
      rIndex.push_back(constTerm + squareTerm/(wl*wl) + quadTerm/pow(wl,4));
      //G4cout << "* OptCoupler rIndex:  " << std::setw(5)
      //       << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
    }
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    // Values estimated from printed plot (to be improved).
    std::vector<G4double> abs_energy = {
      optPhotMinE_,  1.70 * eV,
      1.77 * eV,     2.07 * eV,  2.48 * eV,  2.76 * eV,
      2.92 * eV,     3.10 * eV,  3.31 * eV,  3.54 * eV,
      3.81 * eV,     4.13 * eV
    };

    std::vector<G4double> absLength = {
      noAbsLength_, noAbsLength_,
      1332.8 * mm,  1332.8 * mm,  1332.8 * mm,  666.17 * mm,
      499.5 * mm,   399.5 * mm,   199.5 * mm,  132.83 * mm,
        99.5 * mm,     4.5 * mm
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }



  /// Gaseous Argon ///
  G4MaterialPropertiesTable* GAr(G4double sc_yield,
                                G4double e_lifetime)
  {
    // An argon gas proportional scintillation counter with UV avalanche photodiode scintillation
    // readout C.M.B. Monteiro, J.A.M. Lopes, P.C.P.S. Simoes, J.M.F. dos Santos, C.A.N. Conde

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    const G4int ri_entries = 200;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }

    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      G4double wl = h_Planck * c_light / ri_energy[i] * 1000; // in micron
      // From refractiveindex.info
      rIndex.push_back(1 + 0.012055*(0.2075*pow(wl,2)/(91.012*pow(wl,2)-1) +
                                     0.0415*pow(wl,2)/(87.892*pow(wl,2)-1) +
                                     4.3330*pow(wl,2)/(214.02*pow(wl,2)-1)));
      //G4cout << "* GAr rIndex:  " << std::setw(5) << ri_energy[i]/eV
      //       << " eV -> " << rIndex[i] << G4endl;
    }
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // EMISSION SPECTRUM
    G4double Wavelength_peak  = 128.000 * nm;
    G4double Wavelength_sigma =   2.929 * nm;
    G4double Energy_peak  = (h_Planck*c_light / Wavelength_peak);
    G4double Energy_sigma = (h_Planck*c_light * Wavelength_sigma / pow(Wavelength_peak,2));
    //G4cout << "*** GAr Energy_peak: " << Energy_peak/eV << " eV   Energy_sigma: "
    //       << Energy_sigma/eV << " eV" << G4endl;

    // Sampling from ~110 nm to 150 nm <----> from ~11.236 eV to 8.240 eV
    const G4int sc_entries = 380;
    std::vector<G4double> sc_energy;
    std::vector<G4double> intensity;
    for (int i=0; i<sc_entries; i++){
      sc_energy.push_back(8.240*eV + 0.008*i*eV);
      intensity.push_back(exp(-pow(Energy_peak/eV-sc_energy[i]/eV,2) /
                              (2*pow(Energy_sigma/eV, 2)))/(Energy_sigma/eV*sqrt(pi*2.)));
      //G4cout << "* GAr energy: " << std::setw(6) << sc_energy[i]/eV << " eV  ->  "
      //       << std::setw(6) << intensity[i] << G4endl;
    }
    mpt->AddProperty("SCINTILLATIONCOMPONENT1", sc_energy, intensity);
    mpt->AddProperty("SCINTILLATIONCOMPONENT2", sc_energy, intensity);
    mpt->AddProperty("ELSPECTRUM"             , sc_energy, intensity, 1);

    // CONST PROPERTIES
    mpt->AddConstProperty("SCINTILLATIONYIELD", sc_yield);
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1",   6.*ns);
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT2",   37.*ns);
    mpt->AddConstProperty("SCINTILLATIONYIELD1", .342);
    mpt->AddConstProperty("SCINTILLATIONYIELD2", .658);
    mpt->AddConstProperty("RESOLUTIONSCALE",    1.0);
    mpt->AddConstProperty("ATTACHMENT",         e_lifetime, 1);

    return mpt;
  }



  /// Gaseous Xenon ///
  G4MaterialPropertiesTable* GXe(G4double pressure,
                                G4double temperature,
                                G4int    sc_yield,
                                G4double e_lifetime)
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    const G4int ri_entries = 200;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }

    G4double density = GXeDensity(pressure);
    std::vector<G4double> rIndex;
    for (int i=0; i<ri_entries; i++) {
      rIndex.push_back(XenonRefractiveIndex(ri_energy[i], density));
      // G4cout << "* GXe rIndex:  " << std::setw(7)
      //        << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
    }
    mpt->AddProperty("RINDEX", ri_energy, rIndex, ri_entries);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // EMISSION SPECTRUM
    // Sampling from ~150 nm to 200 nm <----> from 6.20625 eV to 8.20625 eV
    const G4int sc_entries = 200;
    std::vector<G4double> sc_energy;
    for (int i=0; i<sc_entries; i++){
      sc_energy.push_back(6.20625 * eV + 0.01 * i * eV);
    }
    std::vector<G4double> intensity;
    for (G4int i=0; i<sc_entries; i++) {
      intensity.push_back(GXeScintillation(sc_energy[i], pressure));
    }
    //for (int i=0; i<sc_entries; i++) {
    //  G4cout << "* GXe Scint:  " << std::setw(7) << sc_energy[i]/eV
    //         << " eV -> " << intensity[i] << G4endl;
    //}
    mpt->AddProperty("SCINTILLATIONCOMPONENT1", sc_energy, intensity);
    mpt->AddProperty("SCINTILLATIONCOMPONENT2", sc_energy, intensity);
    mpt->AddProperty("ELSPECTRUM"             , sc_energy, intensity, 1);

    // CONST PROPERTIES
    mpt->AddConstProperty("SCINTILLATIONYIELD", sc_yield);
    mpt->AddConstProperty("RESOLUTIONSCALE",    1.0);
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1",   4.5  * ns);
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT2",   100. * ns);
    mpt->AddConstProperty("SCINTILLATIONYIELD1", .1);
    mpt->AddConstProperty("SCINTILLATIONYIELD2", .9);
    mpt->AddConstProperty("ATTACHMENT",         e_lifetime, 1);

    return mpt;
  }

  G4MaterialPropertiesTable* LXe()
  {
    /// The time constants are taken from E. Hogenbirk et al 2018 JINST 13 P10031
    G4MaterialPropertiesTable* LXe_mpt = new G4MaterialPropertiesTable();

    const G4int ri_entries = 200;
    G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

    std::vector<G4double> ri_energy;
    for (int i=0; i<ri_entries; i++) {
      ri_energy.push_back(optPhotMinE_ + i * eWidth);
    }

    G4double density = LXeDensity();
    std::vector<G4double> ri_index;
    for (G4int i=0; i<ri_entries; i++) {
      ri_index.push_back(XenonRefractiveIndex(ri_energy[i], density));
    }
    LXe_mpt->AddProperty("RINDEX", ri_energy, ri_index);

    // for (G4int i=ri_entries-1; i>=0; i--) {
    //   G4cout << h_Planck*c_light/ri_energy[i]/nanometer << " nm, " << rindex[i] << G4endl;
    // }

    // Sampling from ~151 nm to 200 nm <----> from 6.20625 eV to 8.21 eV
    const G4int sc_entries = 500;
    const G4double minE = 6.20625*eV;
    eWidth = (optPhotMaxE_ - minE) / sc_entries;

    std::vector<G4double> sc_energy;
    for (G4int j=0; j<sc_entries; j++){
      sc_energy.push_back(minE + j * eWidth);
    }
    std::vector<G4double> intensity;
    for (G4int i=0; i<sc_entries; i++) {
      intensity.push_back(LXeScintillation(sc_energy[i]));
    }

    LXe_mpt->AddProperty("SCINTILLATIONCOMPONENT1", sc_energy, intensity);
    LXe_mpt->AddProperty("SCINTILLATIONCOMPONENT2", sc_energy, intensity);

    LXe_mpt->AddConstProperty("SCINTILLATIONYIELD", 58708./MeV);
    LXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1);
    LXe_mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.*ns);
    LXe_mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 43.5*ns);
    LXe_mpt->AddConstProperty("SCINTILLATIONYIELD1", .03);
    LXe_mpt->AddConstProperty("SCINTILLATIONYIELD2", .97);
    LXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms, 1);

    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> abs_length = {noAbsLength_, noAbsLength_};

    LXe_mpt->AddProperty("ABSLENGTH", abs_energy, abs_length);

    std::vector<G4double> rayleigh_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> rayleigh_length = {36.*cm, 36.*cm};

    LXe_mpt->AddProperty("RAYLEIGH", rayleigh_energy, rayleigh_length);

    return LXe_mpt;
  }



  /// Fake Grid ///
  G4MaterialPropertiesTable* FakeGrid(G4double pressure,
                                      G4double temperature,
                                      G4double transparency,
                                      G4double thickness,
                                      G4int    sc_yield,
                                      G4double e_lifetime,
                                      G4double photoe_p)
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // PROPERTIES FROM XENON
    G4MaterialPropertiesTable* xenon_pt = opticalprops::GXe(pressure, temperature, sc_yield, e_lifetime);

    mpt->AddProperty("RINDEX",        xenon_pt->GetProperty("RINDEX"));
    mpt->AddProperty("SCINTILLATIONCOMPONENT1", xenon_pt->GetProperty("SCINTILLATIONCOMPONENT1"));
    mpt->AddProperty("SCINTILLATIONCOMPONENT2", xenon_pt->GetProperty("SCINTILLATIONCOMPONENT2"));

    mpt->AddConstProperty("SCINTILLATIONYIELD", xenon_pt->GetConstProperty("SCINTILLATIONYIELD"));
    mpt->AddConstProperty("RESOLUTIONSCALE",    xenon_pt->GetConstProperty("RESOLUTIONSCALE"));
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT1",   xenon_pt->GetConstProperty("SCINTILLATIONTIMECONSTANT1"));
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT2",   xenon_pt->GetConstProperty("SCINTILLATIONTIMECONSTANT2"));
    mpt->AddConstProperty("SCINTILLATIONYIELD1", xenon_pt->GetConstProperty("SCINTILLATIONYIELD1"));
    mpt->AddConstProperty("SCINTILLATIONYIELD2", xenon_pt->GetConstProperty("SCINTILLATIONYIELD2"));
    mpt->AddConstProperty("ATTACHMENT",         xenon_pt->GetConstProperty("ATTACHMENT"), 1);

    // ABSORPTION LENGTH
    G4double abs_length   = -thickness/log(transparency);
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {abs_length, abs_length};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // PHOTOELECTRIC REEMISSION
    // https://aip.scitation.org/doi/10.1063/1.1708797
    G4double stainless_wf = 4.3 * eV; // work function
    mpt->AddConstProperty("WORK_FUNCTION", stainless_wf, 1);
    mpt->AddConstProperty("OP_PHOTOELECTRIC_PROBABILITY", photoe_p, 1);

    return mpt;
  }



  /// PTFE (== TEFLON) ///
  G4MaterialPropertiesTable* PTFE()
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFLECTIVITY
    std::vector<G4double> ENERGIES = {
      optPhotMinE_,  2.8 * eV,  3.5 * eV,  4. * eV,
      6. * eV,       7.2 * eV,  optPhotMaxE_
    };
    std::vector<G4double> REFLECTIVITY = {
      .98,  .98,  .98,  .98,
      .72,  .72,  .72
    };
    mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY);

    // REFLEXION BEHAVIOR
    std::vector<G4double> ENERGIES_2    = {optPhotMinE_, optPhotMaxE_};
    // Specular reflection about the normal to a microfacet.
    // Such a vector is chosen according to a gaussian distribution with
    // sigma = SigmaAlhpa (in rad) and centered in the average normal.
    std::vector<G4double> specularlobe  = {0., 0.};
    // specular reflection about the average normal
    std::vector<G4double> specularspike = {0., 0.};
    // 180 degrees reflection.
    std::vector<G4double> backscatter   = {0., 0.};
    // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

    mpt->AddProperty("SPECULARLOBECONSTANT", ENERGIES_2, specularlobe);
    mpt->AddProperty("SPECULARSPIKECONSTANT",ENERGIES_2, specularspike);
    mpt->AddProperty("BACKSCATTERCONSTANT",  ENERGIES_2, backscatter);

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex = {1.41, 1.41};
    mpt->AddProperty("RINDEX", ENERGIES_2, rIndex);

    return mpt;
  }



  /// TPB (tetraphenyl butadiene) ///
  G4MaterialPropertiesTable* TPB()
  {
    // Data from https://doi.org/10.1140/epjc/s10052-018-5807-z
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> TPB_rIndex      = {1.67    , 1.67};
    mpt->AddProperty("RINDEX", rIndex_energies, TPB_rIndex);

    // ABSORPTION LENGTH
    // Assuming no absorption except WLS
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    // This is a combination of figure 11 (for wavelength > 270 nm) and
    // figure 20 (for 50 nm < wavelength < 250 nm).
    // XXX There is a mismatch in the border of the figures that anyway, we implement.
    // Fig 20 -> WLS_absLength = 400 nm for wavelength = 250 nm
    // Fig 11 -> WLS_absLength = 100 nm for wavelength = 270 nm
    // Values for wavelength shorter than 100 nm NOT included as they fit outside
    // the simulation energy limits set in the header.

    //G4double WLS_abs_energy[] = {
    //  optPhotMinE_,                      h_Planck * c_light / (450. * nm),
    //  h_Planck * c_light / (440. * nm),  h_Planck * c_light / (430. * nm),
    //  h_Planck * c_light / (420. * nm),  h_Planck * c_light / (410. * nm),
    //  h_Planck * c_light / (400. * nm),  h_Planck * c_light / (390. * nm),
    //  h_Planck * c_light / (380. * nm),  h_Planck * c_light / (370. * nm),
    //  h_Planck * c_light / (360. * nm),  h_Planck * c_light / (330. * nm),
    //  h_Planck * c_light / (320. * nm),  h_Planck * c_light / (310. * nm),
    //  h_Planck * c_light / (300. * nm),  h_Planck * c_light / (270. * nm),
    //  h_Planck * c_light / (250. * nm),  h_Planck * c_light / (230. * nm),
    //  h_Planck * c_light / (210. * nm),  h_Planck * c_light / (190. * nm),
    //  h_Planck * c_light / (170. * nm),  h_Planck * c_light / (150. * nm),
    //  h_Planck * c_light / (100. * nm),  optPhotMaxE_
    //};
    //const G4int WLS_abs_entries = sizeof(WLS_abs_energy) / sizeof(G4double);
  //
    //G4double WLS_absLength[] = {
    //  noAbsLength_,  noAbsLength_,  //       450 nm
    //  1.e6 * nm,     1.e5 * nm,     // 440 , 430 nm
    //  2.2e4 * nm,     7.e3 * nm,     // 420 , 410 nm
    //  2.2e3 * nm,     700. * nm,     // 400 , 390 nm
    //  200. * nm,      50. * nm,     // 380 , 370 nm
    //   30. * nm,      30. * nm,     // 360 , 330 nm
    //   50. * nm,      80. * nm,     // 320 , 310 nm
    //  100. * nm,     100. * nm,     // 300 , 270 nm
    //  400. * nm,     400. * nm,     // 250 , 230 nm
    //  350. * nm,     250. * nm,     // 210 , 190 nm
    //  350. * nm,     400. * nm,     // 170 , 150 nm
    //  400. * nm,     noAbsLength_   // 100 nm
    //};

    // WLS ABSORPTION LENGTH (Version NoSecWLS)
    // The NoSecWLS is forced by setting the WLS_absLength to noAbsLength_
    // for wavelengths higher than 380 nm where the WLS emission spectrum starts.
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,
      h_Planck * c_light / (380. * nm),  h_Planck * c_light / (370. * nm),
      h_Planck * c_light / (360. * nm),  h_Planck * c_light / (330. * nm),
      h_Planck * c_light / (320. * nm),  h_Planck * c_light / (310. * nm),
      h_Planck * c_light / (300. * nm),  h_Planck * c_light / (270. * nm),
      h_Planck * c_light / (250. * nm),  h_Planck * c_light / (230. * nm),
      h_Planck * c_light / (210. * nm),  h_Planck * c_light / (190. * nm),
      h_Planck * c_light / (170. * nm),  h_Planck * c_light / (150. * nm),
      h_Planck * c_light / (100. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_absLength = {
      noAbsLength_,
      noAbsLength_,   50. * nm,     // 380 , 370 nm
      30. * nm,      30. * nm,     // 360 , 330 nm
      50. * nm,      80. * nm,     // 320 , 310 nm
      100. * nm,     100. * nm,     // 300 , 270 nm
      400. * nm,     400. * nm,     // 250 , 230 nm
      350. * nm,     250. * nm,     // 210 , 190 nm
      350. * nm,     400. * nm,     // 170 , 150 nm
      400. * nm,     noAbsLength_   // 100 nm
    };

    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* TPB WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (h_Planck * c_light / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / nm << " nm" << G4endl;
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);

    // WLS EMISSION SPECTRUM
    // Implemented with formula (7), with parameter values in table (3)
    // Sampling from ~380 nm to 600 nm <--> from 2.06 to 3.26 eV
    const G4int WLS_emi_entries = 120;
    std::vector<G4double> WLS_emi_energy;
    for (int i=0; i<WLS_emi_entries; i++)
      WLS_emi_energy.push_back(2.06 * eV + 0.01 * i * eV);

    std::vector<G4double> WLS_emiSpectrum;
    G4double A      = 0.782;
    G4double alpha  = 3.7e-2;
    G4double sigma1 = 15.43;
    G4double mu1    = 418.10;
    G4double sigma2 = 9.72;
    G4double mu2    = 411.2;

    for (int i=0; i<WLS_emi_entries; i++) {
      G4double wl = (h_Planck * c_light / WLS_emi_energy[i]) / nm;
      WLS_emiSpectrum.push_back(A * (alpha/2.) * exp((alpha/2.) *
                          (2*mu1 + alpha*pow(sigma1,2) - 2*wl)) *
                          erfc((mu1 + alpha*pow(sigma1,2) - wl) / (sqrt(2)*sigma1)) +
                          (1-A) * (1 / sqrt(2*pow(sigma2,2)*3.1416)) *
                                exp((-pow(wl-mu2,2)) / (2*pow(sigma2,2))));
      // G4cout << "* TPB WLSemi:  " << std::setw(4)
      //        << wl << " nm -> " << WLS_emiSpectrum[i] << G4endl;
    };
    mpt->AddProperty("WLSCOMPONENT", WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 1.2 * ns);

    // WLS Quantum Efficiency
    // According to the paper, the QE of TPB depends on the incident wavelength.
    // As Geant4 doesn't allow this possibility, it is set to the value corresponding
    // to Xe scintillation spectrum peak.
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.65);

    return mpt;
  }



  /// Degraded TPB ///
  G4MaterialPropertiesTable* DegradedTPB(G4double wls_eff)
  {
    // It has all the same properties of TPB except the WaveLengthShifting robability
    // that is set by parameter, trying to model a degraded behaviour of the TPB coating

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // All Optical Material Properties from normal TPB ...
    mpt->AddProperty("RINDEX",       opticalprops::TPB()->GetProperty("RINDEX"));
    mpt->AddProperty("ABSLENGTH",    opticalprops::TPB()->GetProperty("ABSLENGTH"));
    mpt->AddProperty("WLSABSLENGTH", opticalprops::TPB()->GetProperty("WLSABSLENGTH"));
    mpt->AddProperty("WLSCOMPONENT", opticalprops::TPB()->GetProperty("WLSCOMPONENT"));
    mpt->AddConstProperty("WLSTIMECONSTANT", opticalprops::TPB()->GetConstProperty("WLSTIMECONSTANT"));

    // Except WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", wls_eff);

    return mpt;
  }



  /// TPH ///
  G4MaterialPropertiesTable* TPH()
  {
    // from http://omlc.ogi.edu/spectra/PhotochemCAD/html/p-terphenyl.html
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> ri_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> rIndex    = {1.65, 1.65};
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,
      2.5 * eV,  3.0 * eV,  3.4 * eV,  3.8 * eV,
      3.9 * eV,  4.0 * eV,  5.0 * eV,
      optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.00,
      0.00,  0.00,  0.00,  1.00,
      0.00,  0.00,  0.00,
      0.00
    };
    // XXX TPH WLS emission spectrum set flat between 3.7 and 3.9 eV (To be fixed)
    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_, 2.0 * eV,  3.9 * eV,  4.5 * eV,
      5.5 * eV,     8.0 * eV,  optPhotMaxE_
    };

    std::vector<G4double> WLS_absLength = {
      noAbsLength_,  noAbsLength_,  noAbsLength_,  0.0177 * cm,
      noAbsLength_,  noAbsLength_,  noAbsLength_
    };
    // XXX TPH WLS AbsLength modelled being a triangle centered at 4.5 eV ->
    // No absorption for Xe scint light (To be fixed.)
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);

    // CONST PROPERTIES
    mpt->AddConstProperty("WLSTIMECONSTANT", 0.5 * ns);

    return mpt;
  }



  /// EJ-280 ///
  G4MaterialPropertiesTable* EJ280()
  {
    // https://eljentechnology.com/products/wavelength-shifting-plastics/ej-280-ej-282-ej-284-ej-286
    // and data sheets from the provider.
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> ri_energy = {
      optPhotMinE_,
      h_Planck * c_light / (609. * nm),  h_Planck * c_light / (589.26 * nm),
      h_Planck * c_light / (550. * nm),  h_Planck * c_light / (530.   * nm),
      h_Planck * c_light / (500. * nm),  h_Planck * c_light / (490.   * nm),
      h_Planck * c_light / (481. * nm),  h_Planck * c_light / (460.   * nm),
      h_Planck * c_light / (435. * nm),  h_Planck * c_light / (425.   * nm),
      optPhotMaxE_
    };

    std::vector<G4double> rIndex = {
      1.5780,
      1.5780,  1.5800,   // 609 , 589.26 nm
      1.5845,  1.5870,   // 550 , 530 nm
      1.5913,  1.5929,   // 500 , 490 nm
      1.5945,  1.5986,   // 481 , 460 nm
      1.6050,  1.6080,   // 435 , 425 nm
      1.608
    };
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (750. * nm),
      h_Planck * c_light / (740. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (370. * nm),  optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,  noAbsLength_,
      3.0 * m,       3.0 * m,
      noAbsLength_,  noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (500. * nm),
      h_Planck * c_light / (495. * nm),  h_Planck * c_light / (490. * nm),
      h_Planck * c_light / (485. * nm),  h_Planck * c_light / (480. * nm),
      h_Planck * c_light / (475. * nm),  h_Planck * c_light / (470. * nm),
      h_Planck * c_light / (465. * nm),  h_Planck * c_light / (460. * nm),
      h_Planck * c_light / (455. * nm),  h_Planck * c_light / (450. * nm),
      h_Planck * c_light / (445. * nm),  h_Planck * c_light / (440. * nm),
      h_Planck * c_light / (435. * nm),  h_Planck * c_light / (430. * nm),
      h_Planck * c_light / (425. * nm),  h_Planck * c_light / (420. * nm),
      h_Planck * c_light / (415. * nm),  h_Planck * c_light / (410. * nm),
      h_Planck * c_light / (405. * nm),  h_Planck * c_light / (400. * nm),
      h_Planck * c_light / (395. * nm),  h_Planck * c_light / (390. * nm),
      h_Planck * c_light / (385. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (375. * nm),  h_Planck * c_light / (370. * nm),
      h_Planck * c_light / (365. * nm),  h_Planck * c_light / (360. * nm),
      h_Planck * c_light / (355. * nm),  h_Planck * c_light / (350. * nm),
      h_Planck * c_light / (345. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_absLength = {
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
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);
    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* EJ280 WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (h_Planck * c_light / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / mm << " mm" << G4endl;

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,                      h_Planck * c_light / (610. * nm),
      h_Planck * c_light / (605. * nm),  h_Planck * c_light / (600. * nm),
      h_Planck * c_light / (595. * nm),  h_Planck * c_light / (590. * nm),
      h_Planck * c_light / (585. * nm),  h_Planck * c_light / (580. * nm),
      h_Planck * c_light / (575. * nm),  h_Planck * c_light / (570. * nm),
      h_Planck * c_light / (565. * nm),  h_Planck * c_light / (560. * nm),
      h_Planck * c_light / (555. * nm),  h_Planck * c_light / (550. * nm),
      h_Planck * c_light / (545. * nm),  h_Planck * c_light / (540. * nm),
      h_Planck * c_light / (535. * nm),  h_Planck * c_light / (530. * nm),
      h_Planck * c_light / (525. * nm),  h_Planck * c_light / (520. * nm),
      h_Planck * c_light / (515. * nm),  h_Planck * c_light / (510. * nm),
      h_Planck * c_light / (505. * nm),  h_Planck * c_light / (500. * nm),
      h_Planck * c_light / (495. * nm),  h_Planck * c_light / (490. * nm),
      h_Planck * c_light / (485. * nm),  h_Planck * c_light / (480. * nm),
      h_Planck * c_light / (475. * nm),  h_Planck * c_light / (470. * nm),
      h_Planck * c_light / (465. * nm),  h_Planck * c_light / (460. * nm),
      h_Planck * c_light / (455. * nm),  h_Planck * c_light / (450. * nm),
      h_Planck * c_light / (445. * nm),  h_Planck * c_light / (440. * nm),
      h_Planck * c_light / (435. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
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
    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 8.5 * ns);

    // WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.86);

    return mpt;
  }



  /// EJ-286 ///
  G4MaterialPropertiesTable* EJ286()
  {
    // https://eljentechnology.com/products/wavelength-shifting-plastics/ej-280-ej-282-ej-284-ej-286
    // and data sheets from the provider.
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> ri_energy = {
      optPhotMinE_,
      h_Planck * c_light / (609. * nm),    h_Planck * c_light / (589.26 * nm),
      h_Planck * c_light / (550. * nm),    h_Planck * c_light / (530.   * nm),
      h_Planck * c_light / (500. * nm),    h_Planck * c_light / (490.   * nm),
      h_Planck * c_light / (481. * nm),    h_Planck * c_light / (460.   * nm),
      h_Planck * c_light / (435. * nm),    h_Planck * c_light / (425.   * nm),
      optPhotMaxE_
    };

    std::vector<G4double> rIndex = {
      1.5780,
      1.5780,  1.5800,   // 609 , 589.26 nm
      1.5845,  1.5870,   // 550 , 530 nm
      1.5913,  1.5929,   // 500 , 490 nm
      1.5945,  1.5986,   // 481 , 460 nm
      1.6050,  1.6080,   // 435 , 425 nm
      1.6080
    };
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (750. * nm),
      h_Planck * c_light / (740. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (370. * nm),  optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,  noAbsLength_,
      3.0 * m,       3.0 * m,
      noAbsLength_,  noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (445. * nm),
                                        h_Planck * c_light / (440. * nm),
      h_Planck * c_light / (435. * nm),  h_Planck * c_light / (430. * nm),
      h_Planck * c_light / (425. * nm),  h_Planck * c_light / (420. * nm),
      h_Planck * c_light / (415. * nm),  h_Planck * c_light / (410. * nm),
      h_Planck * c_light / (405. * nm),  h_Planck * c_light / (400. * nm),
      h_Planck * c_light / (395. * nm),  h_Planck * c_light / (390. * nm),
      h_Planck * c_light / (385. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (375. * nm),  h_Planck * c_light / (370. * nm),
      h_Planck * c_light / (365. * nm),  h_Planck * c_light / (360. * nm),
      h_Planck * c_light / (355. * nm),  h_Planck * c_light / (350. * nm),
      h_Planck * c_light / (345. * nm),  h_Planck * c_light / (340. * nm),
      h_Planck * c_light / (335. * nm),  h_Planck * c_light / (330. * nm),
      h_Planck * c_light / (325. * nm),  h_Planck * c_light / (320. * nm),
      h_Planck * c_light / (315. * nm),  h_Planck * c_light / (310. * nm),
      h_Planck * c_light / (305. * nm),  h_Planck * c_light / (300. * nm),
      h_Planck * c_light / (295. * nm),  h_Planck * c_light / (290. * nm),
      h_Planck * c_light / (285. * nm),  h_Planck * c_light / (280. * nm),
      h_Planck * c_light / (275. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_absLength = {
      noAbsLength_,         noAbsLength_,
                            (1. / 0.00007) * cm,    //       440 nm
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
      noAbsLength_,         noAbsLength_
    };
    // XXX We are assuming that EJ286 doesn't absorb wave lengths shorter than 280 nm
    // although the spectrum continues ...
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);
    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* EJ286 WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (h_Planck * c_light / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / mm << " mm" << G4endl;

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,
      h_Planck * c_light / (535. * nm),  h_Planck * c_light / (530. * nm),
      h_Planck * c_light / (525. * nm),  h_Planck * c_light / (520. * nm),
      h_Planck * c_light / (515. * nm),  h_Planck * c_light / (510. * nm),
      h_Planck * c_light / (505. * nm),  h_Planck * c_light / (500. * nm),
      h_Planck * c_light / (495. * nm),  h_Planck * c_light / (490. * nm),
      h_Planck * c_light / (485. * nm),  h_Planck * c_light / (480. * nm),
      h_Planck * c_light / (475. * nm),  h_Planck * c_light / (470. * nm),
      h_Planck * c_light / (465. * nm),  h_Planck * c_light / (460. * nm),
      h_Planck * c_light / (455. * nm),  h_Planck * c_light / (450. * nm),
      h_Planck * c_light / (445. * nm),  h_Planck * c_light / (440. * nm),
      h_Planck * c_light / (435. * nm),  h_Planck * c_light / (430. * nm),
      h_Planck * c_light / (425. * nm),  h_Planck * c_light / (420. * nm),
      h_Planck * c_light / (415. * nm),  h_Planck * c_light / (410. * nm),
      h_Planck * c_light / (405. * nm),  h_Planck * c_light / (400. * nm),
      h_Planck * c_light / (395. * nm),  h_Planck * c_light / (390. * nm),
      h_Planck * c_light / (385. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (375. * nm),  h_Planck * c_light / (370. * nm),
      h_Planck * c_light / (365. * nm),  h_Planck * c_light / (360. * nm),
      h_Planck * c_light / (355. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
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
    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 1.2 * ns);

    // WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.92);

    return mpt;
  }



  /// Y-11 ///
  G4MaterialPropertiesTable* Y11()
  {
    // http://kuraraypsf.jp/psf/index.html
    // http://kuraraypsf.jp/psf/ws.html
    // Excel provided by kuraray with Tabulated WLS absorption lengths
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> ri_energy = {
      optPhotMinE_,  optPhotMaxE_
    };
    std::vector<G4double> rIndex = {
      1.59,  1.59
    };
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (750. * nm),
      h_Planck * c_light / (740. * nm),  h_Planck * c_light / (380. * nm),
      h_Planck * c_light / (370. * nm),  optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,  noAbsLength_,
      3.5 * m,       3.5 * m,
      noAbsLength_,  noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,                      h_Planck * c_light / (490. * nm),
      h_Planck * c_light / (485. * nm),  h_Planck * c_light / (475. * nm),
      h_Planck * c_light / (454. * nm),  h_Planck * c_light / (443. * nm),
      h_Planck * c_light / (430. * nm),  h_Planck * c_light / (410. * nm),
      h_Planck * c_light / (405. * nm),  h_Planck * c_light / (359. * nm),
      h_Planck * c_light / (350. * nm),  h_Planck * c_light / (345. * nm),
      optPhotMaxE_
    };
    std::vector<G4double> WLS_absLength = {
      noAbsLength_,  noAbsLength_,    //     , 490 nm
      44.2  * mm,    5.39 * mm,       // 485 , 475 nm
      0.395 * mm,    0.462 * mm,      // 454 , 443 nm
      0.354 * mm,    0.571 * mm,      // 430 , 410 nm
      0.612 * mm,    4.51 * mm,       // 405 , 359 nm
      4.81  * mm,    noAbsLength_,    // 350 , 345 nm
      noAbsLength_
    };
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);
    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* Y11 WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (h_Planck * c_light / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / mm << " mm" << G4endl;

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,                      h_Planck * c_light / (580. * nm),
      h_Planck * c_light / (550. * nm),  h_Planck * c_light / (530. * nm),
      h_Planck * c_light / (525. * nm),  h_Planck * c_light / (520. * nm),
      h_Planck * c_light / (515. * nm),  h_Planck * c_light / (510. * nm),
      h_Planck * c_light / (505. * nm),  h_Planck * c_light / (500. * nm),
      h_Planck * c_light / (495. * nm),  h_Planck * c_light / (490. * nm),
      h_Planck * c_light / (485. * nm),  h_Planck * c_light / (480. * nm),
      h_Planck * c_light / (475. * nm),  h_Planck * c_light / (470. * nm),
      h_Planck * c_light / (465. * nm),  h_Planck * c_light / (460. * nm),
      h_Planck * c_light / (455. * nm),  h_Planck * c_light / (450. * nm),
      h_Planck * c_light / (445. * nm),  optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.000,    0.000,   //     , 580 nm
      0.200,    0.300,   // 550 , 530 nm
      0.400,    0.600,   // 525 , 520 nm
      0.750,    0.750,   // 515 , 510 nm
      0.720,    0.700,   // 505 , 500 nm
      0.680,    0.650,   // 495 , 490 nm
      0.700,    0.900,   // 485 , 480 nm
      1.000,    0.950,   // 475 , 470 nm
      0.500,    0.300,   // 465 , 460 nm
      0.100,    0.050,   // 455 , 450 nm
      0.000,    0.000    // 445 ,     nm
    };
    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 8.5 * ns);

    // WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.87);

    return mpt;
  }



  /// Pethylene ///
  G4MaterialPropertiesTable* Pethylene()
  {
    // Fiber cladding material.
    // Properties from geant4/examples/extended/optical/wls
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> rIndex          = {1.49, 1.49};
    mpt->AddProperty("RINDEX", rIndex_energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }



  /// FPethylene ///
  G4MaterialPropertiesTable* FPethylene()
  {
    // Fiber cladding material.
    // Properties from geant4/examples/extended/optical/wls
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> rIndex          = {1.42, 1.42};
    mpt->AddProperty("RINDEX", rIndex_energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    return mpt;
  }



  /// PMMA == PolyMethylmethacrylate ///
  G4MaterialPropertiesTable* PMMA()
  {
    // Fiber cladding material.
    // Properties from geant4/examples/extended/optical/wls
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> rIndex          = {1.49, 1.49};
    mpt->AddProperty("RINDEX", rIndex_energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,
      2.722 * eV,  3.047 * eV,  3.097 * eV,  3.136 * eV,  3.168 * eV,  3.229 * eV,  3.291 * eV,
      3.323 * eV,  3.345 * eV,  3.363 * eV,  3.397 * eV,  3.451 * eV,  3.511 * eV,  3.590 * eV,
      optPhotMaxE_
    };
    std::vector<G4double> abslength = {
      noAbsLength_,
      noAbsLength_,  4537. * mm,  329.7 * mm,  98.60 * mm,  36.94 * mm,  10.36 * mm,  4.356 * mm,
      2.563 * mm,    1.765 * mm,  1.474 * mm,  1.153 * mm,  0.922 * mm,  0.765 * mm,  0.671 * mm,
      0.671 * mm
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, abslength);

    return mpt;
  }



  /// XXX ///
  G4MaterialPropertiesTable* XXX()
  {
    // Playing material properties
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> rIndex          = {1.0    , 1.0};
    mpt->AddProperty("RINDEX", rIndex_energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {10.*cm, 10.*cm};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> WLS_absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy  = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> WLS_emiSpectrum = {1.0, 1.0};
    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 1. * ns);

    // WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 1.);

    return mpt;
  }
}
