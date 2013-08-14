// ----------------------------------------------------------------------------
//  $Id: Next0Ific.cc 2508 2010-02-15 15:26:10Z jmalbos $
//
//  Authors: J. Martin-Albo <jmalbos@ific.uv.es>
//           F. Monrabal <franmon4@ific.uv.es>
//  Created: 27 Jan 2010
//  
//  Copyright (c) 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next0Ific.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "PmtR8520.h"
#include "PmtR7378A.h"
#include "IonizationSD.h"
#include "PmtSD.h"

#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4SDManager.hh>


namespace nexus {

  
  Next0Ific::Next0Ific(): BaseGeometry()
  {
    DefineGeometry();
  }
  
  
  
  Next0Ific::~Next0Ific()
  {
  }



  void Next0Ific::DefineGeometry()
  {
    // Materials .....................................................

    G4NistManager* nist = G4NistManager::Instance();

    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* teflon = nist->FindOrBuildMaterial("G4_TEFLON");
    G4Material* silica = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    G4Material* steel = MaterialsList::Steel();
    G4Material* gxe = MaterialsList::GXe();

    // Dimensions ....................................................

    G4double active_radius = 19. * mm;
    G4double active_length = 45. * mm;

    G4double teflon_thickn =  3. * mm;

    G4double total_radius = active_radius + teflon_thickn;
    G4double total_length = active_length + teflon_thickn/2.;


    // NEXT-0 volumes ................................................
    
    G4Box* next0_solid = new G4Box("NEXT0", .5*m, .5*m, .5*m);
    
    G4LogicalVolume* next0_logic = 
      new G4LogicalVolume(next0_solid, air, "NEXT0");

    next0_logic->SetVisAttributes(G4VisAttributes::Invisible);

    this->SetLogicalVolume(next0_logic);


    // TEFLON volumes ................................................

    G4Tubs* teflon_solid = 
      new G4Tubs("TEFLON", 0., total_radius, total_length/2., 0., twopi);

    G4LogicalVolume* teflon_logic =
      new G4LogicalVolume(teflon_solid, teflon, "TEFLON");

    G4PVPlacement* teflon_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), teflon_logic, "TEFLON",
			next0_logic, false, 0);

    
    // ACTIVE volumes ................................................

    G4Tubs* active_solid =
      new G4Tubs("ACTIVE", 0., active_radius, active_length/2., 0, twopi);

    gxe->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe());

    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, gxe, "ACTIVE");

    G4double pos_Z = (0., 0., teflon_thickn/4.);

    G4PVPlacement* active_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_Z), active_logic, "ACTIVE",
			 teflon_logic, false, 0);

    G4SDManager* SDmgr = G4SDManager::GetSDMpointer();
    IonizationSD* ionSD = 
      new IonizationSD("/NEXT0/ACTIVE");
    SDmgr->AddNewDetector(ionSD);
    active_logic->SetSensitiveDetector(ionSD);
    
    
    // WINDOW ........................................................

    G4double side = total_radius * 2.;
    G4double thickness = 15. * mm;

    G4Box* window_solid = new G4Box("WINDOW", side/2., side/2., thickness/2.);
    
    silica->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
    
    G4LogicalVolume* window_logic =
      new G4LogicalVolume(window_solid, silica, "WINDOW");

    pos_Z = (total_length + thickness)/2.;

     G4PVPlacement* window_physi =
       new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_Z), window_logic, "WINDOW",
			 next0_logic, false, 0);

     
     // PMT ..........................................................
     
     //PmtR8520* pmt = new PmtR8520();
     PmtR7378A* pmt = new PmtR7378A();
     G4LogicalVolume* pmt_logic = pmt->GetLogicalVolume();

     pos_Z = total_length/2. + thickness + pmt->GetDimensions().z()/2.;

     G4RotationMatrix* rm = new G4RotationMatrix();
     rm->rotateX(pi);

     G4PVPlacement* pmt_physi =
       new G4PVPlacement(rm, G4ThreeVector(0.,0.,pos_Z), pmt_logic, "PMT",
			 next0_logic, false, 0);
  }


} // end namespace nexus
