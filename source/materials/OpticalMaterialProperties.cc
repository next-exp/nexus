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

using namespace nexus;
using namespace CLHEP;


G4MaterialPropertiesTable* OpticalMaterialProperties::Vacuum()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  const G4int entries = 2;
  G4double energy[entries] = {0.01*eV, 100.*eV};

  // REFRACTIVE INDEX
  G4double rindex[entries] = {1., 1.};
  mpt->AddProperty("RINDEX", energy, rindex, entries);

  // ABSORPTION LENGTH
  G4double abslen[entries] = {1.e8*m, 1.e8*m};
  mpt->AddProperty("ABSLENGTH", energy, abslen, entries);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::Epoxy()
{
  // Optical properties of Epoxy adhesives.
  // Obtained from
  // http://www.epotek.com/SSCDocs/techtips/Tech%20Tip%2018%20-%20Understanding%20Optical%20Properties%20for%20Epoxy%20Apps.pdf

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  G4double energy[2] =
    {1*eV, 10.751*eV};
  G4double rindex[2] =
    {1.54, 1.54};

  // ABSORPTION LENGTH /////////////////////////////////////////////////////////

  const G4int abs_entries = 17;

  G4double abs_energy[abs_entries]=
    {1.*eV, 2.132*eV, 2.735*eV, 2.908*eV, 3.119*eV,
     3.320*eV, 3.476*eV, 3.588*eV, 3.749*eV, 3.869*eV,
     3.973*eV, 4.120*eV, 4.224*eV, 4.320*eV, 4.420*eV,
     5.018*eV, 8.*eV};
  G4double abslength[abs_entries] =
    {15000.*cm, 326.*mm, 117.68*mm, 85.89*mm, 50.93*mm,
     31.25*mm, 17.19*mm, 10.46*mm, 5.26*mm, 3.77*mm,
     2.69*mm, 1.94*mm, 1.94*mm, 1.94*mm, 1.94*mm,
     1.94*mm, 1.94*mm};

  mpt->AddProperty("RINDEX", energy, rindex, 2);
  mpt->AddProperty("ABSLENGTH", abs_energy, abslength, abs_entries);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::EpoxyFixedRefr(G4double n)
{
  // Optical properties adjusted in order not to modify the trajectories of photons and
  // not to absorb them before they meet the photocathode. To be set to real properties when available

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  XenonLiquidProperties LXe_prop;

  const G4int ri_entries = 18;
  G4double ri_energy[ri_entries]
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV, 7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.21*eV};

  G4double ri_index[ri_entries];

  for (G4int i=0; i<ri_entries; i++) {
    ri_index[i] = n;
    // ri_index[i] = LXe_prop.RefractiveIndex(ri_energy[i]);
  }

  // ABSORPTION LENGTH /////////////////////////////////////////////////////////

  const G4int abs_entries = 2;

  G4double abs_energy[abs_entries]=
    {1.*eV, 10.*eV};
  G4double abslength[abs_entries] =
    {1.e8*m, 1.e8*m};

  mpt->AddProperty("RINDEX", ri_energy, ri_index, ri_entries);
  mpt->AddProperty("ABSLENGTH", abs_energy, abslength, abs_entries);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::EpoxyLXeRefr()
{
  // Optical properties adjusted in order not to modify the trajectories of photons and
  // not to absorb them before they meet the photocathode. To be set to real properties when available

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  XenonLiquidProperties LXe_prop;

  const G4int ri_entries = 18;
  G4double ri_energy[ri_entries]
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV, 7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.21*eV};

  G4double ri_index[ri_entries];

  for (G4int i=0; i<ri_entries; i++) {
    ri_index[i] = LXe_prop.RefractiveIndex(ri_energy[i]);
  }

  // ABSORPTION LENGTH /////////////////////////////////////////////////////////

  const G4int abs_entries = 2;

  G4double abs_energy[abs_entries]=
    {1.*eV, 10.*eV};
  G4double abslength[abs_entries] =
    {1.e8*m, 1.e8*m};

  mpt->AddProperty("RINDEX", ri_energy, ri_index, ri_entries);
  mpt->AddProperty("ABSLENGTH", abs_energy, abslength, abs_entries);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::FusedSilica()
{
  // Optical properties of Suprasil 311/312(c) synthetic fused silica.
  // Obtained from http://heraeus-quarzglas.com

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX //////////////////////////////////////////////////////////
  // The range is chosen to be up to ~10.7 eV because Sellmeier's equation
  // for fused silica is valid only in that range

  const G4int ri_entries = 200;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = (1 + i*0.049)*eV;
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

  G4double rindex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1)
      + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
      + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
    rindex[i] = sqrt(n2);
  }

  mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);

  // ABSORPTION LENGTH /////////////////////////////////////////////////////////

  const G4int abs_entries = 30;

  G4double abs_energy[abs_entries]=
    {1.*eV, 6.46499*eV, 6.54*eV, 6.59490*eV, 6.64*eV,
     6.72714*eV, 6.73828*eV, 6.75*eV, 6.82104*eV, 6.86*eV,
     6.88*eV, 6.89*eV, 7.*eV, 7.01*eV, 7.01797*eV,
     7.05*eV, 7.08*eV, 7.08482*eV, 7.30*eV, 7.36*eV,
     7.4*eV, 7.48*eV, 7.52*eV, 7.58*eV, 7.67440*eV,
     7.76*eV, 7.89*eV, 7.93*eV, 8.*eV, 10.7*eV};
  G4double abslength[abs_entries] =
    {1500.*cm, 1500.*cm, 200.*cm, 200.*cm, 90.*cm,
     45.*cm, 45*cm, 30*cm, 24*cm, 21*cm,
     20*cm , 19*cm, 16*cm, 14.*cm, 13.*cm,
     8.5*cm, 8.*cm, 6.*cm, 1.5*cm, 1.2*cm,
     1.*cm, .65*cm, .4*cm, .37*cm, .32*cm,
     .28*cm, .220*cm, .215*cm, .00005*cm, .00005*cm};

  mpt->AddProperty("ABSLENGTH", abs_energy, abslength, abs_entries);

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

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = (1 + i*0.049)*eV;
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

  G4double rindex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    G4double n2 = 1 + B_1*pow(lambda,2)/(pow(lambda,2)-C_1)
      + B_2*pow(lambda,2)/(pow(lambda,2)-C_2)
      + B_3*pow(lambda,2)/(pow(lambda,2)-C_3);
    rindex[i] = sqrt(n2);
  }

  mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);

  // ABSORPTION LENGTH /////////////////////////////////////////////////////////
  // It matches the transparency

  G4double abs_length = -thickness/log(transparency);
  const G4int NUMENTRIES  = 2;
  G4double abs_energy[NUMENTRIES] = { .1*eV, 100.*eV };
  G4double ABSL[NUMENTRIES]  = {abs_length, abs_length};

  mpt->AddProperty("ABSLENGTH", abs_energy, ABSL, NUMENTRIES);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::FakeGenericMaterial(G4double transparency, G4double thickness)
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX //////////////////////////////////////////////////////////
  // Liquid xenon refractive index

  const G4int ri_entries = 18;
  G4double ri_energy[ri_entries]
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV, 7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.21*eV};

  XenonLiquidProperties LXe_prop;
  G4double rindex[ri_entries];
  for (G4int i=0; i<ri_entries; i++) {
    rindex[i] = LXe_prop.RefractiveIndex(ri_energy[i]);
  }

  mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);

  // ABSORPTION LENGTH /////////////////////////////////////////////////////////
  // It matches the transparency

  G4double abs_length = -thickness/log(transparency);
  const G4int NUMENTRIES  = 2;
  G4double abs_energy[NUMENTRIES] = { .1*eV, 100.*eV };
  G4double ABSL[NUMENTRIES]  = {abs_length, abs_length};

  mpt->AddProperty("ABSLENGTH", abs_energy, ABSL, NUMENTRIES);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::GlassEpoxy()
{
  // Optical properties of Optorez 1330 glass epoxy.
  // Obtained from http://refractiveindex.info and http://www.zeonex.com/applications_optical.asp

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX //////////////////////////////////////////////////////////

  const G4int ri_entries = 200;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = (1 + i*0.049)*eV;
  }

  G4double rindex[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    G4double n2 = 2.291142 - 3.311944E-2*pow(lambda,2) - 1.630099E-2*pow(lambda,-2) + 7.265983E-3*pow(lambda,-4) - 6.806145E-4*pow(lambda,-6) + 1.960732E-5*pow(lambda,-8);
    rindex[i] = sqrt(n2);
  }

  mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);

  // ABSORPTION LENGTH /////////////////////////////////////////////////////////

  const G4int abs_entries = 16;

  G4double abs_energy[abs_entries]=
    {1.*eV, 2.132*eV, 2.735*eV, 2.908*eV, 3.119*eV,
     3.320*eV, 3.476*eV, 3.588*eV, 3.749*eV, 3.869*eV,
     3.973*eV, 4.120*eV, 4.224*eV, 4.320*eV, 4.420*eV,
     5.018*eV};
  G4double abslength[abs_entries] =
    {15000.*cm, 326.*mm, 117.68*mm, 85.89*mm, 50.93*mm,
     31.25*mm, 17.19*mm, 10.46*mm, 5.26*mm, 3.77*mm,
     2.69*mm, 1.94*mm, 1.33*mm, 0.73*mm, 0.32*mm,
     0.10*mm};

  mpt->AddProperty("ABSLENGTH", abs_energy, abslength, abs_entries);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::Glass()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX ////////////////////////////////////////////////

  const G4int entries = 10;

  G4double energy[entries] = {
    2.58300893*eV, 2.60198171*eV, 2.62289885*eV, 2.66175244*eV, 2.70767479*eV,
    2.80761840*eV, 2.84498460*eV, 3.06361326*eV, 3.40803817*eV, 3.53131384*eV
  };

  G4double rindex[entries] =
    {1.52283, 1.52309, 1.52339, 1.52395, 1.52461,
     1.52611, 1.52668, 1.53024, 1.53649, 1.53894};

  mpt->AddProperty("RINDEX", energy, rindex, entries);

  // ABSORPTION LENGTH ///////////////////////////////////////////////


  return mpt;
}



