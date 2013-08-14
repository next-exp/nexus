// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  F. Monrabal <franmon4@ific.uv.es>    
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
    // Definition of materials .......................................
    
    G4NistManager* nist = G4NistManager::Instance();
    
    G4Material* silica = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    silica->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
    
    G4Material* aluminum = nist->FindOrBuildMaterial("G4_Al");


    // PMT dimensions ................................................
    
    G4double pmt_side   = 2.54 * cm;
    G4double pmt_length = 3.   * cm; 

    G4double window_side   = 24.0 * mm;
    G4double window_length =  1.2 * mm;
    
    G4double phcath_side   = 20.5 * mm;
    G4double phcath_length =  0.1 * mm;
    
    _dimensions.setX(pmt_side);
    _dimensions.setY(pmt_side);
    _dimensions.setZ(pmt_length);
    
    
    // PMT metalic can ...............................................

    G4Box* pmt_solid = 
      new G4Box("PmtR8520",  pmt_side/2., pmt_side/2., pmt_length/2.);
    
    G4LogicalVolume* pmt_logic = 
      new G4LogicalVolume(pmt_solid, aluminum, "PmtR8520");
    
    this->SetLogicalVolume(pmt_logic);

    
    // PMT window ....................................................

    G4Box* window_solid = 
      new G4Box("PMT_WINDOW", window_side/2., window_side/2., window_length/2.);
				    
    G4LogicalVolume* window_logic =
      new G4LogicalVolume(window_solid, silica, "PMT_WINDOW");
    
    G4double pos_z = (pmt_length - window_length) / 2.;
    
    G4PVPlacement* window_physi = 
      new G4PVPlacement(0, G4ThreeVector(0., 0., pos_z),
			window_logic, "PMT_WINDOW", pmt_logic, false, 0);
    
    
    // Photocathode ..................................................
    
    G4Box* phcath_solid = 
      new G4Box("PMT_PHOTOCATHODE", 
		phcath_side/2., phcath_side/2., phcath_length/2.);

    G4LogicalVolume* phcath_logic =
      new G4LogicalVolume(phcath_solid, silica, "PMT_PHOTOCATHODE");

    pos_z = pmt_length/2. - window_length - phcath_length/2.;

    G4PVPlacement* phcath_physi = 
      new G4PVPlacement(0, G4ThreeVector(0., 0., pos_z), phcath_logic, 
			"PMT_PHOTOCATHODE", pmt_logic, false, 0);


    G4SDManager* SDmgr = G4SDManager::GetSDMpointer();
    PmtSD* pmtSD = 
      new PmtSD("/PmtR8520/PHOTOCATHODE", "PmtHitsCollection");
    SDmgr->AddNewDetector(pmtSD);
    window_logic->SetSensitiveDetector(pmtSD);
    
    
    // Definition of optical surfaces ................................
    
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

    new G4LogicalBorderSurface("opsurf_CATHODE", 
			       window_physi, phcath_physi,
			       phcath_opsurf);    
  }
  
  
  
  G4ThreeVector PmtR8520::GetDimensions()
  {
    return _dimensions;
  }

  
} // end namespace nexus
