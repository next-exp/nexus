// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  Javier Muñoz Vidal <jmunoz@ific.uv.es>    
//  Created: 27 Nov 2009
//  
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
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

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {
  
  using namespace CLHEP;
  
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

    // Create a vertex generator for a sphere
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

    // Define solid volume as a sphere
    G4Orb* sphere_solid = new G4Orb(name, _radius);

    // Define the material (LXe or GXe) for the sphere. 
    // We use for this the NIST manager or the nexus materials list.
    G4Material* xenon = 0;
    if (_liquid)
      xenon = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    else
      xenon = MaterialsList::GXe(_pressure);

    // Define the logical volume of the sphere using the material 
    // and the solid volume defined above
    G4LogicalVolume* sphere_logic = 
    new G4LogicalVolume(sphere_solid, xenon, name);
    BaseGeometry::SetLogicalVolume(sphere_logic);

    // Set the logical volume of the sphere as an ionization 
    // sensitive detector, i.e. position, time and energy deposition
    // will be stored for each step of any charged particle crossing
    // the volume.
    IonizationSD* ionizsd = new IonizationSD("/XE_SPHERE");
    G4SDManager::GetSDMpointer()->AddNewDetector(ionizsd);
    sphere_logic->SetSensitiveDetector(ionizsd);
  }
  
  

  G4ThreeVector XeSphere::GenerateVertex(const G4String& region) const
  {
    return _sphere_vertex_gen->GenerateVertex(region);
  }
  

} // end namespace nexus