G4MaterialPropertiesTable* OpticalMaterialProperties::Sapphire()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX ////////////////////////////////////////////////

  G4double um2 = micrometer*micrometer;

  G4double B[3] = {1.4313493, 0.65054713, 5.3414021};
  G4double C[3] = {0.0052799261*um2, 0.0142382647*um2, 325.017834*um2};
  SellmeierEquation seq(B, C);

  const G4double wlmin =  150. * nm;
  const G4double wlmax = 1000. * nm;
  const G4double step  =   10. * nm;

  const G4int entries = G4int((wlmax-wlmin)/step);

  G4double energy[entries];
  G4double rindex[entries];
  for (G4int i=0; i<entries; i++) {

    G4double wl = wlmin + i*step;
    energy[i] = h_Planck*c_light/wl;
    rindex[i] = seq.RefractiveIndex(wl);
  }

  /*const G4int ABS_NUMENTRIES = 18;
  G4double Energies[ABS_NUMENTRIES] = {1.000*eV, 1.231*eV, 1.696*eV, 2.045*eV, 2.838*eV, 3.687*eV,
				       4.276*eV, 4.922*eV, 5.577*eV, 6.150*eV, 6.755*eV, 7.277*eV,
				       7.687*eV, 7.872*eV, 7.993*eV, 8.186*eV, 8.406*eV, 8.915*eV};
  G4double SAPPHIRE_ABSL[ABS_NUMENTRIES] = {577.50*mm, 577.50*mm, 201.52*mm, 99.51*mm, 59.47*mm, 54.42*mm,
					    45.89*mm, 22.25*mm, 11.97*mm, 7.711*mm, 5.027*mm, 3.689*mm,
					    2.847*mm, 1.991*mm, 1.230*mm, 0.923*mm, 0.763*mm, 0.664*mm};*/

  const G4int ABS_NUMENTRIES = 19;
  G4double Energies[ABS_NUMENTRIES] = {1.000*eV, 1.296*eV, 1.683*eV, 2.075*eV, 2.585*eV, 3.088*eV,
				       3.709*eV, 4.385*eV, 4.972*eV, 5.608*eV, 6.066*eV, 6.426*eV,
				       6.806*eV, 7.135*eV, 7.401*eV, 7.637*eV, 7.880*eV, 8.217*eV};
  G4double SAPPHIRE_ABSL[ABS_NUMENTRIES] = {3455.0*mm, 3455.0*mm, 3455.0*mm, 3455.0*mm, 3455.0*mm, 3140.98*mm,
					    2283.30*mm, 1742.11*mm, 437.06*mm, 219.24*mm, 117.773*mm, 80.560*mm,
					    48.071*mm, 28.805*mm, 17.880*mm, 11.567*mm, 7.718*mm, 4.995*mm};

    /*const G4int ABS_NUMENTRIES = 2;
  G4double Energies[ABS_NUMENTRIES] = {1*eV, 8.217*eV};
  G4double SAPPHIRE_ABSL[ABS_NUMENTRIES] = {150000.*mm, 150000*mm};*/

  mpt->AddProperty("RINDEX", energy, rindex, entries);
  mpt->AddProperty("ABSLENGTH", Energies, SAPPHIRE_ABSL, ABS_NUMENTRIES);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::OptCoupler()
{
  //For comparison of optical coupling materials.
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX ////////////////////////////////////////////////
  //Start with curing gel NyeGel OCK-451. sing quoted formula for now.
  G4double um2 = micrometer*micrometer;
  // G4double nm2 = nm*nm;

  // G4double B[3] = {1.4313493, 0.65054713, 5.3414021};
  // G4double C[3] = {0.0052799261*um2, 0.0142382647*um2, 325.017834*um2};
  // SellmeierEquation seq(B, C);
  G4double constTerm = 1.4954;//1.496135;//1.30311;
  G4double squareTerm = 0.008022*um2;//0.00000000006921989*um2;//0.0304109*um2;
  G4double quadTerm = 0.;//0.00008070513*um2*um2;//0.000740633*um2*um2;
  //Ideal coupler (geometric mean).
  //G4double BS[3] = {1.4313493, 0.65054713, 5.3414021};
  //G4double CS[3] = {0.0052799261*um2, 0.0142382647*um2, 325.017834*um2};
  //SellmeierEquation seqS(BS, CS);//Sapphire
  //G4double BFS[3] = {4.73e-1,6.31e-1,9.06e-1};
  //G4double CFS[3] = {1.30e-2*um2,4.13e-3*um2,9.88e+1*um2};
  //SellmeierEquation seqFS(BFS, CFS);//Fused Silica


  const G4double wlmin =  150. * nm;
  const G4double wlmax = 1000. * nm;
  const G4double step  =   10. * nm;

  const G4int entries = G4int((wlmax-wlmin)/step);

  G4double energy[entries];
  G4double rindex[entries];

  for (G4int i=0; i<entries; i++) {

    G4double wl = wlmin + i*step;
    // rindex[i] = seq.RefractiveIndex(wl);
    energy[i] = h_Planck*c_light/wl;
    rindex[i] = constTerm + squareTerm/(wl*wl) + quadTerm/pow(wl,4);
    //rindex[i] = 1.0;//sqrt(seqS.RefractiveIndex(wl)*seqFS.RefractiveIndex(wl));

  }

  //Values estimated from printed plot. Will need to be improved.
  const G4int ABS_NUMENTRIES = 10;
  //NyoGel451
  G4double Energies[ABS_NUMENTRIES] = {1.77*eV, 2.07*eV,2.48*eV,2.76*eV,2.92*eV,3.10*eV,3.31*eV,3.54*eV,3.81*eV,4.13*eV};
  //CargilleOpGel152
  //G4double Energies[ABS_NUMENTRIES] = {1.89*eV, 1.93*eV, 1.96*eV, 2.11*eV, 2.27*eV, 2.55*eV, 2.59*eV, 3.07*eV, 3.40*eV, 3.88*eV};
  //Dow
  //G4double Energies[ABS_NUMENTRIES] = {2.07*eV, 2.16*eV, 2.26*eV, 2.36*eV, 2.48*eV, 2.61*eV, 2.76*eV, 2.92*eV, 3.10*eV, 3.31*eV, 3.55*eV};
  // G4double SAPPHIRE_ABSL[ABS_NUMENTRIES] = {100.*cm, 100.*cm};
  //NyoGel451
  G4double OPTCOUP_ABSL[ABS_NUMENTRIES] = {1332.8*mm,1332.8*mm,1332.8*mm,666.17*mm,499.5*mm,399.5*mm,199.5*mm,132.83*mm,99.5*mm,4.5*mm};
  //CargilleOpGel152 - estimated from data sheet plot
  //G4double OPTCOUP_ABSL[ABS_NUMENTRIES] = {3.32*mm, 3.18*mm, 2.92*mm, 2.59*mm, 1.96*mm, 1.40*mm, 1.36*mm, 0.74*mm, 0.49*mm, 0.31*mm};
  //dow - estimated from paper plot
  //G4double OPTCOUP_ABSL[ABS_NUMENTRIES] = {3.*cm, 2.67*cm, 2.35*cm, 2.14*cm, 1.89*cm, 1.62*cm, 1.52*cm, 1.32*cm, 1.11*cm, 0.95*cm, 0.8*cm};

  mpt->AddProperty("RINDEX", energy, rindex, entries);
  // mpt->AddProperty("ABSLENGTH", Energies, SAPPHIRE_ABSL, ABS_NUMENTRIES);
  mpt->AddProperty("ABSLENGTH", Energies, OPTCOUP_ABSL, ABS_NUMENTRIES);


  return mpt;

}


