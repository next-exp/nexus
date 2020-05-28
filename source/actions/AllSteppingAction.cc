// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <gonzalo.martinez.lema@weizmann.ac.il>
//  Created: 01 May 2020
//
//  Copyright (c) 2020 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "AllSteppingAction.h"
#include "PersistencyManager.h"

#include <G4Step.hh>
#include <G4SteppingManager.hh>
#include <G4VPersistencyManager.hh>
#include <G4ProcessManager.hh>
#include <G4ParticleTable.hh>
#include <G4Geantino.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4VPhysicalVolume.hh>

using namespace nexus;



AllSteppingAction::AllSteppingAction():
G4UserSteppingAction(),
msg_(0),
selected_volumes_()
{
  msg_ = new G4GenericMessenger(this, "/Actions/AllSteppingAction/");

  msg_->DeclareMethod("select_particle",
                      &AllSteppingAction::AddSelectedParticle,
                      "add a new particle to select");

  msg_->DeclareMethod("select_volume",
                      &AllSteppingAction::AddSelectedVolume,
                      "add a new volume to select");

  PersistencyManager* pm = dynamic_cast<PersistencyManager*>
        (G4VPersistencyManager::GetPersistencyManager());

  pm->StoreSteps(true);

}



AllSteppingAction::~AllSteppingAction()
{
}



void AllSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4ParticleDefinition* pdef     = step->GetTrack()->GetDefinition();
  G4int                 track_id = step->GetTrack()->GetTrackID();

  if (!KeepParticle(pdef)) return;

  G4StepPoint* pre  = step->GetPreStepPoint();
  G4StepPoint* post = step->GetPostStepPoint();

  G4ThreeVector initial_pos = pre ->GetPosition();
  G4ThreeVector   final_pos = post->GetPosition();

  G4String initial_volume = pre ->GetTouchableHandle()->GetVolume()->GetName();
  G4String   final_volume = post->GetTouchableHandle()->GetVolume()->GetName();
  G4String      proc_name = post->GetProcessDefinedStep()->GetProcessName();

  if (!KeepVolume(initial_volume, final_volume))
    return;

  initial_volumes_[track_id].push_back(initial_volume);
    final_volumes_[track_id].push_back(  final_volume);
       proc_names_[track_id].push_back(     proc_name);

  initial_poss_   [track_id].push_back(initial_pos);
    final_poss_   [track_id].push_back(  final_pos);
}


void AllSteppingAction::AddSelectedParticle(G4String particle_name)
{
  G4ParticleDefinition* pdef = G4ParticleTable::GetParticleTable()->FindParticle(particle_name);
  if (!pdef) {
    G4String msg = "No particle description was found for particle name " + particle_name;
    G4Exception("[AllSteppingAction]", "AddSelectedParticle()", FatalException, msg);
  }
  selected_particles_.push_back(pdef);
}


void AllSteppingAction::AddSelectedVolume(G4String volume_name)
{
  selected_volumes_.push_back(volume_name);
}


G4bool AllSteppingAction::KeepParticle(G4ParticleDefinition* pdef)
{
  if (!selected_particles_.size()) return true;

  auto it = std::find(selected_particles_.begin(), selected_particles_.end(), pdef);
  return it != selected_particles_.end();
}


G4bool AllSteppingAction::KeepVolume(G4String& initial_volume, G4String& final_volume)
{
  if (!selected_volumes_.size()) return true;

  for (auto volume=selected_volumes_.begin(); volume != selected_volumes_.end(); volume++)
  {
    if (initial_volume.contains(*volume)) return true;
    if (  final_volume.contains(*volume)) return true;
  }

  return false;
}



void AllSteppingAction::Reset()
{
  initial_volumes_.clear();
    final_volumes_.clear();
       proc_names_.clear();

  initial_poss_   .clear();
    final_poss_   .clear();
}
