// ----------------------------------------------------------------------------
//  $Id$
//
///  \author   <javier.munoz.vidal@gmail.com>
///  \date     Oct 2019 
///  \version  $Id$
//
//  Copyright (c) 2019 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "OpticalTrackingAction.h"

#include "Trajectory.h"
#include "TrajectoryMap.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4Trajectory.hh>
#include <G4OpticalPhoton.hh>



using namespace nexus;



OpticalTrackingAction::OpticalTrackingAction(): G4UserTrackingAction()
{
}



OpticalTrackingAction::~OpticalTrackingAction()
{
}



void OpticalTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  // Create a new trajectory associated to the track.
  // N.B. If the processesing of a track is interrupted to be resumed
  // later on (to process, for instance, its secondaries) more than
  // one trajectory associated to the track will be created, but 
  // the event manager will merge them at some point.
  G4VTrajectory* trj = new Trajectory(track);
  
   // Set the trajectory in the tracking manager
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(trj);
 }



void OpticalTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  Trajectory* trj = (Trajectory*) TrajectoryMap::Get(track->GetTrackID());

  // Do nothing if the track has no associated trajectory in the map
  if (!trj) return;

  // Record final time and position of the track
  trj->SetFinalPosition(track->GetPosition());
  trj->SetFinalTime(track->GetGlobalTime());
  trj->SetTrackLength(track->GetTrackLength());

  if (track->GetDefinition() == G4OpticalPhoton::Definition()) {
    // If optical photon has no NextVolume (escaping from the world),
    // assign current volume as the decay one.
    if (track->GetNextVolume()) trj->SetDecayVolume(track->GetNextVolume()->GetName());
    else                        trj->SetDecayVolume(track->GetVolume()->GetName());
  }
  else
    trj->SetDecayVolume(track->GetVolume()->GetName());
}
