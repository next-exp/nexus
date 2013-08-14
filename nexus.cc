// ----------------------------------------------------------------------------
///  \file   nexus.cc
///  \brief  NEXUS - Main program
///
///  \author   Justo Martin-Albo <jmalbos@ific.uv.es>    
///  \date     2 Apr 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ConfigService.h"
#include "NexusFactory.h"
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"
#include "Utils.h"

#include <G4RunManager.hh>  
#include <G4UImanager.hh>   
#include <G4UIterminal.hh>  
#include <G4UItcsh.hh> 
#include <Randomize.hh>
#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#endif

#include <time.h>

using namespace nexus;



void PrintUsage()
{
  G4cerr << "\nUsage: nexus [mode] [mode_options] <config_file>\n" << G4endl;
  G4cerr << "Available modes:" << G4endl;
  G4cerr << "   [-batch(b)]               : "
	 << "batch mode" << G4endl;
  G4cerr << "    -visual(v) [<vis_macro>] : "
	 << "visual mode (default macro: vis.mac)" << G4endl;
  G4cerr << G4endl;
  exit(EXIT_FAILURE);
}



G4int main(int argc, char** argv) 
{
  // command-line options ............................................

  G4bool visual = false;
  G4String vis_macro = "vis.mac"; // default visual macro
  G4String config_file;
  
  // user provides too many or too few arguments
  if (argc < 2 || argc > 4) { 
    PrintUsage();
  }
  // implicit batch mode
  else if (argc == 2) {  
    config_file = argv[1];
  }
  // explicit mode: either batch or visual
  else if (argc > 2) {   
    G4String mode = argv[1];
    config_file   = argv[2];
    
    if (mode == "-visual" || mode == "-v") {
      visual = true;
      if (argc == 4) vis_macro = argv[3];  // non-default macro
    }
  }
  
  
  // initialization of required classes ..............................
 
  // setting run-time configuracion service
  ConfigService::Instance().SetConfigFile(config_file);

  // run manager and module factory initialization
  NexusFactory* factory    = new NexusFactory();
  G4RunManager* runMgr = new G4RunManager();

  // initialization of mandatory modules
    
  G4String name;
    
  name = ConfigService::Instance().Geometry().GetSParam("geometry_name");
  runMgr->SetUserInitialization(factory->CreateDetectorConstruction(name));
  
  name = ConfigService::Instance().Physics().GetSParam("physics_list_name");
  runMgr->SetUserInitialization(factory->CreatePhysicsList(name));
  
  name = ConfigService::Instance().Generation().GetSParam("generator_name");
  runMgr->SetUserAction(factory->CreatePrimaryGeneration(name));

  // initialization of optional user actions

  const ParamStore& actionsCfg = ConfigService::Instance().Actions();
    
  if (actionsCfg.PeekSParam("run_action_name")) {
    name = actionsCfg.GetSParam("run_action_name");
    runMgr->SetUserAction(factory->CreateRunAction(name));
  }
  
  if (actionsCfg.PeekSParam("event_action_name")) {
    name = actionsCfg.GetSParam("event_action_name");
    runMgr->SetUserAction(factory->CreateEventAction(name));
  }

  if (actionsCfg.PeekSParam("tracking_action_name")) {
    name = actionsCfg.GetSParam("tracking_action_name");
    runMgr->SetUserAction(factory->CreateTrackingAction(name));
  }

  if (actionsCfg.PeekSParam("stepping_action_name")) {
    name = actionsCfg.GetSParam("stepping_action_name");
    runMgr->SetUserAction(factory->CreateSteppingAction(name));
  }

    
  delete factory;
  runMgr->Initialize();

    
  // job initialization ..............................................

  // job configuration parameters
  const ParamStore& jobCfg = ConfigService::Instance().Job();

  // setting random number generator
  G4int seed = jobCfg.GetIParam("random_seed");
    
  if (seed < 0) CLHEP::HepRandom::setTheSeed(time(0));
  else CLHEP::HepRandom::setTheSeed(seed);
    
  CLHEP::HepRandom::showEngineStatus();

  // get the pointer to the UI manager and set verbosities
  G4UImanager* UI = G4UImanager::GetUIpointer();
  
  UI->ApplyCommand("/run/verbose 0");
  UI->ApplyCommand("/event/verbose 0");
  UI->ApplyCommand("/tracking/verbose 0");
  
  // visual mode
  if (visual) {
  
    // initialize visualization manager
    G4VisManager* visMgr = new G4VisExecutive;
    visMgr->Initialize();
      
    // G4UIterminal is a (dumb) terminal.
    G4UIsession *session = 0;
    session = new G4UIterminal(new G4UItcsh);
      
    // execute visualization macro and start session
    UI->ApplyCommand("/control/execute " + vis_macro);    
    session->SessionStart();

    // job termination
    delete session;
    delete visMgr;
  }
    
  // batch mode
  else {
    runMgr->BeamOn(jobCfg.GetIParam("number_events"));
  }

  delete runMgr;
  return EXIT_SUCCESS;
} 
