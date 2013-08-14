//
//  DetectorConstruction.cc
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 9 Mar 2009
//     Updated: 2 Apr 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#include "DetectorConstruction.h"
#include "MaterialsList.h"
#include "BaseGeometry.h"
#include "InputManager.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>

#include <bhep/gstore.h>


namespace nexus {
  
  
  DetectorConstruction::DetectorConstruction():
    _geometry(0), _init(false)
  {
    MaterialsList::DefineMaterials();
    //MaterialsList* mat = new MaterialsList();
    //mat->DefineMaterials();
    //delete mat;
  }
  
  
  
  DetectorConstruction::~DetectorConstruction()
  {
    delete _geometry;
  }
  
  
  
  void DetectorConstruction::SetGeometry(BaseGeometry* geometry)
  {
    _geometry = geometry;
    _init = true;
  }
  
  
  
  void DetectorConstruction::AssertGeometry()
  {
    if (!_init) {
      G4cout << "ERROR.- Geometry unset!" << G4endl;
      exit(1);
    }
  }

  
  
  G4VPhysicalVolume* DetectorConstruction::Construct()
  {
    AssertGeometry();
    
    /// Definition of WORLD volumes.
    /// WORLD is a big box "filled" with galactic vacuum.
    ///
    G4double size = 25. * m;
    G4Box* world_solid = new G4Box("WORLD", size/2., size/2., size/2.);
    
    G4LogicalVolume* world_logic = 
      new G4LogicalVolume(world_solid, G4Material::GetMaterial("G4_Galactic"),
			  "WORLD", 0, 0, 0, true);
    
    world_logic->SetVisAttributes(G4VisAttributes::Invisible);

    G4VPhysicalVolume* world_physi = 
      new G4PVPlacement(0, G4ThreeVector(), world_logic, "WORLD", 0, false, 0);
    
    
    /// DETECTOR
    ///
    G4LogicalVolume*   detector_logic = _geometry->GetLogicalVolume();
    G4VPhysicalVolume* detector_physi = 
      new G4PVPlacement(0, G4ThreeVector(0,0,0), detector_logic, "DETECTOR",
    			world_logic, false, 0);

    
    return world_physi;
  }


} // end namespace nexus
