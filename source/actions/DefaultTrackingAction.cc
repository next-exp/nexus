// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 25 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "DefaultTrackingAction.h"

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
  G4ParticleDefinition* pdef = track->GetDefinition();

  if (pdef == G4OpticalPhoton::Definition()) return;

  G4VTrajectory* traj = new G4Trajectory(track);
  fpTrackingManager->SetTrajectory(traj);
  fpTrackingManager->SetStoreTrajectory(true);
  TrajectoryMap::Add(traj);
}



void DefaultTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  return;
}