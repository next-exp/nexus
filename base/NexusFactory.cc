//
//  NexusFactory.cc
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 15 Apr 2009
//     Updated: 30 Apr 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#include "NexusFactory.h"
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"

/// GEOMETRIES
#include "StandardTPC.h"

/// GENERATORS
#include "SingleParticle.h"
#include "GenbbInterface.h"

/// PHYSICS LISTS
#include "EmStandardPhysicsList.h"

/// RUN ACTIONS
#include "DefaultRunAction.h"

/// EVENT ACTIONS
#include "DefaultEventAction.h"

/// TRACKING ACTIONS
#include "DefaultTrackingAction.h"

/// STEPPING ACTIONS
//


namespace nexus {
  
  
  DetectorConstruction* NexusFactory::CreateDetectorConstruction(const G4String& name)
  {
    DetectorConstruction* p = new DetectorConstruction();
    
    if      (name == "STANDARD") {
      p->SetGeometry(new StandardTPC);
    }
    else if (name == "WHATEVER") {
      //
    }
    else {
      G4cout << "ERROR.- Unknown geometry!" << G4endl;
      exit(1);
    }
    
    return p;
  }
  
  
  
  PrimaryGeneration* NexusFactory::CreatePrimaryGeneration(const G4String& name)
  {
    PrimaryGeneration* p = new PrimaryGeneration();
    
    if      (name == "SINGLE_PARTICLE") {
      p->SetGenerator(new SingleParticle); 
    } 
    else if (name == "GENBB") {
      p->SetGenerator(new GenbbInterface);
    }
    else {
      G4cout << "ERROR.- Unknown generator." << G4endl;
      exit(1);
    }
    
    return p;
  }
  
  
  
  G4VUserPhysicsList* NexusFactory::CreatePhysicsList(const G4String& name)
  {
    G4VUserPhysicsList* p = 0;

    if      (name == "EM_STANDARD") {
      p = new EmStandardPhysicsList(); 
    }
    else if (name == "WHATEVER") {
      //
    }
    else {
      G4cout << "ERROR.- Unknown physics list!" << G4endl;
      exit(1);
    }
    
    return p;
  }



  G4UserRunAction* NexusFactory::CreateRunAction(const G4String& name)
  {
    G4UserRunAction* p = 0;
    
    if (name == "DEFAULT") {
      p = new DefaultRunAction; 
    }
    else if (name == "") {
      //
    }
    else {
      G4cout << "ERROR" << G4endl;
      exit(1);
    }

    return p;
  }



  G4UserEventAction* NexusFactory::CreateEventAction(const G4String& name)
  {
    G4UserEventAction* p = 0;
    
    if (name == "DEFAULT") {
      p = new DefaultEventAction; 
    }
    else if (name == "") {
      //
    }
    else {
      G4cout << "ERROR" << G4endl;
    }
    
    return p;
  }
  
  
  
  G4UserTrackingAction* NexusFactory::CreateTrackingAction(const G4String& name)
  {
    G4UserTrackingAction* p = 0;
    
    if (name == "DEFAULT") {
      p = new DefaultTrackingAction; 
    }
    else if (name == "") {
      //
    }
    else {
      G4cout << "ERROR" << G4endl;
    }
    
    return p;
  }

  
} // end namespace nexus
