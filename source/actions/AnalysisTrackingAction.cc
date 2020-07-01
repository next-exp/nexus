// ----------------------------------------------------------------------------
// nexus | AnalysisTrackingAction.cc
//
// This class produces histograms of the wavelengths and the time of production
// of the optical photons.
//
// The  NEXT Collaboration
// ----------------------------------------------------------------------------

#include "AnalysisTrackingAction.h"

#include "Trajectory.h"
#include "TrajectoryMap.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4Trajectory.hh>
#include <G4ParticleDefinition.hh>
#include <G4OpticalPhoton.hh>
#include <G4Electron.hh>
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

#include "TH1F.h"
#include "TFile.h"

#include <iostream>
#include <sstream>

using namespace nexus;



AnalysisTrackingAction::AnalysisTrackingAction(): G4UserTrackingAction(),
                                                  file_name_("OpticalTracking"),
                                                  file_no_(0)
{
  msg_ = new G4GenericMessenger(this, "/Actions/AnalysisTrackingAction/");
  msg_->DeclareProperty("file_name", file_name_, "");
  msg_->DeclareProperty("file_number", file_no_, "");

  /*
  hCherEnergy_ = new TH1F("CherEnergy", "CherEnergy", 1000, 0, 10.);
  hCherEnergy_->GetXaxis()->SetTitle("energy (eV)");

  hScintEnergy_ = new TH1F("ScintEnergy", "ScintEnergy", 1000, 0, 10.);
  hScintEnergy_->GetXaxis()->SetTitle("energy (eV)");
  */

  hCherLambda_ = new TH1F("CherLambda", "CherLambda", 1000, 0, 1500.);
  hCherLambda_->GetXaxis()->SetTitle("wavelength (nm)");
  hCherLambda_->GetYaxis()->SetTitle("Entries / bin");

  hScintLambda_ = new TH1F("ScintLambda", "ScintLambda", 1000, 0, 800.);
  hScintLambda_->GetXaxis()->SetTitle("wavelength (nm)");
  hScintLambda_->GetYaxis()->SetTitle("Entries / bin");

  hScintTime = new TH1F("ScintillationTime", "ScintillationTime", 8000, 0, 40000.);
  hScintTime->GetXaxis()->SetTitle("Time (ps)");
  hScintTime->GetYaxis()->SetTitle("Entries / bin");

}



AnalysisTrackingAction::~AnalysisTrackingAction()
{
  std::ostringstream file_number;
  file_number << file_no_;
  G4String filename = file_name_+"."+file_number.str()+".root";
  OptPhotons_ = new TFile(filename, "recreate");
  // hCherEnergy_->Write();
  // hScintEnergy_->Write();
  hCherLambda_->Write();
  hScintLambda_->Write();
  hScintTime->Write();
  OptPhotons_->Close();

}



void AnalysisTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  // if ( track->GetCreatorProcess())
  //   G4cout << track->GetCreatorProcess()->GetProcessName()  << G4endl;
  // Do nothing if the track is an optical photon

  if (track->GetDefinition() == G4OpticalPhoton::Definition()) {

     if (track->GetCreatorProcess()->GetProcessName() == "Cerenkov") {
       //track->CalculateVelocityForOpticalPhoton()
       hCherLambda_->Fill(h_Planck*c_light/track->GetKineticEnergy()/nanometer);
     }

     else if (track->GetCreatorProcess()->GetProcessName() == "Scintillation") {
       hScintLambda_->Fill(h_Planck*c_light/track->GetKineticEnergy()/nanometer);
       hScintTime->Fill(track->GetGlobalTime()/picosecond);
     }
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



void AnalysisTrackingAction::PostUserTrackingAction(const G4Track* track)
{
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