G4MaterialPropertiesTable* OpticalMaterialProperties::GAr(G4double sc_yield)
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX ////////////////////////////////////////////////

  const G4int ri_entries = 200;

  G4double ri_energy[ri_entries];
  for (int i=0; i<ri_entries; i++) {
    ri_energy[i] = (1 + i*0.049)*eV;
  }

  G4double rindex[ri_entries];

  for (int i=0; i<ri_entries; i++) {
    G4double lambda = h_Planck*c_light/ri_energy[i]*1000; // in micron
    rindex[i] = 1 + 0.012055*(0.2075*pow(lambda,2)/(91.012*pow(lambda,2)-1) + 0.0415*pow(lambda,2)/(87.892*pow(lambda,2)-1) + 4.3330*pow(lambda,2)/(214.02*pow(lambda,2)-1)); // From refractiveindex.info
    //    std::cout << "rindex = " << rindex[i] << std::endl;
  }

  // EMISSION SPECTRUM ////////////////////////////////////////////////

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



  // ABSORTION LENGTH ////////////////////////////////////////////////

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
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // REFRACTIVE INDEX ////////////////////////////////////////////////

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

  // ABSORTION LENGTH ////////////////////////////////////////////////

  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

  G4double fano = 0.11;// Doke et al, NIM 134 (1976)353
  mpt->AddConstProperty("RESOLUTIONSCALE",fano);


  // EMISSION SPECTRUM ////////////////////////////////////////////////

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

  const G4int ri_entries = 9;
  G4double ri_energy[ri_entries]
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 7*eV, 8*eV, 9*eV};

  // G4int ri_entries = 15;
  // G4double ri_energy[ri_entries]
  //   = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV, 7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV};

  G4double rindex[ri_entries];

  for (G4int i=0; i<ri_entries; i++) {
    rindex[i] = GXe_prop.RefractiveIndex(ri_energy[i]);
    // G4cout << ri_energy[i] << ", " << rindex[i] << G4endl;
  }




  // Sampling from ~150 nm to 200 nm <----> from 6.20625 eV to 8.20625 eV
  const G4int sc_entries = 500;
  G4double sc_energy[sc_entries];
  for (int j=0;j<sc_entries;j++){
    sc_energy[j] =  6.20625*eV + 0.004*j*eV;
  }
  G4double intensity[sc_entries];
  GXe_prop.Scintillation(sc_entries, sc_energy, intensity);

  GXe_mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
  GXe_mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  GXe_mpt->AddProperty("ELSPECTRUM", sc_energy, intensity, sc_entries);
  GXe_mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  GXe_mpt->AddConstProperty("SCINTILLATIONYIELD", sc_yield);
  GXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  GXe_mpt->AddConstProperty("FASTTIMECONSTANT",4.5*ns);
  GXe_mpt->AddConstProperty("SLOWTIMECONSTANT",100.*ns);
  //  GXe_mpt->AddConstProperty("ELTIMECONSTANT", 50.*ns);
  GXe_mpt->AddConstProperty("YIELDRATIO",.1);
  GXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  GXe_mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

  return GXe_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::LXe()
{
  XenonLiquidProperties LXe_prop;
  G4MaterialPropertiesTable* LXe_mpt = new G4MaterialPropertiesTable();

   const G4int ri_entries = 18;
   G4double ri_energy[ri_entries]
  = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV, 7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.21*eV};

  //  const G4int ri_entries = 38;
  // G4double ri_energy[ri_entries]
  //   = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV,
  //      7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.4*eV, 8.6*eV, 8.8*eV,
  //      9.*eV, 9.2*eV, 9.4*eV, 9.6*eV, 9.8*eV, 10.*eV, 10.2*eV, 10.4*eV, 10.6*eV, 10.8*eV,
  //      11.*eV, 11.2*eV, 11.4*eV, 11.6*eV, 11.8*eV, 12.*eV, 12.2*eV, 12.4*eV};



  // G4int ri_entries = 15;
  // G4double ri_energy[ri_entries]
  //   = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV, 7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV};

  G4double rindex[ri_entries];

  //  XenonGasProperties GXe_prop(10.*bar, STP_Temperature);

  for (G4int i=0; i<ri_entries; i++) {
    rindex[i] = LXe_prop.RefractiveIndex(ri_energy[i]);
  }

  // for (G4int i=ri_entries-1; i>=0; i--) {
  //   G4cout << h_Planck*c_light/ri_energy[i]/nanometer << ", " << G4endl;
  // }

  // for (G4int i=ri_entries-1; i>=0; i--) {
  //   G4cout  << rindex[i] << ", " << G4endl;
  // }

  // Sampling from ~150 nm to 200 nm <----> from 6.20625 eV to 8.20625 eV
  const G4int sc_entries = 500;
  G4double sc_energy[sc_entries];
  for (int j=0;j<sc_entries;j++){
    sc_energy[j] =  6.20625*eV + 0.004*j*eV;
  }
  G4double intensity[sc_entries];
  LXe_prop.Scintillation(sc_entries, sc_energy, intensity);

  // G4double int2[ri_entries];
  //LXe_prop.Scintillation(ri_entries, ri_energy, int2);

  // for (G4int i=0; i<ri_entries; i++) {
  //   G4cout << ri_energy[i] << ", " << rindex[i] << ", " << int2[i] << G4endl;
  // }

  LXe_mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
  LXe_mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  LXe_mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  // LXe_mpt->AddProperty("ELSPECTRUM", sc_energy, intensity, sc_entries);
  LXe_mpt->AddConstProperty("SCINTILLATIONYIELD", 58708./MeV);
  LXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1);
  LXe_mpt->AddConstProperty("RAYLEIGH", 36.*cm);
  // check constants with the Aprile
  LXe_mpt->AddConstProperty("FASTTIMECONSTANT", 2.2*ns);
  LXe_mpt->AddConstProperty("SLOWTIMECONSTANT", 27.*ns);
  //  LXe_mpt->AddConstProperty("SLOWTIMECONSTANT",40.*ns);
  //  LXe_mpt->AddConstProperty("ELTIMECONSTANT", 50.*ns);
  LXe_mpt->AddConstProperty("YIELDRATIO", 0.065);
  LXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  LXe_mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

  return LXe_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::FakeLXe()
{
  XenonLiquidProperties LXe_prop;
  G4MaterialPropertiesTable* LXe_mpt = new G4MaterialPropertiesTable();

  const G4int ri_entries = 18;
  G4double ri_energy[ri_entries]
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV, 7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.21*eV};

  G4double rindex[ri_entries];

  for (G4int i=0; i<ri_entries; i++) {
    // rindex[i] = LXe_prop.RefractiveIndex(ri_energy[i]);
    rindex[i] = 1.7;
    //   G4cout << ri_energy[i] << ", " << rindex[i] << G4endl;
  }



  LXe_mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
  //LXe_mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  // LXe_mpt->AddProperty("ELSPECTRUM", sc_energy, intensity, sc_entries);
  //  LXe_mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  // LXe_mpt->AddConstProperty("SCINTILLATIONYIELD", 58708./MeV);
  LXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1);
  LXe_mpt->AddConstProperty("RAYLEIGH", 36.*cm);
  // check constants with the Aprile
  //  LXe_mpt->AddConstProperty("FASTTIMECONSTANT",2.2*ns);
  // LXe_mpt->AddConstProperty("SLOWTIMECONSTANT",40.*ns);
  //  LXe_mpt->AddConstProperty("ELTIMECONSTANT", 50.*ns);
  //LXe_mpt->AddConstProperty("YIELDRATIO", .1);
  LXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  LXe_mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

  return LXe_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::LXe_nconst()
{
  XenonLiquidProperties LXe_prop;
  G4MaterialPropertiesTable* LXe_mpt = new G4MaterialPropertiesTable();

  const G4int ri_entries = 18;
  G4double ri_energy[ri_entries]
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV, 7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.21*eV};

  G4double rindex[ri_entries];

  for (G4int i=0; i<ri_entries; i++) {
    rindex[i] = 1.7;
  }

  // Sampling from ~150 nm to 200 nm <----> from 6.20625 eV to 8.20625 eV
  const G4int sc_entries = 500;
  G4double sc_energy[sc_entries];
  for (int j=0;j<sc_entries;j++){
    sc_energy[j] =  6.20625*eV + 0.004*j*eV;
  }
  G4double intensity[sc_entries];
  LXe_prop.Scintillation(sc_entries, sc_energy, intensity);

  LXe_mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
  LXe_mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  LXe_mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  // LXe_mpt->AddProperty("ELSPECTRUM", sc_energy, intensity, sc_entries);
  LXe_mpt->AddConstProperty("SCINTILLATIONYIELD", 58708./MeV);
  LXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1);
  LXe_mpt->AddConstProperty("RAYLEIGH", 36.*cm);
  // check constants with the Aprile
  LXe_mpt->AddConstProperty("FASTTIMECONSTANT", 2.2*ns);
  LXe_mpt->AddConstProperty("SLOWTIMECONSTANT", 27.*ns);
  //  LXe_mpt->AddConstProperty("SLOWTIMECONSTANT",40.*ns);
  //  LXe_mpt->AddConstProperty("ELTIMECONSTANT", 50.*ns);
  LXe_mpt->AddConstProperty("YIELDRATIO", 0.065);
  LXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  LXe_mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

  return LXe_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::LXe_window()
{
  XenonLiquidProperties LXe_prop;
  G4MaterialPropertiesTable* LXe_mpt = new G4MaterialPropertiesTable();

   const G4int ri_entries = 11;
   G4double ri_energy[ri_entries]
     = {4.1328*eV, 4.33944*eV, 4.54608*eV, 4.75272*eV, 4.95936*eV,
	5.166*eV, 5.37264*eV, 5.57928*eV, 5.78592*eV, 5.99256*eV, 6.1992*eV};

  //  const G4int ri_entries = 38;
  // G4double ri_energy[ri_entries]
  //   = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV,
  //      7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.4*eV, 8.6*eV, 8.8*eV,
  //      9.*eV, 9.2*eV, 9.4*eV, 9.6*eV, 9.8*eV, 10.*eV, 10.2*eV, 10.4*eV, 10.6*eV, 10.8*eV,
  //      11.*eV, 11.2*eV, 11.4*eV, 11.6*eV, 11.8*eV, 12.*eV, 12.2*eV, 12.4*eV};

  G4double rindex[ri_entries];

  for (G4int i=0; i<ri_entries; i++) {
    rindex[i] = LXe_prop.RefractiveIndex(ri_energy[i]);
  }

  // Sampling from ~150 nm to 200 nm <----> from 6.20625 eV to 8.20625 eV
  const G4int sc_entries = 500;
  G4double sc_energy[sc_entries];
  for (int j=0;j<sc_entries;j++){
    sc_energy[j] =  6.20625*eV + 0.004*j*eV;
  }
  G4double intensity[sc_entries];
  LXe_prop.Scintillation(sc_entries, sc_energy, intensity);


  // for (G4int i=0; i<ri_entries; i++) {
  //   G4cout << ri_energy[i] << ", " << rindex[i] << ", " << int2[i] << G4endl;
  // }

  LXe_mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
  LXe_mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  LXe_mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  // LXe_mpt->AddProperty("ELSPECTRUM", sc_energy, intensity, sc_entries);
  LXe_mpt->AddConstProperty("SCINTILLATIONYIELD", 58708./MeV);
  LXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1);
  LXe_mpt->AddConstProperty("RAYLEIGH", 36.*cm);
  // check constants with the Aprile
  LXe_mpt->AddConstProperty("FASTTIMECONSTANT", 2.2*ns);
  LXe_mpt->AddConstProperty("SLOWTIMECONSTANT", 27.*ns);
  //  LXe_mpt->AddConstProperty("SLOWTIMECONSTANT",40.*ns);
  //  LXe_mpt->AddConstProperty("ELTIMECONSTANT", 50.*ns);
  LXe_mpt->AddConstProperty("YIELDRATIO", 0.065);
  LXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  LXe_mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

  return LXe_mpt;
}

