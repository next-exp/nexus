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
#include "NexusApp.h"

#include <G4GenericMessenger.hh>
#include <G4Event.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>
#include <G4RunManager.hh>

#include <TList.h>

#include <GATE/Run.h>
#include <GATE/Event.h>
#include <GATE/Particle.h>
#include <GATE/Track.h>
#include <GATE/RootWriter.h>

#include <string>
#include <sstream>
#include <iostream>

using namespace nexus;



PersistencyManager::PersistencyManager(): 
  G4VPersistencyManager(), _msg(0), _historyFile("G4history.macro"), 
  _ready(false), _store_evt(true),  _evt(0), _writer(0)
{
  _msg = new G4GenericMessenger(this, "/nexus/persistency/");
  _msg->DeclareMethod("outputFile", &PersistencyManager::OpenFile, "");
  _msg->DeclareProperty("historyFile", _historyFile, "Name of the file where the configuration information are stored");
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
  if (!_writer) {
    _writer = new gate::RootWriter();
    //_ready = (G4bool) _writer->Open(filename.data(), "RECREATE");
    _writer->Open(filename.data(), "RECREATE");
    //if (!_ready)
    //  G4Exception("OpenFile()", "[PersistencyManager]", 
    //    FatalException, "The path for the output file does not exist.");
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
  if (!_store_evt) {
    TrajectoryMap::Clear();    
    return false;
  }

  // Create a new GATE event
  gate::Event ievt;
  ievt.SetEventID(event->GetEventID());

  // Store the trajectories of the event as Gate particles
  StoreTrajectories(event->GetTrajectoryContainer(), &ievt);

  StoreHits(event->GetHCofThisEvent(), &ievt);


  // Add event to the tree
  //_evt = &ievt;
  _writer->Write(ievt);
  //_evt = 0;

  TrajectoryMap::Clear();
  StoreCurrentEvent(true);

  return true;
}



void PersistencyManager::StoreTrajectories(G4TrajectoryContainer* tc,
                                           gate::Event* ievent)
{
  // If the pointer is null, no trajectories were stored in this event
  if (!tc) return;

  // Reset the map of gate::Particles
  _iprtmap.clear();

  // Loop through the trajectories stored in the container
  for (G4int i=0; i<tc->entries(); ++i) {
    Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
    if (!trj) continue;

    // Create an gate particle to store the trajectory information
    gate::MCParticle* ipart = new gate::MCParticle();
    ipart->SetPDG(trj->GetPDGEncoding());
    
    G4int trackid = trj->GetTrackID();
    ipart->SetID(trj->GetTrackID());
    _iprtmap[trackid] = ipart;

    ipart->SetPathLength(trj->GetTrackLength());

    G4ThreeVector xyz = trj->GetInitialPosition();
    G4double t = trj->GetInitialTime(); 
    ipart->SetInitialVtx(gate::Vector4D(xyz.x(), xyz.y(), xyz.z(),t));
    
    xyz = trj->GetFinalPosition();
    t = trj->GetFinalTime();
    ipart->SetFinalVtx(gate::Vector4D(xyz.x(), xyz.y(), xyz.z(),t));
    
    G4String volume = trj->GetInitialVolume();
    ipart->SetInitialVol(volume);
    volume = trj->GetDecayVolume();
    ipart->SetFinalVol(volume);

    G4double mass = trj->GetParticleDefinition()->GetPDGMass();
    G4ThreeVector mom = trj->GetInitialMomentum();
    G4double energy = sqrt(mom.mag2() + mass*mass);
    ipart->SetInitialMom(mom.x(), mom.y(), mom.z(), energy);

    ievent->AddMCParticle(ipart);
  }

  // We'll set now the family relationships.
  // Loop through the particles we just stored in the gate event.
  const std::vector<gate::MCParticle*>& iparts = ievent->GetMCParticles();
  for (size_t i=0; i<iparts.size(); ++i) {
    gate::MCParticle* ipart = (gate::MCParticle*) iparts.at(i);
    Trajectory* trj = (Trajectory*) TrajectoryMap::Get(ipart->GetID());
    int parent_id = trj->GetParentID();
    ipart->SetCreatorProc(trj->GetCreatorProcess());

    if (parent_id == 0) {
      ipart->SetPrimary(true);
    }
    else {
      gate::MCParticle* mother = _iprtmap[parent_id];
      ipart->SetPrimary(false);
      ipart->SetMother(mother);
      mother->AddDaughter(ipart);
    }
  }
}



void PersistencyManager::StoreHits(G4HCofThisEvent* hce, gate::Event* ievt)
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
                                             gate::Event* ievt)
{
  IonizationHitsCollection* hits = 
    dynamic_cast<IonizationHitsCollection*>(hc);
  if (!hits) return;

  _itrkmap.clear();

  for (G4int i=0; i<hits->entries(); i++) {
    
    IonizationHit* hit = dynamic_cast<IonizationHit*>(hits->GetHit(i));
    if (!hit) continue;

    G4int trackid = hit->GetTrackID();

    gate::MCTrack* itrk = 0;

    std::map<G4int, gate::MCTrack*>::iterator it = _itrkmap.find(trackid);
    if (it != _itrkmap.end()) {
      itrk = it->second;
    }
    else {
      std::string sdname = hits->GetSDname();
      itrk = new gate::MCTrack();
      itrk->SetLabel(sdname);
      _itrkmap[trackid] = itrk;
      itrk->SetParticle(_iprtmap[trackid]);
      _iprtmap[trackid]->AddTrack(itrk);
      ievt->AddMCTrack(itrk);
    }

    G4ThreeVector xyz = hit->GetPosition();
    gate::MCHit* ghit = new gate::MCHit();
    ghit->SetPosition(gate::Point3D(xyz.x(), xyz.y(), xyz.z()));
    ghit->SetTime(hit->GetTime());
    ghit->SetAmplitude(hit->GetEnergyDeposit());
    itrk->AddHit(ghit);
    ievt->AddMCHit(ghit);
   
  }
   
}



