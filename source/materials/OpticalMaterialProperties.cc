// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//           <francesc.monrabal@ific.uv.es>
//           <paola.ferrario@ific.uv.es>
//  Created: 17 August 2009
//
//  Copyright (c) 2009-2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "OpticalMaterialProperties.h"
#include "XenonGasProperties.h"
#include "XenonGasProperties.h"
#include "SellmeierEquation.h"
#include <G4MaterialPropertiesTable.hh>

using namespace nexus;



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
  
  mpt->AddProperty("RINDEX", energy, rindex, 2);

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


  G4double wlmin =  150. * nm;
  G4double wlmax = 1000. * nm;
  G4double step  =   10. * nm;

  const G4int entries = G4int((wlmax-wlmin)/step);
  
  G4double energy[entries];
  G4double rindex[entries];

  for (G4int i=0; i<entries; i++) {

    G4double wl = wlmin + i*step;
    rindex[i] = seq.RefractiveIndex(wl);
    
  }

  const G4int ABS_NUMENTRIES = 2;
  G4double Energies[ABS_NUMENTRIES] = {1*eV, 8.*eV};
  G4double SAPPHIRE_ABSL[ABS_NUMENTRIES] = {100.*cm, 100.*cm};
  
  mpt->AddProperty("RINDEX", energy, rindex, entries);
  mpt->AddProperty("ABSLENGTH", Energies, SAPPHIRE_ABSL, ABS_NUMENTRIES); 

  
  return mpt;
}





G4MaterialPropertiesTable* OpticalMaterialProperties::GXe(G4double pressure, 
							  G4double temperature,
							  G4int sc_yield)
{
  XenonGasProperties GXe_prop(pressure, temperature);
  G4MaterialPropertiesTable* GXe_mpt = new G4MaterialPropertiesTable();

  const G4int ri_entries = 8;
  G4double ri_energy[ri_entries] 
    = {1*eV, 2*eV, 3*eV, 4*eV, 5*eV, 6*eV, 7*eV, 8*eV};

  G4double rindex[ri_entries];

  for (G4int i=0; i<ri_entries; i++) {
    rindex[i] = GXe_prop.RefractiveIndex(ri_energy[i]);
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
  GXe_mpt->AddConstProperty("FASTTIMECONSTANT",1.*ns);
  GXe_mpt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
  GXe_mpt->AddConstProperty("ELTIMECONSTANT", 50.*ns);
  GXe_mpt->AddConstProperty("YIELDRATIO",.9);
  GXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);
  
  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  GXe_mpt->AddProperty("ABSLENGTH", energy, abslen, 2);
 
  return GXe_mpt;
}


