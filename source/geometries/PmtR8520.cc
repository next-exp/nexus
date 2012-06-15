// ----------------------------------------------------------------------------
//  $Id$
//
//  Author: <justo.martin-albo@ific.uv.es> 
//          <edgar.gomez@uan.edu.co>
//  Created: 17 Aug 2009
//  
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "PmtR8520.h"

#include "OpticalMaterialProperties.h"
#include "PmtSD.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4SDManager.hh>


namespace nexus {

  
  PmtR8520::PmtR8520(): BaseGeometry()
  {
    BuildGeometry();
  }
  
  
  
  PmtR8520::~PmtR8520()
  {
  }
  
  
  
  void PmtR8520::BuildGeometry()
  {
    // DIMENSIONS ////////////////////////////////////////////////////
    
    G4double pmt_side   = 24.0 * mm;
    G4double pmt_length = 30.  * mm; 

    G4double window_side   = 24.0 * mm;
    G4double window_thickn =  1.2 * mm;
    
    G4double phcath_side   = 20.5 * mm;
    G4double phcath_thickn =  0.1 * mm;
    
    _dimensions.setX(pmt_side);
    _dimensions.setY(pmt_side);
    _dimensions.setZ(pmt_length);
    
    
    // PMT METALIC CAN ///////////////////////////////////////////////

    G4Box* pmt_solid = 
      new G4Box("PMT_R8520",  pmt_side/2., pmt_side/2., pmt_length/2.);

    G4Material* aluminum =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
    
    G4LogicalVolume* pmt_logic = 
      new G4LogicalVolume(pmt_solid, aluminum, "PMT_R8520");
    
    this->SetLogicalVolume(pmt_logic);

    
    // PMT WINDOW ////////////////////////////////////////////////////

    G4Box* window_solid = 
      new G4Box("PMT_WINDOW", window_side/2., window_side/2., window_thickn/2.);
				    
    G4Material* quartz =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    quartz->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());

    _window_logic =
      new G4LogicalVolume(window_solid, quartz, "PMT_WINDOW");
    
    G4PVPlacement* window_physi =
      new G4PVPlacement(0, G4ThreeVector(0., 0., (pmt_length - window_thickn)/2.),
			_window_logic, "PMT_WINDOW", pmt_logic, false, 0);
    
    
    // PHOTOCATHODE //////////////////////////////////////////////////
    G4Box* phcath_solid = 
      new G4Box("PHOTOCATHODE", 
		phcath_side/2., phcath_side/2., phcath_thickn/2.);

    G4LogicalVolume* phcath_logic =
      new G4LogicalVolume(phcath_solid, aluminum, "PHOTOCATHODE");

    G4double pos_z = pmt_length/2. - window_thickn - phcath_thickn/2.;
    
