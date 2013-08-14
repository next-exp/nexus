//
//  nexus.cc - Main program
//
//     Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 2 Apr 2009
//     Updated: 9 Apr 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#include "InputManager.h"
#include "NexusFactory.h"
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"
#include "DefaultRunAction.h"
#include "DefaultEventAction.h"
#include "DefaultTrackingAction.h"

#include <G4RunManager.hh>  
#include <G4UImanager.hh>   
#include <G4UIterminal.hh>  
#include <G4UItcsh.hh> 
#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#endif

#include <bhep/gstore.h>


using namespace nexus;



int main(int argc, char** argv) 
{
//   if (argc != 2) {
//     G4cout << "Usage: ./nexus <inputcard>" << G4endl;
//     exit(1);
//   }
    
//   G4String inputcard = argv[1];
  
  G4String inputcard = "example.card";
  InputManager::Instance().SetInputCard(inputcard);

  G4RunManager* runManager = new G4RunManager();

  NexusFactory* factory = new NexusFactory();
  
  runManager->SetUserInitialization(factory->CreateDetectorConstruction("STANDARD"));
  runManager->SetUserInitialization(factory->CreatePhysicsList("EM_STANDARD"));
  runManager->SetUserAction(factory->CreatePrimaryGeneration("SINGLE_PARTICLE"));
  
  runManager->SetUserAction(factory->CreateRunAction("DEFAULT"));
  runManager->SetUserAction(factory->CreateEventAction("DEFAULT"));
  runManager->SetUserAction(factory->CreateTrackingAction("DEFAULT"));

   
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

    
  // initialize G4 kernel
  runManager->Initialize();
  
  
  // get the pointer to the UI manager and set verbosities
  G4UImanager* UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("/run/verbose 0");
  UI->ApplyCommand("/event/verbose 0");
  UI->ApplyCommand("/tracking/verbose 0");
  
  // G4UIterminal is a (dumb) terminal.
  G4UIsession *session = 0;
  session = new G4UIterminal(new G4UItcsh);
 
  UI->ApplyCommand("/control/execute vis.mac");    
  session->SessionStart();

  delete session;
      
#ifdef G4VIS_USE
  delete visManager;
#endif
 
  // start a run
  //int numberOfEvent = 3;
  //runManager->BeamOn(numberOfEvent);
  
  // job termination
  delete runManager;
  return 0;
} 
