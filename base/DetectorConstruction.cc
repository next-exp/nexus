// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 9 Mar 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DetectorConstruction.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>


namespace nexus {
  
  
  DetectorConstruction::DetectorConstruction(): _geometry(0)
  {
  }

  
  
  DetectorConstruction::~DetectorConstruction() 
  { 
    delete _geometry; 
  }
  
  
  
  G4VPhysicalVolume* DetectorConstruction::Construct()
  {
    // Check whether a detector geometry has been set
    if (_geometry == 0)
      G4Exception("[DetectorConstruction] ERROR: Geometry not set!");
    

    // Definition of WORLD volumes ............................................
    // WORLD is a big box "filled" with galactic vacuum

    G4NistManager* nist = G4NistManager::Instance();
    G4Material* vacuum = nist->FindOrBuildMaterial("G4_Galactic");
    
    G4double size = _geometry->GetSpan();

    G4Box* world_solid = new G4Box("WORLD", size/2., size/2., size/2.);
    
    G4LogicalVolume* world_logic = 
      new G4LogicalVolume(world_solid, vacuum, "WORLD", 0, 0, 0, true);
    
    world_logic->SetVisAttributes(G4VisAttributes::Invisible);
    
    G4PVPlacement* world_physi = 
      new G4PVPlacement(0, G4ThreeVector(), world_logic, "WORLD", 0, false, 0);
    
    
    // USER GEOMETRY ...........................................................
    
    G4LogicalVolume* geometry_logic = _geometry->GetLogicalVolume();
    
    G4String geometry_name = geometry_logic->GetName();
 
    G4PVPlacement* geometry_physi = 
      new G4PVPlacement(0, G4ThreeVector(0,0,0), geometry_logic, geometry_name,
    			world_logic, false, 0);
    
    
    return world_physi;
  }


} // end namespace nexus
