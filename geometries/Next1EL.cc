// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>
//  Created: 27 Jan 2010
//  
//  Copyright (c) 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next1EL.h"

#include "PmtR7378A.h"
#include "SiPM11.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"

#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Polyhedra.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>


namespace nexus {


  Next1EL::Next1EL(): BaseGeometry()
  {
    DefineGeometry();
  }
  
  
  
  Next1EL::~Next1EL()
  {
  }
  
  
  
  void Next1EL::DefineGeometry()
  {
    // Materials
    
    G4NistManager* nist = G4NistManager::Instance();

    G4Material* air    = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* GXe    = MaterialsList::GXe(10.*bar);
    GXe->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe());
    G4Material* steel  = MaterialsList::Steel();
    G4Material* teflon = nist->FindOrBuildMaterial("G4_TEFLON");
    G4Material* silica = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    
    
    // LAB
    
    G4double lab_size = 2. * m;

    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);

    G4LogicalVolume* lab_logic = new G4LogicalVolume(lab_solid, air, "LAB");
    lab_logic->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic);

    
    // VESSEL

    G4double vessel_diam   = 300. * mm;
    G4double vessel_length = 600. * mm;
    G4double vessel_thickn =   3. * mm;

    G4Tubs* vessel_solid = 
      new G4Tubs("VESSEL", 0., (vessel_diam/2. + vessel_thickn), 
		 (vessel_length/2. + vessel_thickn), 0., twopi);
    
    G4LogicalVolume* vessel_logic = 
      new G4LogicalVolume(vessel_solid, steel, "VESSEL");
    
    G4PVPlacement* vessel_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), vessel_logic,
			"VESSEL", lab_logic, false, 0);
    
    
    // GAS

    G4Tubs* gas_solid = 
      new G4Tubs("GAS", 0., vessel_diam/2., vessel_length/2., 0, twopi);

    G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, GXe, "GAS");
    
    G4PVPlacement* gas_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gas_logic,
			"GAS", vessel_logic, false, 0);
    
    
    // LIGHT TUBE
    
    G4double tube_diam   = 190.  * mm;
    G4double tube_length = 400.  * mm;
    G4double tube_thickn =   0.1 * mm;
    
    G4double zplane[2] = {-tube_length/2., tube_length/2.};
    G4double rinner[2] = {tube_diam/2., tube_diam/2.};
    G4double router[2] = {(tube_diam/2. + tube_thickn), 
			  (tube_diam/2. + tube_thickn)};

    G4Polyhedra* tube_solid =
      new G4Polyhedra("LIGHT_TUBE", 0., twopi, 6, 2, zplane, rinner, router);

    G4LogicalVolume* tube_logic = 
      new G4LogicalVolume(tube_solid, teflon, "LIGHT_TUBE");
    
    G4PVPlacement* tube_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), tube_logic, 
			"LIGHT_TUBE", gas_logic, false, 0);
 
    
    // TRACKING PLANE REFLECTOR
    
    G4double refl_diam = 175. * mm;
    G4double refl_thick =  1. * mm;

    G4double zplane2[2] = {161.*mm, 160*mm};
    G4double rinner2[2] = {0., 0.};
    G4double router2[2] = {refl_diam/2., refl_diam/2.};
    
    G4Polyhedra* reflector_solid =
      new G4Polyhedra("TRACKING", 0., twopi, 6, 2, zplane2, rinner2, router2);
   
    G4LogicalVolume* reflector_logic =
      new G4LogicalVolume(reflector_solid, teflon, "TRACKING");

    G4PVPlacement* reflector_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), reflector_logic,
			"TRACKING", gas_logic, false, 0);


    SiPM11* sipm = new SiPM11();
    G4LogicalVolume* sipm_logic = sipm->GetLogicalVolume();

    G4PVPlacement* sipm_physi;

    G4double x, y;
    G4int copy_no = 0;

    for (G4int i=0; i<12; i++) {
    
      y = -55.*mm + i * 10.*mm;
      
      for (G4int j=0; j<12; j++) {
	
	x = -55.*mm + j * 10.*mm;
	
	sipm_physi = 
	  new G4PVPlacement(0, G4ThreeVector(x,y,155.*mm), sipm_logic,
			    "SILICON_PM", gas_logic, false, copy_no);
	
	copy_no++;
      }
    }


    
    // ACTIVE
    
    G4double active_length = 300. * mm;
    G4double active_diam   = tube_diam;

    zplane[0] = -active_length/2.; zplane[1] = active_length/2.;
    rinner[0] = 0.; rinner[1] = 0.;
    router[0] = active_diam/2.; router[1] = active_diam/2.; 

    G4Polyhedra* active_solid =
      new G4Polyhedra("ACTIVE", 0., twopi, 6, 2, zplane, rinner, router);

    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, GXe, "ACTIVE");
    
    G4PVPlacement* active_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic, 
			"ACTIVE", gas_logic, false, 0);
    
    G4SDManager* SDmgr = G4SDManager::GetSDMpointer();
    IonizationSD* ionSD = new IonizationSD("/NEXT1/ACTIVE");
    SDmgr->AddNewDetector(ionSD);
    active_logic->SetSensitiveDetector(ionSD);

    
    // PMTs

    PmtR7378A* pmt = new PmtR7378A();
    G4LogicalVolume* pmt_logic = pmt->GetLogicalVolume();

    G4double Radius = (175./2) * mm;
    G4double apothem = (3./16) * Radius;
    G4double radius = 2/sqrt(3) * apothem;

    // Temporarily I'll place the pmts by hand

    G4PVPlacement* pmt_physi;

    // first row
    
    G4ThreeVector pos(-2*apothem, 3*radius, -175.*mm);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 0);
    
    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 1);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 2);

    // second row

    pos.setX(-3*apothem);
    pos.setY((3./2)*radius);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 3);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 4);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 5);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 6);

    // third row
    
    pos.setX(-4*apothem);
    pos.setY(0.);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 7);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 8);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 9);
    
    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 10);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 11);

    // fourth row
    pos.setX(-3*apothem);
    pos.setY(-(3./2)*radius);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 12);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 13);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 14);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 15);

    // fifth row
    
    pos.setX(-2*apothem);
    pos.setY(-3*radius);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 16);
    
    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 17);

    pos.setX(pos[0] + 2*apothem);
    pmt_physi = 
      new G4PVPlacement(0, pos, pmt_logic, "PMT_R7378A", gas_logic, false, 18);


    
    // OPTICAL SURFACES

    G4double energies[2] = {4.0*eV, 8.520*eV};
    G4double reflectivity[2] = {1., 1.};
    G4double efficiency[2] = {0., 0.};

    G4MaterialPropertiesTable* teflon_mt = new G4MaterialPropertiesTable();
    teflon_mt->AddProperty("EFFICIENCY", energies, efficiency, 2);
    teflon_mt->AddProperty("REFLECTIVITY", energies, reflectivity, 2);

    G4OpticalSurface* opsurf_tube = 
      new G4OpticalSurface("LIGHT_TUBE", unified, ground, dielectric_metal);
    opsurf_tube->SetSigmaAlpha(2.*pi);
    opsurf_tube->SetMaterialPropertiesTable(teflon_mt);
    
    new G4LogicalSkinSurface("LIGHT_TUBE", tube_logic, opsurf_tube);
    //new G4LogicalSkinSurface("LIGHT_TUBE", reflector_logic, opsurf_tube);
  }
  
  
} // end namespace nexus