G4MaterialPropertiesTable*
OpticalMaterialProperties::FakeGrid(G4double pressure, G4double temperature,
				    G4double transparency, G4double thickness,
				    G4int sc_yield)
{
  XenonGasProperties GXe_prop(pressure, temperature);
  G4MaterialPropertiesTable* FGrid_mpt = new G4MaterialPropertiesTable();

  const G4int ri_entries = 9;
  G4double ri_energy[ri_entries]
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 7*eV, 8*eV, 9*eV};
  G4double rindex[ri_entries];

  for (G4int i=0; i<ri_entries; i++) {
    rindex[i] = GXe_prop.RefractiveIndex(ri_energy[i]);
  }

  const G4int sc_entries = 500;
  G4double sc_energy[sc_entries];
  for (int j=0;j<sc_entries;j++){
    sc_energy[j] =  6.20625*eV + 0.004*j*eV;
  }
  G4double intensity[sc_entries];
  GXe_prop.Scintillation(sc_entries, sc_energy, intensity);

  G4double abs_length = -thickness/log(transparency);
  const G4int NUMENTRIES  = 2;
  G4double abs_energy[NUMENTRIES] = { .1*eV, 100.*eV };
  G4double ABSL[NUMENTRIES]  = {abs_length, abs_length};

  FGrid_mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
  FGrid_mpt->AddProperty("ABSLENGTH", abs_energy, ABSL, NUMENTRIES);
  FGrid_mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  FGrid_mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  FGrid_mpt->AddConstProperty("SCINTILLATIONYIELD", sc_yield);
  FGrid_mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  FGrid_mpt->AddConstProperty("FASTTIMECONSTANT",4.5*ns);
  FGrid_mpt->AddConstProperty("SLOWTIMECONSTANT",100.*ns);
  FGrid_mpt->AddConstProperty("YIELDRATIO",.1);
  FGrid_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  return FGrid_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::PTFE()
{
  G4MaterialPropertiesTable* teflon_mpt = new G4MaterialPropertiesTable();

  const G4int REFL_NUMENTRIES = 2;

  G4double ENERGIES[REFL_NUMENTRIES] = {1.0*eV, 30.*eV};
  /// This is for non-coated teflon panels
  G4double REFLECTIVITY[REFL_NUMENTRIES] = {.72, .72};
  G4double specularlobe[REFL_NUMENTRIES] = {0., 0.}; // specular reflection about the normal to a
  //microfacet. Such a vector is chosen according to a gaussian distribution with
  //sigma = SigmaAlhpa (in rad) and centered in the average normal.
  G4double specularspike[REFL_NUMENTRIES] = {0., 0.}; // specular reflection about the average normal
  G4double backscatter[REFL_NUMENTRIES] = {0., 0.}; //180 degrees reflection
  // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

  teflon_mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, REFL_NUMENTRIES);
  teflon_mpt->AddProperty("SPECULARLOBECONSTANT",ENERGIES,specularlobe,REFL_NUMENTRIES);
  teflon_mpt->AddProperty("SPECULARSPIKECONSTANT",ENERGIES,specularspike,REFL_NUMENTRIES);
  teflon_mpt->AddProperty("BACKSCATTERCONSTANT",ENERGIES,backscatter,REFL_NUMENTRIES);

  return teflon_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::PTFE_LXe(G4double reflectivity)
{
  G4MaterialPropertiesTable* teflon_mpt = new G4MaterialPropertiesTable();
  G4cout << "PTFE with LXe refl = " << reflectivity << G4endl;
  const G4int REFL_NUMENTRIES = 2;

  G4double ENERGIES[REFL_NUMENTRIES] = {1.0*eV, 30.*eV};
  /// This is for non-coated teflon panels
  G4double REFLECTIVITY[REFL_NUMENTRIES] = {reflectivity, reflectivity};
  G4double specularlobe[REFL_NUMENTRIES] = {0., 0.}; // specular reflection about the normal to a
  //microfacet. Such a vector is chosen according to a gaussian distribution with
  //sigma = SigmaAlhpa (in rad) and centered in the average normal.
  G4double specularspike[REFL_NUMENTRIES] = {0., 0.}; // specular reflection about the average normal
  G4double backscatter[REFL_NUMENTRIES] = {0., 0.}; //180 degrees reflection
  // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

  teflon_mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, REFL_NUMENTRIES);
  teflon_mpt->AddProperty("SPECULARLOBECONSTANT",ENERGIES,specularlobe,REFL_NUMENTRIES);
  teflon_mpt->AddProperty("SPECULARSPIKECONSTANT",ENERGIES,specularspike,REFL_NUMENTRIES);
  teflon_mpt->AddProperty("BACKSCATTERCONSTANT",ENERGIES,backscatter,REFL_NUMENTRIES);

  return teflon_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::PTFE_with_TPB()
{
  G4MaterialPropertiesTable* teflon_mpt = new G4MaterialPropertiesTable();

  const G4int REFL_NUMENTRIES = 6;

  G4double ENERGIES[REFL_NUMENTRIES] = {1.0*eV, 2.8*eV, 4.*eV, 6.*eV, 7.2*eV, 30.*eV};
  /// This is for TPB coated teflon panels
  G4double REFLECTIVITY[REFL_NUMENTRIES] = {.98, .98, .98, .72, .72, .72};

  G4double ENERGIES_2[2] = {1.0*eV, 30.*eV};
  G4double specularlobe[2] = {0., 0.}; // specular reflection about the normal to a
  //microfacet. Such a vector is chosen according to a gaussian distribution with
  //sigma = SigmaAlhpa (in rad) and centered in the average normal.
  G4double specularspike[2] = {0., 0.}; // specular reflection about the average normal
  G4double backscatter[2] = {0., 0.}; //180 degrees reflection
  // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

  teflon_mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, REFL_NUMENTRIES);
  teflon_mpt->AddProperty("SPECULARLOBECONSTANT",ENERGIES_2,specularlobe,2);
  teflon_mpt->AddProperty("SPECULARSPIKECONSTANT",ENERGIES_2,specularspike,2);
  teflon_mpt->AddProperty("BACKSCATTERCONSTANT",ENERGIES_2,backscatter,2);

  return teflon_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::PTFE_non_reflectant()
{
  G4MaterialPropertiesTable* teflon_mpt = new G4MaterialPropertiesTable();

  const G4int REFL_NUMENTRIES = 6;

  G4double ENERGIES[REFL_NUMENTRIES] = {1.0*eV, 2.8*eV, 4.*eV, 6.*eV, 7.2*eV, 30.*eV};
  /// This is for TPB coated teflon panels
  G4double REFLECTIVITY[REFL_NUMENTRIES] = {0., 0., 0., 0., 0., 0.};

  G4double ENERGIES_2[2] = {1.0*eV, 30.*eV};
  G4double specularlobe[2] = {0., 0.}; // specular reflection about the normal to a
  //microfacet. Such a vector is chosen according to a gaussian distribution with
  //sigma = SigmaAlhpa (in rad) and centered in the average normal.
  G4double specularspike[2] = {0., 0.}; // specular reflection about the average normal
  G4double backscatter[2] = {0., 0.}; //180 degrees reflection
  // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

  teflon_mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, REFL_NUMENTRIES);
  teflon_mpt->AddProperty("SPECULARLOBECONSTANT",ENERGIES_2,specularlobe,2);
  teflon_mpt->AddProperty("SPECULARSPIKECONSTANT",ENERGIES_2,specularspike,2);
  teflon_mpt->AddProperty("BACKSCATTERCONSTANT",ENERGIES_2,backscatter,2);

  return teflon_mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::TPB(G4double pressure, G4double temperature)
{

  /// This is the simulation of the optical properties of TPB (tetraphenyl butadiene)
  /// a wavelength shifter which allows to converts VUV photons to blue photons.
  /// A WLS material is characterized by its photon absorption and photon emission spectrum
  /// and by a possible time delay between the absorption and re-emission of the photon.

  G4MaterialPropertiesTable* tpb_mpt = new G4MaterialPropertiesTable();

  const G4int ri_entries = 9;
  //const G4int ABSL_NUMENTRIES = 8;
  //const G4int WLSABSL_NUMENTRIES = 7;
  const G4int EMISSION_NUMENTRIES = 55;

  XenonGasProperties GXe_prop(pressure, temperature);
  G4double ri_energy[ri_entries]
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 7*eV, 8*eV, 9*eV};
  G4double rindex[ri_entries];
  for (G4int i=0; i<ri_entries; i++) {
    rindex[i] = GXe_prop.RefractiveIndex(ri_energy[i]);
    //   G4cout << ri_energy[i] << ", " << rindex[i] << G4endl;
  }

