// ----------------------------------------------------------------------------
// nexus | OpticalMaterialProperties.cc
//
// Optical properties of relevant materials.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "OpticalMaterialProperties.h"
#include "XenonGasProperties.h"
#include "XenonLiquidProperties.h"
#include "SellmeierEquation.h"

#include <G4MaterialPropertiesTable.hh>

#include <cassert>

using namespace nexus;
using namespace CLHEP;


G4MaterialPropertiesTable* OpticalMaterialProperties::Vacuum()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  std::vector<G4double> energy = {optPhotMinE_, optPhotMaxE_};

  // REFRACTIVE INDEX
  std::vector<G4double> ri_index = {1., 1.};

  assert(energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", energy.data(), ri_index.data(), energy.size());

  // ABSORPTION LENGTH
  std::vector<G4double> abs_length = {noAbsLength_, noAbsLength_};

  assert(energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", energy.data(), abs_length.data(), energy.size());

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::Epoxy()
{
  // Optical properties of Epoxy adhesives.
  // Obtained from
  // http://www.epotek.com/SSCDocs/techtips/Tech%20Tip%2018%20-%20Understanding%20Optical%20Properties%20for%20Epoxy%20Apps.pdf

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  std::vector<G4double> ri_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> ri_index = {1.54, 1.54};

  assert(ri_energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  // ABSORPTION LENGTH
  std::vector<G4double> abs_energy =
    {optPhotMinE_, 2.132*eV, 2.735*eV, 2.908*eV, 3.119*eV,
     3.320*eV, 3.476*eV, 3.588*eV, 3.749*eV, 3.869*eV,
     3.973*eV, 4.120*eV, 4.224*eV, 4.320*eV, 4.420*eV,
     5.018*eV, 8.*eV, optPhotMaxE_};
  std::vector<G4double> abs_length =
    {15000.*cm, 326.*mm, 117.68*mm, 85.89*mm, 50.93*mm,
     31.25*mm, 17.19*mm, 10.46*mm, 5.26*mm, 3.77*mm,
     2.69*mm, 1.94*mm, 1.94*mm, 1.94*mm, 1.94*mm,
     1.94*mm, 1.94*mm, 1.94*mm};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::EpoxyFixedRefr(G4double n)
{
  // Optical properties adjusted in order not to absorb photons
  // before they meet the photocathode.

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  XenonLiquidProperties LXe_prop;

  std::vector<G4double> ri_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> ri_index  = {n, n};

  assert(ri_energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  // ABSORPTION LENGTH
  std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> abs_length = {noAbsLength_, noAbsLength_};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::EpoxyLXeRefr()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  XenonLiquidProperties LXe_prop;

  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  std::vector<G4double> ri_energy;
  for (int i=0; i<ri_entries; i++) {
    ri_energy.push_back(optPhotMinE_ + i*eWidth);
  }

  std::vector<G4double> ri_index;
  for (G4int i=0; i<ri_entries; i++) {
    ri_index.push_back(LXe_prop.RefractiveIndex(ri_energy[i]));
  }

  assert(ri_energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  // ABSORPTION LENGTH

  std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> abs_length = {noAbsLength_, noAbsLength_};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::FusedSilica()
{
  // Optical properties of Suprasil 311/312(c) synthetic fused silica.
  // Obtained from http://heraeus-quarzglas.com.
  // It has  transmission in the deep ultraviolet down to 160 nm.

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX //////////////////////////////////////////////////////////
  // The range is chosen to be up to ~10.7 eV because Sellmeier's equation
  // for fused silica is valid only in that range
  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  std::vector<G4double> ri_energy;
  for (int i=0; i<ri_entries; i++) {
    ri_energy.push_back(optPhotMinE_ + i*eWidth);
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

  std::vector<G4double> ri_index;
  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1)
      + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
      + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
    ri_index.push_back(sqrt(n2));
  }

  assert(ri_energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  // ABSORPTION LENGTH /////////////////////////////////////////////////////////
  std::vector<G4double> abs_energy =
    {optPhotMinE_, 6.46499*eV, 6.54*eV, 6.59490*eV, 6.64*eV,
     6.72714*eV, 6.73828*eV, 6.75*eV, 6.82104*eV, 6.86*eV,
     6.88*eV, 6.89*eV, 7.*eV, 7.01*eV, 7.01797*eV,
     7.05*eV, 7.08*eV, 7.08482*eV, 7.30*eV, 7.36*eV,
     7.4*eV, 7.48*eV, 7.52*eV, 7.58*eV, 7.67440*eV,
     7.76*eV, 7.89*eV, 7.93*eV, 8.*eV, optPhotMaxE_};
  std::vector<G4double> abs_length =
    {1500.*cm, 1500.*cm, 200.*cm, 200.*cm, 90.*cm,
     45.*cm, 45*cm, 30*cm, 24*cm, 21*cm,
     20*cm , 19*cm, 16*cm, 14.*cm, 13.*cm,
     8.5*cm, 8.*cm, 6.*cm, 1.5*cm, 1.2*cm,
     1.*cm, .65*cm, .4*cm, .37*cm, .32*cm,
     .28*cm, .220*cm, .215*cm, .00005*cm, .00005*cm};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::FakeFusedSilica(G4double transparency, G4double thickness)
{
  // Optical properties of Suprasil 311/312(c) synthetic fused silica.
  // Obtained from http://heraeus-quarzglas.com

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX //////////////////////////////////////////////////////////
  // The range must be the same as fused silica

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

  std::vector<G4double> ri_index;
  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1)
      + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
      + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
    ri_index.push_back(sqrt(n2));
  }

  assert(ri_energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  // ABSORPTION LENGTH

  G4double abs_cnst = -thickness/log(transparency);
  std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> abs_length = {abs_cnst, abs_cnst};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  return mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::FakeGenericMaterial(G4double quartz_rindex)
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  std::vector<G4double> energy = {optPhotMinE_, optPhotMaxE_};

  // REFRACTIVE INDEX
  std::vector<G4double> ri_index = {quartz_rindex, quartz_rindex};

  assert(energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", energy.data(), ri_index.data(), energy.size());

  // Absorption length
  std::vector<G4double> abs_length = {noAbsLength_, noAbsLength_};

  assert(energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", energy.data(), abs_length.data(), energy.size());

  return mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::GlassEpoxy()
{
 // WARNING: This is a deprecated optical property, it is kept for code consistency, but it
 // will be removed in the future.
 // Optical properties of Optorez 1330 glass epoxy.
 // Obtained from http://refractiveindex.info and http://www.zeonex.com/applications_optical.asp

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  std::vector<G4double> ri_energy;
  for (int i=0; i<ri_entries; i++) {
    ri_energy.push_back(optPhotMinE_ + i * eWidth);
  }

  std::vector<G4double> ri_index;
  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    G4double n2 = 2.291142 - 3.311944E-2*pow(lambda,2) - 1.630099E-2*pow(lambda,-2) +
      7.265983E-3*pow(lambda,-4) - 6.806145E-4*pow(lambda,-6) + 1.960732E-5*pow(lambda,-8);
    ri_index.push_back(sqrt(n2));
  }

  assert(ri_energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  // ABSORPTION LENGTH
  std::vector<G4double> abs_energy =
    {optPhotMinE_, 2.132*eV, 2.735*eV, 2.908*eV, 3.119*eV,
     3.320*eV, 3.476*eV, 3.588*eV, 3.749*eV, 3.869*eV,
     3.973*eV, 4.120*eV, 4.224*eV, 4.320*eV, 4.420*eV,
     5.018*eV, optPhotMaxE_};
  std::vector<G4double> abs_length =
    {15000.*cm, 326.*mm, 117.68*mm, 85.89*mm, 50.93*mm,
     31.25*mm, 17.19*mm, 10.46*mm, 5.26*mm, 3.77*mm,
     2.69*mm, 1.94*mm, 1.33*mm, 0.73*mm, 0.32*mm,
     0.10*mm, 0.10*mm};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  return mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::GAr(G4double sc_yield)
{
  // WARNING: before using GAr properties, check that we know the
 // properties of the rest of materials at its scintillation wavelengths.
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  const G4int ri_entries = 200;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = (1 + i*0.049)*eV;
  }

  G4double rindex[ri_entries];

  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    rindex[i] = 1 + 0.012055*(0.2075*pow(lambda,2)/(91.012*pow(lambda,2)-1) +
    0.0415*pow(lambda,2)/(87.892*pow(lambda,2)-1) + 4.3330*pow(lambda,2)/(214.02*pow(lambda,2)-1)); // From refractiveindex.info
    //    std::cout << "rindex = " << rindex[i] << std::endl;
  }

  // EMISSION SPECTRUM
  // Sampling from ~110 nm to 150 nm <----> from ~11.236 eV to 8.240 eV
  const G4int sc_entries = 380;
  G4double sc_energy[sc_entries];
  G4double intensity[sc_entries];

  G4double Wavelength_peak = 128.*nm;
  G4double Wavelength_sigma = 2.929*nm;

  G4double Energy_peak = (h_Planck*c_light/Wavelength_peak);
  G4double Energy_sigma = (h_Planck*c_light*Wavelength_sigma/pow(Wavelength_peak,2));

  //  std::cout << "Energy_peak = " << Energy_peak << std::endl;

  for (int j=0;j<sc_entries;j++){
    sc_energy[j] = 8.240*eV + 0.008*j*eV;
    intensity[j] = exp(-pow(Energy_peak/eV-sc_energy[j]/eV,2)/(2*pow(Energy_sigma/eV, 2)))/(Energy_sigma/eV*sqrt(pi*2.));
    //    std::cout << "(energy, intensity) = (" << sc_energy[j] << "," << intensity[j] << ")" << std::endl;
  }

  // ABSORTION LENGTH
  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

//An argon gas proportional scintillation counter with UV avalanche photodiode scintillation readout C.M.B. Monteiro, J.A.M. Lopes, P.C.P.S. Simoes, J.M.F. dos Santos, C.A.N. Conde

  mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
  mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  mpt->AddConstProperty("SCINTILLATIONYIELD", sc_yield);
  mpt->AddProperty("ELSPECTRUM", sc_energy, intensity, sc_entries);
  mpt->AddConstProperty("FASTTIMECONSTANT",6.*ns);
  mpt->AddConstProperty("SLOWTIMECONSTANT",37.*ns);
  mpt->AddConstProperty("YIELDRATIO",.52);
  mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);

  // mpt->AddConstProperty("ELTIMECONSTANT", 1260.*ns);
  mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  return mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::LAr()
{
 // WARNING: before using LAr properties, check that we know the
 // properties of the rest of materials at its scintillation wavelengths.

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  const G4int ri_entries = 200;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = (1 + i*0.049)*eV;
  }

  G4double rindex[ri_entries];

  for (int i=0; i<ri_entries; i++) {
    G4double epsilon;
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    if(lambda*1000<110*nm)
      epsilon = 1.0e4;
    else{
      epsilon = 1.0 / (lambda * lambda); // 1 / (lambda)^2
      epsilon = 1.2055e-2 * ( 0.2075 / (91.012 - epsilon) +
                  0.0415 / (87.892 - epsilon) +
                  4.3330 / (214.02 - epsilon) );
      epsilon *= (8./12.); // Bideau-Sellmeier -> Clausius-Mossotti
      G4double LArRho = 1400;
      G4double GArRho = 1.784;
      epsilon *= (LArRho / GArRho); // density correction (Ar gas -> LAr liquid)
      if (epsilon < 0.0 || epsilon > 0.999999)
	epsilon = 4.0e6;
      else
	epsilon = (1.0 + 2.0 * epsilon) / (1.0 - epsilon); // solve Clausius-Mossotti
    }
    rindex[i] = sqrt(epsilon);
  }

  mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);

  // ABSORTION LENGTH
  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

  G4double fano = 0.11;// Doke et al, NIM 134 (1976)353
  mpt->AddConstProperty("RESOLUTIONSCALE",fano);


  // EMISSION SPECTRUM
  // Sampling from ~110 nm to 150 nm <----> from ~11.236 eV to 8.240 eV
  const G4int sc_entries = 500;
  G4double sc_energy[sc_entries];
  G4double intensity[sc_entries];

  G4double Wavelength_peak = 128.*nm;
  G4double Wavelength_sigma = 2.929*nm;

  G4double Energy_peak = (h_Planck*c_light/Wavelength_peak);
  G4double Energy_sigma = (h_Planck*c_light*Wavelength_sigma/pow(Wavelength_peak,2));

  for (int j=0;j<sc_entries;j++){
    sc_energy[j] = 8.240*eV + 0.008*j*eV;
    intensity[j] = exp(-pow(Energy_peak/eV-sc_energy[j]/eV,2)/(2*pow(Energy_sigma/eV, 2)))/(Energy_sigma/eV*sqrt(pi*2.));
  }

  mpt->AddProperty("ELSPECTRUM", sc_energy, intensity, sc_entries);

  mpt->AddConstProperty("ELTIMECONSTANT", 1260.*ns);
  mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::GXe(G4double pressure,
							  G4double temperature,
							  G4int sc_yield)
{
  XenonGasProperties GXe_prop(pressure, temperature);
  G4MaterialPropertiesTable* GXe_mpt = new G4MaterialPropertiesTable();

  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  std::vector<G4double> ri_energy;
  for (int i=0; i<ri_entries; i++) {
    ri_energy.push_back(optPhotMinE_ + i * eWidth);
  }

  std::vector<G4double> ri_index;
  for (int i=0; i<ri_entries; i++) {
    ri_index.push_back(GXe_prop.RefractiveIndex(ri_energy[i]));
    // G4cout << "* GXe rIndex:  " << std::setw(7)
    //        << ri_energy[i]/eV << " eV -> " << rIndex[i] << G4endl;
  }

  assert(ri_energy.size() == ri_index.size());
  GXe_mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  // Sampling from ~151 nm to 200 nm <----> from 6.20625 eV to 8.21 eV
  const G4int sc_entries = 500;
  const G4double minE = 6.20625*eV;
  eWidth = (optPhotMaxE_ - minE) / sc_entries;

  std::vector<G4double> sc_energy;
  for (int j=0;j<sc_entries;j++){
    sc_energy.push_back(minE + j * eWidth);
  }
  std::vector<G4double> intensity;
  GXe_prop.Scintillation(sc_energy, intensity);

  assert(sc_energy.size() == intensity.size());
  GXe_mpt->AddProperty("SLOWCOMPONENT", sc_energy.data(), intensity.data(), sc_energy.size());
  GXe_mpt->AddProperty("FASTCOMPONENT", sc_energy.data(), intensity.data(), sc_energy.size());
  GXe_mpt->AddProperty("ELSPECTRUM", sc_energy.data(), intensity.data(), sc_energy.size());

  std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> abs_length = {noAbsLength_, noAbsLength_};

  assert(abs_energy.size() == abs_length.size());
  GXe_mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  GXe_mpt->AddConstProperty("SCINTILLATIONYIELD", sc_yield);
  GXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  GXe_mpt->AddConstProperty("FASTTIMECONSTANT",4.5*ns);
  GXe_mpt->AddConstProperty("SLOWTIMECONSTANT",100.*ns);
  //  GXe_mpt->AddConstProperty("ELTIMECONSTANT", 50.*ns);
  GXe_mpt->AddConstProperty("YIELDRATIO",.1);
  GXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  return GXe_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::LXe()
{
  /// The time constants are taken from E. Hogenbirk et al 2018 JINST 13 P10031
  XenonLiquidProperties LXe_prop;
  G4MaterialPropertiesTable* LXe_mpt = new G4MaterialPropertiesTable();

  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  std::vector<G4double> ri_energy;
  for (int i=0; i<ri_entries; i++) {
    ri_energy.push_back(optPhotMinE_ + i * eWidth);
  }

  std::vector<G4double> ri_index;

  for (G4int i=0; i<ri_entries; i++) {
    ri_index.push_back(LXe_prop.RefractiveIndex(ri_energy[i]));
  }

  assert(ri_energy.size() == ri_index.size());
  LXe_mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  // for (G4int i=ri_entries-1; i>=0; i--) {
  //   G4cout << h_Planck*c_light/ri_energy[i]/nanometer << " nm, " << rindex[i] << G4endl;
  // }

  // Sampling from ~151 nm to 200 nm <----> from 6.20625 eV to 8.21 eV
  const G4int sc_entries = 500;
  const G4double minE = 6.20625*eV;
  eWidth = (optPhotMaxE_ - minE) / sc_entries;

  std::vector<G4double> sc_energy;
  for (int j=0; j<sc_entries; j++){
    sc_energy.push_back(minE + j * eWidth);
  }
  std::vector<G4double> intensity;
  LXe_prop.Scintillation(sc_energy, intensity);

  assert(sc_energy.size() == intensity.size());
  LXe_mpt->AddProperty("FASTCOMPONENT", sc_energy.data(), intensity.data(), sc_energy.size());
  LXe_mpt->AddProperty("SLOWCOMPONENT", sc_energy.data(), intensity.data(), sc_energy.size());

  LXe_mpt->AddConstProperty("SCINTILLATIONYIELD", 58708./MeV);
  LXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1);
  LXe_mpt->AddConstProperty("RAYLEIGH", 36.*cm);
  LXe_mpt->AddConstProperty("FASTTIMECONSTANT", 2.*ns);
  LXe_mpt->AddConstProperty("SLOWTIMECONSTANT", 43.5*ns);
  LXe_mpt->AddConstProperty("YIELDRATIO", 0.03);
  LXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> abs_length = {noAbsLength_, noAbsLength_};

  assert(abs_energy.size() == abs_length.size());
  LXe_mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  return LXe_mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::LXe_nconst()
{
  XenonLiquidProperties LXe_prop;
  G4MaterialPropertiesTable* LXe_mpt = new G4MaterialPropertiesTable();

  std::vector<G4double> ri_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> ri_index  = {1.7, 1.7};

  assert(ri_energy.size() == ri_index.size());
  LXe_mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  // Sampling from ~151 nm to 200 nm <----> from 6.20625 eV to 8.21 eV
  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  const G4int sc_entries = 500;
  const G4double minE = 6.20625*eV;
  eWidth = (optPhotMaxE_ - minE) / sc_entries;

  std::vector<G4double> sc_energy;
  for (int j=0; j<sc_entries; j++){
    sc_energy.push_back(minE + j * eWidth);
  }
  std::vector<G4double> intensity;
  LXe_prop.Scintillation(sc_energy, intensity);

  assert(sc_energy.size() == intensity.size());
  LXe_mpt->AddProperty("FASTCOMPONENT", sc_energy.data(), intensity.data(), sc_energy.size());
  LXe_mpt->AddProperty("SLOWCOMPONENT", sc_energy.data(), intensity.data(), sc_energy.size());

  std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> abs_length = {noAbsLength_, noAbsLength_};

  assert(abs_energy.size() == abs_length.size());
  LXe_mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  LXe_mpt->AddConstProperty("SCINTILLATIONYIELD", 58708./MeV);
  LXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1);
  LXe_mpt->AddConstProperty("RAYLEIGH", 36.*cm);
  LXe_mpt->AddConstProperty("FASTTIMECONSTANT", 2.*ns);
  LXe_mpt->AddConstProperty("SLOWTIMECONSTANT", 43.5*ns);
  LXe_mpt->AddConstProperty("YIELDRATIO", 0.03);
  LXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  return LXe_mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::PTFE()
{
  G4MaterialPropertiesTable* teflon_mpt = new G4MaterialPropertiesTable();

  // REFLECTIVITY
  std::vector<G4double> refl_energy = {
    optPhotMinE_,  2.8 * eV,  3.5 * eV,  4. * eV,
    6. * eV,       7.2 * eV,  optPhotMaxE_};
  std::vector<G4double> reflectivity = {
    .98,  .98,  .98,  .98,
    .72,  .72,  .72};

  assert(refl_energy.size() == reflectivity.size());
  teflon_mpt->AddProperty("REFLECTIVITY", refl_energy.data(), reflectivity.data(), refl_energy.size());

  std::vector<G4double> ENERGIES = {optPhotMinE_, optPhotMaxE_};

  std::vector<G4double> specularlobe = {0., 0.}; // specular reflection about the normal to a
  //microfacet. Such a vector is chosen according to a gaussian distribution with
  //sigma = SigmaAlhpa (in rad) and centered in the average normal.
  std::vector<G4double> specularspike = {0., 0.}; // specular reflection about the average normal
  std::vector<G4double> backscatter = {0., 0.}; //180 degrees reflection
  // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

  assert(ENERGIES.size() == specularlobe.size());
  assert(ENERGIES.size() == specularspike.size());
  assert(ENERGIES.size() == backscatter.size());
  teflon_mpt->AddProperty("SPECULARLOBECONSTANT", ENERGIES.data(), specularlobe.data(), ENERGIES.size());
  teflon_mpt->AddProperty("SPECULARSPIKECONSTANT", ENERGIES.data(), specularspike.data(), ENERGIES.size());
  teflon_mpt->AddProperty("BACKSCATTERCONSTANT", ENERGIES.data(), backscatter.data(), ENERGIES.size());

  return teflon_mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::Pyrex_vidrasa()
{
  G4MaterialPropertiesTable* pyrex_mpt = new G4MaterialPropertiesTable();

  // Refractive index and absorption lenth taken from:
  // http://www.vidrasa.com/esp/productos/borofloat/borofloat_pf.html

  std::vector<G4double> ri_energy = {optPhotMinE_, 2.2542*eV, 2.6338*eV, 3.2370*eV, 3.4768*eV,
                                     3.6304*eV, 3.7125*eV, 3.7549*eV, 3.8204*eV, 3.8882*eV,
                                     3.9114*eV, 3.9585*eV, 4.0562*eV, 4.3003*eV, 4.3198*eV,
                                     4.3394*eV, 4.3792*eV, 4.4403*eV, 4.5678*eV, 6.1992*eV,
                                     optPhotMaxE_};

  std::vector<G4double> ri_index;
  for (int i=0; i<(int)ri_energy.size(); i++) {
    ri_index.push_back(1.472);
  }

  assert(ri_energy.size() == ri_index.size());
  pyrex_mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  std::vector<G4double> abs_length = {99.800*mm, 99.999*mm, 95.000*mm, 70.000*mm,
                                      48.500*mm, 32.323*mm, 22.000*mm, 14.000*mm,
                                      8.963*mm, 7.170*mm,  5.800*mm,   4.643*mm,
                                      3.345*mm, 0.800*mm,  0.710*mm,   0.630*mm,
                                      0.570*mm, 0.485*mm,  0.370*mm,   0.0*mm,
                                      0.0*mm};

  assert(ri_energy.size() == abs_length.size());
  pyrex_mpt->AddProperty("ABSLENGTH", ri_energy.data(), abs_length.data(), ri_energy.size());

  return pyrex_mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::TPB(G4double decay_time)
{

  /// This is the simulation of the optical properties of TPB (tetraphenyl butadiene)
  /// a wavelength shifter which allows to converts VUV photons to blue photons.
  /// A WLS material is characterized by its photon absorption and photon emission spectrum
  /// and by a possible time delay between the absorption and re-emission of the photon.

  // Data from https://doi.org/10.1140/epjc/s10052-018-5807-z
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  std::vector<G4double> rIndex_energies = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> TPB_rIndex      = {1.67, 1.67};

  assert(rIndex_energies.size() == TPB_rIndex.size());
  mpt->AddProperty("RINDEX", rIndex_energies.data(), TPB_rIndex.data(), rIndex_energies.size());

  // ABSORPTION LENGTH
  // Assuming no absorption except WLS
  std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> abs_length  = {noAbsLength_, noAbsLength_};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  // WLS ABSORPTION LENGTH (Version No Secondary WLS)
  // The No Secondary WLS is forced by setting the WLS_absLength to noAbsLength_
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
                               h_Planck * c_light / (100. * nm),  optPhotMaxE_};

  std::vector<G4double> WLS_absLength = {
                              noAbsLength_,
                              noAbsLength_,   50. * nm,     // 380 , 370 nm
                              30. * nm,      30. * nm,     // 360 , 330 nm
                              50. * nm,      80. * nm,     // 320 , 310 nm
                              100. * nm,     100. * nm,     // 300 , 270 nm
                              400. * nm,     400. * nm,     // 250 , 230 nm
                              350. * nm,     250. * nm,     // 210 , 190 nm
                              350. * nm,     400. * nm,     // 170 , 150 nm
                              400. * nm,     noAbsLength_ };// 100 nm

  assert(WLS_abs_energy.size() == WLS_absLength.size());
  mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy.data(),
                   WLS_absLength.data(), WLS_abs_energy.size());

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

  assert(WLS_emi_energy.size() == WLS_emiSpectrum.size());
  mpt->AddProperty("WLSCOMPONENT", WLS_emi_energy.data(),
                   WLS_emiSpectrum.data(), WLS_emi_energy.size());

  // WLS Delay
  mpt->AddConstProperty("WLSTIMECONSTANT", decay_time);

  // WLS Quantum Efficiency
  // According to the paper, the QE of TPB depends on the incident wavelength.
  // As Geant4 doesn't allow this possibility, it is set to the value corresponding
  // to Xe scintillation spectrum peak.
  mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.65);
  //mpt->AddConstProperty("WLSTIMECONSTANT",2.2*ns);

  return mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::TPB_LXe(G4double decay_time)
{
  /// This is the simulation of the optical properties of TPB (tetraphenyl butadiene)
  /// a wavelength shifter which allows to converts VUV photons to blue photons.
  /// A WLS material is characterized by its photon absorption and photon emission spectrum
  /// and by a possible time delay between the absorption and re-emission of the photon.
  /// The difference with TPB is that the refraction index is set equal to that of LXe.


  // Data from https://doi.org/10.1140/epjc/s10052-018-5807-z
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  XenonLiquidProperties LXe_prop;

  const G4int ri_entries = 200;
  G4double eWidth = (optPhotMaxE_ - optPhotMinE_) / ri_entries;

  std::vector<G4double> ri_energy;
  for (int i=0; i<ri_entries; i++) {
    ri_energy.push_back(optPhotMinE_ + i * eWidth);
  }

  std::vector<G4double> ri_index;

  for (G4int i=0; i<ri_entries; i++) {
    ri_index.push_back(LXe_prop.RefractiveIndex(ri_energy[i]));
  }

  assert(ri_energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  // ABSORPTION LENGTH
  // Assuming no absorption except WLS
  std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> abs_length  = {noAbsLength_, noAbsLength_};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  // WLS ABSORPTION LENGTH (Version No Secondary WLS)
  // The No Secondary WLS is forced by setting the WLS_absLength to noAbsLength_
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
                               h_Planck * c_light / (100. * nm),  optPhotMaxE_};

  std::vector<G4double> WLS_absLength = {
                              noAbsLength_,
                              noAbsLength_,   50. * nm,     // 380 , 370 nm
                              30. * nm,      30. * nm,     // 360 , 330 nm
                              50. * nm,      80. * nm,     // 320 , 310 nm
                              100. * nm,     100. * nm,     // 300 , 270 nm
                              400. * nm,     400. * nm,     // 250 , 230 nm
                              350. * nm,     250. * nm,     // 210 , 190 nm
                              350. * nm,     400. * nm,     // 170 , 150 nm
                              400. * nm,     noAbsLength_ };// 100 nm

  assert(WLS_abs_energy.size() == WLS_absLength.size());
  mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy.data(),
                   WLS_absLength.data(), WLS_abs_energy.size());

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

  assert(WLS_emi_energy.size() == WLS_emiSpectrum.size());
  mpt->AddProperty("WLSCOMPONENT", WLS_emi_energy.data(),
                   WLS_emiSpectrum.data(), WLS_emi_energy.size());

  // WLS Delay
  mpt->AddConstProperty("WLSTIMECONSTANT", decay_time);

  // WLS Quantum Efficiency
  // According to the paper, the QE of TPB depends on the incident wavelength.
  // As Geant4 doesn't allow this possibility, it is set to the value corresponding
  // to Xe scintillation spectrum peak.
  mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.65);
  //mpt->AddConstProperty("WLSTIMECONSTANT",2.2*ns);

  return mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::TPB_LXe_nconst(G4double decay_time)
{
    /// This is the simulation of the optical properties of TPB (tetraphenyl butadiene)
  /// a wavelength shifter which allows to converts VUV photons to blue photons.
  /// A WLS material is characterized by its photon absorption and photon emission spectrum
  /// and by a possible time delay between the absorption and re-emission of the photon.
  /// The difference with TPB is that the refraction index is constant and set to 1.7.


  // Data from https://doi.org/10.1140/epjc/s10052-018-5807-z
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX
  std::vector<G4double> ri_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> rindex    = {1.7, 1.7};

  assert(ri_energy.size() == rindex.size());
  mpt->AddProperty("RINDEX", ri_energy.data(), rindex.data(), ri_energy.size());

  // ABSORPTION LENGTH
  // Assuming no absorption except WLS
  std::vector<G4double> abs_energy = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> abs_length  = {noAbsLength_, noAbsLength_};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  // WLS ABSORPTION LENGTH (Version No Secondary WLS)
  // The No Secondary WLS is forced by setting the WLS_absLength to noAbsLength_
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
                               h_Planck * c_light / (100. * nm),  optPhotMaxE_};

  std::vector<G4double> WLS_absLength = {
                              noAbsLength_,
                              noAbsLength_,   50. * nm,     // 380 , 370 nm
                              30. * nm,      30. * nm,     // 360 , 330 nm
                              50. * nm,      80. * nm,     // 320 , 310 nm
                              100. * nm,     100. * nm,     // 300 , 270 nm
                              400. * nm,     400. * nm,     // 250 , 230 nm
                              350. * nm,     250. * nm,     // 210 , 190 nm
                              350. * nm,     400. * nm,     // 170 , 150 nm
                              400. * nm,     noAbsLength_ };// 100 nm

  assert(WLS_abs_energy.size() == WLS_absLength.size());
  mpt->AddProperty("WLSABSLENGTH", WLS_abs_energy.data(),
                   WLS_absLength.data(), WLS_abs_energy.size());

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

  assert(WLS_emi_energy.size() == WLS_emiSpectrum.size());
  mpt->AddProperty("WLSCOMPONENT", WLS_emi_energy.data(),
                   WLS_emiSpectrum.data(), WLS_emi_energy.size());

  // WLS Delay
  mpt->AddConstProperty("WLSTIMECONSTANT", decay_time);

  // WLS Quantum Efficiency
  // According to the paper, the QE of TPB depends on the incident wavelength.
  // As Geant4 doesn't allow this possibility, it is set to the value corresponding
  // to Xe scintillation spectrum peak.
  mpt->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.65);
  //mpt->AddConstProperty("WLSTIMECONSTANT",2.2*ns);

  return mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::LYSO()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // Refractive index taken by "Optical and Scintillation Properties of Inorganic Scintillators in High Energy Physics", R. Mao, Liyuan Zhang, and Ren-Yuan Zhu, IEEE TRANSACTIONS ON NUCLEAR SCIENCE, VOL. 55, NO. 4, AUGUST 2008
  // http://www.hep.caltech.edu/~zhu/papers/08_tns_crystal.pdf
  // Scintillation spectrum extracted from
  // https://www.crystals.saint-gobain.com/sites/imdf.crystals.com/files/documents/lyso-material-data-sheet.pdf
  std::vector<G4double> ri_energy = {lyso_minE_, 2.2708*eV, 2.4028*eV, 2.5511*eV,
                                     2.6895*eV, 2.8437*eV, 2.9520*eV, 3.0613*eV,
                                     lyso_maxE_};
  std::vector<G4double> ri_index = {1.806, 1.806, 1.810, 1.813,
                                    1.818, 1.822, 1.827, 1.833,
                                    1.833};

  // G4cout << "*** LYSO refractive index ***" << G4endl;
  //  for (G4int i=1; i<ri_entries; i++) {
  //   G4cout  << ri_energy[i] << " eV --> " << rindex[i] << ", " << G4endl;
  // }
  assert(ri_energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  std::vector<G4double> sc_energy =
   {lyso_minE_, 1.9908574214449701*eV, 2.017487971587794*eV, 2.049118407065618*eV,
    2.075153360986118*eV, 2.1108921203127564*eV, 2.131530815691394*eV, 2.1549516183402426*eV,
    2.1788791714153226*eV, 2.2008686890176556*eV, 2.225818446565337*eV, 2.2461704051922378*eV,
    2.2774394451688957*eV, 2.2933838194379406*eV, 2.309537681443672*eV, 2.3287011454278184*eV,
    2.34534235398821*eV, 2.365099103796332*eV, 2.382250183859144*eV, 2.4085359216953424*eV,
    2.4233406375845*eV, 2.456622921785144*eV, 2.4782281493172356*eV, 2.503411781001508*eV,
    2.52588828700936*eV, 2.5487533714262676*eV, 2.572055225192039*eV, 2.588926183988832*eV,
    2.6025967807599906*eV, 2.623400276029348*eV, 2.6374283564632037*eV, 2.6588048713474666*eV,
    2.6623164236046244*eV, 2.6877929388662145*eV, 2.713729981831732*eV, 2.7325282355525173*eV,
    2.7593839494903407*eV, 2.7907427243849288*eV, 2.83096946231753*eV, 2.885218973585188*eV,
    2.9197414497478893*eV, 2.9417873478198326*eV, 2.9732295992116424*eV, 2.991505736849123*eV,
    3.005429157443019*eV, 3.0194959021101067*eV, 3.0242886485355824*eV, 3.0432131984421833*eV,
    3.048094952090428*eV, 3.057784590715629*eV, 3.0675360309025064*eV, 3.0870759513756334*eV,
    3.1271371999622284*eV, 3.1373650393337758*eV, 3.1527828353680922*eV, 3.1734564909877436*eV,
    3.189158818959311*eV, 3.199797076377322*eV, 3.2210492254756646*eV, 3.242646039884366*eV,
    3.2699528217360423*eV, 3.2976765066977722*eV, lyso_maxE_};


  std::vector<G4double> intensity =
    {0., 0.0704225352112644, 0.3521126760563362, 0.633802816901401, 0.7746478873239369,
     1.1971830985915446, 1.5492957746478881, 1.8309859154929529, 2.2535211267605604, 2.676056338028168,
     3.2394366197183047, 3.8732394366197203, 4.507042253521121, 5.0, 5.633802816901407,
     6.126760563380272, 6.901408450704224, 7.464788732394362, 8.38028169014084, 9.014084507042256,
     9.788732394366193, 10.985915492957744, 12.183098591549294, 13.38028169014084, 14.29577464788732,
     15.352112676056334, 16.267605633802816, 17.183098591549292, 17.816901408450704, 18.591549295774648,
     19.295774647887324, 20.0, 20.704225352112672, 21.338028169014088, 22.183098591549296,
     23.02816901408451, 23.59154929577465, 24.15492957746479, 24.788732394366196, 24.507042253521124,
     23.80281690140845, 23.09859154929577, 22.04225352112676, 21.408450704225352, 20.56338028169014,
     19.64788732394366, 18.943661971830984, 18.239436619718308, 17.464788732394368, 16.549295774647888,
     15.633802816901408, 14.577464788732392, 11.338028169014079, 10.281690140845065, 8.943661971830984,
     7.746478873239432, 6.760563380281688, 5.7042253521126725, 4.718309859154921, 3.450704225352105,
     2.3239436619718252, 1.4084507042253518, 0.};

  assert(sc_energy.size() == intensity.size());
  mpt->AddProperty("FASTCOMPONENT", sc_energy.data(), intensity.data(), sc_energy.size());

  // S Seifert et al 2012 JINST 7 P09004
  mpt->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.072*ns); //1.7/Ln(9)
  mpt->AddConstProperty("SCINTILLATIONYIELD", 32000./MeV);
  mpt->AddConstProperty("RESOLUTIONSCALE", 1);
  mpt->AddConstProperty("FASTTIMECONSTANT", 41*ns);

  std::vector<G4double> abs_energy = {lyso_minE_, lyso_maxE_};
  std::vector<G4double> abs_length = {41.2*cm, 41.2*cm};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::LYSO_nconst()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // Refractive index taken by "Optical and Scintillation Properties of Inorganic Scintillators in High Energy Physics", R. Mao, Liyuan Zhang, and Ren-Yuan Zhu, IEEE TRANSACTIONS ON NUCLEAR SCIENCE, VOL. 55, NO. 4, AUGUST 2008
  // http://www.hep.caltech.edu/~zhu/papers/08_tns_crystal.pdf
  std::vector<G4double> ri_energy = {lyso_minE_, lyso_maxE_};
  std::vector<G4double> ri_index    = {1.8, 1.8};

  assert(ri_energy.size() == ri_index.size());
  mpt->AddProperty("RINDEX", ri_energy.data(), ri_index.data(), ri_energy.size());

  std::vector<G4double> sc_energy =
    {lyso_minE_, 1.9908574214449701*eV, 2.017487971587794*eV, 2.049118407065618*eV,
     2.075153360986118*eV, 2.1108921203127564*eV, 2.131530815691394*eV, 2.1549516183402426*eV,
     2.1788791714153226*eV, 2.2008686890176556*eV, 2.225818446565337*eV, 2.2461704051922378*eV,
     2.2774394451688957*eV, 2.2933838194379406*eV, 2.309537681443672*eV, 2.3287011454278184*eV,
     2.34534235398821*eV, 2.365099103796332*eV, 2.382250183859144*eV, 2.4085359216953424*eV,
     2.4233406375845*eV, 2.456622921785144*eV, 2.4782281493172356*eV, 2.503411781001508*eV,
     2.52588828700936*eV, 2.5487533714262676*eV, 2.572055225192039*eV, 2.588926183988832*eV,
     2.6025967807599906*eV, 2.623400276029348*eV, 2.6374283564632037*eV, 2.6588048713474666*eV,
     2.6623164236046244*eV, 2.6877929388662145*eV, 2.713729981831732*eV, 2.7325282355525173*eV,
     2.7593839494903407*eV, 2.7907427243849288*eV, 2.83096946231753*eV, 2.885218973585188*eV,
     2.9197414497478893*eV, 2.9417873478198326*eV, 2.9732295992116424*eV, 2.991505736849123*eV,
     3.005429157443019*eV, 3.0194959021101067*eV, 3.0242886485355824*eV, 3.0432131984421833*eV,
     3.048094952090428*eV, 3.057784590715629*eV, 3.0675360309025064*eV, 3.0870759513756334*eV,
     3.1271371999622284*eV, 3.1373650393337758*eV, 3.1527828353680922*eV, 3.1734564909877436*eV,
     3.189158818959311*eV, 3.199797076377322*eV, 3.2210492254756646*eV, 3.242646039884366*eV,
     3.2699528217360423*eV, 3.2976765066977722*eV, lyso_maxE_};

  std::vector<G4double> intensity =
    {0., 0.0704225352112644, 0.3521126760563362, 0.633802816901401, 0.7746478873239369,
     1.1971830985915446, 1.5492957746478881, 1.8309859154929529, 2.2535211267605604, 2.676056338028168,
     3.2394366197183047, 3.8732394366197203, 4.507042253521121, 5.0, 5.633802816901407,
     6.126760563380272, 6.901408450704224, 7.464788732394362, 8.38028169014084, 9.014084507042256,
     9.788732394366193, 10.985915492957744, 12.183098591549294, 13.38028169014084, 14.29577464788732,
     15.352112676056334, 16.267605633802816, 17.183098591549292, 17.816901408450704, 18.591549295774648,
     19.295774647887324, 20.0, 20.704225352112672, 21.338028169014088, 22.183098591549296,
     23.02816901408451, 23.59154929577465, 24.15492957746479, 24.788732394366196, 24.507042253521124,
     23.80281690140845, 23.09859154929577, 22.04225352112676, 21.408450704225352, 20.56338028169014,
     19.64788732394366, 18.943661971830984, 18.239436619718308, 17.464788732394368, 16.549295774647888,
     15.633802816901408, 14.577464788732392, 11.338028169014079, 10.281690140845065, 8.943661971830984,
     7.746478873239432, 6.760563380281688, 5.7042253521126725, 4.718309859154921, 3.450704225352105,
     2.3239436619718252, 1.4084507042253518, 0.};

  assert(sc_energy.size() == intensity.size());
  mpt->AddProperty("FASTCOMPONENT", sc_energy.data(), intensity.data(), sc_energy.size());

  // S Seifert et al 2012 JINST 7 P09004
  mpt->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.072*ns); //1.7/Ln(9)
  mpt->AddConstProperty("SCINTILLATIONYIELD", 32000./MeV);
  mpt->AddConstProperty("RESOLUTIONSCALE", 1);
  mpt->AddConstProperty("FASTTIMECONSTANT", 41*ns);

  std::vector<G4double> abs_energy = {lyso_minE_, lyso_maxE_};
  std::vector<G4double> abs_length = {41.2*cm, 41.2*cm};

  assert(abs_energy.size() == abs_length.size());
  mpt->AddProperty("ABSLENGTH", abs_energy.data(), abs_length.data(), abs_energy.size());


  return mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::ReflectantSurface(G4double reflectivity)
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  std::vector<G4double> ENERGIES = {optPhotMinE_, optPhotMaxE_};
  std::vector<G4double> REFLECTIVITY = {reflectivity, reflectivity};
  std::vector<G4double> specularlobe = {0., 0.}; // specular reflection about the normal to a
  //microfacet. Such a vector is chosen according to a gaussian distribution with
  //sigma = SigmaAlhpa (in rad) and centered in the average normal.
  std::vector<G4double> specularspike = {0., 0.}; // specular reflection about the average normal
  std::vector<G4double> backscatter = {0., 0.}; //180 degrees reflection
  // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

  assert(ENERGIES.size() == REFLECTIVITY.size());
  mpt->AddProperty("REFLECTIVITY", ENERGIES.data(), REFLECTIVITY.data(), ENERGIES.size());

  assert(ENERGIES.size() == specularlobe.size());
  assert(ENERGIES.size() == specularspike.size());
  assert(ENERGIES.size() == backscatter.size());
  mpt->AddProperty("SPECULARLOBECONSTANT", ENERGIES.data(), specularlobe.data(), ENERGIES.size());
  mpt->AddProperty("SPECULARSPIKECONSTANT", ENERGIES.data(), specularspike.data(), ENERGIES.size());
  mpt->AddProperty("BACKSCATTERCONSTANT", ENERGIES.data(), backscatter.data(), ENERGIES.size());

  return mpt;
}
