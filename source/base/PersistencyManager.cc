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
#include "PmtSD.h"

#include <G4GenericMessenger.hh>
#include <G4Event.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>

#include <irene/Event.h>
#include <irene/Particle.h>
#include <irene/Track.h>
#include <irene/SensorHit.h>
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
  _iprtmap.clear(); 
  _itrkmap.clear();

  // Create a new irene event
  irene::Event ievt(event->GetEventID());

  // Store the trajectories of the event as Irene particles
  StoreTrajectories(event->GetTrajectoryContainer(), &ievt);

  StoreHits(event->GetHCofThisEvent(), &ievt);


  // Add event to the tree
  //_evt = &ievt;
  _writer->Write(ievt);
  _evt = 0;

  TrajectoryMap::Clear();

  return true;
}



void PersistencyManager::StoreTrajectories(G4TrajectoryContainer* tc,
                                           irene::Event* ievent)
{
  // If the pointer is null, no trajectories were stored in this event
  if (!tc) return;

  // Loop through the trajectories stored in the container
  for (G4int i=0; i<tc->entries(); ++i) {
    Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
    if (!trj) continue;

    // Create an irene particle to store the trajectory information
    irene::Particle* ipart = new irene::Particle(trj->GetPDGEncoding());
    
    G4int trackid = trj->GetTrackID();
    ipart->SetParticleID(trj->GetTrackID());
    _iprtmap[trackid] = ipart;

    G4double tracklength = trj->GetTrackLength();
    ipart->SetTrackLength(tracklength);

    G4ThreeVector xyz = trj->GetInitialPosition();
    G4double t = trj->GetInitialTime();
    ipart->SetInitialVertex(xyz.x(), xyz.y(), xyz.z(), t);      

    xyz = trj->GetFinalPosition();
    t = trj->GetFinalTime();
    ipart->SetDecayVertex(xyz.x(), xyz.y(), xyz.z(), t);

    G4String volume = trj->GetInitialVolume();
    ipart->SetInitialVolume(volume);

    volume = trj->GetDecayVolume();
    ipart->SetDecayVolume(volume);

    G4double mass = trj->GetParticleDefinition()->GetPDGMass();
    G4ThreeVector mom = trj->GetInitialMomentum();
    G4double energy = sqrt(mom.mag2() + mass*mass);
    ipart->SetInitialMomentum(mom.x(), mom.y(), mom.z(), energy);

    ievent->AddParticle(ipart);
  }

  // We'll set now the family relationships.
  // Loop through the particles we just stored in the irene event.
  TObjArray* iparts = ievent->GetParticles();
  for (unsigned int i=0; i<iparts->GetEntries(); ++i) {
    irene::Particle* ipart = (irene::Particle*) iparts->At(i);
    Trajectory* trj = (Trajectory*) TrajectoryMap::Get(ipart->GetParticleID());
    int parent_id = trj->GetParentID();
    ipart->SetCreatorProcess(trj->GetCreatorProcess());

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



void PersistencyManager::StoreHits(G4HCofThisEvent* hce, irene::Event* ievt)
{
  if (!hce) return; 

  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
  G4HCtable* hct = sdmgr->GetHCtable();

  // Loop through the hits collections
  for (int i=0; i<hct->entries(); i++) {

    // Collection are identified univocally (in principle) using 
    // their id number, and this can be obtained using the collection
    // and sensitive detector names.
    G4String hcname = hct->GetHCname(i);
    G4String sdname = hct->GetSDname(i);
    int hcid = sdmgr->GetCollectionID(sdname+"/"+hcname);

    // Fetch collection using the id number
    G4VHitsCollection* hits = hce->GetHC(hcid);

    if (hcname == IonizationSD::GetCollectionUniqueName())
      StoreIonizationHits(hits, ievt);
    else if (hcname == PmtSD::GetCollectionUniqueName())
      StorePmtHits(hits, ievt);
    else {
      G4String msg = 
        "Collection of hits '" + sdname + "/" + hcname
        + "' is of an unknown type and will not be stored.";
      G4Exception("StoreHits()", "[PersistencyManager]", JustWarning, msg);
      return;
    }
  }
}



void PersistencyManager::StoreIonizationHits(G4VHitsCollection* hc, 
                                             irene::Event* ievt)
{
  IonizationHitsCollection* hits = 
    dynamic_cast<IonizationHitsCollection*>(hc);
  if (!hits) return;

  for (G4int i=0; i<hits->entries(); i++) {
    
    IonizationHit* hit = dynamic_cast<IonizationHit*>(hits->GetHit(i));
    if (!hit) continue;

    G4int trackid = hit->GetTrackID();

    irene::Track* itrk = 0;

    std::map<G4int, irene::Track*>::iterator it = _itrkmap.find(trackid);
    if (it != _itrkmap.end()) {
      itrk = it->second;
    }
    else {
      std::string sdname = hits->GetSDname();
      itrk = new irene::Track(sdname);
      _itrkmap[trackid] = itrk;
      itrk->SetParticle(_iprtmap[trackid]);
      _iprtmap[trackid]->AddTrack(itrk);
      ievt->AddTrack(itrk);
    }

    G4ThreeVector xyz = hit->GetPosition();
    itrk->AddHit(xyz.x(), xyz.y(), xyz.z(), hit->GetTime(),
                 hit->GetEnergyDeposit());
  }
}



void PersistencyManager::StorePmtHits(G4VHitsCollection* hc, 
                                      irene::Event* ievt)
{
  PmtHitsCollection* hits = dynamic_cast<PmtHitsCollection*>(hc);
  if (!hits) return;

  for (G4int i=0; i<hits->entries(); i++) {


    PmtHit* hit = dynamic_cast<PmtHit*>(hits->GetHit(i));
    if (!hit) continue;

    std::string sdname = hits->GetSDname();
    irene::SensorHit* isnr = new irene::SensorHit(sdname);

    isnr->SetID(hit->GetPmtID());
    isnr->SetBinWidth(hit->GetBinSize());

    G4ThreeVector xyz = hit->GetPosition();
    isnr->SetPosition(xyz.x(), xyz.y(), xyz.z());

    const std::map<G4double, G4int>& wvfm = hit->GetHistogram();
    std::map<G4double, G4int>::const_iterator it;

    G4double amplitude = 0.;
    for (it = wvfm.begin(); it != wvfm.end(); ++it) {
      isnr->SetSample((*it).second, (*it).first);
      amplitude = amplitude + (*it).second;
    }
    isnr->SetAmplitude(amplitude);

   
    // Add the sensor hit to the irene event
    ievt->AddSensorHit(isnr);
  }
}



G4bool PersistencyManager::Store(const G4Run*)
{
  // TODO. Implement here the persistency of the configuration macros
  return true;
}
