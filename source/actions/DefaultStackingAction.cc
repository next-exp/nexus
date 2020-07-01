// ----------------------------------------------------------------------------
// nexus | DefaultStackingAction.cc
//
// This class is an example of how to implement a stacking action, if needed.
// At the moment, it is not used in the NEXT simulations.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DefaultStackingAction.h"


using namespace nexus;



DefaultStackingAction::DefaultStackingAction(): G4UserStackingAction()
{
}



DefaultStackingAction::~DefaultStackingAction()
{
}



G4ClassificationOfNewTrack
DefaultStackingAction::ClassifyNewTrack(const G4Track* /*track*/)
{
  return fUrgent;
}



void DefaultStackingAction::NewStage()
{
  return;
}



void DefaultStackingAction::PrepareNewEvent()
{
  return;
}
