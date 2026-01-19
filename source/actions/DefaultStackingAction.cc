// ----------------------------------------------------------------------------
// nexus | DefaultStackingAction.cc
//
// This class is an example of how to implement a stacking action, if needed.
// At the moment, it is not used in the NEXT simulations.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------


#include "DefaultStackingAction.h"
#include "DefaultEventAction.h"
#include "FactoryBase.h"
#include "IonizationElectron.h"

#include <G4OpticalPhoton.hh>
#include <G4EventManager.hh>
#include <G4Track.hh>

using namespace nexus;

REGISTER_CLASS(DefaultStackingAction, G4UserStackingAction)

DefaultStackingAction::DefaultStackingAction(): G4UserStackingAction(), stage_()
{
}



DefaultStackingAction::~DefaultStackingAction()
{
}



G4ClassificationOfNewTrack
DefaultStackingAction::ClassifyNewTrack(const G4Track* track)
{
  // Delay photon and ionization electron propagation during the first stage
  // only. This means that they are only delayed once.
  // For instance, using a gamma source, the gamma is propagated; if it
  // interacts producing an electron, it is propagated too. Optical photons and
  // ionization electrons produced while the main electron is propagated are
  // delayed. When everything that is not an optical photon or an ionization
  // electron is tracked, we go to the next stage, i.e. propagating the
  // particles that we have kept on hold. In this stage we don't delay particle
  // propagation any further and we track all of them eagerly.

  if (stage_> 0) return G4ClassificationOfNewTrack::fUrgent;

  static auto opticalphoton = G4OpticalPhoton::Definition();
  static auto ielectron     = IonizationElectron::Definition();
  auto pdef = track->GetParticleDefinition();

  return (pdef == opticalphoton) || (pdef == ielectron) ?
    G4ClassificationOfNewTrack::fWaiting                :
    G4ClassificationOfNewTrack::fUrgent                 ;
}



void DefaultStackingAction::NewStage()
{
  /// At this point, tracks in the waiting stack have already been moved
  /// to the urgent stack, so if the deposited energy does not fall in
  /// the specified range, we clear them
  static auto event_action =
    static_cast<DefaultEventAction*>(G4EventManager::GetEventManager() -> GetUserEventAction());

  if (!event_action -> IsDepositedEnergyInRange())
    stackManager -> ClearUrgentStack();

  stage_++;
}



void DefaultStackingAction::PrepareNewEvent()
{
  stage_ = 0;
  return;
}