/* Right border of the bins*/
  G4double WLS_Emission_Energies[EMISSION_NUMENTRIES] = {1.96800306364 *eV,  1.98230541148 *eV,  1.99681716413 *eV,  2.01154295443 *eV,  2.0264875529 *eV,  2.04165587291 *eV,  2.05705297602 *eV,  2.07268407766 *eV,  2.08855455299 *eV,  2.10466994306 *eV,  2.12103596128 *eV,  2.13765850016 *eV,  2.15454363839 *eV,  2.17169764825 *eV,  2.18912700337 *eV,  2.2068383869 *eV,  2.2248387 *eV,  2.24313507089 *eV,  2.26173486418 *eV,  2.28064569081 *eV,  2.29987541838 *eV,  2.31943218215 *eV,  2.3393243964 *eV,  2.35956076661 *eV,  2.3801503021 *eV,  2.4011023294 *eV,  2.4224265064 *eV,  2.4441328371 *eV,  2.46623168735 *eV,  2.48873380128 *eV,  2.51165031879 *eV,  2.53499279387 *eV,  2.55877321408 *eV,  2.58300402103 *eV,  2.60769813212 *eV,  2.63286896352 *eV,  2.65853045439 *eV,  2.68469709272 *eV,  2.71138394255 *eV,  2.7386066729 *eV,  2.76638158844 *eV,  2.7947256621 *eV,  2.82365656957 *eV,  2.85319272616 *eV,  2.8833533258 *eV,  2.91415838269 *eV,  2.9456287756 *eV,  2.97778629498 *eV,  3.01065369338 *eV,  3.04425473906 *eV,  3.07861427337 *eV,  3.11375827192 *eV,  3.14971391017 *eV,  3.18650963341 *eV,  3.22417523192 *eV};
;

 G4double TPB_Emission[EMISSION_NUMENTRIES] = {5e-05 , 5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  0.0001 ,  0.0002 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0002 ,  0.0007 ,  0.0007 ,  0.001 ,  0.0011 ,  0.0013 ,  0.0016 ,  0.0018 ,  0.0022 ,  0.0029 ,  0.0035 ,  0.0034 ,  0.0041 ,  0.0046 ,  0.0053 ,  0.0061 ,  0.0069 ,  0.008 ,  0.0087 ,  0.0101 ,  0.0101 ,  0.0103 ,  0.0105 ,  0.0112 ,  0.0117 ,  0.0112 ,  0.011 ,  0.014 ,  0.008 ,  0.008 ,  0.007 ,  0.0038 ,  0.0012 ,  0.0008, 0.0004};


 // Values taken from Gehman's paper http://arxiv.org/pdf/1104.3259.pdf
 const G4int wls_numentries = 14;
 G4double WLS_Energies[wls_numentries] =
   {1.*eV, 2.8*eV, 4.*eV, 4.95937*eV, 5.16601*eV, 5.39062*eV,
    5.63565*eV, 5.90401*eV, 6.19921*eV, 6.52548*eV, 6.88801*eV, 7.29319*eV,
    7.74901*eV, 8.26561*eV};
 G4double TPB_ConvEfficiency[wls_numentries] =
   {0., 0., 0., .86, .90, .94,
    .90, .80, .75, .70, .75, .82,
    .85, .92};

 tpb_mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
 tpb_mpt->AddProperty("WLSCOMPONENT", WLS_Emission_Energies,
		      TPB_Emission, EMISSION_NUMENTRIES);
 tpb_mpt->AddProperty("WLSCONVEFFICIENCY", WLS_Energies,
		      TPB_ConvEfficiency, wls_numentries);
 tpb_mpt->AddConstProperty("WLSTIMECONSTANT",2.2*ns);

  return tpb_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::TPB_LXe(G4double decay)
{
  /// This is the simulation of the optical properties of TPB (tetraphenyl butadiene)
  /// a wavelength shifter which allows to converts VUV photons to blue photons.
  /// A WLS material is characterized by its photon absorption and photon emission spectrum
  /// and by a possible time delay between the absorption and re-emission of the photon.

  G4MaterialPropertiesTable* tpb_mpt = new G4MaterialPropertiesTable();

  // const G4int ri_entries = 9;
  //const G4int ABSL_NUMENTRIES = 8;
  //const G4int WLSABSL_NUMENTRIES = 7;
  const G4int EMISSION_NUMENTRIES = 55;

  XenonLiquidProperties LXe_prop;

  const G4int ri_entries = 18;
  G4double ri_energy[ri_entries]
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV, 7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.21*eV};

  // const G4int ri_entries = 38;
  // G4double ri_energy[ri_entries]
  //   = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV,
  //      7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.4*eV, 8.6*eV, 8.8*eV,
  //      9.*eV, 9.2*eV, 9.4*eV, 9.6*eV, 9.8*eV, 10.*eV, 10.2*eV, 10.4*eV, 10.6*eV, 10.8*eV,
  //      11.*eV, 11.2*eV, 11.4*eV, 11.6*eV, 11.8*eV, 12.*eV, 12.2*eV, 12.4*eV};

  G4double rindex[ri_entries];
  for (G4int i=0; i<ri_entries; i++) {
    rindex[i] = LXe_prop.RefractiveIndex(ri_energy[i]);
    // rindex[i] = 1.7;
    //   G4cout << ri_energy[i] << ", " << rindex[i] << G4endl;
  }

