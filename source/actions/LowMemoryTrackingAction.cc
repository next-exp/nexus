// ----------------------------------------------------------------------------
// nexus | LowMemoryTrackingAction.cc
//
// This class is a tracking action of the NEXT simulation.
// It stores in memory the trajectories of particles -- with the exception of
// optical photons, ionization electrons, and electrons starting in
// Rock or water -- with the relevant tracking information that will be
// saved to the output file. First version useful for TonneScale with no
// Cerenkov studies (June 2020).
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------


#include "LowMemoryTrackingAction.h"

#include "Trajectory.h"
#include "TrajectoryMap.h"
#include "IonizationElectron.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4Trajectory.hh>
#include <G4ParticleDefinition.hh>
#include <G4OpticalPhoton.hh>
#include <G4Neutron.hh>



using namespace nexus;



LowMemoryTrackingAction::LowMemoryTrackingAction(): G4UserTrackingAction()
{
}



LowMemoryTrackingAction::~LowMemoryTrackingAction()
{
}



void LowMemoryTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  // Do nothing if the track is an optical photon or an ionization electron
  if (track->GetDefinition() == G4OpticalPhoton::Definition() ||
      track->GetDefinition() == IonizationElectron::Definition()) {
    fpTrackingManager->SetStoreTrajectory(false);
    return;
  }

  if (track->GetParentID()           == 0                       ||
      track->GetParticleDefinition() == G4Neutron::Definition() ||
      track->GetParticleDefinition()->IsGeneralIon()              ){

    // Create a new trajectory associated to the track.
    // N.B. If the processesing of a track is interrupted to be resumed
    // later on (to process, for instance, its secondaries) more than
    // one trajectory associated to the track will be created, but
    // the event manager will merge them at some point.
    G4VTrajectory* trj = new Trajectory(track);
    
    // Set the trajectory in the tracking manager
    fpTrackingManager->SetStoreTrajectory(true);
    fpTrackingManager->SetTrajectory(trj);
    return;
  }

  fpTrackingManager->SetStoreTrajectory(false);
 }



void LowMemoryTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  // Do nothing if the track is an optical photon or an ionization electron
  if (track->GetDefinition() == G4OpticalPhoton::Definition() ||
      track->GetDefinition() == IonizationElectron::Definition()) return;

  Trajectory* trj = (Trajectory*) TrajectoryMap::Get(track->GetTrackID());

  // Do nothing if the track has no associated trajectory in the map
  if (!trj) return;

  // Record final time and position of the track
  trj->SetFinalPosition(track->GetPosition());
  trj->SetFinalTime(track->GetGlobalTime());
  trj->SetTrackLength(track->GetTrackLength());
  trj->SetFinalVolume(track->GetVolume()->GetName());
  trj->SetFinalMomentum(track->GetMomentum());

  // Record last process of the track
  G4String proc_name = track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  trj->SetFinalProcess(proc_name);
}
