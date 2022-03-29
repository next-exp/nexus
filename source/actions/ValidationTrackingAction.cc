// ----------------------------------------------------------------------------
// nexus | ValidationTrackingAction.cc
//
// This class is based on DefaultTrackinAction.
// In addition, it creates and saves ROOT histograms with the energy
// of the gammas produced in the simulation. Its purpose is to produce
// histograms to be compared across different versions of GEANT4,
// before changing version.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ValidationTrackingAction.h"

#include "Trajectory.h"
#include "TrajectoryMap.h"
#include "IonizationElectron.h"
#include "FactoryBase.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4Trajectory.hh>
#include <G4OpticalPhoton.hh>
#include <G4Gamma.hh>



using namespace nexus;

REGISTER_CLASS(ValidationTrackingAction, G4UserTrackingAction)

ValidationTrackingAction::ValidationTrackingAction(): G4UserTrackingAction()
{

  // Get analysis manager
  fG4AnalysisMan_ = G4AnalysisManager::Instance();
  
  // Create histogram(s)
  fG4AnalysisMan_->CreateH1("GammaEnergy","GammaEnergy", 10000, 0, 4000.);
  fG4AnalysisMan_->SetH1XAxisTitle(0, "Energy (keV)");
  fG4AnalysisMan_->SetH1YAxisTitle(0, "Entries");
}



ValidationTrackingAction::~ValidationTrackingAction()
{

  // Open an output file and write histogram to file
  fG4AnalysisMan_->OpenFile("GammaEnergy.root");
  fG4AnalysisMan_->Write();
  fG4AnalysisMan_->CloseFile();
}



void ValidationTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  // Do nothing if the track is an optical photon or an ionization electron
  if (track->GetDefinition() == G4OpticalPhoton::Definition() ||
      track->GetDefinition() == IonizationElectron::Definition()) {
      fpTrackingManager->SetStoreTrajectory(false);
      return;
  }

  if (track->GetDefinition() == G4Gamma::Definition()) {
    fG4AnalysisMan_->FillH1(0, track->GetKineticEnergy()/CLHEP::keV);
  }

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



void ValidationTrackingAction::PostUserTrackingAction(const G4Track* track)
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
