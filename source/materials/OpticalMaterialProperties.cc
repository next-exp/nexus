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
    G4double eWidth = (optPhotFusedSilicaMaxE_ - optPhotMinE_) / ri_entries;

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
      G4double lambda = hc_/ri_energy[i]*1000; // in micron
      G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1)
        + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
        + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
      rIndex.push_back(sqrt(n2));
      // G4cout << "* FusedSilica rIndex:  " << std::setw(5) << ri_energy[i]/eV
      //       << " eV -> " << rIndex[i] << G4endl;
    }
    ri_energy.push_back(optPhotMaxE_);          // This sets the refractive index between optPhotFusedSilicaMaxE_ and
    rIndex.push_back(rIndex[rIndex.size()-1]);  // optPhotMaxE_ to the value obtained at optPhotFusedSilicaMaxE_
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
    G4double eWidth = (optPhotFusedSilicaMaxE_ - optPhotMinE_) / ri_entries;

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
      G4double lambda = hc_/ri_energy[i]*1000; // in micron
      G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1)
        + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
        + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
      rIndex.push_back(sqrt(n2));
      //G4cout << "* FakeFusedSilica rIndex:  " << std::setw(5)
      //       << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
    }
    ri_energy.push_back(optPhotMaxE_);          // This sets the refractive index between optPhotFusedSilicaMaxE_ and
    rIndex.push_back(rIndex[rIndex.size()-1]);  // optPhotMaxE_ to the value obtained at optPhotFusedSilicaMaxE_
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH (Set to match the transparency)
    G4double abs_length     = -thickness / log(transparency);
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> abs_l      = {abs_length, abs_length};
    mpt->AddProperty("ABSLENGTH", abs_energy, abs_l);

    return mpt;
  }


  G4MaterialPropertiesTable* Epoxy()
  {
    // This is the material used as a window for NEXT-100 SiPMs.
    // The only information we have is that n = 1.55 at a
    // not specified wavelength. It seems that it's common to measure
    // the refractive index at the D line of sodium (590 nm),
    // therefore we assume that.
    // The dependence of n on the wavelength is taken from
    // https://www.epotek.com/docs/en/Related/Tech%20Tip%2018%20Understanding%20Optical%20Properties%20of%20Epoxy%20Applications.pdf,
    // shifting the whole graphs in y down to match 1.55 at 590 nm.
    // We fill the values outside the range of the plot withg the
    // minimum and maximum values.

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
     std::vector<G4double> ri_energy = {
      optPhotMinE_,
      hc_ / (2451.63 * nm), hc_ / (2430.88 * nm), hc_ / (2405.51 * nm),
      hc_ / (2380.14 * nm), hc_ / (2354.77 * nm), hc_ / (2329.41 * nm),
      hc_ / (2304.04 * nm), hc_ / (2278.67 * nm), hc_ / (2253.3 * nm),
      hc_ / (2227.94 * nm), hc_ / (2202.57 * nm), hc_ / (2177.2 * nm),
      hc_ / (2151.83 * nm), hc_ / (2126.47 * nm), hc_ / (2101.1 * nm),
      hc_ / (2075.73 * nm), hc_ / (2050.36 * nm), hc_ / (2025.0 * nm),
      hc_ / (1968.5 * nm),  hc_ / (1951.2 * nm),  hc_ / (1925.83 * nm),
      hc_ / (1900.47 * nm), hc_ / (1875.1 * nm),  hc_ / (1849.73 * nm),
      hc_ / (1824.36 * nm), hc_ / (1799.0 * nm),  hc_ / (1773.63 * nm),
      hc_ / (1747.6 * nm),  hc_ / (1722.89 * nm), hc_ / (1697.53 * nm),
      hc_ / (1672.16 * nm), hc_ / (1646.79 * nm), hc_ / (1621.42 * nm),
      hc_ / (1596.06 * nm), hc_ / (1570.69 * nm), hc_ / (1545.32 * nm),
      hc_ / (1519.95 * nm), hc_ / (1494.59 * nm), hc_ / (1469.22 * nm),
      hc_ / (1443.85 * nm), hc_ / (1418.49 * nm), hc_ / (1393.12 * nm),
      hc_ / (1367.75 * nm), hc_ / (1342.38 * nm), hc_ / (1317.02 * nm),
      hc_ / (1291.65 * nm), hc_ / (1266.28 * nm), hc_ / (1240.91 * nm),
      hc_ / (1215.55 * nm), hc_ / (1190.18 * nm), hc_ / (1164.81 * nm),
      hc_ / (1139.44 * nm), hc_ / (1114.08 * nm), hc_ / (1088.71 * nm),
      hc_ / (1063.34 * nm), hc_ / (1037.97 * nm), hc_ / (1012.61 * nm),
      hc_ / (987.24 * nm),  hc_ / (961.87 * nm),  hc_ / (936.5 * nm),
      hc_ / (911.14 * nm),  hc_ / (885.77 * nm),  hc_ / (860.4 * nm),
      hc_ / (835.03 * nm),  hc_ / (809.67 * nm),  hc_ / (784.3 * nm),
      hc_ / (758.93 * nm),  hc_ / (733.57 * nm),  hc_ / (708.2 * nm),
      hc_ / (682.83 * nm),  hc_ / (657.46 * nm),  hc_ / (624.02 * nm),
      hc_ / (606.73 * nm),  hc_ / (587.13 * nm),  hc_ / (569.83 * nm),
      hc_ / (554.84 * nm),  hc_ / (541.0 * nm),   hc_ / (519.1 * nm),
      hc_ / (509.87 * nm),  hc_ / (499.49 * nm),  hc_ / (490.27 * nm),
      hc_ / (481.04 * nm),  hc_ / (470.67 * nm),  hc_ / (456.83 * nm),
      hc_ / (451.06 * nm),  hc_ / (442.99 * nm),   hc_ / (434.92 * nm),
      hc_ / (426.85 * nm),  hc_ / (417.63 * nm), hc_ / (401.48 * nm),
      hc_ / (395.95 * nm), optPhotMaxE_
     };

     std::vector<G4double> rIndex = {
       1.524,
       1.524, 1.525, 1.525,
       1.525, 1.525, 1.526,
       1.526, 1.526, 1.526,
       1.526, 1.527, 1.527,
       1.527, 1.527, 1.527,
       1.528, 1.528, 1.528,
       1.528, 1.528, 1.528,
       1.529, 1.529, 1.529,
       1.529, 1.529, 1.53,
       1.53,  1.53,  1.53,
       1.531, 1.531, 1.531,
       1.531, 1.532, 1.532,
       1.532, 1.532, 1.533,
       1.533, 1.533, 1.533,
       1.534, 1.534, 1.534,
       1.534, 1.535, 1.535,
       1.535, 1.535, 1.536,
       1.536, 1.536, 1.536,
       1.536, 1.537, 1.537,
       1.537, 1.538, 1.538,
       1.539, 1.539, 1.54,
       1.54,  1.541, 1.542,
       1.543, 1.544, 1.545,
       1.546, 1.547, 1.549,
       1.55,  1.551, 1.552,
       1.554, 1.555, 1.558,
       1.559, 1.56,  1.562,
       1.563, 1.565, 1.567,
       1.568, 1.569, 1.57,
       1.572, 1.573, 1.576,
       1.577, 1.577
     };

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



  /// ITO ///
  G4MaterialPropertiesTable* ITO()
  {
    // Input data: complex refraction index obtained from:
    // https://refractiveindex.info/?shelf=other&book=In2O3-SnO2&page=Moerland
    // Only valid in [1000 - 400] nm

    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    std::vector<G4double> energies = {
      optPhotMinE_,
      hc_ / (1000. * nm),  hc_ / (800. * nm),   hc_ / ( 700. * nm),
      hc_ / (600. * nm),   hc_ / ( 580. * nm),  hc_ / (560. * nm),
      hc_ / ( 540. * nm),  hc_ / (520. * nm),   hc_ / ( 500. * nm),
      hc_ / (480. * nm),   hc_ / ( 460. * nm),  hc_ / (440. * nm),
      hc_ / ( 420. * nm),  hc_ / (400. * nm),
      optPhotMaxE_ };

    std::vector<G4double> rIndex = {
      1.635,
      1.635, 1.775, 1.835,
      1.894, 1.906, 1.919,
      1.931, 1.945, 1.960,
      1.975, 1.993, 2.012,
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
      hc_ / (1097. * nm),  hc_ / (1000. * nm),  hc_ / ( 950. * nm),
      hc_ / ( 900. * nm),  hc_ / ( 800. * nm),  hc_ / ( 700. * nm),
      hc_ / ( 600. * nm),  hc_ / ( 550. * nm),  hc_ / ( 500. * nm),
      hc_ / ( 450. * nm),  hc_ / ( 420. * nm),  hc_ / ( 400. * nm),
      hc_ / ( 370. * nm),  hc_ / ( 350. * nm),  hc_ / ( 302. * nm),
      optPhotMaxE_ };

    std::vector<G4double> rIndex = {
      1.4760,
      1.4760, 1.4662, 1.4665,
      1.4693, 1.4802, 1.4935,
      1.5080, 1.5155, 1.5235,
      1.5328, 1.5391, 1.5439,
      1.5522, 1.5587, 1.5805,
      1.5805 };
    mpt->AddProperty("RINDEX", energies, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_length = {
      (1097. * nm) / (4*pi * 0.1191),
      (1097. * nm) / (4*pi * 0.1191),  (1000. * nm) / (4*pi * 0.0859),
      ( 950. * nm) / (4*pi * 0.0701),  ( 900. * nm) / (4*pi * 0.0561),
      ( 800. * nm) / (4*pi * 0.0340),  ( 700. * nm) / (4*pi * 0.0197),
      ( 600. * nm) / (4*pi * 0.0107),  ( 550. * nm) / (4*pi * 0.0076),
      ( 500. * nm) / (4*pi * 0.0051),  ( 450. * nm) / (4*pi * 0.0035),
      ( 420. * nm) / (4*pi * 0.0025),  ( 400. * nm) / (4*pi * 0.00194),
      ( 370. * nm) / (4*pi * 0.00135), ( 350. * nm) / (4*pi * 0.00103),
      ( 302. * nm) / (4*pi * 0.0004),  ( 302. * nm) / (4*pi * 0.0004) };
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
      G4double lambda = hc_/ri_energy[i]*1000; // in micron
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
      rIndex.push_back(seq.RefractiveIndex(hc_/ri_energy[i]));
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
      G4double wl = hc_ / ri_energy[i];
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
    //
    // May 2023:
    // Updated scintillation decay and yields from:
    // Triplet Lifetime in Gaseous Argon. Michael Akashi-Ronquest et al.

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
      G4double wl = hc_ / ri_energy[i] * 1000; // in micron
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
    G4double Energy_peak  = (hc_ / Wavelength_peak);
    G4double Energy_sigma = (hc_ * Wavelength_sigma / pow(Wavelength_peak,2));
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
    mpt->AddConstProperty("SCINTILLATIONTIMECONSTANT2",   3480.*ns);
    mpt->AddConstProperty("SCINTILLATIONYIELD1", .136);
    mpt->AddConstProperty("SCINTILLATIONYIELD2", .864);
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
    //   G4cout << hc_/ri_energy[i]/nanometer << " nm, " << rindex[i] << G4endl;
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

  /// PolishedAl ///
  G4MaterialPropertiesTable* PolishedAl()
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    std::vector<G4double> ENERGIES = {
       hc_ / (2456.42541 * nm), hc_ / (2396.60266 * nm), hc_ / (2276.95716 * nm),
       hc_ / (2159.52733 * nm), hc_ / (2037.66617 * nm), hc_ / (1918.02068 * nm),
       hc_ / (1798.37518 * nm), hc_ / (1676.51403 * nm), hc_ / (1559.08419 * nm),
       hc_ / (1437.22304 * nm), hc_ / (1319.79321 * nm), hc_ / (1197.93205 * nm),
       hc_ / (1078.28656 * nm), hc_ / (956.42541 * nm),  hc_ / (838.99557 * nm),
       hc_ / (717.13442 * nm),  hc_ / (597.48892 * nm),  hc_ / (477.84343 * nm),
       hc_ / (418.02068 * nm),  hc_ / (358.19793 * nm),  hc_ / (293.94387 * nm)
    };
    std::vector<G4double> REFLECTIVITY = {
      .99088, .99082, .98925,
      .98623, .98611, .98163,
      .98006, .97849, .97401,
      .97098, .96941, .96784,
      .96481, .96033, .96167,
      .96301, .96289, .96278,
      .96126, .95830, .94224
    };
    // DOI:10.4236/ampc.2015.511046
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
    std::vector<G4double> ENERGIES_3    = {
      0.005 * eV, 0.19581 * eV, 0.43227 * eV,
      0.84211 * eV, 1.2254 * eV, 1.4477 * eV,
      1.7831 * eV, 2.8203 * eV, 3.6216 * eV,
      5.0548 * eV, 7.0554 * eV, 9.4450 * eV,
      12.645 * eV, 14.939 * eV, 16.238 * eV,
      18.4 * eV, 20. * eV
    };
    std::vector<G4double> rIndex = {
      473.49, 12.843, 3.8841, 1.437, 1.4821, 2.4465, 1.6203, 0.58336, 0.32634, 0.1686,
      0.089866, 0.051461, 0.039232, 0.11588, 0.39013, 0.58276, 0.66415
    };
    // from https://refractiveindex.info/?shelf=3d&book=metals&page=aluminium
    mpt->AddProperty("RINDEX", ENERGIES_3, rIndex);

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
    //  optPhotMinE_,                      hc_ / (450. * nm),
    //  hc_ / (440. * nm),  hc_ / (430. * nm),
    //  hc_ / (420. * nm),  hc_ / (410. * nm),
    //  hc_ / (400. * nm),  hc_ / (390. * nm),
    //  hc_ / (380. * nm),  hc_ / (370. * nm),
    //  hc_ / (360. * nm),  hc_ / (330. * nm),
    //  hc_ / (320. * nm),  hc_ / (310. * nm),
    //  hc_ / (300. * nm),  hc_ / (270. * nm),
    //  hc_ / (250. * nm),  hc_ / (230. * nm),
    //  hc_ / (210. * nm),  hc_ / (190. * nm),
    //  hc_ / (170. * nm),  hc_ / (150. * nm),
    //  hc_ / (100. * nm),  optPhotMaxE_
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
      hc_ / (380. * nm),  hc_ / (370. * nm), hc_ / (360. * nm),
      hc_ / (330. * nm),  hc_ / (320. * nm), hc_ / (310. * nm),
      hc_ / (300. * nm),  hc_ / (270. * nm), hc_ / (250. * nm),
      hc_ / (230. * nm),  hc_ / (210. * nm), hc_ / (190. * nm),
      hc_ / (170. * nm),  hc_ / (150. * nm),
      optPhotMaxE_
    };

    std::vector<G4double> WLS_absLength = {
      noAbsLength_,                 // ~6200 nm
      noAbsLength_, 50. * nm, 30. * nm, // 380, 370, 360 nm
      30. * nm, 50. * nm, 80. * nm,     // 330, 320, 310 nm
      100. * nm, 100. * nm, 400. * nm,  // 300, 270, 250 nm
      400. * nm, 350. * nm, 250. * nm,  // 230, 210, 190 nm
      350. * nm, 400. * nm, 400. * nm   // 170, 150, ~108 nm
    };

    //for (int i=0; i<WLS_abs_energy.size(); i++)
    //  G4cout << "* TPB WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (hc_ / WLS_abs_energy[i]) / nm
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
      G4double wl = (hc_ / WLS_emi_energy[i]) / nm;
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


  /// TPH (p-terphenyl) ///
  G4MaterialPropertiesTable* TPH()
  {
    // Data from https://doi.org/10.1016/j.nima.2011.12.036
    // and https://iopscience.iop.org/article/10.1088/1748-0221/5/04/P04007/
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // REFRACTIVE INDEX
    std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> PTP_rIndex      = {1.65    , 1.65};
    mpt->AddProperty("RINDEX", rIndex_energies, PTP_rIndex);

    // ABSORPTION LENGTH
    // Assuming no absorption except WLS
    std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
    std::vector<G4double> absLength  = {noAbsLength_, noAbsLength_};
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);


    // WLS ABSORPTION LENGTH
    // There are no tabulated values in the literature for the PTP absorption
    // length as a function of the wavelength.
    // However in https://iopscience.iop.org/article/10.1088/1748-0221/5/04/P04007/
    // it says that "the thickness >=150 nm of a polycrystalline p-terphenyl layer
    // is enough for absorption of >=99.9% of the xenon light". Thus, using the
    // Beer-Lambert law 1 - P = e^(-x/lambda), where lambda is the absorption length
    // and P is the absorption probability, we can place an upper limit of 21 nm
    // on the absorption length at 175 nm. This is reasonably close to the TPB value.
    // Then, we scale accordingly to the absorption spectrum (which is in a.u.).
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,
      hc_ / (337. * nm), hc_ / (318. * nm), hc_ / (292. * nm),
      hc_ / (276. * nm), hc_ / (253. * nm), hc_ / (238. * nm),
      hc_ / (222. * nm), hc_ / (204. * nm), hc_ / (190. * nm),
      hc_ / (175. * nm), hc_ / (169. * nm),
      optPhotMaxE_
    };

    float XePeakAbsValue = 1.879;
    float XePeakAbsLength = 21 * nm;

    std::vector<float> PTP_absorption = {
      0.002, 0.174, 0.414, // 337, 318, 292
      0.949, 0.540, 1.218, // 276, 253, 238
      1.858, 1.429, 1.716, // 222, 204, 190
      1.879, 1.803 // 175, 169
    };

    std::vector<G4double> WLS_absLength = {noAbsLength_};
    for (auto& abs_value : PTP_absorption)
      WLS_absLength.push_back(XePeakAbsLength / (abs_value / XePeakAbsValue));

    WLS_absLength.push_back(noAbsLength_);

    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* TPB WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (hc_ / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / nm << " nm" << G4endl;
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,
      hc_ / (452. * nm), hc_ / (430. * nm), hc_ / (412. * nm), hc_ / (398. * nm),
      hc_ / (385. * nm), hc_ / (371. * nm), hc_ / (361. * nm), hc_ / (354. * nm),
      hc_ / (336. * nm), hc_ / (317. * nm),
      optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.,
      0.044, 0.179, 0.351, 0.514,
      0.849, 0.993, 0.745, 0.421,
      0.173, 0.022,
      0.
    };

    mpt->AddProperty("WLSCOMPONENT", WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    // https://www.sciencedirect.com/science/article/pii/S0168900219301652
    mpt->AddConstProperty("WLSTIMECONSTANT", 2.4 * ns);

    // WLS Quantum Efficiency
    // This is set to QE at the Xenon peak, which the paper claims to be >90%
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.9);

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
      hc_ / (609. * nm),  hc_ / (589.26 * nm), hc_ / (550. * nm),
      hc_ / (530. * nm),  hc_ / (500. * nm),   hc_ / (490. * nm),
      hc_ / (481. * nm),  hc_ / (460. * nm),   hc_ / (435. * nm),
      hc_ / (425. * nm),
      optPhotMaxE_
    };

    std::vector<G4double> rIndex = {
      1.5780,
      1.5780, 1.5800, 1.5845,  // 609 , 589.26, 550 nm
      1.5870, 1.5913, 1.5929,  // 530, 500, 490 nm
      1.5945, 1.5986, 1.6050,  // 481, 460, 435 nm
      1.6080,                  // 425 nm
      1.608
    };
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,
      hc_ / (750. * nm), hc_ / (740. * nm), hc_ / (380. * nm), hc_ / (370. * nm),
      optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,
      noAbsLength_, 3.0 * m, 3.0 * m, noAbsLength_,
      noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,
      hc_ / (500. * nm), hc_ / (495. * nm), hc_ / (490. * nm),
      hc_ / (485. * nm), hc_ / (480. * nm), hc_ / (475. * nm),
      hc_ / (470. * nm), hc_ / (465. * nm), hc_ / (460. * nm),
      hc_ / (455. * nm), hc_ / (450. * nm), hc_ / (445. * nm),
      hc_ / (440. * nm), hc_ / (435. * nm), hc_ / (430. * nm),
      hc_ / (425. * nm), hc_ / (420. * nm), hc_ / (415. * nm),
      hc_ / (410. * nm), hc_ / (405. * nm), hc_ / (400. * nm),
      hc_ / (395. * nm), hc_ / (390. * nm), hc_ / (385. * nm),
      hc_ / (380. * nm), hc_ / (375. * nm), hc_ / (370. * nm),
      hc_ / (365. * nm), hc_ / (360. * nm), hc_ / (355. * nm),
      hc_ / (350. * nm), hc_ / (345. * nm),
      optPhotMaxE_
    };

    std::vector<G4double> WLS_absLength = {
      noAbsLength_,
      noAbsLength_, (1. / 0.0080) * cm, (1. / 0.0165) * cm,
      (1. / 0.0325) * cm, (1. / 0.0815) * cm, (1. / 0.2940) * cm,
      (1. / 0.9640) * cm, (1. / 2.8600) * cm, (1. / 6.3900) * cm,
      (1. / 9.9700) * cm, (1. / 11.0645)* cm, (1. / 10.198) * cm,
      (1. / 9.4465) * cm, (1. / 10.2145)* cm, (1. / 12.240) * cm,
      (1. / 12.519) * cm, (1. / 10.867) * cm, (1. / 9.0710) * cm,
      (1. / 8.0895) * cm, (1. / 7.6650) * cm, (1. / 6.7170) * cm,
      (1. / 5.2460) * cm, (1. / 4.1185) * cm, (1. / 3.3175) * cm,
      (1. / 2.6800) * cm, (1. / 1.9610) * cm, (1. / 1.4220) * cm,
      (1. / 1.0295) * cm, (1. / 0.7680) * cm, (1. / 0.6865) * cm,
      (1. / 0.5885) * cm, noAbsLength_,
      noAbsLength_
    };
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);
    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* EJ280 WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (hc_ / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / mm << " mm" << G4endl;

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,
      hc_ / (610. * nm), hc_ / (605. * nm), hc_ / (600. * nm),
      hc_ / (595. * nm), hc_ / (590. * nm), hc_ / (585. * nm),
      hc_ / (580. * nm), hc_ / (575. * nm), hc_ / (570. * nm),
      hc_ / (565. * nm), hc_ / (560. * nm), hc_ / (555. * nm),
      hc_ / (550. * nm), hc_ / (545. * nm), hc_ / (540. * nm),
      hc_ / (535. * nm), hc_ / (530. * nm), hc_ / (525. * nm),
      hc_ / (520. * nm), hc_ / (515. * nm), hc_ / (510. * nm),
      hc_ / (505. * nm), hc_ / (500. * nm), hc_ / (495. * nm),
      hc_ / (490. * nm), hc_ / (485. * nm), hc_ / (480. * nm),
      hc_ / (475. * nm), hc_ / (470. * nm), hc_ / (465. * nm),
      hc_ / (460. * nm), hc_ / (455. * nm), hc_ / (450. * nm),
      hc_ / (445. * nm), hc_ / (440. * nm), hc_ / (435. * nm),
      optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.000,
      0.000, 0.003, 0.006,  // 610, 605, 600 nm
      0.007, 0.009, 0.014,  // 595, 590, 585 nm
      0.017, 0.024, 0.033,  // 580, 575, 570 nm
      0.042, 0.051, 0.063,  // 565, 560, 555 nm
      0.081, 0.112, 0.157,  // 550, 545, 540 nm
      0.211, 0.274, 0.329,  // 535, 530, 525 nm
      0.341, 0.325, 0.346,  // 520, 515, 510 nm
      0.433, 0.578, 0.792,  // 505, 500, 495 nm
      1.000, 0.966, 0.718,  // 490, 485, 480 nm
      0.604, 0.681, 0.708,  // 475, 470, 465 nm
      0.525, 0.242, 0.046,  // 460, 455, 450 nm
      0.012, 0.003, 0.000,  // 445, 440, 435 nm
      0.000
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
      hc_ / (609. * nm), hc_ / (589.26 * nm), hc_ / (550. * nm),
      hc_ / (530. * nm), hc_ / (500. * nm),   hc_ / (490. * nm),
      hc_ / (481. * nm), hc_ / (460. * nm),   hc_ / (435. * nm),
      hc_ / (425. * nm),
      optPhotMaxE_
    };

    std::vector<G4double> rIndex = {
      1.5780,
      1.5780, 1.5800, 1.5845,  // 609, 589.26, 550 nm
      1.5870, 1.5913, 1.5929,  // 530, 500, 490 nm
      1.5945, 1.5986, 1.6050,  // 481, 460, 435 nm
      1.6080,                  // 425 nm
      1.6080
    };
    mpt->AddProperty("RINDEX", ri_energy, rIndex);

    // ABSORPTION LENGTH
    std::vector<G4double> abs_energy = {
      optPhotMinE_,
      hc_ / (750. * nm), hc_ / (740. * nm), hc_ / (380. * nm), hc_ / (370. * nm),
      optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,
      noAbsLength_, 3.0 * m, 3.0 * m, noAbsLength_,
      noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,
      hc_ / (445. * nm), hc_ / (440. * nm), hc_ / (435. * nm),
      hc_ / (430. * nm), hc_ / (425. * nm), hc_ / (420. * nm),
      hc_ / (415. * nm), hc_ / (410. * nm), hc_ / (405. * nm),
      hc_ / (400. * nm), hc_ / (395. * nm), hc_ / (390. * nm),
      hc_ / (385. * nm), hc_ / (380. * nm), hc_ / (375. * nm),
      hc_ / (370. * nm), hc_ / (365. * nm), hc_ / (360. * nm),
      hc_ / (355. * nm), hc_ / (350. * nm), hc_ / (345. * nm),
      hc_ / (340. * nm), hc_ / (335. * nm), hc_ / (330. * nm),
      hc_ / (325. * nm), hc_ / (320. * nm), hc_ / (315. * nm),
      hc_ / (310. * nm), hc_ / (305. * nm), hc_ / (300. * nm),
      hc_ / (295. * nm), hc_ / (290. * nm), hc_ / (285. * nm),
      hc_ / (280. * nm), hc_ / (275. * nm),
      optPhotMaxE_
    };

    std::vector<G4double> WLS_absLength = {
      noAbsLength_,
      noAbsLength_,        (1. / 0.00007) * cm, (1. /  0.0003) * cm,
      (1. / 0.00104) * cm, (1. / 0.00223) * cm, (1. / 0.00408) * cm,
      (1. /  0.0104) * cm, (1. / 0.18544) * cm, (1. /  1.4094) * cm,
      (1. /  3.7088) * cm, (1. /  7.4176) * cm, (1. / 11.8682) * cm,
      (1. / 16.6155) * cm, (1. / 22.2529) * cm, (1. / 27.8162) * cm,
      (1. / 33.3794) * cm, (1. / 37.8671) * cm, (1. / 40.4262) * cm,
      (1. / 41.5388) * cm, (1. / 41.1679) * cm, (1. / 38.9426) * cm,
      (1. / 35.0113) * cm, (1. / 31.1541) * cm, (1. / 27.4453) * cm,
      (1. / 23.4398) * cm, (1. / 20.0276) * cm, (1. / 16.3188) * cm,
      (1. / 13.3518) * cm, (1. / 10.5331) * cm, (1. /  8.1594) * cm,
      (1. /  6.1196) * cm, (1. /  4.6731) * cm, (1. /  3.6346) * cm,
      (1. /  3.0412) * cm,  noAbsLength_,
      noAbsLength_
    };
    // XXX We are assuming that EJ286 doesn't absorb wave lengths shorter than 280 nm
    // although the spectrum continues ...
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);
    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* EJ286 WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (hc_ / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / mm << " mm" << G4endl;

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,
      hc_ / (535. * nm), hc_ / (530. * nm), hc_ / (525. * nm), hc_ / (520. * nm),
      hc_ / (515. * nm), hc_ / (510. * nm), hc_ / (505. * nm), hc_ / (500. * nm),
      hc_ / (495. * nm), hc_ / (490. * nm), hc_ / (485. * nm), hc_ / (480. * nm),
      hc_ / (475. * nm), hc_ / (470. * nm), hc_ / (465. * nm), hc_ / (460. * nm),
      hc_ / (455. * nm), hc_ / (450. * nm), hc_ / (445. * nm), hc_ / (440. * nm),
      hc_ / (435. * nm), hc_ / (430. * nm), hc_ / (425. * nm), hc_ / (420. * nm),
      hc_ / (415. * nm), hc_ / (410. * nm), hc_ / (405. * nm), hc_ / (400. * nm),
      hc_ / (395. * nm), hc_ / (390. * nm), hc_ / (385. * nm), hc_ / (380. * nm),
      hc_ / (375. * nm), hc_ / (370. * nm), hc_ / (365. * nm), hc_ / (360. * nm),
      hc_ / (355. * nm),
      optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.0000,
      0.0000, 0.0089, 0.0100, 0.0181, // 535, 530, 525, 520 nm
      0.0210, 0.0270, 0.0380, 0.0496, // 515, 510, 505, 500 nm
      0.0600, 0.0721, 0.0900, 0.1125, // 495, 490, 485, 480 nm
      0.1500, 0.1848, 0.2100, 0.2388, // 475, 470, 465, 460 nm
      0.2800, 0.3289, 0.4000, 0.4956, // 455, 450, 445, 440 nm
      0.5700, 0.6230, 0.6450, 0.6667, // 435, 430, 425, 420 nm
      0.8000, 0.9800, 0.9900, 0.8559, // 415, 410, 405, 400 nm
      0.7118, 0.7400, 0.8000, 0.6702, // 395, 390, 385, 380 nm
      0.3800, 0.1082, 0.0400, 0.0089, // 375, 370, 365, 360 nm
      0.0000,                         // 355 nm
      0.0000
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
      optPhotMinE_,
      hc_ / (750. * nm), hc_ / (740. * nm), hc_ / (380. * nm), hc_ / (370. * nm),
      optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,
      noAbsLength_, 3.5 * m, 3.5 * m, noAbsLength_,
      noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,
      hc_ / (490. * nm), hc_ / (485. * nm), hc_ / (475. * nm), hc_ / (454. * nm),
      hc_ / (443. * nm), hc_ / (430. * nm), hc_ / (410. * nm), hc_ / (405. * nm),
      hc_ / (359. * nm), hc_ / (350. * nm), hc_ / (345. * nm),
      optPhotMaxE_
    };
    std::vector<G4double> WLS_absLength = {
      noAbsLength_,
      noAbsLength_, 44.2 * mm, 5.39 * mm, 0.395 * mm, // 490, 485, 475, 454 nm
      0.462 * mm, 0.354 * mm, 0.571 * mm, 0.612 * mm, // 443, 430, 410, 405 nm
      4.51 * mm,  4.81  * mm, noAbsLength_,           // 359, 350, 345  nm
      noAbsLength_
    };
    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);
    //for (int i=0; i<WLS_abs_entries; i++)
    //  G4cout << "* Y11 WLS absLength:  " << std::setw(8) << WLS_abs_energy[i] / eV
    //         << " eV  ==  " << std::setw(8) << (hc_ / WLS_abs_energy[i]) / nm
    //         << " nm  ->  " << std::setw(6) << WLS_absLength[i] / mm << " mm" << G4endl;

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,
      hc_ / (580. * nm), hc_ / (550. * nm), hc_ / (530. * nm),
      hc_ / (525. * nm), hc_ / (520. * nm), hc_ / (515. * nm),
      hc_ / (510. * nm), hc_ / (505. * nm), hc_ / (500. * nm),
      hc_ / (495. * nm), hc_ / (490. * nm), hc_ / (485. * nm),
      hc_ / (480. * nm), hc_ / (475. * nm), hc_ / (470. * nm),
      hc_ / (465. * nm), hc_ / (460. * nm), hc_ / (455. * nm),
      hc_ / (450. * nm), hc_ / (445. * nm),
      optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.000,
      0.000, 0.200, 0.300, // 580, 550, 530 nm
      0.400, 0.600, 0.750, // 525, 520, 515 nm
      0.750, 0.720, 0.700, // 510, 505, 500 nm
      0.680, 0.650, 0.700, // 495, 490, 485 nm
      0.900, 1.000, 0.950, // 480, 475, 470 nm
      0.500, 0.300, 0.100, // 465, 460, 455 nm
      0.050, 0.000,        // 450, 445 nm
      0.000
    };
    mpt->AddProperty("WLSCOMPONENT",  WLS_emi_energy, WLS_emiSpectrum);

    // WLS Delay
    mpt->AddConstProperty("WLSTIMECONSTANT", 8.5 * ns);

    // WLS Quantum Efficiency
    mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.87);

    return mpt;
  }


