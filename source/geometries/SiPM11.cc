// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 2 March 2010
//  
//  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "SiPM11.h"

#include "PmtSD.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>



namespace nexus {

  
  SiPM11::SiPM11(): BaseGeometry()
  {
    Construct();
  }
  
  
  
  SiPM11::~SiPM11()
  {
  }



  G4ThreeVector SiPM11::GetDimensions() const
  {
    return _dimensions;
  }
  
  
  
  void SiPM11::Construct()
  {
    // PACKAGE ///////////////////////////////////////////////////////

    G4double sipm_x = 2.425 * mm;
    G4double sipm_y = 1.900 * mm;
    G4double sipm_z = 0.850 * mm;

    _dimensions.setX(sipm_x);
    _dimensions.setY(sipm_y);
    _dimensions.setZ(sipm_z);

    G4Box* sipm_solid = new G4Box("SIPM11", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* epoxy = MaterialsList::Epoxy();
    epoxy->SetMaterialPropertiesTable(OpticalMaterialProperties::Epoxy());
    
    G4LogicalVolume* sipm_logic = 
      new G4LogicalVolume(sipm_solid, epoxy, "SIPM11");

    this->SetLogicalVolume(sipm_logic);

    
    // ACTIVE WINDOW /////////////////////////////////////////////////
    
    G4double active_side     = 1.0   * mm;
    G4double active_depth    = 0.3   * mm;
    G4double active_offset_x = 0.975 * mm;
    
    G4Box* active_solid =
      new G4Box("PHOTODIODES", active_side/2., active_side/2., active_depth/2);
    
    G4Material* silicon = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    G4double pos_x = - sipm_x/2. + active_offset_x + active_side/2.;
    G4double pos_z = sipm_z/2. - active_depth/2.;

    new G4PVPlacement(0, G4ThreeVector(pos_x, 0., pos_z), active_logic,
		      "PHOTODIODES", sipm_logic, false, 0);
    
    
    // OPTICAL SURFACES //////////////////////////////////////////////

    // TODO. Set proper sensitivity of the SiPM.
    
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
    
    
    // SENSITIVE DETECTOR ////////////////////////////////////////////

    G4String sdname = "/SIPM11/PHOTODIODES";
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
    
    if (!G4SDManager::GetSDMpointer()->FindSensitiveDetector(sdname, false)) {
      PmtSD* sipmsd = new PmtSD(sdname);
      sipmsd->SetDetectorVolumeDepth(0);
      sipmsd->SetDetectorNamingOrder(1000.);
      sipmsd->SetTimeBinning(1.*microsecond);
      sipmsd->SetMotherVolumeDepth(1);
      
      G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
      sipm_logic->SetSensitiveDetector(sipmsd);
    }

    // Visibilities
    active_logic->SetVisAttributes(G4VisAttributes::Invisible);

  }
  
  
//   void SiPM11::SetSensitiveDetector()
//   {
//     PmtSD* sensdet = new PmtSD("/SIPM11/PHOTODIODES", "SIPM11");
//     sensdet->SetDetectorVolumeDepth(0);
    
//   }
  
  
} // end namespace nexus
