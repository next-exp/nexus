// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 29 Mar 2009
//  
//  Copyright (c) 2009-2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "CylindricChamber.h"

#include "ConfigService.h"
#include "IonizationSD.h"
#include "CylinderPointSampler.h"
#include "MaterialsList.h"

#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
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
    _gxe_pressure = cfg.GetDParam("GXe_pressure");

  }
  
  
  
  void CylindricChamber::BuildGeometry()
  {
    // CHAMBER /////////////////////////////////////////////////////////////////
    
    // Chamber radius and length defined by config parameters are
    // internal dimensions. We add chamber thickness to obtain the
    // external (total) size.
    G4double total_length = _length + 2*_thickn;
    G4double total_radius = _radius + _thickn;

    G4Tubs* chamber_solid = 
      new G4Tubs("CHAMBER", 0., total_radius, total_length/2., 0., twopi);

    //G4Material* chamber_mat = MaterialsList::Steel();
    G4Material* copper = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    
    G4LogicalVolume* chamber_logic = 
      new G4LogicalVolume(chamber_solid, copper, "CHAMBER");

    this->SetLogicalVolume(chamber_logic);
    
    // ACTIVE //////////////////////////////////////////////////////////////////

    G4Tubs* active_solid = 
      new G4Tubs("ACTIVE", 0., _radius, _length/2., 0., twopi);
    
    G4Material* gxe = MaterialsList::GXe(_gxe_pressure, 303.*kelvin);

    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, gxe, "ACTIVE");
    
    active_logic->SetUserLimits(new G4UserLimits(1.*mm));
    
    G4PVPlacement* active_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic, "ACTIVE",
 			chamber_logic, false, 0);

    // Define this volume as an ionization sensitive detector
    G4String sdname = "/CYLINDRIC_CHAMBER/CHAMBER/ACTIVE";
    IonizationSD* ionizationSD = new IonizationSD(sdname);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionizationSD);
    active_logic->SetSensitiveDetector(ionizationSD);
  }
  
  
  
  G4ThreeVector CylindricChamber::GenerateVertex(const G4String& region) const
  {
    return _chamber_vertex_gen->GenerateVertex(region);
  }
  
  
} // end namespace nexus
