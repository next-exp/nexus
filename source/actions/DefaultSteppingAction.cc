// ----------------------------------------------------------------------------
// nexus | DefaultSteppingAction.cc
//
// This class is the default stepping action of the NEXT simulation.
// It adds the deposited energy to the accumulator in the event action
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DefaultSteppingAction.h"
#include "DefaultEventAction.h"
#include "FactoryBase.h"
#include "IonizationElectron.h"

#include <G4OpticalPhoton.hh>
#include <G4ParticleDefinition.hh>
#include <G4EventManager.hh>
#include <G4Step.hh>

using namespace nexus;

REGISTER_CLASS(DefaultSteppingAction, G4UserSteppingAction)

DefaultSteppingAction::DefaultSteppingAction() : G4UserSteppingAction()
{
}

DefaultSteppingAction::~DefaultSteppingAction()
{
}

bool IsSensitive(const G4String &s) {
  return (s == "ACTIVE") ? true :
         (s == "BUFFER") ? true :
         (s == "EL_GAP")
    ;
}

void DefaultSteppingAction::UserSteppingAction(const G4Step* step)
{
  auto track = step->GetTrack();
  // Do nothing if the track is an optical photon or an ionization electron
  if (track->GetDefinition() ==    G4OpticalPhoton::Definition() ||
      track->GetDefinition() == IonizationElectron::Definition() )
    return;


  auto  pre_vol_name = step-> GetPreStepPoint()->GetTouchable()->GetVolume()->GetName();
  if (!IsSensitive(pre_vol_name)) return;

  auto post_vol_name = step->GetPostStepPoint()->GetTouchable()->GetVolume()->GetName();
  if (!IsSensitive(post_vol_name)) return;

  static auto event_action = static_cast<DefaultEventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());
  event_action->AddToEventEnergy(step->GetTotalEnergyDeposit());
}
