// ----------------------------------------------------------------------------
// nexus | DefaultRunAction.cc
//
// This is the default run action of the NEXT simulations.
// A message at the beginning and at the end of the simulation is printed.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DefaultRunAction.h"

#include <G4Run.hh>

using namespace nexus;



DefaultRunAction::DefaultRunAction(): G4UserRunAction()
{
}



DefaultRunAction::~DefaultRunAction()
{
}



void DefaultRunAction::BeginOfRunAction(const G4Run* run)
{
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
}


void DefaultRunAction::EndOfRunAction(const G4Run* run)
{
  G4cout << "### Run " << run->GetRunID() << " end." << G4endl;
}