    G4PVPlacement* phcath_physi = 
    new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z), phcath_logic, 
		      "PHOTOCATHODE", pmt_logic, false, 0);
    
    // G4SDManager* SDmgr = G4SDManager::GetSDMpointer();
    // PmtSD* pmtSD = new PmtSD("/PmtR8520/PHOTOCATHODE", "PmtHitsCollection"); 
    // //    pmtSD->SetDetectorVolumeDepth(1);
    // SDmgr->AddNewDetector(pmtSD);   
    // window_logic->SetSensitiveDetector(pmtSD);
    
    
    
    // OPTICAL SURFACES //////////////////////////////////////////////
    
    const G4int entries = 69;

    G4double ENERGIES[entries] =
      { 8.85571463448661*eV, 7.71019930863262*eV, 7.52305854871435*eV, 
	7.29294146369485*eV, 7.08457170758929*eV, 6.80087794200836*eV,
	6.59819078673829*eV, 6.40061976679466*eV, 6.19900024414062*eV,
	5.90380975632440*eV, 5.63545476740057*eV, 5.39043499490489*eV,
	5.16583353678385*eV, 4.95920019531250*eV, 4.76846172626202*eV,
        4.59185203269676*eV, 4.42785731724330*eV, 4.27517258216595*eV, 
	4.13266682942708*eV, 3.99935499621976*eV, 3.87437515258789*eV,
	3.75696984493371*eV, 3.64647073184743*eV, 3.54228585379464*eV,     
	3.44388902452257*eV, 3.35081094277872*eV, 3.26263170744243*eV, 
	3.17897448417468*eV, 3.09950012207031*eV, 3.02390255811738*eV,
	2.95190487816220*eV, 2.88325592750727*eV, 2.81772738370028*eV,
	2.75511121961806*eV, 2.69521749745245*eV, 2.63787244431516*eV,     
	2.58291676839193*eV, 2.53020418128189*eV, 2.47960009765625*eV, 
	2.43098048789828*eV, 2.38423086313101*eV, 2.33924537514741*eV,
	2.29592601634838*eV, 2.25418190696023*eV, 2.21392865862165*eV,
	2.17508780496162*eV, 2.13758629108297*eV, 2.10135601496292*eV,     
	2.06633341471354*eV, 2.03245909643955*eV, 1.99967749810988*eV, 
	1.96793658544147*eV, 1.93718757629395*eV, 1.90738469050481*eV,
	1.87848492246686*eV, 1.85044783407183*eV, 1.82323536592371*eV,
	1.79681166496830*eV, 1.77114292689732*eV, 1.74619725187060*eV,     
	1.72194451226128*eV, 1.69835623127140*eV, 1.67540547138936*eV, 
	1.65306673177083*eV, 1.63131585372122*eV, 1.61012993354302*eV,
	1.58948724208734*eV, 1.56936715041535*eV, 1.54975006103516*eV };

    G4double REFLECTIVITY[entries] = 
      { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
	0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
	0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,	0., 0., 0., 
	0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
	0., 0., 0., 0., 0., 0.,	0., 0., 0. };

    G4double EFFICIENCY[entries] = 
      {	0.000000, 0.223139, 0.364444, 0.409789, 0.391126, 0.325817,
	0.311208, 0.290758, 0.282341, 0.277207, 0.274127, 0.271971,
	0.283778, 0.288604, 0.292916, 0.295996, 0.302977, 0.312731,
	0.323511, 0.327823, 0.332136, 0.331520, 0.328234, 0.327618,
	0.324127, 0.322998, 0.323717, 0.319424, 0.315108, 0.309044,
	0.301644, 0.291675, 0.286125, 0.276259, 0.263823, 0.250154,
	0.233196, 0.217472, 0.205036, 0.193628, 0.174409, 0.145221,
	0.119322, 0.101028, 0.086228, 0.073073, 0.061871, 0.051593,
	0.041932, 0.033710, 0.026516, 0.019527, 0.014286, 0.010277,
	0.006989, 0.004747, 0.002993, 0.001858, 0.001149, 0.000627,
	0.000418, 0.000210, 0.000105, 0.000105, 0.000000, 0.000000,
	0.000000, 0.000000, 0.000000 };


    G4MaterialPropertiesTable* phcath_mpt = new G4MaterialPropertiesTable(); 
    phcath_mpt->AddProperty("EFFICIENCY", ENERGIES, EFFICIENCY, entries);
    phcath_mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, entries);

    G4OpticalSurface* phcath_opsur = 
      new G4OpticalSurface("PHOTOCATHODE", unified, polished, dielectric_metal);
    phcath_opsur->SetMaterialPropertiesTable(phcath_mpt);
    
    new G4LogicalBorderSurface("PHOTOCATHODE",
			       window_physi, phcath_physi, phcath_opsur);    
  }
  
  
  
  G4LogicalVolume* PmtR8520::GetWindowLogic()
  {
    return _window_logic;
    
  }
  
  
  
  G4ThreeVector PmtR8520::GetDimensions() const
  {
    return _dimensions;
  }

  
} // end namespace nexus
