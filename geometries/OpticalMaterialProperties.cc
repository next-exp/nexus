// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J Martin-Albo <jmalbos@ific.uv.es>
//           F Monrabal <franmon4@ific.uv.es>
//           L. Serra   <sediaz@ific.uv.es>
//  Created: 17 August 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "OpticalMaterialProperties.h"

#include <G4MaterialPropertiesTable.hh>


namespace nexus {


  G4MaterialPropertiesTable* OpticalMaterialProperties::GXe(G4double pressure)
  {
    G4MaterialPropertiesTable* GXe_mpt = new G4MaterialPropertiesTable();
    
    const G4int SCIN_NUMENTRIES = 7;
    const G4int RIN_NUMENTRIES  = 27;
    const G4int ABSL_NUMENTRIES = 27;

    G4double Energy[RIN_NUMENTRIES] = 
      { 3.71*eV, 6.2*eV , 6.26*eV, 6.33*eV, 6.39*eV, 6.46*eV, 
 	6.53*eV, 6.59*eV, 6.67*eV, 6.74*eV, 6.81*eV, 6.89*eV, 
 	6.97*eV, 7.04*eV, 7.13*eV, 7.21*eV, 7.29*eV, 7.38*eV,
 	7.47*eV, 7.56*eV, 7.65*eV, 7.75*eV, 7.85*eV, 7.95*eV,
 	8.05*eV, 8.16*eV, 8.32*eV };

    G4double Scn_PP[SCIN_NUMENTRIES] = 
      { 6.2*eV, 6.97*eV, 7.29*eV, 7.51*eV, 7.75*eV, 8.16*eV, 8.27*eV};

    G4double Scn_FAST[SCIN_NUMENTRIES] = 
      {0., 0.13, 0.24, 0.26, 0.23, 0.14, 0.};
    
    G4double HPXe_RIND[RIN_NUMENTRIES]  = 
      { 1., 1., 1.,1.,1., 1., 1.,1., 1., 1.,1., 1., 1.,	1., 1., 
	1.,1., 1., 1.,1., 1., 1.,1., 1., 1., 1., 1.};

    G4double HPXe_ABSL[ABSL_NUMENTRIES]  = 
      { 35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm, 
	35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm, 
	35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm, 
	35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm, 
	35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm, 
	35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm,
	35000.*cm, 35000.*cm, 35000.*cm};


    //HPXe_mt->AddProperty("FASTCOMPONENT", Energy, HPXe_SCINT, NUMENTRIES);
    GXe_mpt->AddProperty("FASTCOMPONENT", Scn_PP, Scn_FAST, SCIN_NUMENTRIES);
    //HPXe_mt->AddProperty("SLOWCOMPONENT", Energy, HPXe_SCINT, NUMENTRIES);
    GXe_mpt->AddProperty("RINDEX",        Energy, HPXe_RIND, RIN_NUMENTRIES);
    GXe_mpt->AddProperty("ABSLENGTH",     Energy, HPXe_ABSL, RIN_NUMENTRIES);
    GXe_mpt->AddConstProperty("SCINTILLATIONYIELD", 100000./MeV); 
    GXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
    GXe_mpt->AddConstProperty("FASTTIMECONSTANT",1.*ns);
    GXe_mpt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
    GXe_mpt->AddConstProperty("YIELDRATIO",1.0);
    
    return GXe_mpt;
  }