/* Right border of the bins*/
  G4double WLS_Emission_Energies[EMISSION_NUMENTRIES] = {1.96800306364 *eV,  1.98230541148 *eV,  1.99681716413 *eV,  2.01154295443 *eV,  2.0264875529 *eV,  2.04165587291 *eV,  2.05705297602 *eV,  2.07268407766 *eV,  2.08855455299 *eV,  2.10466994306 *eV,  2.12103596128 *eV,  2.13765850016 *eV,  2.15454363839 *eV,  2.17169764825 *eV,  2.18912700337 *eV,  2.2068383869 *eV,  2.2248387 *eV,  2.24313507089 *eV,  2.26173486418 *eV,  2.28064569081 *eV,  2.29987541838 *eV,  2.31943218215 *eV,  2.3393243964 *eV,  2.35956076661 *eV,  2.3801503021 *eV,  2.4011023294 *eV,  2.4224265064 *eV,  2.4441328371 *eV,  2.46623168735 *eV,  2.48873380128 *eV,  2.51165031879 *eV,  2.53499279387 *eV,  2.55877321408 *eV,  2.58300402103 *eV,  2.60769813212 *eV,  2.63286896352 *eV,  2.65853045439 *eV,  2.68469709272 *eV,  2.71138394255 *eV,  2.7386066729 *eV,  2.76638158844 *eV,  2.7947256621 *eV,  2.82365656957 *eV,  2.85319272616 *eV,  2.8833533258 *eV,  2.91415838269 *eV,  2.9456287756 *eV,  2.97778629498 *eV,  3.01065369338 *eV,  3.04425473906 *eV,  3.07861427337 *eV,  3.11375827192 *eV,  3.14971391017 *eV,  3.18650963341 *eV,  3.22417523192 *eV};
;

 G4double TPB_Emission[EMISSION_NUMENTRIES] = {5e-05 , 5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  0.0001 ,  0.0002 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0002 ,  0.0007 ,  0.0007 ,  0.001 ,  0.0011 ,  0.0013 ,  0.0016 ,  0.0018 ,  0.0022 ,  0.0029 ,  0.0035 ,  0.0034 ,  0.0041 ,  0.0046 ,  0.0053 ,  0.0061 ,  0.0069 ,  0.008 ,  0.0087 ,  0.0101 ,  0.0101 ,  0.0103 ,  0.0105 ,  0.0112 ,  0.0117 ,  0.0112 ,  0.011 ,  0.014 ,  0.008 ,  0.008 ,  0.007 ,  0.0038 ,  0.0012 ,  0.0008, 0.0004};


 // Values taken from Gehman's paper http://arxiv.org/pdf/1104.3259.pdf
 // const G4int wls_numentries = 14;
 // G4double WLS_Energies[wls_numentries] =
 //   {1.*eV, 2.8*eV, 4.*eV, 4.95937*eV, 5.16601*eV, 5.39062*eV,
 //    5.63565*eV, 5.90401*eV, 6.19921*eV, 6.52548*eV, 6.88801*eV, 7.29319*eV,
 //    7.74901*eV, 8.26561*eV};
 // G4double TPB_ConvEfficiency[wls_numentries] =
 //   {0., 0., 0., .86, .90, .94,
 //    .90, .80, .75, .70, .75, .82,
 //    .85, .92};

  const G4int wls_numentries = 17;
 G4double WLS_Energies[wls_numentries] =
   {1.*eV, 2.8*eV, 4.*eV, 4.95937*eV, 5.16601*eV, 5.39062*eV,
    5.63565*eV, 5.90401*eV, 6.19921*eV, 6.52548*eV, 6.88801*eV, 7.29319*eV,
    7.74901*eV, 8.26561*eV, 9.*eV, 10.*eV, 12.4*eV};
 G4double TPB_ConvEfficiency[wls_numentries] =
   {0., 0., 0., .86, .90, .94,
    .90, .80, .75, .70, .75, .82,
    .85, .92, 1., 1., 1.};

 tpb_mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
 tpb_mpt->AddProperty("WLSCOMPONENT", WLS_Emission_Energies,
		      TPB_Emission, EMISSION_NUMENTRIES);
 tpb_mpt->AddProperty("WLSCONVEFFICIENCY", WLS_Energies,
		      TPB_ConvEfficiency, wls_numentries);
 tpb_mpt->AddConstProperty("WLSTIMECONSTANT", decay);
 //tpb_mpt->AddConstProperty("WLSTIMECONSTANT",0.1*picosecond);

 G4cout << "Decay time of TPB = " << decay/nanosecond << " nanoseconds." << G4endl;

  return tpb_mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::TPB_LXe_nconst(G4double decay)
{
  /// This is the simulation of the optical properties of TPB (tetraphenyl butadiene)
  /// a wavelength shifter which allows to converts VUV photons to blue photons.
  /// A WLS material is characterized by its photon absorption and photon emission spectrum
  /// and by a possible time delay between the absorption and re-emission of the photon.

  G4MaterialPropertiesTable* tpb_mpt = new G4MaterialPropertiesTable();

  // const G4int ri_entries = 9;
  //const G4int ABSL_NUMENTRIES = 8;
  //const G4int WLSABSL_NUMENTRIES = 7;
  const G4int EMISSION_NUMENTRIES = 55;

  XenonLiquidProperties LXe_prop;

  const G4int ri_entries = 18;
  G4double ri_energy[ri_entries]
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 6.2*eV, 6.4*eV, 6.6*eV, 6.8*eV, 7*eV, 7.2*eV, 7.4*eV, 7.6*eV, 7.8*eV, 8*eV, 8.2*eV, 8.21*eV};

  G4double rindex[ri_entries];
  for (G4int i=0; i<ri_entries; i++) {
    rindex[i] = 1.7;
    //   G4cout << ri_energy[i] << ", " << rindex[i] << G4endl;
  }

/* Right border of the bins*/
  G4double WLS_Emission_Energies[EMISSION_NUMENTRIES] = {1.96800306364 *eV,  1.98230541148 *eV,  1.99681716413 *eV,  2.01154295443 *eV,  2.0264875529 *eV,  2.04165587291 *eV,  2.05705297602 *eV,  2.07268407766 *eV,  2.08855455299 *eV,  2.10466994306 *eV,  2.12103596128 *eV,  2.13765850016 *eV,  2.15454363839 *eV,  2.17169764825 *eV,  2.18912700337 *eV,  2.2068383869 *eV,  2.2248387 *eV,  2.24313507089 *eV,  2.26173486418 *eV,  2.28064569081 *eV,  2.29987541838 *eV,  2.31943218215 *eV,  2.3393243964 *eV,  2.35956076661 *eV,  2.3801503021 *eV,  2.4011023294 *eV,  2.4224265064 *eV,  2.4441328371 *eV,  2.46623168735 *eV,  2.48873380128 *eV,  2.51165031879 *eV,  2.53499279387 *eV,  2.55877321408 *eV,  2.58300402103 *eV,  2.60769813212 *eV,  2.63286896352 *eV,  2.65853045439 *eV,  2.68469709272 *eV,  2.71138394255 *eV,  2.7386066729 *eV,  2.76638158844 *eV,  2.7947256621 *eV,  2.82365656957 *eV,  2.85319272616 *eV,  2.8833533258 *eV,  2.91415838269 *eV,  2.9456287756 *eV,  2.97778629498 *eV,  3.01065369338 *eV,  3.04425473906 *eV,  3.07861427337 *eV,  3.11375827192 *eV,  3.14971391017 *eV,  3.18650963341 *eV,  3.22417523192 *eV};
