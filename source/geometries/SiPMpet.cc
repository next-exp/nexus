// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 2 March 2010
//  
//  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "SiPMpet.h"
#include "PmtSD.h"
#include "MaterialsList.h"
#include <G4GenericMessenger.hh>
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
#include <G4PhysicalConstants.hh>

#include <CLHEP/Units/SystemOfUnits.h>


namespace nexus {

  using namespace CLHEP;
  
  SiPMpet::SiPMpet(): BaseGeometry(),
		    _visibility(0)

  {
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/PetalX/", "Control commands of geometry.");
    _msg->DeclareProperty("SiPMpet_vis", _visibility, "SiPMpet Visibility");
  }
  
  
  
  SiPMpet::~SiPMpet()
  {
  }



  G4ThreeVector SiPMpet::GetDimensions() const
  {
    return _dimensions;
  }
  
  
  
  void SiPMpet::Construct()
  {
    // PACKAGE ///////////////////////////////////////////////////////

    G4double sipm_x = 7. * mm;
    G4double sipm_y = 7. * mm;
    G4double sipm_z = 0.850 * mm;

    _dimensions.setX(sipm_x);
    _dimensions.setY(sipm_y);
    _dimensions.setZ(sipm_z);

    G4Box* sipm_solid = new G4Box("SIPMpet", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* epoxy = MaterialsList::Epoxy();
    epoxy->SetMaterialPropertiesTable(OpticalMaterialProperties::GlassEpoxy());
    
    G4LogicalVolume* sipm_logic = 
      new G4LogicalVolume(sipm_solid, epoxy, "SIPMpet");

    this->SetLogicalVolume(sipm_logic);


    // PCB ///////////////////////////////////////////////////////

    // G4double pcb_z = 0.550 * mm;
    
    // G4Material* plastic = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYCARBONATE");

    // G4Box* plastic_solid = new G4Box("PLASTIC", sipm_x/2., sipm_y/2., pcb_z/2);
    
    // G4LogicalVolume* plastic_logic = 
    // new G4LogicalVolume(plastic_solid, plastic, "PLASTIC");
    
    // G4double epoxy_z = 0.300 * mm;

    // new G4PVPlacement(0, G4ThreeVector(0, 0., epoxy_z/2), plastic_logic,
    // 		      "PLASTIC", sipm_logic, false, 0, false);

    // ACTIVE WINDOW /////////////////////////////////////////////////

    G4double active_side     = 6.0   * mm;
    G4double active_depth    = 0.01   * mm;
    
    G4Box* active_solid =
      new G4Box("PHOTODIODES", active_side/2., active_side/2., active_depth/2);
    
    G4Material* silicon = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), active_logic,
		      "PHOTODIODES", sipm_logic, false, 0, false);
    
    
    // OPTICAL SURFACES //////////////////////////////////////////////

    // SiPM efficiency set using the official Hamamatsu specs.
    
    const G4int entries = 26;
    
    G4double energies[entries]     = {1.405*eV, 1.456*eV, 1.515*eV, 1.597*eV, 1.689*eV,
				      1.763*eV, 1.836*eV, 1.915*eV, 2.007*eV, 2.105*eV, 
				      2.190*eV, 2.285*eV, 2.366*eV, 2.448*eV, 2.563*eV, 
				      2.718*eV, 2.838*eV, 2.977*eV, 3.099*eV, 3.243*eV, 
				      3.387*eV, 3.525*eV, 3.608*eV, 3.695*eV, 3.762*eV, 
				      3.857*eV };
    G4double reflectivity[entries] = {0.      ,0.      ,0.      ,0.      ,0.      ,
				      0.      ,0.      ,0.      ,0.      ,0.      ,
				      0.      ,0.      ,0.      ,0.      ,0.      ,
				      0.      ,0.      ,0.      ,0.      ,0.      ,
				      0.      ,0.      ,0.      ,0.      ,0.      ,
				      0.      };
    G4double efficiency[entries]   = {0.0556  ,0.0698  ,0.0893  ,0.1250  ,0.1661  ,
				      0.1983  ,0.2341  ,0.2663  ,0.3058  ,0.3488  ,
				      0.3868  ,0.4247  ,0.4499  ,0.4734  ,0.4915  ,
				      0.4991  ,0.4898  ,0.4662  ,0.4355  ,0.4002  ,
				      0.3471  ,0.2878  ,0.2308  ,0.1620  ,0.0804  ,
				      0.0390  };


    G4double efficiency_red[entries];
    for (G4int i=0; i<entries; ++i) {
      efficiency_red[i] = efficiency[i]*.6;
    }
    
    G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
    sipm_mt->AddProperty("EFFICIENCY", energies, efficiency_red, entries);
    sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

    G4OpticalSurface* sipm_opsurf = 
      new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
    sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);

    new G4LogicalSkinSurface("SIPM_OPSURF", active_logic, sipm_opsurf);    
    
    
    // SENSITIVE DETECTOR ////////////////////////////////////////////

    G4String sdname = "/SIPM11/SiPM";
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
    
    if (!sdmgr->FindSensitiveDetector(sdname, false)) {
      PmtSD* sipmsd = new PmtSD(sdname);
      sipmsd->SetDetectorVolumeDepth(0);
      sipmsd->SetDetectorNamingOrder(1000.);
      sipmsd->SetTimeBinning(1.*microsecond);
      sipmsd->SetMotherVolumeDepth(1);
      
      G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
      sipm_logic->SetSensitiveDetector(sipmsd);
    }

    // Visibilities
    if (_visibility) {
      G4VisAttributes sipm_col(G4Colour(.40,.55,.55));
      sipm_logic->SetVisAttributes(sipm_col);
      G4VisAttributes active_col(G4Colour(1.,1.,0.));
      active_col.SetForceSolid(true);
      active_logic->SetVisAttributes(active_col);
    }
    else {
      sipm_logic->SetVisAttributes(G4VisAttributes::Invisible);
      active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }
  
  
} // end namespace nexus
