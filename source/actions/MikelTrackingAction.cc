// ----------------------------------------------------------------------------
//  $Id: MikelTrackingAction.cc 2013-08-28 minegui $
//
//  Author : <miquel.nebot@ific.uv.es>    
//  Created: 28 August 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "MikelTrackingAction.h"

#include "Trajectory.h"
#include "TrajectoryMap.h"
#include "IonizationElectron.h"

#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4Trajectory.hh>
#include <G4ParticleDefinition.hh>
#include <G4OpticalPhoton.hh>
#include <G4Gamma.hh>

#include <TFile.h>
#include <TH1F.h>


using namespace nexus;



MikelTrackingAction::MikelTrackingAction(): G4UserTrackingAction()
{

  GammaEnergy = new TH1F("h ","Gammas emitted by ;Entries/bin;Energy(MeV)",300,0,3);
}



MikelTrackingAction::~MikelTrackingAction()
{
  TFile *f1=new TFile("./histo/Gammas_Th238.root","RECREATE");
  f1->cd();
  GammaEnergy->Write();
  //std::cout<<"writen"<<std::endl;
}



void MikelTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  if (track->GetDefinition() == G4Gamma::Definition()) {
    
    G4double kinE = track->GetKineticEnergy();
    // G4double myenergy = 10. * MeV; //Remember base units of Geant4
    GammaEnergy->Fill(kinE/MeV);
    //std::cout<<kinE<<"!!!!!!"<<std::endl;
  }
    
  // Do nothing if the track is an optical photon or an ionization electron
  if (track->GetDefinition() == G4OpticalPhoton::Definition() || 
      track->GetDefinition() == IonizationElectron::Definition()) return;

  // Create a new trajectory associated to the track.
  // N.B. If the processesing of a track is interrupted to be resumed
  // later on (to process, for instance, its secondaries) more than
  // one trajectory associated to the track will be created, but 
  // the event manager will merge them at some point.
  G4VTrajectory* trj = new Trajectory(track);
  
  // We register only the first trajectory of a track in the trajectory map
  //if (!TrajectoryMap::Get(track->GetTrackID()))
  //TrajectoryMap::Add(trj);

  // Set the trajectory in the tracking manager
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(trj);
 }



void MikelTrackingAction::PostUserTrackingAction(const G4Track* track)
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
  trj->SetDecayVolume(track->GetVolume()->GetName());


}

