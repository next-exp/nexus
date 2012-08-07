// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 29 Mar 2009
//  
//  Copyright (c) 2009-2012 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "CylindricChamber.h"

#include "ConfigService.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "UniformElectricDriftField.h"
#include "IonizationSD.h"
#include "CylinderPointSampler.h"

#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Region.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <G4ThreeVector.hh>


namespace nexus {

  
  CylindricChamber::CylindricChamber(): BaseGeometry()
  {
    SetParameters();
    BuildGeometry();
    
    // Creating the vertex generator
    _chamber_vertex_gen = 
      new CylinderPointSampler(_radius, _length, _thickn, _thickn);
  }
  
  
  
  CylindricChamber::~CylindricChamber()
  {
    delete _chamber_vertex_gen;
  }
  
  
  
  void CylindricChamber::SetParameters()
  {
    // Get geometry configuration parameters
    const ParamStore& cfg = ConfigService::Instance().Geometry();

    // chamber dimensions
    _radius = cfg.GetDParam("chamber_radius");
    _length = cfg.GetDParam("chamber_length");
    _thickn = cfg.GetDParam("chamber_thickness");

    // xenon gas pressure
    _gxe_pressure = cfg.GetDParam("gxe_pressure");

  }
  
  
  
  void CylindricChamber::BuildGeometry()
  {
    // CHAMBER ///////////////////////////////////////////////////////
    // Chamber radius and length defined by config parameters are
    // internal dimensions. We add chamber thickness to obtain the
    // external (total) size.
    G4double total_length = _length + 2*_thickn;
    G4double total_radius = _radius + _thickn;

    G4Tubs* chamber_solid = 
      new G4Tubs("CHAMBER", 0., total_radius, total_length/2., 0., twopi);

    G4Material* steel = MaterialsList::Steel316Ti();
    
    G4LogicalVolume* chamber_logic = 
      new G4LogicalVolume(chamber_solid, steel, "CHAMBER");

    this->SetLogicalVolume(chamber_logic);
    
    // GAS ///////////////////////////////////////////////////////////
    // This is the volume that represents the gas filling the
    // cylindric chamber. Its dimensions are therefore the internal
    // dimensions of the chamber.

    G4Tubs* gas_solid = 
      new G4Tubs("GAS", 0., _radius, _length/2., 0., twopi);
    
    G4Material* gxe = MaterialsList::GXe(_gxe_pressure, 303.*kelvin);
    gxe->
      SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_gxe_pressure,303));

    G4LogicalVolume* gas_logic = 
      new G4LogicalVolume(gas_solid, gxe, "GAS");

    G4PVPlacement* gas_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), gas_logic,
			"GAS", chamber_logic, false, 0, true);

    // ACTIVE ////////////////////////////////////////////////////////

    G4double active_length = 0.75 * _length;
    G4double active_side = 2. * _radius * sin(pi/4.);

    G4Box* active_solid = 
      new G4Box("ACTIVE", active_side/2., active_side/2., active_length/2.);

    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, gxe, "ACTIVE");
    
    active_logic->SetUserLimits(new G4UserLimits(1.*mm));
    
    G4PVPlacement* active_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic, 
			"ACTIVE", gas_logic, false, 0, true);

    // Define a drift field for this volume
    UniformElectricDriftField* drift_field = new UniformElectricDriftField();
    drift_field->SetCathodePosition(-active_length/2.);
    drift_field->SetAnodePosition(active_length/2.);
    drift_field->SetDriftVelocity(1.*mm/microsecond);
    drift_field->SetTransverseDiffusion(1.*mm/sqrt(cm));
    drift_field->SetLongitudinalDiffusion(.5*mm/sqrt(cm));

    G4Region* drift_region = new G4Region("DRIFT_REGION");
    drift_region->SetUserInformation(drift_field);
    drift_region->AddRootLogicalVolume(active_logic);

    // Define this volume as an ionization sensitive detector
    G4String sdname = "/CYLINDRIC_CHAMBER/CHAMBER/ACTIVE";
    IonizationSD* ionizationSD = new IonizationSD(sdname);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionizationSD);
    active_logic->SetSensitiveDetector(ionizationSD);

    // ELECTROLUMINESCENCE GAP ///////////////////////////////////////

    G4double elgap_length = 1. * cm;

    G4Box* elgap_solid = 
      new G4Box("EL_GAP", active_side/2., active_side/2., elgap_length/2.);
    
    G4LogicalVolume* elgap_logic = 
      new G4LogicalVolume(elgap_solid, gxe, "EL_GAP");

    G4double pos_z = active_length/2. + elgap_length/2.;

    G4PVPlacement* elgap_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z),
			elgap_logic, "EL_GAP", gas_logic, false, 0, true);

    // Define a drift field with EL generation for this volume
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    el_field->SetCathodePosition(active_length/2.);
    el_field->SetAnodePosition(active_length/2.+elgap_length);
    el_field->SetDriftVelocity(5.*mm/microsecond);
    el_field->SetTransverseDiffusion(1.*mm/sqrt(cm));
    el_field->SetLongitudinalDiffusion(.5*mm/sqrt(cm));
    el_field->SetLightYield(1000);
    
    G4Region* el_region = new G4Region("EL_REGION");
    el_region->SetUserInformation(el_field);
    el_region->AddRootLogicalVolume(elgap_logic);
  }
  
  
  
  G4ThreeVector CylindricChamber::GenerateVertex(const G4String& region) const
  {
    return _chamber_vertex_gen->GenerateVertex(region);
  }
  
  
} // end namespace nexus
