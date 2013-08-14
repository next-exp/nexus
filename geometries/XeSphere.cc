// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  Javier Muñoz Vidal <jmunoz@ific.uv.es>    
//  Created: 27 Nov 2009
//  
//  Copyright (c) 2009 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "XeSphere.h"

#include "ConfigService.h"
#include "IonizationSD.h"
#include "SphereVertexGenerator.h"

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
    _sphere_vertex_gen = new SphereVertexGenerator(_radius, _thickn);
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
    _thickn = config.GetDParam("sphere_thickness");

    // read materials. FIXME. Table with friendly materials names
    _sphere_mat = config.GetSParam("sphere_material");
    if      (_sphere_mat == "copper") _sphere_mat = "G4_Cu";
    else if (_sphere_mat == "steel")  _sphere_mat = "SSteel";

    _tracking_mat  = config.GetSParam("tracking_material");
    if (_tracking_mat == "LXe") _tracking_mat = "G4_lXe";
  }
  
  
  
  void XeSphere::BuildGeometry()
  {
    G4double outer_radius = _radius + _thickn;

    // The SPHERE volume ...........................................

    G4Orb* sphere_solid = new G4Orb("SPHERE", outer_radius);
    
    G4LogicalVolume* sphere_logic = 
      new G4LogicalVolume(sphere_solid, G4Material::GetMaterial(_sphere_mat),
			  "SPHERE");

    SetLogicalVolume(sphere_logic);
  
    
    // ACTIVE (tracking) volumes ....................................

    G4Orb* active_solid = new G4Orb("ACTIVE", _radius);
    
    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, G4Material::GetMaterial(_tracking_mat),
 			  "ACTIVE");
    
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
