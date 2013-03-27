// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 15 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "PersistencyManager.h"

#include <G4GenericMessenger.hh>
#include <G4Event.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>

// #include <TTree.h>
// #include <TFile.h>

#include <irene/Event.h>
#include <irene/Particle.h>
#include <irene/RootWriter.h>

using namespace nexus;



PersistencyManager::PersistencyManager(): 
  G4VPersistencyManager(), _msg(0), _ready(false), 
  _file(0), _evttree(0), _evt(0)
{
  _msg = new G4GenericMessenger(this, "/nexus/persistency/");
  _msg->DeclareMethod("outputFile", &PersistencyManager::OpenFile, "");
}



PersistencyManager::~PersistencyManager()
{
  delete _evttree;
  delete _file;
  delete _msg;
}



void PersistencyManager::Initialize()
{
  // Get a pointer to the current singleton instance of the persistency
  // manager using the method of the base class
  PersistencyManager* current = dynamic_cast<PersistencyManager*>
    (G4VPersistencyManager::GetPersistencyManager());

  // If no instance exists yet, create a new one.
  // (Notice that the above dynamic cast would also return 0 if an instance
  // of another G4VPersistencyManager-derived was previously set, resulting
  // in the leak of that object since the pointer will no longer be
  // accessible.)
  if (!current) current = new PersistencyManager();
}



void PersistencyManager::OpenFile(const G4String& filename)
{
  // If the output file was not set yet, do so
  if (!_ready) {
    _file = new TFile(filename.data(), "RECREATE");
    _evttree = new TTree("EVENT", "Irene event tree");
    _evttree->Branch("EventBranch", "irene::Event", &_evt);
    _ready = true;
  }
  else {
    G4Exception("[PersistencyManager]", "OpenFile()", 
      JustWarning, "An output file was previously opened.");
  }
}



void PersistencyManager::CloseFile()
{
  if (!_file || !_file->IsOpen()) return;

  _file->Write();
  _file->Close();
}


G4bool PersistencyManager::Store(const G4Event* event)
{
  // Create a new irene event
  irene::Event ievt(event->GetEventID());


  // Store the trajectories of the event
  G4TrajectoryContainer* tc = event->GetTrajectoryContainer();

  for (size_t i=0; i<tc->size(); ++i) {

    G4Trajectory* trj = (G4Trajectory*) (*tc)[i];

    irene::Particle ipart(trj->GetPDGEncoding());

    G4ThreeVector position = trj->GetInitialMomentum();
    //ipart.SetInitialMomentum()

    

    ipart.SetParticleID(trj->GetTrackID());

    irene::Particle* ipartp = &ipart;

    ievt.AddParticle(ipartp);
  }

  // Add event to the tree
  _evt = &ievt;
  _evttree->Fill();
  _evt = 0;

  return true;
}


G4bool PersistencyManager::Store(const G4Run*)
{
  return true;
}
