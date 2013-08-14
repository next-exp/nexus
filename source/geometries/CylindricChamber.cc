// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 29 Mar 2009
//  
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "CylindricChamber.h"

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

  
  CylindricChamber::CylindricChamber(): BaseGeometry()
  {
    SetParameters();
    BuildGeometry();
    
    // Creating the vertex generator
    _chamber_vertex_gen = 
      new CylinderVertexGenerator(_radius, _length, _thickn, _thickn);
  }
  
  
  
  CylindricChamber::~CylindricChamber()
  {
    delete _chamber_vertex_gen;
  }
  
  
  
  void CylindricChamber::SetParameters()
  {
    // Get geometry configuration parameters
    const ParamStore& cfg = ConfigService::Instance().Geometry();

    // read chamber dimensions
    _radius = cfg.GetDParam("chamber_radius");
    _length = cfg.GetDParam("chamber_length");
    _thickn = cfg.GetDParam("chamber_thickness");

    // chamber material
    if (cfg.GetSParam("chamber_material") == "copper")
      _chamber_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    else 
      _chamber_mat =  MaterialsList::Steel();

    // tracking material
    if (cfg.GetSParam("tracking_material") == "LXe") {
      _tracking_mat = G4NistManager::Instance()->FindOrBuildMaterial("lXe");
    }
    else {
      G4double pressure = cfg.GetDParam("GXe_pressure");
      _tracking_mat = MaterialsList::GXe(pressure);
    }
  }
  
  
  
  void CylindricChamber::BuildGeometry()
  {
    // CHAMBER volumes ..............................................
    
    // Chamber radius and length defined by config parameters are
    // internal dimensions. We add chamber thickness to obtain the
    // external (total) size.
    G4double total_length = _length + 2*_thickn;
    G4double total_radius = _radius + _thickn;

    G4Tubs* chamber_solid = 
      new G4Tubs("CHAMBER", 0., total_radius, total_length/2., 0., twopi);
    
    G4LogicalVolume* chamber_logic = 
      new G4LogicalVolume(chamber_solid, _chamber_mat, "CHAMBER");

    this->SetLogicalVolume(chamber_logic);
    

    // ACTIVE (tracking) volumes ....................................

    G4Tubs* active_solid = 
      new G4Tubs("ACTIVE", 0., _radius, _length/2., 0., twopi);
    
    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, _tracking_mat, "ACTIVE");

    active_logic->SetUserLimits(new G4UserLimits(5.*mm));
    
    G4PVPlacement* active_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic, "ACTIVE",
 			chamber_logic, false, 0);

    //ACTIVE defined as a sensitive detector
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
    G4String sdname = "/CYLINDRIC_CHAMBER/CHAMBER/ACTIVE";
    IonizationSD* ionizationSD = new IonizationSD(sdname);
    sdmgr->AddNewDetector(ionizationSD);
    active_logic->SetSensitiveDetector(ionizationSD);
  }
  
  
  
  G4ThreeVector CylindricChamber::GenerateVertex(const G4String& region) const
  {
    if (region == "CENTER")
      return G4ThreeVector(0., 0., 0.); 
    else 
      return _chamber_vertex_gen->GenerateVertex(region);
  }


  
  G4ThreeVector CylindricChamber::GenerateVertex(G4int option) const
  {
    Region region = Region(option);
    
    if (region == HERE)
      return G4ThreeVector(0., 0., 10.);
    else if (region == THERE)
      return G4ThreeVector(0., 0., 10.);
  }


} // end namespace nexus
