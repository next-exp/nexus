// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 15 March 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "PersistencyManager.h"

#include "Trajectory.h"

#include <G4GenericMessenger.hh>
#include <G4Event.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>

#include <irene/Event.h>
#include <irene/Particle.h>
#include <irene/RootWriter.h>

using namespace nexus;



PersistencyManager::PersistencyManager(): 
  G4VPersistencyManager(), _msg(0), _ready(false), _evt(0), _writer(0)
{
  _msg = new G4GenericMessenger(this, "/nexus/persistency/");
  _msg->DeclareMethod("outputFile", &PersistencyManager::OpenFile, "");
}



PersistencyManager::~PersistencyManager()
{
  delete _writer;
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
    _writer = new irene::RootWriter();
    _writer->Open(filename.data(), "RECREATE");
    _ready = true;
  }
  else {
    G4Exception("[PersistencyManager]", "OpenFile()", 
      JustWarning, "An output file was previously opened.");
  }
}



void PersistencyManager::CloseFile()
{
  if (!_writer || !_writer->IsOpen()) return;

  _writer->Close();
}



G4bool PersistencyManager::Store(const G4Event* event)
{
  // Create a new irene event
  irene::Event ievt(event->GetEventID());

  // Store the trajectories of the event as Irene particles
  StoreTrajectories(event, &ievt);

  


  // Add event to the tree
  //_evt = &ievt;
  std::cout << ievt << std::endl;
  _writer->Write(ievt);
  _evt = 0;

  return true;
}



void PersistencyManager::StoreTrajectories(const G4Event* event,
                                           irene::Event* ievent)
{
  // Loop through the trajectories stored in the event
  G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
  for (unsigned int i=0; i<tc->size(); ++i) {

    G4cerr << "i = " << i << G4endl;

    Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
    if (!trj) continue;
    G4cerr << "i = " << i << G4endl;
    irene::Particle* ipart = new irene::Particle(trj->GetPDGEncoding());

    ievent->AddParticle(ipart);
    //delete ipart;
  }
}



G4bool PersistencyManager::Store(const G4Run*)
{
  // TODO. Implement here the persistency of the configuration macros
  return true;
}
