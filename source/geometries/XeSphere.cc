// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  Javier Muñoz Vidal <jmunoz@ific.uv.es>    
//  Created: 27 Nov 2009
//  
//  Copyright (c) 2009-2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "XeSphere.h"

#include "ConfigService.h"
#include "IonizationSD.h"
#include "SpherePointSampler.h"
#include "MaterialsList.h"

#include <G4NistManager.hh>
#include <G4Orb.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <Randomize.hh>


namespace nexus {
  
  
  XeSphere::XeSphere(): BaseGeometry()
  {
    SetParameters();
    BuildGeometry();

    // Creating the vertex generator
    _sphere_vertex_gen = new SpherePointSampler(_radius, _thickn);
  }
  
  
  
  XeSphere::~XeSphere()
  {
    delete _sphere_vertex_gen;
  }
  


  void XeSphere::SetParameters()
  {
    // get geometry configuration parameters
    const ParamStore& config = ConfigService::Instance().Geometry();

    // read chamber dimensions
    _radius = config.GetDParam("sphere_radius");
    _thickn = config.GetDParam("sphere_thickn");

    // read materials. FIXME. Table with friendly materials names
    G4String sphere_mat_name = config.GetSParam("shell_material");
    if ((sphere_mat_name == "copper") or (sphere_mat_name == "Copper")) {
      _shell_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    }
    else if ((sphere_mat_name == "steel")  or (sphere_mat_name == "Steel")) {
      _shell_mat = MaterialsList::Steel();
    }
    else if ((sphere_mat_name == "air") or (sphere_mat_name == "Air")) {
      _shell_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Air");
    }
    else if ((sphere_mat_name == "titanium") or (sphere_mat_name == "Titanium")) {
      _shell_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Ti");
    }

    G4String tracking_mat_name = config.GetSParam("tracking_material");    
    if (tracking_mat_name == "LXe") {
      _tracking_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    }
    else if (tracking_mat_name == "GXe") {
      G4double pressure = config.GetDParam("GXe_pressure");
      _tracking_mat = MaterialsList::GXe(pressure);
    }  
  }
  
  
  
  void XeSphere::BuildGeometry()
  {
    G4double outer_radius = _radius + _thickn;

    // The SPHERE volume ...........................................

    G4Orb* sphere_solid = new G4Orb("SPHERE", outer_radius);
    
    G4LogicalVolume* sphere_logic = 
      new G4LogicalVolume(sphere_solid, _shell_mat, "SPHERE");

    SetLogicalVolume(sphere_logic);
  
    
    // ACTIVE (tracking) volumes ....................................

    G4Orb* active_solid = new G4Orb("ACTIVE", _radius);
    
    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, _tracking_mat, "ACTIVE");

    active_logic->SetUserLimits(new G4UserLimits(5.*mm));

    G4PVPlacement* active_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic, "ACTIVE",
 			sphere_logic, false, 0);
    
    G4VisAttributes white(G4Colour(1., 1., 1.));
    active_logic->SetVisAttributes(white);
    
    // ACTIVE defined as a sensitive detector
    G4SDManager* SDman = G4SDManager::GetSDMpointer();

    G4String SDname = "/XeSphere/SPHERE/ACTIVE";
    IonizationSD* ionizationSD = new IonizationSD(SDname);
    SDman->AddNewDetector(ionizationSD);
    active_logic->SetSensitiveDetector(ionizationSD);
  }
  
  

  G4ThreeVector XeSphere::GenerateVertex(const G4String& region) const
  {
    if (region == "CENTER") {
      return G4ThreeVector(0., 0., 0.); 
    }
    // Generating in any part of the CHAMBER
    else {
      return _sphere_vertex_gen->GenerateVertex(region);
    }
  }
  

} // end namespace nexus
