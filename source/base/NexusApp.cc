// ----------------------------------------------------------------------------
// nexus | NexusApp.cc
//
// This class is the run manager of the nexus simulation. It takes care of
// setting up the simulation (geometry, physics lists, generators, actions),
// so that it is ready to be run.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NexusApp.h"

#include "BatchSession.h"
#include "GeometryBase.h"
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"
#include "FactoryBase.h"

#include <G4GenericPhysicsList.hh>
#include <G4UImanager.hh>
#include <G4StateManager.hh>
#include <G4VPrimaryGenerator.hh>
#include <G4VPersistencyManager.hh>
#include <G4UserRunAction.hh>
#include <G4UserEventAction.hh>
#include <G4UserTrackingAction.hh>
#include <G4UserSteppingAction.hh>
#include <G4UserStackingAction.hh>

using namespace nexus;
using std::make_unique;
using std::unique_ptr;


NexusApp::NexusApp(G4String init_macro): G4RunManager(), gen_name_(""),
                                         geo_name_(""), pm_name_(""),
                                         runact_name_(""), evtact_name_(""),
                                         stepact_name_(""), trkact_name_(""),
                                         stkact_name_("")
{
  // Create and configure a generic messenger for the app
  msg_ = make_unique<G4GenericMessenger>(this, "/nexus/", "Nexus control commands.");

  // Define the command to register a configuration macro.
  // The user may invoke the command as many times as needed.
  msg_->DeclareMethod("RegisterMacro", &NexusApp::RegisterMacro, "");

  // Some commands, which we call 'delayed', only work if executed
  // after the initialization of the application. The user may include
  // them in configuration macros registered with the command defined below.
  msg_->DeclareMethod("RegisterDelayedMacro",
                      &NexusApp::RegisterDelayedMacro, "");

  // Define a command to set a seed for the random number generator.
  msg_->DeclareMethod("random_seed", &NexusApp::SetRandomSeed,
                      "Set a seed for the random number generator.");

// Define the command to set the desired generator
  msg_->DeclareProperty("RegisterGenerator", gen_name_, "");

  // Define the command to set the desired geometry
  msg_->DeclareProperty("RegisterGeometry", geo_name_, "");

// Define the command to set the desired persistency manager
  msg_->DeclareProperty("RegisterPersistencyManager", pm_name_, "");

// Define the commands to set the desired actions
  msg_->DeclareProperty("RegisterRunAction", runact_name_, "");
  msg_->DeclareProperty("RegisterEventAction", evtact_name_, "");
  msg_->DeclareProperty("RegisterSteppingAction", stepact_name_, "");
  msg_->DeclareProperty("RegisterTrackingAction", trkact_name_, "");
  msg_->DeclareProperty("RegisterStackingAction", stkact_name_, "");


  /////////////////////////////////////////////////////////

  // We will set now the user initialization class instances
  // in the run manager. In order to do so, we use our own factory
  // so that the messenger commands are already defined
  // by the time we process the initialization macro.

  // The physics lists are handled with Geant4's own 'factory'
  // The generic physics list must be initialized before
  // processing the init macro, where the physics lists are registered
  auto pl = make_unique<G4GenericPhysicsList>();

  BatchSession(init_macro.c_str()).SessionStart();

  // Set the physics list in the run manager
  this->SetUserInitialization(pl.release());

  // Set the detector construction instance in the run manager
  auto dc = make_unique<DetectorConstruction>();
  if (geo_name_ == "") {
    G4Exception("[NexusApp]", "NexusApp()", FatalException, "A geometry must be specified.");
  }
  dc->SetGeometry(ObjFactory<GeometryBase>::Instance().CreateObject(geo_name_));
  this->SetUserInitialization(dc.release());

  // Set the primary generation instance in the run manager
  auto pg = make_unique<PrimaryGeneration>();
  if (gen_name_ == "") {
    G4Exception("[NexusApp]", "NexusApp()", FatalException, "A generator must be specified.");
  }
  pg->SetGenerator(ObjFactory<G4VPrimaryGenerator>::Instance().CreateObject(gen_name_));
  this->SetUserAction(pg.release());

  if (pm_name_ == "") {
    G4Exception("[NexusApp]", "NexusApp()", FatalException, "A persistency manager must be specified.");
  }
  pm_ = ObjFactory<PersistencyManagerBase>::Instance().CreateObject(pm_name_);
  pm_->SetMacros(init_macro, macros_, delayed_);

 // PersistencyManager::Initialize(init_macro, macros_, delayed_);

  // Set the user action instances, if any, in the run manager
  if (runact_name_ != "") {
    auto runact = ObjFactory<G4UserRunAction>::Instance().CreateObject(runact_name_);
    this->SetUserAction(runact.release());
  }

  if (evtact_name_ != "") {
    auto evtact = ObjFactory<G4UserEventAction>::Instance().CreateObject(evtact_name_);
    this->SetUserAction(evtact.release());
  }

  if (stkact_name_ != "") {
    auto stkact = ObjFactory<G4UserStackingAction>::Instance().CreateObject(stkact_name_);
    this->SetUserAction(stkact.release());
  }

  if (trkact_name_ != "") {
    auto trkact = ObjFactory<G4UserTrackingAction>::Instance().CreateObject(trkact_name_);
    this->SetUserAction(trkact.release());
  }

  if (stepact_name_ != "") {
    auto stepact = ObjFactory<G4UserSteppingAction>::Instance().CreateObject(stepact_name_);
    this->SetUserAction(stepact.release());
  }


  /////////////////////////////////////////////////////////

  // Set by default a random seed (system time) for the random
  // number generator
  SetRandomSeed(-1);

}



NexusApp::~NexusApp()
{
  // Close output file before finishing
  pm_->CloseFile();
}



void NexusApp::RegisterMacro(G4String macro)
{
  // Store the name of the macro file
  macros_.push_back(macro);
}



void NexusApp::RegisterDelayedMacro(G4String macro)
{
  // Store the name of the macro file
  delayed_.push_back(macro);
}



void NexusApp::Initialize()
{
  // Execute all command macro files before initializing the app
  // so that all objects get configured
  // G4UImanager* UI = G4UImanager::GetUIpointer();

  for (unsigned int i=0; i<macros_.size(); i++) {
    ExecuteMacroFile(macros_[i].data());
  }

  G4RunManager::Initialize();

  for (unsigned int j=0; j<delayed_.size(); j++) {
    ExecuteMacroFile(delayed_[j].data());
  }

  // Execute command to enable triggering of sensitive detectors.
  // If the optical physics is not loaded, it is not applied,
  // but no error is raised.
  G4UImanager* UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("/process/optical/boundary/setInvokeSD true");
}



void NexusApp::ExecuteMacroFile(const char* filename)
{
  G4UImanager* UI = G4UImanager::GetUIpointer();
  auto batchSession = make_unique<BatchSession>(filename, UI->GetSession());
  UI->SetSession(batchSession.get());
  G4UIsession* previousSession = batchSession->SessionStart();
  UI->SetSession(previousSession);
}



void NexusApp::SetRandomSeed(G4int seed)
{
  // Set the seed chosen by the user for the pseudo-random number
  // generator unless a negative number was provided, in which case
  // we will set as seed the system time.
  if (seed < 0) CLHEP::HepRandom::setTheSeed(time(0));
  else CLHEP::HepRandom::setTheSeed(seed);
}
