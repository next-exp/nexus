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

#include <TTree.h>
#include <TFile.h>

#include <irene/Event.h>


using namespace nexus;



PersistencyManager::PersistencyManager(): 
  G4VPersistencyManager(), _file(0), _evttree(0), _evt(0)
{
  _msg = new G4GenericMessenger(this, "/nexus/persistency/");
  //_msg->DeclareProperty("output_file", _filename, "");
}



PersistencyManager::~PersistencyManager()
{
}



void PersistencyManager::Initialize()
{
  PersistencyManager* current = dynamic_cast<PersistencyManager*>(G4VPersistencyManager::GetPersistencyManager());

  if (!current) current = new PersistencyManager();
}



void PersistencyManager::OpenFile()
{
  if (_file) delete _file;

  G4String filename = "";

  _file = new TFile(filename.c_str(), "RECREATE");
  _evttree = new TTree("event", "event tree");
  _evttree->Branch("EventBranch", "irene::Event", &_evt);
}



void PersistencyManager::CloseFile()
{
  if (!_file) return;
  _file->Write();
  _file->Close();
}


G4bool PersistencyManager::Store(const G4Event* event)
{
  G4cout  << "Trajectories: " << event->GetTrajectoryContainer()->size()
          << G4endl;

  G4VTrajectory* traj = (*event->GetTrajectoryContainer())[0];

  G4cout  << "Steps given by primary particle: " 
          << traj->GetPointEntries()
          << G4endl;

  // if (_evt) delete _evt;

  // _evt = new irene::Event();

  // _evttree->Fill();

  return true;
}


G4bool PersistencyManager::Store(const G4Run*)
{
  G4cout << "the run" << G4endl;
  return true;
}


G4bool PersistencyManager::Store(const G4VPhysicalVolume*)
{ return true; }