void PersistencyManager::StorePmtHits(G4VHitsCollection* hc, 
                                      gate::Event* ievt)
{
  PmtHitsCollection* hits = dynamic_cast<PmtHitsCollection*>(hc);
  if (!hits) return;

  for (G4int i=0; i<hits->entries(); i++) {


    PmtHit* hit = dynamic_cast<PmtHit*>(hits->GetHit(i));
    if (!hit) continue;
    
    std::string sdname = hits->GetSDname();
    gate::Hit* isnr = new gate::Hit();
    isnr->SetLabel(sdname);
    isnr->SetSensorID(hit->GetPmtID());
    G4ThreeVector xyz = hit->GetPosition();
    isnr->SetPosition(gate::Point3D(xyz.x(), xyz.y(), xyz.z()));
    
    if (hit->GetPmtID()<1000) isnr->SetSensorType(gate::PMT);
    else isnr->SetSensorType(gate::SIPM);

    gate::Waveform* wf = new gate::Waveform();
    isnr->SetWaveform(wf);
    double binsize = hit->GetBinSize();
    wf->SetSampWidth(binsize);
    
    const std::map<G4double, G4int>& wvfm = hit->GetHistogram();
    std::map<G4double, G4int>::const_iterator it;
    std::vector< std::pair<unsigned short,unsigned short> > data;
    G4double amplitude = 0.;
    unsigned short idx=0;  
  for (it = wvfm.begin(); it != wvfm.end(); ++it) {
        //isnr->SetSample((*it).second, (*it).first);
        data.push_back(std::make_pair((*it).first/binsize,(unsigned short)(*it).second));
        amplitude = amplitude + (*it).second;
        idx++;}
    wf->SetData(data);
    isnr->SetAmplitude(amplitude);

    // Add the sensor hit to the ate event
    ievt->AddMCSensHit(isnr);    
  }
}



G4bool PersistencyManager::Store(const G4Run*)
{
    
    gate::Run grun = gate::Run(); 
    
  std::ifstream history(_historyFile, std::ifstream::in);
  while (history.good()) {

    std::string key, value;
    std::getline(history, key, ' ');
    std::getline(history, value);

    if (key != "") {
      
        grun.fstore(key,value);
        
        //gate::ParameterInfo* info = new gate::ParameterInfo(key.c_str());
        //info->SetContent(value);
      //_writer->WriteMetadata(info); 
    }
  } 

  history.close();

  // Store the number of events to be processed 
  NexusApp* app = (NexusApp*) G4RunManager::GetRunManager();
  G4int num_events = app->GetNumberOfEventsToBeProcessed();

  std::stringstream ss;
  ss << num_events;

  //gate::ParameterInfo* info = new gate::ParameterInfo("num_events");
  //info->SetContent(ss.str());
  //_writer->WriteMetadata(info); 
 
  grun.store("num_events",ss.str());
  
  _writer->WriteRunInfo(grun);

  return true;
}
