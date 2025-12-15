// ----------------------------------------------------------------------------
// nexus | DefaultStackingAction.cc
//
// This class is an example of how to implement a stacking action, if needed.
// At the moment, it is not used in the NEXT simulations.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------


#include "DefaultStackingAction.h"
#include "FactoryBase.h"
#include "IonizationElectron.h"

#include <G4OpticalPhoton.hh>
#include <G4Track.hh>

using namespace nexus;

REGISTER_CLASS(DefaultStackingAction, G4UserStackingAction)

DefaultStackingAction::DefaultStackingAction(): G4UserStackingAction()
{
}



DefaultStackingAction::~DefaultStackingAction()
{
}



G4ClassificationOfNewTrack
DefaultStackingAction::ClassifyNewTrack(const G4Track* track)
{
  static auto opticalphoton = G4OpticalPhoton::Definition();
  static auto ielectron     = IonizationElectron::Definition();
  auto pdef = track->GetParticleDefinition();

  return (pdef == opticalphoton) || (pdef == ielectron) ?
    G4ClassificationOfNewTrack::fWaiting                :
    G4ClassificationOfNewTrack::fUrgent                 ;
}



void DefaultStackingAction::NewStage()
{
  return;
}



void DefaultStackingAction::PrepareNewEvent()
{
  return;
}
