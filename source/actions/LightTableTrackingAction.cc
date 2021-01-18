// ----------------------------------------------------------------------------
// nexus | LightTableTrackingAction.cc
//
// This class is the tracking action to be used when runing simulations
// to create the LightTables.
// It stores in memory just the trajectory of the first photon of each event
// as its initial vertex is shared by all of them.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------


#include "LightTableTrackingAction.h"

#include "Trajectory.h"
#include "TrajectoryMap.h"
#include "IonizationElectron.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4Trajectory.hh>
#include <G4ParticleDefinition.hh>
#include <G4OpticalPhoton.hh>



using namespace nexus;



LightTableTrackingAction::LightTableTrackingAction(): G4UserTrackingAction()
{
}



LightTableTrackingAction::~LightTableTrackingAction()
{
}



void LightTableTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  if (track->GetTrackID() == 1) {
    G4VTrajectory* trj = new Trajectory(track);
    fpTrackingManager->SetStoreTrajectory(true);
    fpTrackingManager->SetTrajectory(trj);
  }
}



void LightTableTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  G4int track_id = track->GetTrackID();

  if (track_id != 1) return;

  Trajectory* trj = (Trajectory*) TrajectoryMap::Get(track_id);
  // Record final time and position of the track
  trj->SetFinalPosition(track->GetPosition());
  trj->SetFinalTime(track->GetGlobalTime());
  trj->SetTrackLength(track->GetTrackLength());
  trj->SetFinalMomentum(track->GetMomentum());

  if (track->GetNextVolume()) trj->SetFinalVolume(track->GetNextVolume()->GetName());
  else                        trj->SetFinalVolume(track->GetVolume()->GetName());

  // Record last process of the track
  G4String proc_name = track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  trj->SetFinalProcess(proc_name);
}
