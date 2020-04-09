// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>
//  Created: 25 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
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
  _msg = new G4GenericMessenger(this, "/Actions/AnalysisTrackingAction/");
  _msg->DeclareProperty("file_name", file_name_, "");
  _msg->DeclareProperty("file_number", file_no_, "");

  /*
  hCherEnergy = new TH1F("CherEnergy", "CherEnergy", 1000, 0, 10.);
  hCherEnergy->GetXaxis()->SetTitle("energy (eV)");

  hScintEnergy = new TH1F("ScintEnergy", "ScintEnergy", 1000, 0, 10.);
  hScintEnergy->GetXaxis()->SetTitle("energy (eV)");
  */

  hCherLambda = new TH1F("CherLambdaProd", "Production wavelength", 1000, 0, 1500.);
  hCherLambda->GetXaxis()->SetTitle("wavelength (nm)");

  hScintLambda = new TH1F("ScintLambda", "ScintLambda", 1000, 0, 800.);
  hScintLambda->GetXaxis()->SetTitle("wavelength (nm)");

}



AnalysisTrackingAction::~AnalysisTrackingAction()
{
  std::ostringstream file_number;
  file_number << file_no_;
  G4String filename = file_name_+"."+file_number.str()+".root";
  Times = new TFile(filename, "recreate");
  // hCherEnergy->Write();
  // hScintEnergy->Write();
  hCherLambda->Write();
  hScintLambda->Write();
  Times->Close();

}



void AnalysisTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  // if ( track->GetCreatorProcess())
  //   G4cout << track->GetCreatorProcess()->GetProcessName()  << G4endl;
  // Do nothing if the track is an optical photon

  if (track->GetDefinition() == G4OpticalPhoton::Definition()) {
    fpTrackingManager->SetStoreTrajectory(false);

     if (track->GetCreatorProcess()->GetProcessName() == "Cerenkov") {
       //track->CalculateVelocityForOpticalPhoton()
       hCherLambda->Fill(h_Planck*c_light/track->GetKineticEnergy()/nanometer);
     }

     else if (track->GetCreatorProcess()->GetProcessName() == "Scintillation") {
       hScintLambda->Fill(h_Planck*c_light/track->GetKineticEnergy()/nanometer);
     }

      return;
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
  // Do nothing if the track is an optical photon or an ionization electron
  if (track->GetDefinition() == G4OpticalPhoton::Definition()) return;

  Trajectory* trj = (Trajectory*) TrajectoryMap::Get(track->GetTrackID());

  // Do nothing if the track has no associated trajectory in the map
  if (!trj) return;

  // Record final time and position of the track
  trj->SetFinalPosition(track->GetPosition());
  trj->SetFinalTime(track->GetGlobalTime());
  trj->SetTrackLength(track->GetTrackLength());
  trj->SetDecayVolume(track->GetVolume()->GetName());
}
