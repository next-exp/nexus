// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <gonzalo.martinez.lema@weizmann.ac.il>
//  Created: 01 May 2020
//
//  Copyright (c) 2020 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "GeantinoSteppingAction.h"

#include <G4Step.hh>
#include <G4SteppingManager.hh>
#include <G4ProcessManager.hh>
#include <G4Geantino.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4VPhysicalVolume.hh>

using namespace nexus;



GeantinoSteppingAction::GeantinoSteppingAction():
G4UserSteppingAction(),
msg_(0),
selected_volumes_()
{
  msg_ = new G4GenericMessenger(this, "/Actions/GeantinoSteppingAction/");

  msg_->DeclareMethod("select_volume",
                      &GeantinoSteppingAction::AddSelectedVolume,
                      "add a new volume to select");
}



GeantinoSteppingAction::~GeantinoSteppingAction()
{
}



void GeantinoSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4ParticleDefinition* pdef     = step->GetTrack()->GetDefinition();
  G4int                 track_id = step->GetTrack()->GetTrackID();

  if (pdef != G4Geantino::Definition()) return;

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


void GeantinoSteppingAction::AddSelectedVolume(G4String volume_name)
{
  selected_volumes_.push_back(volume_name);
}


G4bool GeantinoSteppingAction::KeepVolume(G4String& initial_volume, G4String& final_volume)
{
  if (!selected_volumes_.size()) return true;

  for (auto volume=selected_volumes_.begin(); volume != selected_volumes_.end(); volume++)
  {
    if (initial_volume == *volume) return true;
    if (  final_volume == *volume) return true;
  }

  return false;
}


void GeantinoSteppingAction::Reset()
{
  initial_volumes_.clear();
    final_volumes_.clear();
       proc_names_.clear();

  initial_poss_   .clear();
    final_poss_   .clear();
}
