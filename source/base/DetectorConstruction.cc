// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 9 Mar 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "DetectorConstruction.h"

#include "BaseGeometry.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>
//#include <G4GenericMessenger.hh>

using namespace nexus;



DetectorConstruction::DetectorConstruction(): 
  _msg(0), _geometry(0)
{
  // _msg = new G4GenericMessenger(this, "/Geometry/");
  // _msg->DeclareMethod("RegisterGeometry", 
  //   &DetectorConstruction::CreateGeometry, "");
}



DetectorConstruction::~DetectorConstruction()
{
  delete _geometry;
}



G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Check whether a detector geometry has been set
  if (!_geometry) {
    G4Exception("[DetectorConstruction]", "Construct()", 
      FatalException, "Geometry not set!");
  }

  // At this point the user should have loaded the configuration
  // parameters of the geometry or it will get built with the 
  // default values.
  _geometry->Construct();

  // We define now the world volume as an empty (filled, actually with
  // galactic vacuum) box big enough to fit the user's geometry inside.

  G4double size = _geometry->GetSpan();

  G4Box* world_solid = new G4Box("WORLD", size/2., size/2., size/2.);
  
  G4Material* vacuum = 
  G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  
  G4LogicalVolume* world_logic = 
  new G4LogicalVolume(world_solid, vacuum, "WORLD", 0, 0, 0, true);
  
  world_logic->SetVisAttributes(G4VisAttributes::Invisible);
  
  G4PVPlacement* world_physi = 
  new G4PVPlacement(0, G4ThreeVector(), world_logic, "WORLD", 0, false, 0);
  
  // We place the user's geometry in the center of the world

  G4LogicalVolume* geometry_logic = _geometry->GetLogicalVolume();
  
  G4PVPlacement* geometry_physi = new G4PVPlacement(0, G4ThreeVector(0,0,0),
    geometry_logic, geometry_logic->GetName(), world_logic, false, 0);


  return world_physi;
}


//////////////////////////////////////////////////////////////////////

// #include "XeSphere.h"


// void DetectorConstruction::CreateGeometry(G4String name)
// {
//   if (_geometry)
//     G4Exception("[DetectorConstruction]", "CreateGeometry()",
//       JustWarning, "Geometry was already set!");

//   // Geometry factory

//   if (name == "XE_SPHERE")
//     _geometry = new XeSphere();

//   else
//     G4Exception("[DetectorConstruction]", "CreateGeometry()",
//       FatalException, "Unknown geometry!");
// }