;

 G4double TPB_Emission[EMISSION_NUMENTRIES] = {5e-05 , 5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  0.0001 ,  0.0002 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0002 ,  0.0007 ,  0.0007 ,  0.001 ,  0.0011 ,  0.0013 ,  0.0016 ,  0.0018 ,  0.0022 ,  0.0029 ,  0.0035 ,  0.0034 ,  0.0041 ,  0.0046 ,  0.0053 ,  0.0061 ,  0.0069 ,  0.008 ,  0.0087 ,  0.0101 ,  0.0101 ,  0.0103 ,  0.0105 ,  0.0112 ,  0.0117 ,  0.0112 ,  0.011 ,  0.014 ,  0.008 ,  0.008 ,  0.007 ,  0.0038 ,  0.0012 ,  0.0008, 0.0004};


 // Values taken from Gehman's paper http://arxiv.org/pdf/1104.3259.pdf
 const G4int wls_numentries = 14;
 G4double WLS_Energies[wls_numentries] =
   {1.*eV, 2.8*eV, 4.*eV, 4.95937*eV, 5.16601*eV, 5.39062*eV,
    5.63565*eV, 5.90401*eV, 6.19921*eV, 6.52548*eV, 6.88801*eV, 7.29319*eV,
    7.74901*eV, 8.26561*eV};
 G4double TPB_ConvEfficiency[wls_numentries] =
   {0., 0., 0., .86, .90, .94,
    .90, .80, .75, .70, .75, .82,
    .85, .92};

 tpb_mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
 tpb_mpt->AddProperty("WLSCOMPONENT", WLS_Emission_Energies,
		      TPB_Emission, EMISSION_NUMENTRIES);
 tpb_mpt->AddProperty("WLSCONVEFFICIENCY", WLS_Energies,
		      TPB_ConvEfficiency, wls_numentries);
 tpb_mpt->AddConstProperty("WLSTIMECONSTANT", decay);
 //tpb_mpt->AddConstProperty("WLSTIMECONSTANT", 0.1*picosecond);

 G4cout << "Decay time of TPB = " << decay/nanosecond << " nanoseconds." << G4endl;

  return tpb_mpt;
}



G4MaterialPropertiesTable* OpticalMaterialProperties::TPBOld()
{
  /// This is the simulation of the optical properties of TPB (tetraphenyl butadiene)
  /// a wavelength shifter which allows to converts VUV photons to blue photons.
  /// A WLS material is characterized by its photon absorption and photon emission spectrum
  /// and by a possible time delay between the absorption and re-emission of the photon.

  G4MaterialPropertiesTable* tpb_mpt = new G4MaterialPropertiesTable();

  const G4int RIN_NUMENTRIES  = 5;
  const G4int ABSL_NUMENTRIES = 8;
  const G4int WLSABSL_NUMENTRIES = 7;
  const G4int EMISSION_NUMENTRIES = 55;

  G4double Energies[RIN_NUMENTRIES] = {0.1*eV, 1.1*eV, 5.*eV, 10.*eV, 100.*eV};
  G4double TPB_RIND[ABSL_NUMENTRIES]  = {1.3, 1.3, 1.3, 1.3, 1.3};

/* Right border of the bins*/
  G4double WLS_Emission_Energies[EMISSION_NUMENTRIES] = {1.96800306364 *eV,  1.98230541148 *eV,  1.99681716413 *eV,  2.01154295443 *eV,  2.0264875529 *eV,  2.04165587291 *eV,  2.05705297602 *eV,  2.07268407766 *eV,  2.08855455299 *eV,  2.10466994306 *eV,  2.12103596128 *eV,  2.13765850016 *eV,  2.15454363839 *eV,  2.17169764825 *eV,  2.18912700337 *eV,  2.2068383869 *eV,  2.2248387 *eV,  2.24313507089 *eV,  2.26173486418 *eV,  2.28064569081 *eV,  2.29987541838 *eV,  2.31943218215 *eV,  2.3393243964 *eV,  2.35956076661 *eV,  2.3801503021 *eV,  2.4011023294 *eV,  2.4224265064 *eV,  2.4441328371 *eV,  2.46623168735 *eV,  2.48873380128 *eV,  2.51165031879 *eV,  2.53499279387 *eV,  2.55877321408 *eV,  2.58300402103 *eV,  2.60769813212 *eV,  2.63286896352 *eV,  2.65853045439 *eV,  2.68469709272 *eV,  2.71138394255 *eV,  2.7386066729 *eV,  2.76638158844 *eV,  2.7947256621 *eV,  2.82365656957 *eV,  2.85319272616 *eV,  2.8833533258 *eV,  2.91415838269 *eV,  2.9456287756 *eV,  2.97778629498 *eV,  3.01065369338 *eV,  3.04425473906 *eV,  3.07861427337 *eV,  3.11375827192 *eV,  3.14971391017 *eV,  3.18650963341 *eV,  3.22417523192 *eV};
;

 G4double TPB_Emission[EMISSION_NUMENTRIES] = {5e-05 , 5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  5e-05 ,  0.0001 ,  0.0002 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0003 ,  0.0002 ,  0.0007 ,  0.0007 ,  0.001 ,  0.0011 ,  0.0013 ,  0.0016 ,  0.0018 ,  0.0022 ,  0.0029 ,  0.0035 ,  0.0034 ,  0.0041 ,  0.0046 ,  0.0053 ,  0.0061 ,  0.0069 ,  0.008 ,  0.0087 ,  0.0101 ,  0.0101 ,  0.0103 ,  0.0105 ,  0.0112 ,  0.0117 ,  0.0112 ,  0.011 ,  0.014 ,  0.008 ,  0.008 ,  0.007 ,  0.0038 ,  0.0012 ,  0.0008, 0.0004};


 /// Initially, the absorption efficiency (that is, the number of shifted photons/number
  /// of photons impinguing on the material) was set to 90% for VUV light and 0 for
  /// any other wavelength, for a thickness of 1 micron of TPB (that is the thickness
  /// assigned to the light tube coating).
  /// That means I/I_0 = .1 = exp-(1/lambda) --> lambda = .43 micron. The absorption spectrum
  /// (WLSABSLENGTH property) is then assigned this value for the whole scintillation range of energies
  /// and no absorption for any other wavelegngth.
  /// The coating of the SiPMs is way thinner (100 nm), thus that efficiency was
  /// too low to have any shifting there. Therefore we set the absorption constant to
  /// .00000043 cm, which gives 100% shifting efficiency to a 100-nm thickness, thus to
  /// the 1-mu one, too.
   G4double WLS_Abs_Energies[WLSABSL_NUMENTRIES] =
     {0.1*eV, 4.*eV, 6.20625*eV, 7.2*eV, 8.20625*eV, 9.*eV, 100.*eV};
   G4double TPB_WLSABSL[WLSABSL_NUMENTRIES]  =
     {1000.*cm, 1000.*cm, 0.00000043*cm, 0.00000043*cm, 0.00000043*cm, 1000*cm, 1000.*cm};

   G4cout << "Old TPB" << G4endl;
  tpb_mpt->AddProperty("RINDEX", Energies, TPB_RIND, RIN_NUMENTRIES);
  tpb_mpt->AddProperty("WLSCOMPONENT", WLS_Emission_Energies, TPB_Emission, EMISSION_NUMENTRIES);
  tpb_mpt->AddProperty("WLSABSLENGTH", WLS_Abs_Energies, TPB_WLSABSL,WLSABSL_NUMENTRIES);
  tpb_mpt->AddConstProperty("WLSTIMECONSTANT",1.*ns);

  return tpb_mpt;
}



