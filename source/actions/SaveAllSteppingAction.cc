// ----------------------------------------------------------------------------
// nexus | SaveAllSteppingAction.cc
//
// This class adds a new group and table to the output file, "/DEBUG/steps".
// This table contains information (position and volume of both the
// pre- and post-step points, average time, process name and other identifiers)
// of some steps of the simulation. By default all steps are stored. However,
// a subset of them can be selected by cherry-picking the volumes and particles
// involved in the step. This can be achieved with the commands
// /Actions/SaveAllSteppingAction/select_particle
// and
// /Actions/SaveAllSteppingAction/select_volume
// without the need for re-compilation.
// It must be noted that the files produced with this action become large
// very quickly. Therefore, strict filtering and small number of events are
// encouraged.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SaveAllSteppingAction.h"
#include "PersistencyManager.h"
#include "FactoryBase.h"

#include <G4Step.hh>
#include <G4VPersistencyManager.hh>
#include <G4ProcessManager.hh>
#include <G4ParticleTable.hh>

using namespace nexus;

REGISTER_CLASS(SaveAllSteppingAction, G4UserSteppingAction)

SaveAllSteppingAction::SaveAllSteppingAction():
G4UserSteppingAction(),
msg_(0),
selected_volumes_(),
selected_particles_(),
initial_volumes_(),
final_volumes_(),
proc_names_(),
initial_poss_(),
final_poss_(),
times_(),
kill_after_selection_(false)
{
  msg_ = new G4GenericMessenger(this, "/Actions/SaveAllSteppingAction/");

  msg_->DeclareMethod("select_particle",
                      &SaveAllSteppingAction::AddSelectedParticle,
                      "Add a new particle to select");

  msg_->DeclareMethod("select_volume", &SaveAllSteppingAction::AddSelectedVolume,
                      "Add a new volume to select");

  msg_->DeclareProperty("kill_after_selection", kill_after_selection_,
                        "Whether to kill a particle after a step has been selected");

  PersistencyManager* pm = dynamic_cast<PersistencyManager*>
        (G4VPersistencyManager::GetPersistencyManager());

  pm->StoreSteps(true);

}



SaveAllSteppingAction::~SaveAllSteppingAction()
{
}



void SaveAllSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4ParticleDefinition* pdef          = step->GetTrack()->GetDefinition();
  G4int                 track_id      = step->GetTrack()->GetTrackID();
  G4String              particle_name = pdef->GetParticleName();

  if (!KeepParticle(pdef)) return;

  G4StepPoint* pre  = step->GetPreStepPoint();
  G4StepPoint* post = step->GetPostStepPoint();

  G4ThreeVector initial_pos = pre ->GetPosition();
  G4ThreeVector   final_pos = post->GetPosition();
  G4double        step_time = (pre->GetGlobalTime()  +
                              post->GetGlobalTime()) / 2.;

  if (! post->GetTouchableHandle()->GetVolume()) return; // Particle exits the world

  G4String initial_volume = pre ->GetTouchableHandle()->GetVolume()->GetName();
  G4String   final_volume = post->GetTouchableHandle()->GetVolume()->GetName();
  G4String      proc_name = post->GetProcessDefinedStep()->GetProcessName();

  if (!KeepVolume(initial_volume, final_volume))
    return;

  std::pair<G4int, G4String> key = std::make_pair(track_id, particle_name);

  initial_volumes_[key].push_back(initial_volume);
    final_volumes_[key].push_back(  final_volume);
       proc_names_[key].push_back(     proc_name);

  initial_poss_   [key].push_back(initial_pos);
    final_poss_   [key].push_back(  final_pos);
         times_   [key].push_back(  step_time);

  if (kill_after_selection_)
    step->GetTrack()->SetTrackStatus(fStopAndKill);
}


void SaveAllSteppingAction::AddSelectedParticle(G4String particle_name)
{
  G4ParticleDefinition* pdef = G4ParticleTable::GetParticleTable()->FindParticle(particle_name);
  if (!pdef) {
    G4String msg = "No particle description was found for particle name " + particle_name;
    G4Exception("[SaveAllSteppingAction]", "AddSelectedParticle()", FatalException, msg);
  }
  selected_particles_.push_back(pdef);
}


void SaveAllSteppingAction::AddSelectedVolume(G4String volume_name)
{
  selected_volumes_.push_back(volume_name);
}


G4bool SaveAllSteppingAction::KeepParticle(G4ParticleDefinition* pdef)
{
  if (!selected_particles_.size()) return true;

  auto it = std::find(selected_particles_.begin(), selected_particles_.end(), pdef);
  return it != selected_particles_.end();
}


G4bool SaveAllSteppingAction::KeepVolume(G4String& initial_volume, G4String& final_volume)
{
  if (!selected_volumes_.size()) return true;

  for (auto volume=selected_volumes_.begin(); volume != selected_volumes_.end(); volume++)
  {
    if (G4StrUtil::contains(initial_volume, *volume)) return true;
    if (G4StrUtil::contains(  final_volume, *volume)) return true;
  }

  return false;
}



void SaveAllSteppingAction::Reset()
{
  initial_volumes_.clear();
    final_volumes_.clear();
       proc_names_.clear();

  initial_poss_   .clear();
    final_poss_   .clear();
         times_   .clear();
}
