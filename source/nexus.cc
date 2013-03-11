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

#include "NexusApp.h"
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
  G4cerr  << "\nUsage: ./nexus [-b|i] <config_macro>\n" << G4endl;
  G4cerr  << "Available options:" << G4endl;
  G4cerr  << "   -b, --batch           : Run in batch mode (default)\n"
          << "   -i, --interactive     : Run in interactive mode\n"
          << G4endl;
  exit(EXIT_FAILURE);
}



G4int main(int argc, char** argv) 
{
  ////////////////////////////////////////////////////////////////////
  // PARSE COMMAND-LINE OPTIONS

  // Abort if no command-line argument is provided. The user must
  // specify at least the name of a configuration macro.
  if (argc < 2) PrintUsage(); 

  G4bool batch = true;
  

  static struct option long_options[] =
  {
    {"batch",       0, 0, 'b'},
    {"interactive", 0, 0, 'i'},
    {0, 0, 0, 0}
  };

  int c;

  while (true) {

    int option_index = 0;
    opterr = 0;
    c = getopt_long(argc, argv, "bi", long_options, 0);
    
    if (c==-1) break; // Exit if we are done reading options

    switch (c) {

      case 'b':
        batch = true;
        break;

      case 'i':
        batch = false;
        break;

      case '?':
        break;

      default:
        abort();
    }
  }

  // If there is no other command-line argument to be processed, abort
  // because the user has not provided a configuration macro.
  // (The variable optind is set by getopt_long to the index of the next
  // element of the argv array to be processed. Once getopt has found all 
  // of the option arguments, this variable can be used to determine where 
  // the remaining non-option arguments begin.)
  if (optind == argc) PrintUsage();

  // Assume that the name of the configuration macro is the first
  // command-line parameters that is not a GNU option.
  G4String macro_filename = argv[optind];
  G4cout << macro_filename << G4endl;

  if (macro_filename == "") PrintUsage(); 
    


  ////////////////////////////////////////////////////////////////////

  
  NexusApp* app = new NexusApp(macro_filename);


  G4UImanager* UI = G4UImanager::GetUIpointer();

  // G4int seed = 1;
  // G4String vis_macro = "";

  // if (seed < 0) CLHEP::HepRandom::setTheSeed(time(0));
  // else CLHEP::HepRandom::setTheSeed(seed);
  
  // CLHEP::HepRandom::showEngineStatus();

  app->Initialize();

  // visual mode
  if (!batch) {
    G4VisManager* vismgr = new G4VisExecutive();
    vismgr->Initialize();

    G4UIsession* session = new G4UIterminal(new G4UItcsh);
    UI->ApplyCommand("/control/execute vis.mac");
    session->SessionStart();

    delete session;
    delete vismgr;
  }
  else {
    app->BeamOn(10);
  }

  delete app;
  return EXIT_SUCCESS;
} 
