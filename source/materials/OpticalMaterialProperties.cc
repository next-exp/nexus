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
    {1.54, 1.54*eV};
  
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
							  G4double temperature)
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
  GXe_mpt->AddProperty("SLOWCOMPONENT", sc_energy, intensity, sc_entries);
  GXe_mpt->AddConstProperty("SCINTILLATIONYIELD", 10000./MeV);
  GXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
  GXe_mpt->AddConstProperty("FASTTIMECONSTANT",1.*ns);
  GXe_mpt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
  GXe_mpt->AddConstProperty("YIELDRATIO",.9);
  GXe_mpt->AddConstProperty("ATTACHMENT", 1000.*ms);

  
  G4double energy[2] = {0.01*eV, 100.*eV};
  G4double abslen[2] = {1.e8*m, 1.e8*m};
  GXe_mpt->AddProperty("ABSLENGTH", energy, abslen, 2);
 
  return GXe_mpt;
}


G4MaterialPropertiesTable* 
OpticalMaterialProperties::FakeGrid(G4double pressure, G4double temperature,
				    G4double transparency, G4double thickness)
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
  FGrid_mpt->AddConstProperty("SCINTILLATIONYIELD", 10000./MeV); 
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
  G4double REFLECTIVITY[REFL_NUMENTRIES] = {.5, .5};

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
  G4double REFLECTIVITY[REFL_NUMENTRIES] = {.97, .97, .97, .5, .5, .5};

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

  // The emission spectrum (WLSCOMPONENT property) is peaked in 2.97 eV (~ 430 nm).
  G4double WLS_Emission_Energies[ABSL_NUMENTRIES] = {0.1*eV, 2.9*eV, 2.97*eV, 3.*eV, 100.*eV};
  G4double TPB_Emission[ABSL_NUMENTRIES] = {0., 0., 1., 0., 0.};

  tpb_mpt->AddProperty("RINDEX", Energies, TPB_RIND, RIN_NUMENTRIES);
  tpb_mpt->AddProperty("WLSABSLENGTH", WLS_Abs_Energies, TPB_WLSABSL, WLSABSL_NUMENTRIES); 
  tpb_mpt->AddProperty("WLSCOMPONENT", WLS_Emission_Energies, TPB_Emission, ABSL_NUMENTRIES);
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

