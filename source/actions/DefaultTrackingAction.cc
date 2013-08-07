// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 25 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "DefaultTrackingAction.h"

#include "Trajectory.h"
#include "TrajectoryMap.h"

#include <G4Track.hh>
#include <G4ParticleDefinition.hh>
#include <G4OpticalPhoton.hh>
#include <G4TrackingManager.hh>
#include <G4Trajectory.hh>

using namespace nexus;



DefaultTrackingAction::DefaultTrackingAction(): G4UserTrackingAction()
{

}



DefaultTrackingAction::~DefaultTrackingAction()
{
}



void DefaultTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  if (track->GetDefinition() == G4OpticalPhoton::Definition()) return;

  // Create a new trajectory associated to the current track 
  // and register it in the map
  G4VTrajectory* trj = new Trajectory(track);
  TrajectoryMap::Add(trj);

  // Set the trajectory in the tracking manager
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(trj);
}



void DefaultTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  Trajectory* trj = (Trajectory*) fpTrackingManager->GimmeTrajectory();
  trj->SetFinalPosition(track->GetPosition());
  trj->SetFinalTime(track->GetGlobalTime());
}