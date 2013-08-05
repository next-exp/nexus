// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  Javier Mu?oz Vidal <jmunoz@ific.uv.es>    
//  Created: 27 Nov 2009
//  
//  Copyright (c) 2009-2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "XeSphere.h"

#include "SpherePointSampler.h"
#include "MaterialsList.h"
#include "IonizationSD.h"

#include <G4GenericMessenger.hh>
#include <G4Orb.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <Randomize.hh>


namespace nexus {
  
  
  XeSphere::XeSphere(): 
    BaseGeometry(), _liquid(true), _pressure(STP_Pressure), _radius(1.*m), _sphere_vertex_gen(0) 
  {
    _msg = new G4GenericMessenger(this, "/Geometry/XeSphere/",
      "Control commands of geometry XeSphere.");

    _msg->DeclareProperty("LXe", _liquid, 
      "Build the sphere with liquid xenon.");

    G4GenericMessenger::Command& pressure_cmd =
      _msg->DeclareProperty("pressure", _pressure,
      "Set pressure for gaseous xenon (if selected).");
    pressure_cmd.SetUnitCategory("Pressure");
    pressure_cmd.SetParameterName("pressure", false);
    pressure_cmd.SetRange("pressure>0.");

    G4GenericMessenger::Command& radius_cmd =
      _msg->DeclareProperty("radius", _radius, "Radius of the xenon sphere.");
    radius_cmd.SetUnitCategory("Length");
    radius_cmd.SetParameterName("radius", false);
    radius_cmd.SetRange("radius>0.");

    // Creating the vertex generator
    _sphere_vertex_gen = new SpherePointSampler(_radius, 0.);
  }
  
  
  
  XeSphere::~XeSphere()
  {
    delete _sphere_vertex_gen;
    delete _msg;
  }
    
  
  
  void XeSphere::Construct()
  {
    G4String name = "XE_SPHERE";

    G4Orb* sphere_solid = new G4Orb(name, _radius);

    G4Material* xenon = 0;
    if (_liquid)
      xenon = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    else
      xenon = MaterialsList::GXe(_pressure);

    G4LogicalVolume* sphere_logic = 
    new G4LogicalVolume(sphere_solid, xenon, name);
    BaseGeometry::SetLogicalVolume(sphere_logic);

    IonizationSD* ionizsd = new IonizationSD("/XE_SPHERE");
    G4SDManager::GetSDMpointer()->AddNewDetector(ionizsd);
    sphere_logic->SetSensitiveDetector(ionizsd);
  }
  
  

  G4ThreeVector XeSphere::GenerateVertex(const G4String& region) const
  {
    return G4ThreeVector(0., 0., 0.); 
    //return _sphere_vertex_gen->GenerateVertex(region);
  }
  

} // end namespace nexus