  G4MaterialPropertiesTable* OpticalMaterialProperties::FusedSilica()
  {
    const G4int NUMENTRIES = 3;
    const G4int NUMENTRIES2 = 27;
    const G4int NUMENTRIES3 = 37;
    const G4int NUMENTRIES4 = 54;

    G4double Energy[NUMENTRIES] = { 5.0*eV , 7.07*eV, 8.32*eV };

    G4double Energy2[NUMENTRIES2] = { 3.71*eV, 6.2*eV , 6.26*eV, 6.33*eV, 6.39*eV, 6.46*eV, 6.53*eV, 6.59*eV, 6.67*eV, 6.74*eV, 6.81*eV, 6.89*eV, 6.97*eV, 7.04*eV , 7.13*eV, 7.21*eV, 7.29*eV, 7.38*eV, 7.47*eV, 7.56*eV, 7.65*eV, 7.75*eV, 7.85*eV, 7.95*eV, 8.05*eV, 8.16*eV, 8.32*eV };

    G4double Energy3[NUMENTRIES3] = { 4.17*eV, 4.39*eV, 4.63*eV, 4.79*eV, 5.08*eV, 5.91*eV, 6.37*eV , 6.63*eV, 6.84*eV, 6.91*eV, 6.98*eV, 7.1*eV , 7.16*eV, 7.22*eV, 7.27*eV, 7.32*eV, 7.35*eV, 7.41*eV, 7.45*eV, 7.48*eV, 7.51*eV, 7.54*eV, 7.55*eV, 7.6*eV, 7.63*eV, 7.6301*eV, 7.66*eV, 7.68*eV, 7.69*eV, 7.72*eV, 7.72*eV, 7.76*eV, 7.77*eV, 7.8*eV, 7.82*eV, 7.88, 8.32*eV };

    G4double Energy4[NUMENTRIES4] = {4.21*eV, 4.74*eV, 5.77*eV, 6.08*eV, 6.28*eV , 6.51*eV, 6.58*eV, 6.62*eV, 6.7*eV, 6.72*eV, 6.77*eV, 6.81*eV, 6.86*eV, 6.88*eV, 6.95*eV,  6.98*eV, 7.0*eV, 7.03*eV, 7.07*eV , 7.1*eV, 7.13*eV, 7.16*eV, 7.17*eV, 7.2*eV, 7.23*eV, 7.26*eV, 7.265*eV, 7.27*eV, 7.275*eV, 7.31*eV, 7.34*eV, 7.37*eV, 7.4*eV, 7.45*eV, 7.46*eV, 7.49*eV, 7.51*eV, 7.52*eV, 7.54*eV, 7.55*eV, 7.58*eV, 7.62*eV, 7.67*eV, 7.71*eV, 7.76*eV, 7.78*eV, 7.79*eV, 7.8*eV, 7.83*eV, 7.87, 7.92*eV, 7.95*eV, 8.27*eV };   


    
    G4double QUARTZ_SCINT[NUMENTRIES] = { 0.1, 0.1, 0.1 };

    G4double QUARTZ_RIND2[NUMENTRIES2]  = {1.47974, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.547};

    G4double QUARTZ_ABSL[NUMENTRIES]  = { 3.5*cm, 3.5*cm, 3.5*cm}; 


    G4double QUARTZ_ABSL3[NUMENTRIES3]  = { 11.55*cm, 10.86*cm, 10.97*cm, 10.30*cm, 10.07*cm, 9.07*cm, 8.88*cm, 8.67*cm, 8.47*cm, 8.46*cm, 8.45*cm, 8.25*cm, 7.82*cm, 7.820*cm, 7.25*cm, 6.45*cm, 5.80*cm, 5.07*cm, 4.28*cm, 3.78*cm, 3.21*cm, 2.52*cm, 2.11*cm, 1.82*cm, 1.55*cm, 1.27*cm, 1.11*cm, 1.02*cm, 0.89*cm, 0.76*cm, 0.67*cm, 0.56*cm, 0.47*cm,  0.40*cm, 0.3*cm, 0.0001*cm};

    G4double QUARTZ_ABSL4[NUMENTRIES4]  = {11.799*cm, 10.817*cm, 10.066*cm, 9.497*cm, 9.241*cm, 8.999*cm, 8.744*cm, 8.497*cm, 8.044*cm, 8.042*cm, 7.628*cm, 7.250*cm, 6.906*cm, 6.590*cm, 6.031*cm, 5.783*cm, 5.444*cm, 5.139*cm, 4.778*cm, 4.535*cm, 4.176*cm, 4.047*cm, 3.865*cm, 3.540*cm, 3.217*cm, 2.940*cm, 2.608*cm, 2.4638*cm, 2.4645*cm, 1.811*cm, 1.419*cm, 1.168*cm, 0.95*cm, 0.739*cm, 0.612*cm, 0.488*cm, 0.414*cm, 0.379*cm, 0.334*cm, 0.291*cm, 0.256*cm, 0.223*cm, 0.191*cm, 0.165*cm, 0.0498*cm, 0.04908*cm,  0.04352*cm, 0.0384*cm, 0.03835*cm, 0.03472*cm, 0.0316*cm, 0.02014*cm, 0.01420*cm, 0.00977*cm};


    G4double WLS_ABSL[3] = {10.*m, 10.*m,10.*m};

    G4double WLS_Emission[9] = {6.6*eV,6.7*eV,6.8*eV,6.9*eV,7.0*eV,                                                               7.1*eV,7.2*eV,7.3*eV,7.4*eV};



    G4MaterialPropertiesTable* QUARTZ_mt = new G4MaterialPropertiesTable();
    QUARTZ_mt->AddProperty("FASTCOMPONENT", Energy, QUARTZ_SCINT, NUMENTRIES);
    QUARTZ_mt->AddProperty("SLOWCOMPONENT", Energy, QUARTZ_SCINT, NUMENTRIES);
    QUARTZ_mt->AddProperty("RINDEX",        Energy2, QUARTZ_RIND2, NUMENTRIES2);
    //    QUARTZ_mt->AddProperty("WLSABSLENGTH",     Energy, WLS_ABSL, 3);
    //    QUARTZ_mt->AddProperty("ABSLENGTH",     Energy3, QUARTZ_ABSL3, NUMENTRIES3);
    QUARTZ_mt->AddConstProperty("SCINTILLATIONYIELD",0./MeV); 
    QUARTZ_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
    QUARTZ_mt->AddConstProperty("FASTTIMECONSTANT",20.*ns);
    QUARTZ_mt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
    QUARTZ_mt->AddConstProperty("YIELDRATIO",1.0);

    return QUARTZ_mt;
  }




//   G4MaterialPropertiesTable* OpticalMaterialProperties::GXe(G4double pressure)
//   {
//     G4MaterialPropertiesTable* GXe_mpt = new G4MaterialPropertiesTable();
    
//     const G4int SCIN_NUMENTRIES = 7;
//     const G4int RIN_NUMENTRIES  = 27;
//     const G4int ABSL_NUMENTRIES = 27;
//     const G4int EMI_NUMENTRIES = 23;
    
    
//     FWHM and peak of emission extracted from paper: 
//     Physical review A, Volume 9, Number 2, 
//     February 1974. Koehler, Ferderber, Redhead and Ebert.
    
//     G4double emission_FWHM = 
//       2.*sqrt(2*log(2)) * (-0.117 * pressure + 15.42) * nm;

//     G4double Emission_peak = (0.05*pressure+169.45)*nm;
    
//     G4double Wave_Length[EMI_NUMENTRIES] = 
//       {140*nm, 150*nm, 152*nm, 154*nm, 156*nm, 158*nm, 160*nm, 
//        162*nm, 164*nm, 166*nm, 168*nm, 170*nm, 172*nm, 174*nm, 
//        176*nm,178*nm,180*nm, 182*nm, 184*nm, 186*nm, 188*nm, 
//        190*nm,200*nm };
    
//     G4double Emission_AMP[EMI_NUMENTRIES];
    
//     for (int i =0;i<EMI_NUMENTRIES;i++)
//       {
// 	double wavelength =  Wave_Length[i];
// 	double result = 
// 	  exp(-pow(Emission_peak-wavelength,2)/(2*pow(Emission_FWHM, 2)))/(Emission_FWHM*sqrt(pi*2.));
	
// 	Emission_AMP[i] =result;
      
//       }

//     G4double Energy[RIN_NUMENTRIES] = 
//       { 3.71*eV, 6.2*eV , 6.26*eV, 6.33*eV, 6.39*eV, 6.46*eV, 
//  	6.53*eV, 6.59*eV, 6.67*eV, 6.74*eV, 6.81*eV, 6.89*eV, 
//  	6.97*eV, 7.04*eV, 7.13*eV, 7.21*eV, 7.29*eV, 7.38*eV,
//  	7.47*eV, 7.56*eV, 7.65*eV, 7.75*eV, 7.85*eV, 7.95*eV,
//  	8.05*eV, 8.16*eV, 8.32*eV };

//     G4double Scn_PP[SCIN_NUMENTRIES] = 
//       { 6.2*eV, 6.97*eV, 7.29*eV, 7.51*eV, 7.75*eV, 8.16*eV, 8.27*eV};

//     G4double Scn_FAST[SCIN_NUMENTRIES] = 
//       {0., 0.13, 0.24, 0.26, 0.23, 0.14, 0.};
    
//     G4double HPXe_RIND[RIN_NUMENTRIES]  = 
//       { 1., 1., 1.,1.,1., 1., 1.,1., 1., 1.,1., 1., 1.,	1., 1., 
// 	1.,1., 1., 1.,1., 1., 1.,1., 1., 1., 1., 1.};

//     G4double HPXe_ABSL[ABSL_NUMENTRIES]  = 
//       { 35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm, 
// 	35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm, 
// 	35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm, 
// 	35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm, 
// 	35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm, 
// 	35000.*cm, 35000.*cm, 35000.*cm, 35000.*cm,
// 	35000.*cm, 35000.*cm, 35000.*cm};


//     GXe_mpt->AddProperty("FASTCOMPONENT", Scn_PP, Scn_FAST, SCIN_NUMENTRIES);
//     HPXe_mt->AddProperty("SLOWCOMPONENT", Energy, HPXe_SCINT, NUMENTRIES);
//     GXe_mpt->AddProperty("RINDEX",        Energy, HPXe_RIND, RIN_NUMENTRIES);
//     GXe_mpt->AddProperty("ABSLENGTH",     Energy, HPXe_ABSL, RIN_NUMENTRIES);
//     GXe_mpt->AddProperty("EMISSIONAMPLITUDE", Wave_Length, Emission_AMP, EMI_NUMENTRIES);
//     GXe_mpt->AddConstProperty("SCINTILLATIONYIELD", 100000./MeV); 
//     GXe_mpt->AddConstProperty("RESOLUTIONSCALE", 1.0);
//     GXe_mpt->AddConstProperty("FASTTIMECONSTANT",1.*ns);
//     GXe_mpt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
//     GXe_mpt->AddConstProperty("YIELDRATIO",1.0);
    
//     return GXe_mpt;
//   }



//   G4MaterialPropertiesTable* OpticalMaterialProperties::FusedSilica()
//   {
//     const G4int NUMENTRIES = 3;
//     const G4int NUMENTRIES2 = 27;
//     const G4int NUMENTRIES3 = 37;
//     const G4int NUMENTRIES4 = 54;

//     G4double Energy[NUMENTRIES] = { 5.0*eV , 7.07*eV, 8.32*eV };

//     G4double Energy2[NUMENTRIES2] = { 3.71*eV, 6.2*eV , 6.26*eV, 6.33*eV, 6.39*eV, 6.46*eV, 6.53*eV, 6.59*eV, 6.67*eV, 6.74*eV, 6.81*eV, 6.89*eV, 6.97*eV, 7.04*eV , 7.13*eV, 7.21*eV, 7.29*eV, 7.38*eV, 7.47*eV, 7.56*eV, 7.65*eV, 7.75*eV, 7.85*eV, 7.95*eV, 8.05*eV, 8.16*eV, 8.32*eV  };

//     G4double Energy3[NUMENTRIES3] = { 4.17*eV, 4.39*eV, 4.63*eV, 4.79*eV, 5.08*eV, 5.91*eV, 6.37*eV , 6.63*eV, 6.84*eV, 6.91*eV, 6.98*eV, 7.1*eV , 7.16*eV, 7.22*eV, 7.27*eV, 7.32*eV, 7.35*eV, 7.41*eV, 7.45*eV, 7.48*eV, 7.51*eV, 7.54*eV, 7.55*eV, 7.6*eV, 7.63*eV, 7.6301*eV, 7.66*eV, 7.68*eV, 7.69*eV, 7.72*eV, 7.72*eV, 7.76*eV, 7.77*eV, 7.8*eV, 7.82*eV, 7.88, 8.32*eV };

//     G4double Energy4[NUMENTRIES4] = {4.21*eV, 4.74*eV, 5.77*eV, 6.08*eV, 6.28*eV , 6.51*eV, 6.58*eV, 6.62*eV, 6.7*eV, 6.72*eV, 6.77*eV, 6.81*eV, 6.86*eV, 6.88*eV, 6.95*eV,  6.98*eV, 7.0*eV, 7.03*eV, 7.07*eV , 7.1*eV, 7.13*eV, 7.16*eV, 7.17*eV, 7.2*eV, 7.23*eV, 7.26*eV, 7.265*eV, 7.27*eV, 7.275*eV, 7.31*eV, 7.34*eV, 7.37*eV, 7.4*eV, 7.45*eV, 7.46*eV, 7.49*eV, 7.51*eV, 7.52*eV, 7.54*eV, 7.55*eV, 7.58*eV, 7.62*eV, 7.67*eV, 7.71*eV, 7.76*eV, 7.78*eV, 7.79*eV, 7.8*eV, 7.83*eV, 7.87, 7.92*eV, 7.95*eV, 8.27*eV };   


    
//     G4double QUARTZ_SCINT[NUMENTRIES] = { 0.1, 0.1, 0.1 };

//     G4double QUARTZ_RIND2[NUMENTRIES2]  = {1.47974, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.56, 1.547};

//     G4double QUARTZ_ABSL[NUMENTRIES]  = { 3.5*cm, 3.5*cm, 3.5*cm}; 


//     G4double QUARTZ_ABSL3[NUMENTRIES3]  = { 11.55*cm, 10.86*cm, 10.97*cm, 10.30*cm, 10.07*cm, 9.07*cm, 8.88*cm, 8.67*cm, 8.47*cm, 8.46*cm, 8.45*cm, 8.25*cm, 7.82*cm, 7.820*cm, 7.25*cm, 6.45*cm, 5.80*cm, 5.07*cm, 4.28*cm, 3.78*cm, 3.21*cm, 2.52*cm, 2.11*cm, 1.82*cm, 1.55*cm, 1.27*cm, 1.11*cm, 1.02*cm, 0.89*cm, 0.76*cm, 0.67*cm, 0.56*cm, 0.47*cm,  0.40*cm, 0.3*cm, 0.0001*cm};

//     G4double QUARTZ_ABSL4[NUMENTRIES4]  = {11.799*cm, 10.817*cm, 10.066*cm, 9.497*cm, 9.241*cm, 8.999*cm, 8.744*cm, 8.497*cm, 8.044*cm, 8.042*cm, 7.628*cm, 7.250*cm, 6.906*cm, 6.590*cm, 6.031*cm, 5.783*cm, 5.444*cm, 5.139*cm, 4.778*cm, 4.535*cm, 4.176*cm, 4.047*cm, 3.865*cm, 3.540*cm, 3.217*cm, 2.940*cm, 2.608*cm, 2.4638*cm, 2.4645*cm, 1.811*cm, 1.419*cm, 1.168*cm, 0.95*cm, 0.739*cm, 0.612*cm, 0.488*cm, 0.414*cm, 0.379*cm, 0.334*cm, 0.291*cm, 0.256*cm, 0.223*cm, 0.191*cm, 0.165*cm, 0.0498*cm, 0.04908*cm,  0.04352*cm, 0.0384*cm, 0.03835*cm, 0.03472*cm, 0.0316*cm, 0.02014*cm, 0.01420*cm, 0.00977*cm};


//     G4double WLS_ABSL[3] = {10.*m, 10.*m,10.*m};

//     G4double WLS_Emission[9] = {6.6*eV,6.7*eV,6.8*eV,6.9*eV,7.0*eV,7.1*eV,7.2*eV,7.3*eV,7.4*eV};



//     G4MaterialPropertiesTable* QUARTZ_mt = new G4MaterialPropertiesTable();
//     QUARTZ_mt->AddProperty("FASTCOMPONENT", Energy, QUARTZ_SCINT, NUMENTRIES);
//     QUARTZ_mt->AddProperty("SLOWCOMPONENT", Energy, QUARTZ_SCINT, NUMENTRIES);
//     QUARTZ_mt->AddProperty("RINDEX",        Energy2, QUARTZ_RIND2, NUMENTRIES2);
//        QUARTZ_mt->AddProperty("WLSABSLENGTH",     Energy, WLS_ABSL, 3);
//        QUARTZ_mt->AddProperty("ABSLENGTH",     Energy3, QUARTZ_ABSL3, NUMENTRIES3);
//     QUARTZ_mt->AddConstProperty("SCINTILLATIONYIELD",0./MeV); 
//     QUARTZ_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
//     QUARTZ_mt->AddConstProperty("FASTTIMECONSTANT",20.*ns);
//     QUARTZ_mt->AddConstProperty("SLOWTIMECONSTANT",45.*ns);
//     QUARTZ_mt->AddConstProperty("YIELDRATIO",1.0);

//     return QUARTZ_mt;
//   }
  
  
} // end namespace nexus
