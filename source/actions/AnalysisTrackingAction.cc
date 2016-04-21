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
#include "IonizationElectron.h"

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



AnalysisTrackingAction::AnalysisTrackingAction(): G4UserTrackingAction(), file_no_(0)
{
  _msg = new G4GenericMessenger(this, "/Actions/AnalysisTrackingAction/");
  _msg->DeclareProperty("file_number", file_no_, "");

    /*
  cer = 0;
  // scint = 0;
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
  /*
  G4cout << "Tot Cerenkov = " << cer << G4endl;
  // G4cout << "Tot scintillation = " << scint << G4endl;
  */
  std::ostringstream file_number;
  file_number << file_no_;
  G4String filename = "ProductionCherLXe."+file_number.str()+".root";
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
  // Do nothing if the track is an optical photon or an ionization electron

  // if (track->GetDefinition() == G4Electron::Definition() ) 
    //   G4cout << "Electron"<< G4endl;
   
  if (track->GetDefinition() == G4OpticalPhoton::Definition() || 
      track->GetDefinition() == IonizationElectron::Definition()) {
    fpTrackingManager->SetStoreTrajectory(false);    
      
 
     if (track->GetCreatorProcess()->GetProcessName() == "Cerenkov") {
  	cer+= 1;
	//track->CalculateVelocityForOpticalPhoton()
	       
	//	G4cout << "Cherenkov"<< G4endl;
	//	if (track->GetPosition().z() < 0.) {
	//  	  G4double time = sqrt(track->GetPosition().x()*track->GetPosition().x() + track->GetPosition().y()*track->GetPosition().y() + track->GetPosition().z()*track->GetPosition().z())/c_light;
	  //	  G4cout << "Cher starts "<< track->GetKineticEnergy()/eV<< G4endl;
	  // 	  hCherEnergy->Fill(track->GetKineticEnergy()/eV);
	  hCherLambda->Fill(h_Planck*c_light/track->GetKineticEnergy()/nanometer);
	  //	}
  	//G4cout << "Cherenkov" << G4endl;
     } 
     
     else if (track->GetCreatorProcess()->GetProcessName() == "Scintillation") {
       scint += 1;
       //	hScintEnergy->Fill(track->GetKineticEnergy()/eV);
       hScintLambda->Fill(h_Planck*c_light/track->GetKineticEnergy()/nanometer);
       //	G4cout << "Scintillation" << G4endl;
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


    // if (track->GetCreatorProcess()->GetProcessName() == "Cerenkov") {
      
    // }

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
  trj->SetDecayVolume(track->GetVolume()->GetName());
}
