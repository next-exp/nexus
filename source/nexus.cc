// ----------------------------------------------------------------------------
// nexus | nexus.cc
//
// This is the main program of nexus.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NexusApp.h"

#include <G4UImanager.hh>   
#include <G4UIterminal.hh>  
#include <G4UItcsh.hh> 
#include <G4VisExecutive.hh>

#include <getopt.h>

using namespace nexus;





void PrintUsage()
{
  G4cerr  << "\nUsage: ./nexus [-b|i] [-n number] <init_macro>\n" << G4endl;
  G4cerr  << "Available options:" << G4endl;
  G4cerr  << "   -b, --batch           : Run in batch mode (default)\n"
          << "   -i, --interactive     : Run in interactive mode\n"
          << "   -n, --nevents         : Number of events to simulate"
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
  G4int nevents = 0;

  static struct option long_options[] =
  {
    {"batch",       no_argument,       0, 'b'},
    {"interactive", no_argument,       0, 'i'},
    {"nevents",       required_argument, 0, 'n'},
    {0, 0, 0, 0}
  };

  int c;

  while (true) {

    //  int option_index = 0;
    opterr = 0;
    c = getopt_long(argc, argv, "bin:", long_options, 0);
    
    if (c==-1) break; // Exit if we are done reading options

    switch (c) {

      case 'b':
        batch = true;
        break;

      case 'i':
        batch = false;
        break;

      case 'n':
        nevents = atoi(optarg);
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

  if (macro_filename == "") PrintUsage(); 
    


  ////////////////////////////////////////////////////////////////////

  
  NexusApp* app = new NexusApp(macro_filename);
  app->Initialize();

  G4UImanager* UI = G4UImanager::GetUIpointer();

  // if (seed < 0) CLHEP::HepRandom::setTheSeed(time(0));
  // else CLHEP::HepRandom::setTheSeed(seed);
  
  // CLHEP::HepRandom::showEngineStatus();

  // visual mode
  if (!batch) {
    G4VisManager* vismgr = new G4VisExecutive();
    vismgr->Initialize();

    G4UIsession* session = new G4UIterminal(new G4UItcsh);
    UI->ApplyCommand("/control/execute macros/vis.mac");
    session->SessionStart();

    delete session;
    delete vismgr;
  }
  else {
    app->BeamOn(nevents);
  }

  delete app;
  return EXIT_SUCCESS;
} 
