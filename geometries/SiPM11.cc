// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  L. Serra <luis.serra@ific.uv.es>    
//  Created: 17 Aug 2009
//  
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "SiPM11.h"

#include "OpticalMaterialProperties.h"
#include "PmtSD.h"
#include "MaterialsList.h"

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

  
  SiPM11::SiPM11(): BaseGeometry()
  {
    BuildGeometry();
  }
  
  
  
  SiPM11::~SiPM11()
  {
  }
  
  
  
  void SiPM11::BuildGeometry()
  {
    // Materials
    
    G4Material* silicon = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4Material* epoxy = MaterialsList::Epoxy();
    epoxy->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
    

    // Dimensions

    G4double total_side = 1.9 * mm;
    G4double total_thickn = 0.85 * mm;

    G4double active_side = 1.0 * mm;
    G4double active_thickn = 0.05 * mm;
    G4double active_z = 0.3 * mm;

    _dimensions.setX(total_side);
    _dimensions.setY(total_side);
    _dimensions.setZ(total_thickn);


    // Epoxy package
    
    G4Box* sipm_solid = 
      new G4Box("SIPM11", total_side/2., total_side/2., total_thickn/2.);

    G4LogicalVolume* sipm_logic = 
      new G4LogicalVolume(sipm_solid, epoxy, "SIPM11");
    
    this->SetLogicalVolume(sipm_logic);


    // Active area (silicon diodes)

    G4Box* active_solid = 
      new G4Box("PHOTODIODES", 
		active_side/2.,active_side/2.,active_thickn/2.); 

    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    G4double pos_z = total_thickn/2. - active_z;
    
    G4PVPlacement* active_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z), active_logic, 
			"PHOTODIODES", sipm_logic, false, 0 );


    // Sensitive detector

    PmtSD* sipm_sd = new PmtSD("/SIPM11/PHOTODIODES", "SiPMHitsCollection");
    sipm_sd->SetDetectorVolumeDepth(1);
    G4SDManager::GetSDMpointer()->AddNewDetector(sipm_sd);
    sipm_logic->SetSensitiveDetector(sipm_sd);
    

    // Definition of optical surfaces

    const G4int entries = 2;
    
    G4double energies[entries]     = {2.0*eV, 10.*eV};
    G4double reflectivity[entries] = {0.    ,  0.   };
    G4double efficiency[entries]   = {1.    ,  1.   };
    
    G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
    sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
    sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

    G4OpticalSurface* sipm_opsurf = 
      new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
    sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);

    new G4LogicalSkinSurface("SIPM_OPSURF", active_logic, sipm_opsurf);    
  }


  
  G4ThreeVector SiPM11::GetDimensions()
  {
    return _dimensions;
  }



} // end namespace nexus
