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
#include "TrajectoryMap.h"
#include "IonizationSD.h"

#include <G4GenericMessenger.hh>
#include <G4Event.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>

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
    G4Exception("OpenFile()", "[PersistencyManager]", 
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
  StoreTrajectories(event->GetTrajectoryContainer(), &ievt);

  //StoreHits(event, &ievt);


  // Add event to the tree
  //_evt = &ievt;
  _writer->Write(ievt);
  _evt = 0;

  return true;
}



void PersistencyManager::StoreTrajectories(G4TrajectoryContainer* tc,
                                           irene::Event* ievent)
{
  // Loop through the trajectories stored in the container
  for (G4int i=0; i<tc->entries(); ++i) {

    Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
    if (!trj) continue;

    // Create an irene particle to store the trajectory information
    irene::Particle* ipart = new irene::Particle(trj->GetPDGEncoding());
    
    G4int trackid = trj->GetTrackID();
    ipart->SetParticleID(trj->GetTrackID());
    _iprtmap[trackid] = ipart;

      

    //ipart->SetInitialVertex();

    ievent->AddParticle(ipart);
  }

  // We'll set now the family relationships.
  // Loop through the particles we just stored in the irene event.
  TObjArray* iparts = ievent->GetParticles();
  for (unsigned int i=0; i<iparts->GetEntries(); ++i) {

    irene::Particle* ipart = (irene::Particle*) iparts->At(i);
    G4VTrajectory* trj = TrajectoryMap::Get(ipart->GetParticleID());
    int parent_id = trj->GetParentID();

    if (parent_id == 0) {
      ipart->SetPrimary(true);
    }
    else {
      irene::Particle* mother = _iprtmap[parent_id];
      ipart->SetMother(mother);
      mother->AddDaughter(ipart);
    }
  }
}



void PersistencyManager::StoreHits(const G4Event* evt, irene::Event* ievt)
{
  G4HCofThisEvent* hce = evt->GetHCofThisEvent();
  if (!hce) return;

  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
  G4HCtable* hct = sdmgr->GetHCtable();


  for (int i=0; i<hct->entries(); i++) {
    G4String sdname = hct->GetSDname(i);
    G4cout << "SD name: " << sdname << G4endl;
    G4String hcname = hct->GetHCname(i);
    G4cout << "HC name: " << hcname << G4endl;
    int hcid = sdmgr->GetCollectionID(sdname+"/"+hcname);
    G4VHitsCollection* hits = hce->GetHC(hcid);

    if (hcname == IonizationSD::GetCollectionUniqueName()) {
      G4cout << "IONIZATION HITS COLLECTION" << G4endl;
      IonizationHitsCollection* ihc = (IonizationHitsCollection*) hits;
      for (G4int j=0; j<ihc->entries(); j++) {
        G4cout << "j: " << j << G4endl;
        IonizationHit* ahit = (IonizationHit*) ihc->GetHit(j);
        G4cout << "Track ID: " << ahit->GetTrackID() << G4endl;
      } 
      
    }
  }
}



// void PersistencyManager::StoreIonizationHits(IonizationHitsCollection* hc,
//                                              irene::Event* ievt)
// {
//   for (G4int i=0; i<hc->entries(); i++) {
    
//     IonizationHit* hit = dynamic_cast<IonizationHit*>(hc->GetHit(i));
//     if (!hit) continue;

    
//   }
// }


G4bool PersistencyManager::Store(const G4Run*)
{
  // TODO. Implement here the persistency of the configuration macros
  return true;
}
