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
  
  
  PmtR7378A::PmtR7378A()
  {
    BuildGeometry();
  }
  
  
  
  PmtR7378A::~PmtR7378A()
  {
  }
  
  
  
  void PmtR7378A::BuildGeometry()
  {
    G4NistManager* nist = G4NistManager::Instance();
    
    G4Material* aluminum = nist->FindOrBuildMaterial("G4_Al");	
    
    G4Material* silica = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"); 
    silica->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
    
    
    // PMT

    G4double pmt_diam   = 25.4 * mm;
    G4double pmt_length = 43.0 * mm; 

    _dimensions.setX(pmt_diam);
    _dimensions.setY(pmt_diam);
    _dimensions.setZ(pmt_length);
    
    G4Tubs* window_solid =
      new G4Tubs("PMT_WINDOW", 0., pmt_diam/2., pmt_length/2., 0., twopi);
    
    G4LogicalVolume* window_logic = 
      new G4LogicalVolume(window_solid, silica, "PMT_WINDOW");
    
    this->SetLogicalVolume(window_logic);
    
    
    // PHOTOCATHODE
    
    G4double phcath_diam   = 22.0 * mm;
    G4double phcath_height =  5.0 * mm;
    G4double phcath_thickn =  0.1 * mm;
    
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
      new G4PVPlacement(0, G4ThreeVector(0.,0.,rmax/3.), phcath_logic,
			"PHOTOCATHODE", window_logic, false, 0);
    
    
    // PMT BODY
    
    G4double body_length = 35 * mm;

    G4Tubs* body_solid = 
      new G4Tubs("PMT_BODY", 0., pmt_diam/2., body_length/2., 0., twopi);

    G4LogicalVolume* body_logic =
      new G4LogicalVolume(body_solid, aluminum, "PMT_BODY");
    
    G4double pos_z = -pmt_length/2. + body_length/2.;

    G4PVPlacement* body_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z), body_logic,
			"PMT_BODY", window_logic, false, 0);
    
    body_logic->SetVisAttributes(vis_solid);
    
    
    // Sensitive detector
    PmtSD* pmtsd = new PmtSD("/PMT_R7378A/PHOTOCATHODE", "PmtHitsCollection");
    pmtsd->SetDetectorVolumeDepth(1);
    G4SDManager::GetSDMpointer()->AddNewDetector(pmtsd);
    window_logic->SetSensitiveDetector(pmtsd);
    
    
    // Definition of optical surfaces 
    
    const G4int entries = 2;
 
    G4double energies[entries]     = {2.0*eV, 10.*eV};
    G4double reflectivity[entries] = {0.    ,  0.   };
    G4double efficiency[entries]   = {1.    ,  1.   };

    G4MaterialPropertiesTable* phcath_mt = new G4MaterialPropertiesTable();
    phcath_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
    phcath_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);


    G4OpticalSurface* phcath_opsurf = 
      new G4OpticalSurface("PMT_photocath", unified, polished, dielectric_metal);
    phcath_opsurf->SetMaterialPropertiesTable(phcath_mt);

    new G4LogicalSkinSurface("opsurf_CATHODE", 
			     phcath_logic,
			     phcath_opsurf);    

  }
  
  
  
  G4ThreeVector PmtR7378A::GetDimensions() const
  {
    return _dimensions;
  }


} // end namespace nexus
