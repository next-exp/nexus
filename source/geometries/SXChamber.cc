// ----------------------------------------------------------------------------
//  Author:  Javier Muñoz-Vidal <jmunoz@ific.uv.es>    
//  Created: 15 Sep 2010
//  
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "SXChamber.h"

#include "ConfigService.h"
#include "IonizationSD.h"
#include "CylinderVertexGenerator.h"
#include "MaterialsList.h"
#include "UniformElectricDriftField.h"

#include <G4Region.hh>
#include <G4NistManager.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <G4SPSPosDistribution.hh>
#include <Randomize.hh>



namespace nexus {

  
  SXChamber::SXChamber(): BaseGeometry()
  {
    SetParameters();
    BuildGeometry();
    
    // Creating the vertex generator
    _chamber_vertex_gen = 
      new CylinderVertexGenerator(_chamber_radius, _chamber_length, _chamber_thickn, _chamber_thickn);
  }
  
  
  
  SXChamber::~SXChamber()
  {
    delete _chamber_vertex_gen;
  }
  
  
  
  void SXChamber::SetParameters()
  {
    // Get geometry configuration parameters
    const ParamStore& cfg = ConfigService::Instance().Geometry();

    // read dimensions
    _chamber_radius = cfg.GetDParam("chamber_radius");
    _chamber_length = cfg.GetDParam("chamber_length");
    _chamber_thickn = cfg.GetDParam("chamber_thickness");
    _scint_thickn   = cfg.GetDParam("scint_thickness");

    // tracking material
    if (cfg.GetSParam("tracking_material") == "LXe") {
      _tracking_mat = G4NistManager::Instance()->FindOrBuildMaterial("lXe");
    }
    else {
      G4double pressure = cfg.GetDParam("GXe_pressure");
      _tracking_mat = MaterialsList::GXe(pressure);
    }

    // chamber material
    if (cfg.GetSParam("chamber_material") == "G4_POLYSTYRENE")
      _chamber_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYSTYRENE");
    else 
      _chamber_mat =  MaterialsList::Steel();

    // scintillator material
    if (cfg.GetSParam("scint_material") == "G4_WATER")
      _scint_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
  }
  
  
  
  void SXChamber::BuildGeometry()
  {

    // Calculating Dimensions ..............................................
    G4double chamber_ext_length = _chamber_length + 2 * _chamber_thickn;
    G4double chamber_ext_radius = _chamber_radius + _chamber_thickn;

    G4double scint_ext_length = chamber_ext_length + 2 * _scint_thickn;
    G4double scint_ext_radius = chamber_ext_radius + _scint_thickn;

    
    // defining SCINTillator
    G4Tubs* scint_solid = 
      new G4Tubs("SCINT", 0., scint_ext_radius, scint_ext_length/2., 0., twopi);
    
    G4LogicalVolume* scint_logic = 
      new G4LogicalVolume(scint_solid, _scint_mat, "SCINT");

    this->SetLogicalVolume(scint_logic);


    // defining CHAMBER
    G4Tubs* chamber_solid = 
      new G4Tubs("CHAMBER", 0., chamber_ext_radius, chamber_ext_length/2., 0., twopi);
    
    G4LogicalVolume* chamber_logic = 
      new G4LogicalVolume(chamber_solid, _chamber_mat, "CHAMBER");

    G4PVPlacement* chamber_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), chamber_logic, "CHAMBER",
 			scint_logic, false, 0);


    // defining ACTIVE

    G4Tubs* active_solid = 
      new G4Tubs("ACTIVE", 0., _chamber_radius, _chamber_length/2., 0., twopi);
    
    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, _tracking_mat, "ACTIVE");

    active_logic->SetUserLimits(new G4UserLimits(5.*mm));
    
    G4PVPlacement* active_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic, "ACTIVE",
 			chamber_logic, false, 0);


    //ACTIVE defined as a sensitive detector
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
    G4String sdname = "/SX_CHAMBER/ACTIVE";
    IonizationSD* ionizationSD = new IonizationSD(sdname);
    sdmgr->AddNewDetector(ionizationSD);
    active_logic->SetSensitiveDetector(ionizationSD);
  }
  
  
  
  G4ThreeVector SXChamber::GenerateVertex(const G4String& region) const
  {
    if (region == "CENTER")
      return G4ThreeVector(0., 0., 0.); 
    else 
      return _chamber_vertex_gen->GenerateVertex(region);
  }



} // end namespace nexus
