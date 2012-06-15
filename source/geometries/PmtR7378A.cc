// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>
//  Created: 17 Feb 2010
//  
//  Copyright (c) 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "PmtR7378A.h"

#include "PmtSD.h"
#include "OpticalMaterialProperties.h"
#include "MaterialsList.h"

#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>

#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>

#include <G4VisAttributes.hh>

#include <G4SDManager.hh>


namespace nexus {
  
  
  PmtR7378A::PmtR7378A(): BaseGeometry()
  {
    BuildGeometry();
  }
  
  
  
  PmtR7378A::~PmtR7378A()
  {
  }
  
  
  
  void PmtR7378A::BuildGeometry()
  {
    // PMT BODY //////////////////////////////////////////////////////

    _pmt_diam   = 25.4 * mm;
    _pmt_length = 43.0 * mm; 
    
    G4Tubs* pmt_solid =
      new G4Tubs("PMT_R7378A", 0., _pmt_diam/2., _pmt_length/2., 0., twopi);
    
    G4Material* aluminum = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");	

    G4LogicalVolume* pmt_logic = 
      new G4LogicalVolume(pmt_solid, aluminum, "PMT_R7378A");

    this->SetLogicalVolume(pmt_logic);
    
    
    // PMT WINDOW ////////////////////////////////////////////////////
    
    G4double window_diam = _pmt_diam;
    G4double window_length = 6. * mm;

    G4Tubs* window_solid =
      new G4Tubs("PMT_WINDOW", 0., window_diam/2., window_length/2., 0., twopi);

    G4Material* quartz = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    quartz->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
    
    G4LogicalVolume* window_logic =
      new G4LogicalVolume(window_solid, quartz, "PMT_WINDOW");

    new G4PVPlacement(0, G4ThreeVector(0., 0., (_pmt_length-window_length)/2.),
		      window_logic, "PMT_WINDOW", pmt_logic, false, 0);
    G4VisAttributes * visattrib_blue = new G4VisAttributes;
    visattrib_blue->SetColor(0., 0., 1.);
    window_logic->SetVisAttributes(visattrib_blue);
    
    
    // PHOTOCATHODE //////////////////////////////////////////////////
    
    G4double phcath_diam   = 22.0 * mm;
    G4double phcath_height =  4.0 * mm;
    G4double phcath_thickn =  0.1 * mm;
    G4double phcath_posz   =-16.0 * mm;
    
    G4double rmax = 
      0.5 * (phcath_diam*phcath_diam / (4*phcath_height) + phcath_height);
    G4double rmin = rmax - phcath_thickn;
    G4double theta = asin(phcath_diam/(2.*rmax));

    G4Sphere* phcath_solid =
      new G4Sphere("PHOTOCATHODE", rmin, rmax, 0, twopi, 0, theta);
    
    G4LogicalVolume* phcath_logic = 
      new G4LogicalVolume(phcath_solid, aluminum, "PHOTOCATHODE");

    G4VisAttributes vis_solid;
    vis_solid.SetForceSolid(true);
    phcath_logic->SetVisAttributes(vis_solid);

    G4PVPlacement* phcath_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,phcath_posz), phcath_logic,
			"PHOTOCATHODE", window_logic, false, 0);
    
    // Sensitive detector
    PmtSD* pmtsd = new PmtSD("/PMT_R7378A/PHOTOCATHODE", "PMT");
    pmtsd->SetDetectorVolumeDepth(1);
    pmtsd->SetTimeBinning(1.*microsecond);
    G4SDManager::GetSDMpointer()->AddNewDetector(pmtsd);
    window_logic->SetSensitiveDetector(pmtsd);

    
    // OPTICAL SURFACES //////////////////////////////////////////////
    
    const G4int entries = 30;

    G4double ENERGIES[entries] =
      {	1.72194*eV, 1.77114*eV, 1.82324*eV, 1.87848*eV, 1.93719*eV, 
	1.99968*eV, 2.06633*eV, 2.13759*eV, 2.21393*eV, 2.29593*eV, 
	2.38423*eV, 2.47960*eV, 2.58292*eV, 2.69522*eV, 2.81773*eV, 
	2.95190*eV, 3.0995*eV, 3.26263*eV, 3.44389*eV, 3.64647*eV, 
	3.87438*eV, 4.13267*eV, 4.42786*eV, 4.76846*eV, 5.16583*eV, 
	5.63545*eV, 6.19900*eV, 6.88778*eV, 7.74875*eV, 8.85571*eV};
    
    G4double EFFICIENCY[entries] =
      { 0.0000104, 0.000418, 0.001135, 0.006083, 0.001028, 
	0.021416, 0.040832, 0.061387, 0.102220, 0.143052, 
	0.173885, 0.204440, 0.234995, 0.255550, 0.276105,
	0.306937, 0.344292, 0.335154, 0.325674, 0.325021,
	0.328994, 0.321807, 0.316674, 0.296140, 0.275606, 
	0.265339, 0.234805, 0.201866, 0.48037, 1.000000};
    
    G4double REFLECTIVITY[entries] =
      { 0., 0., 0., 0., 0.,
	0., 0., 0., 0., 0.,
	0., 0., 0., 0., 0.,
	0., 0., 0., 0., 0.,
	0., 0., 0., 0., 0.,
	0., 0., 0., 0., 0. };
    
    G4MaterialPropertiesTable* phcath_mpt = new G4MaterialPropertiesTable();
    phcath_mpt->AddProperty("EFFICIENCY", ENERGIES, EFFICIENCY, entries);
    phcath_mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, entries);
    
    G4OpticalSurface* phcath_opsur =
      new G4OpticalSurface("PHOTOCATHODE", unified, polished, dielectric_metal);
    phcath_opsur->SetMaterialPropertiesTable(phcath_mpt);
    
    new G4LogicalSkinSurface("PHOTOCATHODE", phcath_logic, phcath_opsur);
  }
  
  
} // end namespace nexus