G4MaterialPropertiesTable* OpticalMaterialProperties::LYSO()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // Refractive index taken by "Optical and Scintillation Properties of Inorganic Scintillators in High Energy Physics", R. Mao, Liyuan Zhang, and Ren-Yuan Zhu, IEEE TRANSACTIONS ON NUCLEAR SCIENCE, VOL. 55, NO. 4, AUGUST 2008
  // http://www.hep.caltech.edu/~zhu/papers/08_tns_crystal.pdf
  const G4int ri_entries = 16;
  // G4double ri_energy[ri_entries] = {1.9074*eV, 2.2708*eV, 2.4028*eV, 2.5511*eV, 2.6895*eV, 2.8437*eV, 2.9520*eV, 3.0613*eV, 3.2542*eV};
  // G4double rindex[ri_entries] = { 1.802, 1.806, 1.810, 1.813, 1.818, 1.822, 1.827, 1.833, 1.842};
  G4double ri_energy[ri_entries] = {1.9074*eV, 2.0891*eV, 2.2708*eV,
				    2.3368*eV, 2.4028*eV, 2.5511*eV, 2.6203*eV,
				    2.6895*eV, 2.7665*eV, 2.8437*eV, 2.89785*eV,
				    2.9520*eV, 3.00665*eV, 3.0613*eV,
				    3.1578*eV, 3.2542*eV};
  G4double rindex[ri_entries] = { 1.802, 1.804, 1.806,
				  1.808, 1.810, 1.813, 1.8155,
				  1.818, 1.820, 1.822, 1.8245,
				  1.827, 1.830, 1.833,
				  1.838, 1.842};

  // for (G4int i=ri_entries-1; i>0; i--) {
  //   G4cout << h_Planck*c_light/ri_energy[i]/nanometer << ", " << c_light/( rindex[i] + (rindex[i]-rindex[i-1])/(std::log(ri_energy[i]) - std::log(ri_energy[i-1]) ))/mm*picosecond << ": " << rindex[i]-rindex[i-1] << ", "
  // 	   << std::log(ri_energy[i]) - std::log(ri_energy[i-1]) << G4endl;
  // }

  //  for (G4int i=ri_entries-1; i>=0; i--) {
  for (G4int i=1; i<ri_entries; i++) {
    G4cout  << rindex[i] << ", " << G4endl;
  }

 //  for (G4int i=1; i<ri_entries; i++) {
//      G4cout  << ri_energy[i]/eV<< ", ";
//    }
//   G4cout << G4endl;

// for (G4int i=1; i<ri_entries; i++) {
//   G4cout << (rindex[i]-rindex[i-1])/(std::log(ri_energy[i]) - std::log(ri_energy[i-1]) ) << ", ";
//  }

  // const G4int sc_entries = 11;
  // G4double sc_energy[sc_entries] =
  //   {1.9074*eV, 2.2543*eV, 2.4797*eV, 2.5511*eV, 2.6436*eV, 2.6895*eV, 2.8437*eV, 2.8700*eV, 2.9520*eV, 3.0996*eV, 3.2542*eV};

  // G4double intensity[sc_entries] = {0., 0.0643, 0.1929, 0.2500, 0.4, 0.5214, 0.9571, 1, 0.8286, 0.4071, 0.};
const G4int sc_entries = 13;
  G4double sc_energy[sc_entries] =
    {1.9074*eV, 2.2543*eV, 2.4797*eV, 2.5511*eV, 2.6436*eV, 2.6895*eV, 2.8437*eV, 2.8700*eV, 2.9520*eV, 3.0258*eV, 3.0996*eV, 3.1769*eV, 3.2542*eV};

  G4double intensity[sc_entries] = {0., 0.0643, 0.1929, 0.2500, 0.4, 0.5214, 0.9571, 1, 0.8286, 0.6, 0.4071, 0.2, 0.};




  mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
  mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  mpt->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.773703*ns); //1.7/Ln(9)
  // mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  mpt->AddConstProperty("SCINTILLATIONYIELD", 32000./MeV);
  // mpt->AddConstProperty("SCINTILLATIONYIELD", 100./MeV);
  mpt->AddConstProperty("RESOLUTIONSCALE", 1);
  //mpt->AddConstProperty("RAYLEIGH", 36000.*cm);
  // check constants with the Aprile
  mpt->AddConstProperty("FASTTIMECONSTANT", 41*ns);
  // mpt->AddConstProperty("SLOWTIMECONSTANT", 27.*ns);
  //  LXe_mpt->AddConstProperty("SLOWTIMECONSTANT",40.*ns);
  //  LXe_mpt->AddConstProperty("ELTIMECONSTANT", 50.*ns);
  // mpt->AddConstProperty("YIELDRATIO", 1.);

  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {41.2*cm, 41.2*cm};

  mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::LYSO_nconst()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  // Refractive index taken by "Optical and Scintillation Properties of Inorganic Scintillators in High Energy Physics", R. Mao, Liyuan Zhang, and Ren-Yuan Zhu, IEEE TRANSACTIONS ON NUCLEAR SCIENCE, VOL. 55, NO. 4, AUGUST 2008
  // http://www.hep.caltech.edu/~zhu/papers/08_tns_crystal.pdf
  const G4int ri_entries = 9;
  G4double ri_energy[ri_entries] = {1.9074*eV, 2.2708*eV, 2.4028*eV, 2.5511*eV, 2.6895*eV, 2.8437*eV, 2.9520*eV, 3.0613*eV, 3.2542*eV};

  G4double rindex[ri_entries] = { 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8};


  const G4int sc_entries = 7;
  G4double sc_energy[sc_entries] =
    {1.9074*eV, 2.2543*eV, 2.4797*eV, 2.6436*eV, 2.8700*eV, 3.0996*eV, 3.2542*eV};

  G4double intensity[sc_entries] = {0., 0.0643, 0.1929, 0.4, 1, 0.4071, 0.};


  mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);
  mpt->AddProperty("FASTCOMPONENT", sc_energy, intensity, sc_entries);
  mpt->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.773703*ns);
  // mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  mpt->AddConstProperty("SCINTILLATIONYIELD", 32000./MeV);
  // mpt->AddConstProperty("SCINTILLATIONYIELD", 100./MeV);
  mpt->AddConstProperty("RESOLUTIONSCALE", 1);
  //mpt->AddConstProperty("RAYLEIGH", 36000.*cm);
  // check constants with the Aprile
  mpt->AddConstProperty("FASTTIMECONSTANT", 41*ns);
  // mpt->AddConstProperty("SLOWTIMECONSTANT", 27.*ns);
  //  LXe_mpt->AddConstProperty("SLOWTIMECONSTANT",40.*ns);
  //  LXe_mpt->AddConstProperty("ELTIMECONSTANT", 50.*ns);
  // mpt->AddConstProperty("YIELDRATIO", 1.);

  G4double energy[2] = {0.01*eV, 100.*eV};

  G4double abslen[2] = {1.e8*m, 1.e8*m};
  mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::FakeLYSO()
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

  const G4int ri_entries = 9;
  G4double ri_energy[ri_entries] = {1.9074*eV, 2.2708*eV, 2.4028*eV, 2.5511*eV, 2.6895*eV, 2.8437*eV, 2.9520*eV, 3.0613*eV, 3.2542*eV};
  //G4double rindex[ri_entries] = { 1.802, 1.806, 1.810, 1.813, 1.818, 1.822, 1.827, 1.833, 1.842};
  G4double rindex[ri_entries] = { 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8};

  mpt->AddProperty("RINDEX", ri_energy, rindex, ri_entries);

  G4double energy[2] = {0.01*eV, 100.*eV};
  // G4double abslen[2] = {41.2*cm, 41.2*cm};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  mpt->AddProperty("ABSLENGTH", energy, abslen, 2);

  return mpt;
}

G4MaterialPropertiesTable* OpticalMaterialProperties::ReflectantSurface(G4double reflectivity)
{
  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
  G4cout << "Surface reflectivity = " << reflectivity << G4endl;
  const G4int REFL_NUMENTRIES = 2;

  G4double ENERGIES[REFL_NUMENTRIES] = {1.0*eV, 30.*eV};
  G4double REFLECTIVITY[REFL_NUMENTRIES] = {reflectivity, reflectivity};
  G4double specularlobe[REFL_NUMENTRIES] = {0., 0.}; // specular reflection about the normal to a
  //microfacet. Such a vector is chosen according to a gaussian distribution with
  //sigma = SigmaAlhpa (in rad) and centered in the average normal.
  G4double specularspike[REFL_NUMENTRIES] = {0., 0.}; // specular reflection about the average normal
  G4double backscatter[REFL_NUMENTRIES] = {0., 0.}; //180 degrees reflection
  // 1 - the sum of these three last parameters is the percentage of Lambertian reflection

  mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, REFL_NUMENTRIES);
  mpt->AddProperty("SPECULARLOBECONSTANT",ENERGIES,specularlobe,REFL_NUMENTRIES);
  mpt->AddProperty("SPECULARSPIKECONSTANT",ENERGIES,specularspike,REFL_NUMENTRIES);
  mpt->AddProperty("BACKSCATTERCONSTANT",ENERGIES,backscatter,REFL_NUMENTRIES);

  return mpt;
}
