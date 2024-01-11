// ----------------------------------------------------------------------------
// nexus | nexus.cc
//
// This is the main program of nexus.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NexusApp.h"
#include "NexusExceptionHandler.h"

#include <G4StateManager.hh>
#include <G4UImanager.hh>
#include <G4UIExecutive.hh>
#include <G4VisExecutive.hh>
#include <G4SteppingVerbose.hh>

#include <getopt.h>

using namespace nexus;


void PrintUsage()
{
  G4cerr  << "\nUsage: ./nexus [-b|i] [-n number] <init_macro>\n" << G4endl;
  G4cerr  << "Available options:" << G4endl;
  G4cerr  << "   -b, --batch           : Run in batch mode (default)\n"
          << "   -i, --interactive     : Run in interactive mode\n"
          << "   -o, --overlap-check   : Turn warnings into exceptions and increase precision in overlap check\n"
          << "   -n, --nevents         : Number of events to simulate\n"
          << "   -p, --precision       : Number of significant figures in verbosity"
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
  G4bool overlap_check = false;
  G4int nevents = 0;
  G4int precision = -1;

  static struct option long_options[] =
  {
    {"batch",       no_argument,       0, 'b'},
    {"interactive", no_argument,       0, 'i'},
    {"overlaps",    no_argument,       0, 'o'},
    {"precision",   required_argument, 0, 'p'},
    {"nevents",     required_argument, 0, 'n'},
    {0, 0, 0, 0}
  };

  int c;

  while (true) {

    //  int option_index = 0;
    opterr = 0;
    c = getopt_long(argc, argv, "biop:n:", long_options, 0);

    if (c==-1) break; // Exit if we are done reading options

    switch (c) {

      case 'b':
        batch = true;
        break;

      case 'i':
        batch = false;
        break;

      case 'o':
        overlap_check = true;
        break;

      case 'p':
        precision = atoi(optarg);
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

  G4SteppingVerbose::UseBestUnit(precision);

  if (overlap_check) {
    G4StateManager::GetStateManager()->SetExceptionHandler(new NexusExceptionHandler());
  }

  NexusApp* app = new NexusApp(macro_filename);
  app->Initialize();

  G4UImanager* UI = G4UImanager::GetUIpointer();

  if (overlap_check) {
    UI->ApplyCommand("/geometry/test/resolution 1000000");
    UI->ApplyCommand("/geometry/test/run");
  }

  // if (seed < 0) CLHEP::HepRandom::setTheSeed(time(0));
  // else CLHEP::HepRandom::setTheSeed(seed);

  // CLHEP::HepRandom::showEngineStatus();

  // visual mode
  if (!batch) {
    std::unique_ptr<G4UIExecutive> ui{new G4UIExecutive{1, argv}};
    std::unique_ptr<G4VisManager> visManager{new G4VisExecutive};
    visManager->Initialize();
    UI->ApplyCommand("/control/execute macros/vis.mac");
    ui->SessionStart();
  }
  else {
    app->BeamOn(nevents);
  }

  delete app;
  return EXIT_SUCCESS;
}
