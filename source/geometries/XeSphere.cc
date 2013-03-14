// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  Javier Mu?oz Vidal <jmunoz@ific.uv.es>    
//  Created: 27 Nov 2009
//  
//  Copyright (c) 2009-2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "XeSphere.h"

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

    // Creating the vertex generator
    _sphere_vertex_gen = new SpherePointSampler(_radius, 0.);
  }
  
  
  
  XeSphere::~XeSphere()
  {
    delete _sphere_vertex_gen;
  }
  


  // void XeSphere::SetParameters()
  // {
  // }
  
  
  
  void XeSphere::Construct()
  {
    G4String name = "XE_SPHERE";

    G4Orb* sphere_solid = new G4Orb(name, _radius);
    
    _xenon = MaterialsList::GXe();

    G4LogicalVolume* sphere_logic = 
    new G4LogicalVolume(sphere_solid, _xenon, name);

    SetLogicalVolume(sphere_logic);
  }
  
  

  G4ThreeVector XeSphere::GenerateVertex(const G4String& region) const
  {
    //if (region == "CENTER") {
    return G4ThreeVector(0., 0., 0.); 
    //}
    // Generating in any part of the CHAMBER
    //else {
    //return _sphere_vertex_gen->GenerateVertex(region);
    //}
  }
  

} // end namespace nexus
