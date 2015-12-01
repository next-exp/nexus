// ----------------------------------------------------------------------------
//  $Id: Lab.cc 10054 2015-02-09 14:50:49Z paola $
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 29 August 2013
//  
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "Lab.h"

#include "Pet2boxes.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "UniformElectricDriftField.h"
#include "IonizationSD.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>


namespace nexus {
  
  using namespace CLHEP;

  Lab::Lab(): 
    BaseGeometry(), _msg(0)
  {
    module_ = new Pet2boxes();
  }



  Lab::~Lab()
  {
    delete _msg;
  } 



  void Lab::Construct()
  {
    // LAB /////////////////////////////////////////////////////////////
    // This is just a volume of air surrounding the detector so that
  // events (from calibration sources or cosmic rays) can be generated 
  // on the outside.

    G4double lab_size (2. * m);
    G4Box* lab_solid = 
      new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
    
    G4LogicalVolume* lab_logic =
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "AIR");
    lab_logic->SetVisAttributes(G4VisAttributes::Invisible);
    
    // Set this volume as the wrapper for the whole geometry 
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(lab_logic);
   
    module_->Construct();

    G4LogicalVolume* module_logic = module_->GetLogicalVolume();
    new G4PVPlacement(0, G4ThreeVector(0.,0., -10.*cm - 1.5*cm), module_logic, "MODULE_0",
        lab_logic, false, 0, true);

    G4RotationMatrix rot;
    rot.rotateY(pi);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0.,0., 10.*cm + 1.5*cm)), module_logic, "MODULE_1",
        lab_logic, false, 1, true);
    
  }



  G4ThreeVector Lab::GenerateVertex(const G4String& /*region*/) const
  {
    return G4ThreeVector(0.,0.,0.);
  }
  

} // end namespace nexus