G4MaterialPropertiesTable* 
OpticalMaterialProperties::FakeGrid(G4double pressure, G4double temperature,
				    G4double transparency, G4double thickness,
				    G4int sc_yield)
{
  XenonGasProperties GXe_prop(pressure, temperature);
  G4MaterialPropertiesTable* FGrid_mpt = new G4MaterialPropertiesTable();

  const G4int ri_entries = 4;
  G4double ri_energy[ri_entries] = {0.01*eV, 1*eV, 10*eV, 100*eV};
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
  FGrid_mpt->AddConstProperty("FASTTIMECONSTANT",1.*ns);
  FGrid_mpt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
  FGrid_mpt->AddConstProperty("YIELDRATIO",.9);
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


G4MaterialPropertiesTable* OpticalMaterialProperties::TPB()
{
  /// This is the simulation of the optical properties of TPB (tetraphenyl butadiene)
  /// a wavelength shifter which allows to converts VUV photons to blue photons.
  /// A WLS material is characterized by its photon absorption and photon emission spectrum
  /// and by a possible time delay between the absorption and re-emission of the photon.

  G4MaterialPropertiesTable* tpb_mpt = new G4MaterialPropertiesTable();
    
  const G4int RIN_NUMENTRIES  = 5;
  const G4int ABSL_NUMENTRIES = 5;
  const G4int WLSABSL_NUMENTRIES = 7;
  const G4int EMISSION_NUMENTRIES = 335;

  G4double Energies[RIN_NUMENTRIES] = {0.1*eV, 1.1*eV, 5.*eV, 10.*eV, 100.*eV};
  G4double TPB_RIND[ABSL_NUMENTRIES]  = {1.3, 1.3, 1.3, 1.3, 1.3};

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

  // Option 1 (simpler): the emission spectrum (WLSCOMPONENT property) is peaked in 2.97 eV (~ 430 nm).
  // G4double WLS_Emission_Energies[ABSL_NUMENTRIES] = {0.1*eV, 2.9*eV, 2.97*eV, 3.*eV, 100.*eV};
  // G4double TPB_Emission[ABSL_NUMENTRIES] = {0., 0., 1., 0., 0.};

  // Option 2: simulate the measured emission spectrum of TPB (reference: David Lorca)
  G4double WLS_Emission_Energies[EMISSION_NUMENTRIES] =  {1.9683831928*eV,  1.97070240354*eV,  1.97302721459*eV,  1.97535763995*eV,  1.97769370629*eV,  1.98003543095*eV,  1.98238283448*eV,  1.9847359344*eV,  1.98709475781*eV,  1.98945932243*eV,  1.99182964923*eV,  1.99420575928*eV,  1.99658767377*eV,  1.99897541398*eV,  2.00136900131*eV,  2.00376845724*eV,  2.00617380661*eV,  2.00858506466*eV,  2.01100225643*eV,  2.01342540711*eV,  2.01585453219*eV,  2.01828965707*eV,  2.0207308073*eV,  2.02317799865*eV,  2.02563126018*eV,  2.02809060785*eV,  2.03055607099*eV,  2.03302766909*eV,  2.03550542172*eV,  2.03798935859*eV,  2.04047949951*eV,  2.04297586775*eV,  2.04547848668*eV,  2.0479873798*eV,  2.05050257071*eV,  2.05302408316*eV,  2.05555194099*eV,  2.05808616818*eV,  2.06062678882*eV,  2.06317382711*eV,  2.0657273074*eV,  2.0682872576*eV,  2.0708536954*eV,  2.07342664895*eV,  2.07600614655*eV,  2.07859220623*eV,  2.08118486004*eV,  2.0837841262*eV,  2.08639003707*eV,  2.0890026111*eV,  2.09162188092*eV,  2.09424786877*eV,  2.0968806005*eV,  2.09952009857*eV,  2.10216639621*eV,  2.10481951615*eV,  2.10747948478*eV,  2.11014632865*eV,  2.11282007444*eV,  2.11550074896*eV,  2.11818837918*eV,  2.12088299218*eV,  2.12358461884*eV,  2.12629327928*eV,  2.12900900464*eV,  2.13173182259*eV,  2.13446176462*eV,  2.13719885133*eV,  2.13994311819*eV,  2.14269458607*eV,  2.14545329077*eV,  2.14821925342*eV,  2.15099251015*eV,  2.15377308235*eV,  2.1565610065*eV,  2.15935630803*eV,  2.16215901624*eV,  2.16496915681*eV,  2.16778676693*eV,  2.17061187259*eV,  2.17344450374*eV,  2.17628469048*eV,  2.17913246305*eV,  2.18198785189*eV,  2.18485088756*eV,  2.18772160469*eV,  2.19060002647*eV,  2.19348618784*eV,  2.19638012002*eV,  2.19928185834*eV,  2.2021914266*eV,  2.20510886435*eV,  2.20803419569*eV,  2.21096746057*eV,  2.2139086834*eV,  2.2168579045*eV,  2.2198151486*eV,  2.22278045643*eV,  2.22575385701*eV,  2.2287353835*eV,  2.23172506523*eV,  2.23472294374*eV,  2.23772904875*eV,  2.24074341414*eV,  2.24376607401*eV,  2.2467970626*eV,  2.24983641439*eV,  2.25288416403*eV,  2.25594034635*eV,  2.2590050005*eV,  2.26207815349*eV,  2.26515984484*eV,  2.26825011434*eV,  2.2713489895*eV,  2.27445651049*eV,  2.27757271771*eV,  2.28069763922*eV,  2.28383132004*eV,  2.28697378858*eV,  2.29012509032*eV,  2.29328525829*eV,  2.29645432996*eV,  2.29963233871*eV,  2.30281933097*eV,  2.30601534055*eV,  2.30922040579*eV,  2.3124345652*eV,  2.31565785754*eV,  2.31889032176*eV,  2.32213199706*eV,  2.32538292286*eV,  2.32864313879*eV,  2.33191268472*eV,  2.33519160514*eV,  2.33847993159*eV,  2.34177770901*eV,  2.34508498262*eV,  2.34840178459*eV,  2.35172816061*eV,  2.35506415665*eV,  2.3584098055*eV,  2.36176515812*eV,  2.36513025226*eV,  2.36850512585*eV,  2.37188983066*eV,  2.37528440515*eV,  2.37868889251*eV,  2.38210333166*eV,  2.38552777546*eV,  2.38896226332*eV,  2.39240683949*eV,  2.39586154846*eV,  2.39932643499*eV,  2.40280154412*eV,  2.40628692581*eV,  2.40978261628*eV,  2.41328866602*eV,  2.41680512114*eV,  2.42033203274*eV,  2.42386943803*eV,  2.42741738865*eV,  2.43097593658*eV,  2.4345451198*eV,  2.43812499562*eV,  2.44171560254*eV,  2.44531699851*eV,  2.44892922738*eV,  2.45255233322*eV,  2.45618637498*eV,  2.45983139732*eV,  2.46348745008*eV,  2.4671545834*eV,  2.47083284278*eV,  2.47452228879*eV,  2.47822296757*eV,  2.48193493546*eV,  2.48565823421*eV,  2.48939292075*eV,  2.49313904741*eV,  2.4968966668*eV,  2.50066583188*eV,  2.504446601*eV,  2.50823901764*eV,  2.51204314081*eV,  2.51585902986*eV,  2.51968672922*eV,  2.52352630405*eV,  2.52737779939*eV,  2.53124128116*eV,  2.53511680019*eV,  2.53900440762*eV,  2.5429041705*eV,  2.54681614069*eV,  2.55074037558*eV,  2.55467693292*eV,  2.55862587083*eV,  2.56258724248*eV,  2.56656111731*eV,  2.57054755458*eV,  2.57454660325*eV,  2.57855832861*eV,  2.58258279101*eV,  2.58662005119*eV,  2.59067017028*eV,  2.59473321521*eV,  2.59880923706*eV,  2.60289830354*eV,  2.60700048285*eV,  2.61111582712*eV,  2.61524441087*eV,  2.619386287*eV,  2.62354153088*eV,  2.62771020176*eV,  2.63189235921*eV,  2.63608807998*eV,  2.64029742452*eV,  2.64452045927*eV,  2.64875724546*eV,  2.65300786169*eV,  2.65727237008*eV,  2.66155083883*eV,  2.66584333659*eV,  2.67014993245*eV,  2.67447069595*eV,  2.67880570291*eV,  2.68315501223*eV,  2.68751870062*eV,  2.69189683948*eV,  2.69628950656*eV,  2.7006967625*eV,  2.7051186919*eV,  2.70955535629*eV,  2.71400683537*eV,  2.71847320941*eV,  2.72295454729*eV,  2.72745091832*eV,  2.7319624103*eV,  2.73648909356*eV,  2.74103103889*eV,  2.74558833575*eV,  2.75016105597*eV,  2.75474927797*eV,  2.75935308069*eV,  2.76397254361*eV,  2.76860774675*eV,  2.77325877067*eV,  2.77792569649*eV,  2.78260860589*eV,  2.7873075811*eV,  2.7920227112*eV,  2.79675406701*eV,  2.80150173874*eV,  2.80626581726*eV,  2.811046375*eV,  2.81584350398*eV,  2.82065729692*eV,  2.8254878279*eV,  2.83033519727*eV,  2.83519948671*eV,  2.84008077847*eV,  2.84497917486*eV,  2.84989475937*eV,  2.85482762263*eV,  2.85977784926*eV,  2.86474554433*eV,  2.8697307938*eV,  2.87473369087*eV,  2.87975432938*eV,  2.88479280386*eV,  2.8898492095*eV,  2.89492364216*eV,  2.90001620519*eV,  2.90512698228*eV,  2.91025607815*eV,  2.91540359144*eV,  2.92056962838*eV,  2.9257542753*eV,  2.9309576468*eV,  2.93617983055*eV,  2.9414209427*eV,  2.94668107232*eV,  2.95196033714*eV,  2.95725883468*eV,  2.96257666314*eV,  2.96791394269*eV,  2.97327077308*eV,  2.97864726192*eV,  2.98404351041*eV,  2.98945964208*eV,  2.99489575977*eV,  3.00035197428*eV,  3.00582839724*eV,  3.01132514111*eV,  3.01684232652*eV,  3.02238005295*eV,  3.02793844269*eV,  3.03351761161*eV,  3.03911768385*eV,  3.04473876214*eV,  3.05038097237*eV,  3.05604444145*eV,  3.0617292746*eV,  3.06743560809*eV,  3.07316354886*eV,  3.07891323515*eV,  3.08468477564*eV,  3.09047831055*eV,  3.0962939581*eV,  3.1021318451*eV,  3.10799209933*eV,  3.11387484175*eV,  3.11978021771*eV,  3.12570835021*eV,  3.13165937098*eV,  3.13763341284*eV,  3.14363060959*eV,  3.14965109608*eV,  3.15569501627*eV,  3.16176249106*eV,  3.16785366651*eV,  3.17396868173*eV,  3.18010768508*eV,  3.18627080157*eV,  3.19245818177*eV,  3.19866997746*eV,  3.20490631685*eV,  3.21116736229*eV,  3.21745325252*eV,  3.2237641273*eV,  3.2301001527*eV,  3.23646147089*eV,  3.2428482251*eV,  3.24926058522*eV,  3.25569869695*eV,  3.26216271572*eV};

  G4double TPB_Emission[EMISSION_NUMENTRIES] = {78.8672186773 ,  79.4270475529 ,  79.9925337766 ,  80.5637483348 ,  81.1407663859 ,  81.7236619053 ,  82.3125107789 ,  82.9073892499 ,  83.5083771566 ,  84.1155531392 ,  84.7289978619 ,  85.348793236 ,  85.9750224416 ,  86.6077699502 ,  87.2471215472 ,  87.8931643548 ,  88.5459877403 ,  89.2056798113 ,  89.8723327192 ,  90.5460400869 ,  91.2268942679 ,  91.9149918197 ,  92.6104308502 ,  93.3133081894 ,  94.0237260017 ,  94.7417842362 ,  95.4675883135 ,  96.2012424082 ,  96.9428523333 ,  97.6925286149 ,  98.4503805285 ,  99.2165201161 ,  99.991061224 ,  100.774119535 ,  101.565812603 ,  102.366259884 ,  103.175582774 ,  103.993904638 ,  104.821350855 ,  105.658048846 ,  106.504128113 ,  107.359721439 ,  108.224960302 ,  109.099981825 ,  109.984925423 ,  110.879929209 ,  111.785138406 ,  112.700695752 ,  113.626751294 ,  114.563452569 ,  115.510954619 ,  116.469411254 ,  117.438980121 ,  118.419820171 ,  119.412097005 ,  120.415974998 ,  121.431622621 ,  122.459211172 ,  123.498914832 ,  124.550910717 ,  125.615378931 ,  126.692502622 ,  127.782469509 ,  128.885466064 ,  130.001686357 ,  131.131326276 ,  132.274586583 ,  133.431666817 ,  134.602776154 ,  135.788121027 ,  136.987917788 ,  138.202380023 ,  139.431731522 ,  140.676193285 ,  141.935996813 ,  143.211372518 ,  144.502556466 ,  145.809787018 ,  147.133311952 ,  148.473377996 ,  149.830237915 ,  151.204148834 ,  152.595372322 ,  154.00417447 ,  155.430825972 ,  156.875604158 ,  158.338785297 ,  159.820656316 ,  161.321507154 ,  162.841634832 ,  164.381335346 ,  165.940918118 ,  167.520689539 ,  169.120969819 ,  170.742076109 ,  172.384339788 ,  174.04808914 ,  175.733667097 ,  177.441415772 ,  179.171685463 ,  180.924830165 ,  182.701217117 ,  184.501212784 ,  186.325192346 ,  188.173537458 ,  190.046636365 ,  191.944883999 ,  193.868682094 ,  195.81843929 ,  197.794573911 ,  199.797503439 ,  201.827660523 ,  203.885485305 ,  205.971417088 ,  208.085911002 ,  210.229430011 ,  212.402436229 ,  214.605411249 ,  216.838832946 ,  219.103199217 ,  221.399007232 ,  223.726765485 ,  226.08698774 ,  228.480205837 ,  230.906950821 ,  233.367765655 ,  235.863202187 ,  238.393821247 ,  240.960192752 ,  243.562895652 ,  246.202518688 ,  248.879659287 ,  251.594924651 ,  254.348935236 ,  257.142310028 ,  259.97568875 ,  262.84972111 ,  265.765055075 ,  268.722360025 ,  271.722315514 ,  274.765599011 ,  277.852914204 ,  280.984962651 ,  284.162455756 ,  287.386127816 ,  290.656710497 ,  293.974949779 ,  297.341597176 ,  300.757427788 ,  304.223213222 ,  307.739739484 ,  311.307802429 ,  314.928207669 ,  318.601770457 ,  322.32932058 ,  326.111682185 ,  329.949703526 ,  333.844236892 ,  337.796148658 ,  341.806297817 ,  345.875567415 ,  350.00484874 ,  354.195024064 ,  358.447005126 ,  362.761687888 ,  367.139997584 ,  371.582848052 ,  376.091158122 ,  380.665869514 ,  385.307910044 ,  390.018216972 ,  394.797730289 ,  399.647385397 ,  404.568138286 ,  409.560925729 ,  414.626696955 ,  419.766378984 ,  424.980916121 ,  430.271241768 ,  435.638283853 ,  441.082963451 ,  446.606200747 ,  452.208883839 ,  457.89191134 ,  463.65616894 ,  469.502503923 ,  475.431777324 ,  481.444799303 ,  487.542390229 ,  493.725322324 ,  499.994340674 ,  506.350186142 ,  512.793542252 ,  519.325065515 ,  525.945373883 ,  532.655043335 ,  539.454595135 ,  546.344528448 ,  553.325271233 ,  560.397174798 ,  567.560556207 ,  574.815656423 ,  582.162644594 ,  589.601612823 ,  597.132580787 ,  604.755449629 ,  612.470056184 ,  620.276137004 ,  628.173289873 ,  636.161039997 ,  644.238748371 ,  652.405689323 ,  660.660967335 ,  669.003540418 ,  677.432245223 ,  685.945721093 ,  694.542444879 ,  703.22069826 ,  711.978603751 ,  720.81404445 ,  729.724698993 ,  738.70801837 ,  747.761213833 ,  756.881244377 ,  766.064816063 ,  775.308319586 ,  784.607889923 ,  793.959343226 ,  803.35818695 ,  812.799554664 ,  822.278278477 ,  831.788772671 ,  841.325105918 ,  850.880947168 ,  860.449523432 ,  870.023628318 ,  879.595643354 ,  889.157444307 ,  898.700422043 ,  908.215501773 ,  917.693056628 ,  927.122932851 ,  936.494424293 ,  945.796256151 ,  955.016569193 ,  964.142904644 ,  973.162189844 ,  982.060724851 ,  990.82417017 ,  999.437547126 ,  1007.88518273 ,  1016.15077078 ,  1024.21733053 ,  1032.06717205 ,  1039.68195956 ,  1047.04267948 ,  1054.12961718 ,  1060.92243274 ,  1067.40010591 ,  1073.54098219 ,  1079.32281583 ,  1084.72274598 ,  1089.71736024 ,  1094.28271659 ,  1098.39440919 ,  1102.02758042 ,  1105.15700129 ,  1107.75712775 ,  1109.80217116 ,  1111.26617684 ,  1112.12311108 ,  1112.3469569 ,  1111.91181867 ,  1110.79203842 ,  1108.96231783 ,  1106.39784891 ,  1103.07447245 ,  1098.96880516 ,  1094.05843869 ,  1088.32206874 ,  1081.73972961 ,  1074.29292394 ,  1065.96488013 ,  1056.74073752 ,  1046.6077167 ,  1035.55541044 ,  1023.57595678 ,  1010.66429133 ,  996.818300194 ,  982.039143653 ,  966.331404999 ,  949.703316201 ,  932.166955789 ,  913.73842396 ,  894.438078215 ,  874.290566207 ,  853.325037376 ,  831.575187691 ,  809.079442668 ,  785.880814965 ,  762.026978368 ,  737.570315301 ,  712.567594712 ,  687.080082873 ,  661.173076672 ,  634.915916921 ,  608.38139962 ,  581.645646519 ,  554.787535581 ,  527.888247686 ,  501.030758749 ,  474.29910135 ,  447.777934822 ,  421.551716285 ,  395.704014793 ,  370.316762916 ,  345.469491128 ,  321.238524254 ,  297.696343136 ,  274.910644311 ,  252.943754521 ,  231.851902231 ,  211.68472034 ,  192.484544663 ,  174.286057558 ,  157.116008118 ,  140.992810359 ,  125.926588271 ,  111.919108355 ,  98.9638655246 ,  87.0464583784 ,  76.1449168541 ,  66.2301740124 ,  57.266771409 ,  49.2135156631};


  tpb_mpt->AddProperty("RINDEX", Energies, TPB_RIND, RIN_NUMENTRIES);
  tpb_mpt->AddProperty("WLSABSLENGTH", WLS_Abs_Energies, TPB_WLSABSL, WLSABSL_NUMENTRIES); 
  tpb_mpt->AddProperty("WLSCOMPONENT", WLS_Emission_Energies, TPB_Emission, EMISSION_NUMENTRIES);
  tpb_mpt->AddConstProperty("WLSTIMECONSTANT",1.*ns);
    
  return tpb_mpt;
}


G4MaterialPropertiesTable* OpticalMaterialProperties::PS()
{
  G4MaterialPropertiesTable* PS_mpt = new G4MaterialPropertiesTable();
     
  const G4int RIN_NUMENTRIES  = 3;
  const G4int Emission_NUMENTRIES = 7;
  const G4int WLS_ABS_NUMENTRIES = 5;
  const G4int ABS_NUMENTRIES = 7;

  //extrapolated from http://refractiveindex.info/?group=PLASTICS&material=PS
     
  G4double Energy[RIN_NUMENTRIES] = {1*eV, 3.1*eV, 7.3*eV};
  G4double PS_RIND[RIN_NUMENTRIES]  = {1.57, 1.62, 1.87};


  //    G4double PS_ABSL[RIN_NUMENTRIES] = {100.*m, 100.*m};

  G4double PS_Emission_Energies[Emission_NUMENTRIES] = {2.5*eV, 3.*eV, 3.7*eV, 3.8*eV, 3.9*eV, 4.*eV, 5.*eV};
  G4double PS_Emission[Emission_NUMENTRIES] = {0., 0., 0., 1., 0., 0., 0.};

  // from Lally et al., Nucl. Instr. and Meth.  Volume 117, Issue 4, 1 October 1996, Pages 421-427 

  G4double PS_WSLABS_Energies[WLS_ABS_NUMENTRIES] = {2.*eV, 7.*eV, 7.2*eV, 7.5*eV, 8*eV};
  //    G4double PS_WLSABS[WLS_ABS_NUMENTRIES] = {1000.*cm, 1000.*cm,  0.014*cm, 1000.*cm, 1000.*cm};  // quantum efficiency of 99.9%
  //    G4double PS_WLSABS[WLS_ABS_NUMENTRIES] = {1000.*cm, 1000.*cm, 0.448142*cm, 1000.*cm, 1000.*cm};  // quantum efficiency of 20% 
  G4double PS_WLSABS[WLS_ABS_NUMENTRIES] = {1000.*cm, 1000.*cm, .0217147*cm, 1000.*cm, 1000.*cm};  // quantum efficiency of 99%
  G4double PS_ABS_Energies[ABS_NUMENTRIES] = {2.5*eV, 3.6*eV, 3.7*eV, 3.8*eV, 3.9*eV, 7.2*eV, 8*eV};
  // G4double PS_ABS[ABS_NUMENTRIES] = {1000.*cm, 1000.*cm,  0.14427*cm, 0.14427*cm,  0.14427*cm, 0.0621335*cm, 1000.*cm}; // absorption of 50%
  //    G4double PS_ABS[ABS_NUMENTRIES] = {1000.*cm, 1000.*cm, 0.04343*cm, 0.04343*cm, 0.04343*cm, 0.0621335*cm, 1000.*cm}; // absorption of 90% a 3.8 eV e 80% a 7.2 eV  
  //    G4double PS_ABS[ABS_NUMENTRIES] = {1000.*cm, 1000.*cm, 0.621335*cm, 0.621335*cm, 0.621335*cm, .144765*cm, 1000.*cm}; // absorption of 80% (a 10 mm) a 3.8 eV e 99.9% (for 10 mm) a 7.2 eV según Lally 
  G4double PS_ABS[ABS_NUMENTRIES] = {1000.*cm, 1000.*cm, 1800.*cm,  1800.*cm,  1800.*cm, 0.006*cm, 1000.*cm};// absorption of 85% for 3.8 and 99.9999999 % for 7.2 
  //  G4double PS_ABS[ABS_NUMENTRIES] = {1000.*cm, 1000.*cm, 0.04343*cm, 0.04343*cm, 0.04343*cm, 0.140184*cm, 1000.*cm}; // absorption of 90% a 3.8 eV e 51% a 7.2 eV
  //    G4double PS_ABS[ABS_NUMENTRIES] = {1000.*cm, 0.14427*cm, 0.02172*cm, 0.02172*cm, 0.02172*cm, 0.02172*cm}; // absorption of 99% until 3.7*eV, then decreases
  // G4double PS_ABS[ABS_NUMENTRIES] = {1000.*cm, 1000.*cm, 0.949122*cm, 1000.*cm, 1000.*cm}; absorption of 10%
  //G4double PS_ABS[ABS_NUMENTRIES] = {1000.*cm, 1000.*cm, 9.94992*cm, 1000.*cm, 1000.*cm}; // absorption of 1% 0.448142


  PS_mpt->AddProperty("RINDEX", Energy, PS_RIND, RIN_NUMENTRIES);
  PS_mpt->AddProperty("WLSABSLENGTH", PS_WSLABS_Energies, PS_WLSABS, WLS_ABS_NUMENTRIES); 
  PS_mpt->AddProperty("ABSLENGTH", PS_ABS_Energies, PS_ABS, ABS_NUMENTRIES); 
  PS_mpt->AddProperty("WLSCOMPONENT", PS_Emission_Energies, PS_Emission, Emission_NUMENTRIES);
  PS_mpt->AddConstProperty("WLSTIMECONSTANT",0.5*ns);
    
  //  G4double abs1 = PS_mpt->GetProperty("ABSLENGTH")->GetProperty(7.21*eV);
    
  return PS_mpt;
}



///Properties of Scintillation TPH form 
///http://omlc.ogi.edu/spectra/PhotochemCAD/html/p-terphenyl.html
G4MaterialPropertiesTable* OpticalMaterialProperties::TPH()
{
  G4MaterialPropertiesTable* TPH_mpt = new G4MaterialPropertiesTable();
    
  const G4int SCIN_NUMENTRIES = 7;
  const G4int RIN_NUMENTRIES  = 2;
  const G4int Emission_NUMENTRIES = 7;
  const G4int ABS_NUMENTRIES = 5;

  G4double Energy[RIN_NUMENTRIES] = {0.1*eV, 100.*eV};
  G4double TPH_RIND[RIN_NUMENTRIES]  = {1.65, 1.65};

  //    G4double TPH_ABSL[RIN_NUMENTRIES] = {100.*m, 100.*m};

  G4double TPH_Emission_Energies[Emission_NUMENTRIES] = {2.5*eV, 3.*eV, 3.7*eV, 3.8*eV, 3.9*eV, 4.*eV, 5.*eV};
  G4double TPH_Emission[Emission_NUMENTRIES] = {0., 0., 0., 1., 0., 0., 0.};

  G4double TPH_WSLABS_Energies[ABS_NUMENTRIES] = {2.*eV, 7.*eV, 7.2*eV, 7.5*eV, 8*eV};
  G4double TPH_WLSABS[ABS_NUMENTRIES] = {1000.*cm, 1000.*cm, 0.0177*cm, 1000.*cm, 1000.*cm};


  TPH_mpt->AddProperty("RINDEX", Energy, TPH_RIND, RIN_NUMENTRIES);
  TPH_mpt->AddProperty("WLSABSLENGTH", TPH_WSLABS_Energies, TPH_WLSABS, ABS_NUMENTRIES); 
  //    TPH_mpt->AddProperty("ABSLENGTH", Energy, TPH_ABSL, RIN_NUMENTRIES); 
  TPH_mpt->AddProperty("WLSCOMPONENT", TPH_Emission_Energies, TPH_Emission, Emission_NUMENTRIES);
  TPH_mpt->AddConstProperty("WLSTIMECONSTANT",0.5*ns);

  return TPH_mpt;
}







///Properties of WLS BC480  
///Ref: Optical properties of wavelength shifting panels. P .Soler and Z.H.Wang School ofPhysics, Unioersity of Sydney, Sydney, NSW, 2006, Australia
///Saint Gobain crystals
G4MaterialPropertiesTable* OpticalMaterialProperties::BC480()
{
  G4MaterialPropertiesTable* bc480_mpt = new G4MaterialPropertiesTable();
    
  const G4int SCIN_NUMENTRIES = 7;
  const G4int RIN_NUMENTRIES  = 30;
  const G4int Emission_NUMENTRIES = 5;
  const G4int WLS_ABS_NUMENTRIES = 7;
  const G4int ABS_NUMENTRIES = 9;

  G4double Energy[RIN_NUMENTRIES] =
    {0.1 *eV, 1.5488*eV, 1.5695*eV, 1.6288*eV, 1.7157*eV, 1.7851*eV, 1.8700*eV, 1.9635*eV, 2.0620*eV, 2.1498*eV, 
     2.2540*eV, 2.3911*eV, 2.5645*eV, 2.7564*eV, 2.9396*eV, 3.1210*eV, 3.3965*eV, 3.7095*eV, 3.9230*eV, 4.1526*eV, 
     4.3247*eV, 4.5000*eV, 4.6412*eV, 4.8173*eV, 5.1378*eV, 5.3703*eV, 5.5882*eV, 5.8445*eV, 6.1252*eV, 100.*eV};
  G4double BC480_RIND[RIN_NUMENTRIES]  = 
    {1.45025, 1.45025, 1.45075, 1.45104, 1.45208, 1.45310, 1.45438, 1.45541, 1.45718, 1.45820, 
     1.45972, 1.46198, 1.46500, 1.46875, 1.47250, 1.47624, 1.48247, 1.49018, 1.49614, 1.50285, 
     1.50781, 1.51352, 1.51774, 1.52369, 1.53238, 1.54031, 1.55220, 1.56162, 1.57203, 1.572};
    
  G4double BC480_Emission_Energies[Emission_NUMENTRIES] = {1.5*eV, 2.95*eV, 2.97*eV, 2.99*eV, 8.*eV};
  G4double BC480_Emission[Emission_NUMENTRIES] = {0., 0., 1., 0., 0.};

  G4double BC480_Absorption_Energies[WLS_ABS_NUMENTRIES] = {2.*eV, 3.6*eV, 3.7*eV, 3.8*eV, 3.9*eV, 4.*eV, 8.*eV};
  // G4double BC480_WLSABSL[ABS_NUMENTRIES]  = 
  //   {1000*cm, 1000*cm, 0.1097*cm, 0.1097*cm, 0.1097*cm, 1000*cm};
  G4double BC480_WLSABSL[WLS_ABS_NUMENTRIES]  = 
    {1000*cm, 1000*cm, 1.01912*cm,1.01912*cm,1.01912*cm, 1000*cm, 1000*cm};
  G4double BC480_Abs_Energies[ABS_NUMENTRIES]={1.*eV, 2.*eV, 2.9*eV, 3.4*eV ,3.7*eV, 3.8*eV, 3.9*eV, 4.*eV, 8.*eV};
  //    G4double BC480_ABSL[ABS_NUMENTRIES]={400.*cm, 400.*cm, 400*cm, 400.*cm, 0.259853*cm, 0.259853*cm,0.259853*cm, .01*cm, .001*cm};
  G4double BC480_ABSL[ABS_NUMENTRIES]={1000.*cm, 2500.*cm, 2500*cm, 2500.*cm, 0.237438*cm, 0.237438*cm, 0.237438*cm, .01*cm, .001*cm};

  //    G4cout << "Energia: " << h_Planck*c_light/(900*0.000001) << G4endl;
  // G4cout << "Wavelength per 7.2 eV (mm): " << h_Planck*c_light/(7.2*0.000001) << G4endl;
  // G4cout << "Wavelength per 3.8 eV (mm): " << h_Planck*c_light/(3.8*0.000001) << G4endl;
  // G4cout << "Wavelength per 2.9 eV (mm): " << h_Planck*c_light/(2.9*0.000001) << G4endl;

   
  // for(int i = 0; i<ABSL_NUMENTRIES; i++){
  //   BC480_ABSL[i] = BC480_ABSL[i]*100000.;
  // }

  // for (int i=0;i<ABSL_NUMENTRIES;i++){
  //   G4cout << h_Planck*c_light/BC480_Absorption_Energies[i]*1000000. << ", ";
  // }
  // G4cout << h_Planck*c_light/(417*0.000001) ;
  //  G4cout << G4endl;
  // for (int i=0;i<ABSL_NUMENTRIES;i++){
  //   G4cout << 1/(BC480_WLSABSL[i]*0.1) << ", ";
  // }

  // for (int i=0;i<ABSL_NUMENTRIES;i++){
  //   G4cout << "{" << h_Planck*c_light/BC480_Emission_Energies[i]*1000000 << "," <<  BC480_Emission[i]  << "},";
  // }

    
  bc480_mpt->AddProperty("RINDEX", Energy, BC480_RIND, RIN_NUMENTRIES);
  bc480_mpt->AddProperty("WLSABSLENGTH", BC480_Absorption_Energies, BC480_WLSABSL, WLS_ABS_NUMENTRIES); 
  bc480_mpt->AddProperty("ABSLENGTH",  BC480_Abs_Energies, BC480_ABSL, ABS_NUMENTRIES); 
  bc480_mpt->AddProperty("WLSCOMPONENT", BC480_Emission_Energies, BC480_Emission, Emission_NUMENTRIES);
  bc480_mpt->AddConstProperty("WLSTIMECONSTANT",0.5*ns);

  //  G4double abs = bc480_mpt->GetProperty("RINDEX")->GetProperty(2.9*eV);
  //  G4cout << "R index for 2.9 eV in bicron: " << abs << G4endl;
  return bc480_mpt;
}

