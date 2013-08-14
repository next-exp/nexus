//
//  StandardTPC.cc
//
//     Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 29 Mar 2009
//     Updated: 14 Apr 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#include "StandardTPC.h"
#include "InputManager.h"
#include "MaterialsList.h"
#include "TrackingSD.h"

#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>

#include <bhep/gstore.h>


namespace nexus {

  
  StandardTPC::StandardTPC():
    _input(InputManager::Instance().GetGeometryInput())
  {
    Initialize();
  }
  
  
  
  void StandardTPC::Initialize()
  {
    try {
      _logVol = DefineGeometry();
    }
    catch (bhep::internal_logic& e) {
      G4cout << "ERROR.- Parameter missing in GEOMETRY input!" << G4endl;
      exit(1);
    }
  }
  
  
  
  G4LogicalVolume* StandardTPC::GetLogicalVolume()
  {
    return _logVol;
  }
  
  
  
  G4LogicalVolume* StandardTPC::DefineGeometry()
  {
    G4double chamber_radius = _input.fetch_dstore("chamber_radius") * cm;
    G4double chamber_length = _input.fetch_dstore("chamber_length") * cm;
    G4double chamber_thickn = _input.fetch_dstore("chamber_thickness") * cm;
    
    G4String chamber_material = "G4_AIR";
    G4String tracking_mixture = "GXe";
    
    G4double total_radius = chamber_radius + chamber_thickn;
    G4double total_length = chamber_length + 2 * chamber_thickn;
    
    G4Tubs* chamber_solid = 
      new G4Tubs("CHAMBER", 0., total_radius, total_length/2., 0., twopi);
    
    G4LogicalVolume* chamber_logic = 
      new G4LogicalVolume(chamber_solid, G4Material::GetMaterial(chamber_material),
			  "CHAMBER");
    
    G4Tubs* active_solid = 
      new G4Tubs("ACTIVE", 0., chamber_radius, chamber_length/2., 0., twopi);
    
    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, G4Material::GetMaterial(tracking_mixture),
 			  "ACTIVE");
    
    G4PVPlacement* active_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic, "ACTIVE",
 			chamber_logic, false, 0);
    
    G4VisAttributes* red  = new G4VisAttributes(G4Colour(1., 0., 0.));
    active_logic->SetVisAttributes(red);


    G4SDManager* SDman = G4SDManager::GetSDMpointer();

    G4String SDname = "/mydet/TPC";
    TrackingSD* trackingSD = new TrackingSD(SDname);
    SDman->AddNewDetector(trackingSD);
    active_logic->SetSensitiveDetector(trackingSD);

    return chamber_logic;
   }
  
  
  
  G4ThreeVector StandardTPC::GetRandomVertex(const G4String& volume_name)
  {
    return G4ThreeVector(0., 0., 0.);
  }


} // end namespace nexus
