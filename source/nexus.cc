// ----------------------------------------------------------------------------
///  \file   nexus.cc
///  \brief  NEXUS - Main program
///
///  \author   Justo Martin-Albo <jmalbos@ific.uv.es>    
///  \date     2 Apr 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NexusFactory.h"
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"
#include "BhepUtils.h"

#include <G4RunManager.hh>  
#include <G4UImanager.hh>   
#include <G4UIterminal.hh>  
#include <G4UItcsh.hh> 
#include <Randomize.hh>
//#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
//#endif

#include <getopt.h>
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



void PrintBanner()
{
}



bool ParseCmdLineOptions(int argc, char** argv)
{ 
  static struct option long_options[] =
  {
    {"interactive", required_argument, 0, 'i'},
    {"batch",       required_argument, 0, 'b'}
  };

  int c;

  while (true) {
    
    int option_index = 0;
    c = getopt_long(argc, argv, "b:i:", long_options, &option_index);

    if (c == -1) break;

    switch (c) {

      case 'i':
      //G4String macro_filename = optarg;
      break;

      case 'b':
      printf("Option -b with value %s\n", optarg);

      case '?':
      std::cout << "Non-recognized option." << std::endl;

      default:
      abort();
    }
  }
}




G4int main(int argc, char** argv) 
{
  ParseCmdLineOptions(argc, argv);

  G4String macro_filename = "macros/nexus_example1.macro";

  ////////////////////////////////////////////////////////////////////

  // Create an instance of the nexus factory
  NexusFactory factory;
  
  // Get a pointer to the UI manager and read the macro 
  G4UImanager* UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("/control/execute " + macro_filename);

  // Create an instance of the Geant4 run manager
  G4RunManager* runmgr = new G4RunManager();
 
  runmgr->SetUserInitialization(factory.CreateDetectorConstruction());
  runmgr->SetUserInitialization(factory.CreatePhysicsList());
  runmgr->SetUserAction(factory.CreatePrimaryGeneration());

  if (UI->GetCurrentValues("/NexusFactory/RunAction"))
    runmgr->SetUserAction(factory.CreateRunAction());
  if (UI->GetCurrentValues("/NexusFactory/EventAction"))
    runmgr->SetUserAction(factory.CreateEventAction());
  if (UI->GetCurrentValues("/NexusFactory/TrackingAction"))
    runmgr->SetUserAction(factory.CreateSteppingAction());
  
  runmgr->Initialize();
  
  G4int seed = 1;
  G4String vis_macro = "";

  if (seed < 0) CLHEP::HepRandom::setTheSeed(time(0));
  else CLHEP::HepRandom::setTheSeed(seed);
  
  CLHEP::HepRandom::showEngineStatus();



  // G4int tracking_verbosity = 0;
  // if (jobCfg.PeekIParam("tracking_verbosity"))
  //   tracking_verbosity = jobCfg.GetIParam("tracking_verbosity");

  // UI->ApplyCommand("/run/verbose 0");
  // UI->ApplyCommand("/event/verbose 0");
  // UI->ApplyCommand("/tracking/verbose " 
  //  + bhep::to_string(tracking_verbosity));
  
  // visual mode
  if (false) {

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
    runmgr->BeamOn(0);
  }


  delete runmgr;
  return EXIT_SUCCESS;
} 