/// B-2 ///
  G4MaterialPropertiesTable* B2()
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
      optPhotMinE_,
      hc_ / (750. * nm), hc_ / (740. * nm), hc_ / (380. * nm), hc_ / (370. * nm),
      optPhotMaxE_
    };
    std::vector<G4double> absLength = {
      noAbsLength_,
      noAbsLength_, 3.5 * m, 3.5 * m, noAbsLength_,
      noAbsLength_
    };
    mpt->AddProperty("ABSLENGTH", abs_energy, absLength);

    // WLS ABSORPTION LENGTH
    // For B2 fibers Kuraray provides absorption spectrum and not
    // absorption length. We assume that the absorption length at the
    // absorption maximum is the same as with the Y11 fiber and
    // scale according to the absorption spectrum. This is not perfect
    // but it was verified to be a good approximation with the Y11 fiber,
    // for which Kuraray did provide the absorption length.

    std::vector<G4double> WLS_abs_energy = {
      optPhotMinE_,
      hc_ / (418. * nm), hc_ / (412. * nm), hc_ / (405. * nm), hc_ / (400. * nm),
      hc_ / (394. * nm), hc_ / (387. * nm), hc_ / (384. * nm), hc_ / (382. * nm),
      hc_ / (378. * nm), hc_ / (370. * nm), hc_ / (361. * nm), hc_ / (353. * nm),
      hc_ / (345. * nm), hc_ / (341. * nm), hc_ / (336. * nm), hc_ / (331. * nm),
      hc_ / (316. * nm), hc_ / (301. * nm), hc_ / (280. * nm),
      optPhotMaxE_
    };

    float minAbsLength = 0.395 * mm;

    std::vector<float> B2_absorption {
      -0.01, -0.06, -0.26, -0.44, // 418, 412, 405, 400
      -0.59, -0.59, -0.64, -0.77, // 394, 387, 384, 382
      -0.92, -1.00, -0.93, -0.85, // 378, 370, 361, 353
      -0.87, -0.87, -0.77, -0.56, // 345, 341, 336, 331
      -0.35, -0.22, -0.12         // 316, 301, 280
    };

    std::vector<G4double> WLS_absLength {noAbsLength_};

    for (auto &abs_value : B2_absorption)
      WLS_absLength.push_back(- minAbsLength / abs_value);

    WLS_absLength.push_back(noAbsLength_);

    mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy, WLS_absLength);

    // WLS EMISSION SPECTRUM
    std::vector<G4double> WLS_emi_energy = {
      optPhotMinE_,
      hc_ / (542 * nm), hc_ / (525 * nm), hc_ / (508 * nm), hc_ / (497 * nm),
      hc_ / (488 * nm), hc_ / (479 * nm), hc_ / (473 * nm), hc_ / (467 * nm),
      hc_ / (463 * nm), hc_ / (458 * nm), hc_ / (454 * nm), hc_ / (449 * nm),
      hc_ / (445 * nm), hc_ / (442 * nm), hc_ / (440 * nm), hc_ / (438 * nm),
      hc_ / (433 * nm), hc_ / (429 * nm), hc_ / (424 * nm), hc_ / (420 * nm),
      hc_ / (418 * nm), hc_ / (416 * nm), hc_ / (411 * nm), hc_ / (404 * nm),
      hc_ / (402 * nm), hc_ / (399 * nm), hc_ / (398 * nm), hc_ / (396 * nm),
      hc_ / (395 * nm), hc_ / (394 * nm), hc_ / (392 * nm), hc_ / (391 * nm),
      hc_ / (386 * nm), hc_ / (380 * nm),
      optPhotMaxE_
    };

    std::vector<G4double> WLS_emiSpectrum = {
      0.000,
      0.053, 0.070, 0.109, 0.143, // 542, 525, 508, 497
      0.199, 0.270, 0.337, 0.423, // 488, 479, 473, 467
      0.497, 0.582, 0.615, 0.645, // 463, 458, 454, 449
      0.679, 0.750, 0.801, 0.857, // 445, 442, 440, 438
      0.957, 0.999, 0.949, 0.906, // 433, 429, 424, 420
      0.855, 0.809, 0.750, 0.750, // 418, 416, 411, 404
      0.719, 0.671, 0.590, 0.500, // 402, 399, 398, 396
      0.421, 0.327, 0.217, 0.138, // 395, 394, 392, 391
      0.065, 0.023,               // 386, 380
      0.000
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
