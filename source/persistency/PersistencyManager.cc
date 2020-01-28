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
#include "DetectorConstruction.h"
#include "BaseGeometry.h"
#include "HDF5Writer.h"

#include <G4GenericMessenger.hh>
#include <G4Event.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>
#include <G4RunManager.hh>
#include <G4Run.hh>

#include <string>
#include <sstream>
#include <iostream>

using namespace nexus;



PersistencyManager::PersistencyManager(G4String historyFile_init, G4String historyFile_conf):
  G4VPersistencyManager(), _msg(0),
  _ready(false), _store_evt(true), _interacting_evt(false),
  event_type_("other"), _saved_evts(0), _interacting_evts(0),
  _pmt_bin_size(-1), _sipm_bin_size(-1),
  _nevt(0), _start_id(0), _first_evt(true), _h5writer(0)
{

  _historyFile_init = historyFile_init;
  _historyFile_conf = historyFile_conf;

  _msg = new G4GenericMessenger(this, "/nexus/persistency/");
  _msg->DeclareMethod("outputFile", &PersistencyManager::OpenFile, "");
  _msg->DeclareProperty("eventType", event_type_, "Type of event: bb0nu, bb2nu or background.");
  _msg->DeclareProperty("start_id", _start_id, "Starting event ID for this job.");
}



PersistencyManager::~PersistencyManager()
{
  delete _msg;
  delete _h5writer;
}



void PersistencyManager::Initialize(G4String historyFile_init, G4String historyFile_conf)
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
  if (!current) current = new PersistencyManager(historyFile_init, historyFile_conf);
}


void PersistencyManager::OpenFile(G4String filename)
{
  // If the output file was not set yet, do so
  if (!_h5writer) {
    _h5writer = new HDF5Writer();
    G4String hdf5file = filename + ".h5";
    _h5writer->Open(hdf5file);
    return;
  } else {
    G4Exception("OpenFile()", "[PersistencyManager]",
		JustWarning, "An output file was previously opened.");
  }
}



void PersistencyManager::CloseFile()
{
  if (!_h5writer) return;

  _h5writer->Close();
}



G4bool PersistencyManager::Store(const G4Event* event)
{
  if (_interacting_evt) {
    _interacting_evts++;
  }

  if (!_store_evt) {
    TrajectoryMap::Clear();
    return false;
  }

  _saved_evts++;

  if (_first_evt) {
    _first_evt = false;
    _nevt = _start_id;
  }

  _event_info.first = _nevt;


  // if (event_type_ == "bb0nu") {
  //   ievt.SetMCEventType(gate::BB0NU);
  // } else if (event_type_ == "bb2nu") {
  //   ievt.SetMCEventType(gate::BB2NU);
  // } else if (event_type_ == "background") {
  //   ievt.SetMCEventType(gate::BKG);
  // } else {
  //   ievt.SetMCEventType(gate::NOETYPE);
  // }

  // Store the trajectories of the event
  StoreTrajectories(event->GetTrajectoryContainer());

  StoreHits(event->GetHCofThisEvent());

  //Save event-related information in hdf5 file
  // _h5writer->WriteEventInfo(_event_info.first, _event_info.second);

  _nevt++;

  TrajectoryMap::Clear();
  StoreCurrentEvent(true);

  return true;
}



void PersistencyManager::StoreTrajectories(G4TrajectoryContainer* tc)
{
  // If the pointer is null, no trajectories were stored in this event
  if (!tc) return;

  // Loop through the trajectories stored in the container
  for (G4int i=0; i<tc->entries(); ++i) {
    Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
    if (!trj) continue;

    G4int trackid = trj->GetTrackID();

    //ipart->SetPathLength(trj->GetTrackLength());

    G4ThreeVector ini_xyz = trj->GetInitialPosition();
    G4double ini_t = trj->GetInitialTime();

    G4ThreeVector final_xyz = trj->GetFinalPosition();
    G4double final_t = trj->GetFinalTime();

    G4String ini_volume = trj->GetInitialVolume();
    G4String final_volume = trj->GetDecayVolume();

    G4double mass = trj->GetParticleDefinition()->GetPDGMass();
    G4ThreeVector ini_mom = trj->GetInitialMomentum();
    G4double energy = sqrt(ini_mom.mag2() + mass*mass);
    G4ThreeVector final_mom = trj->GetFinalMomentum();

    float ini_pos[4] = {(float)ini_xyz.x(), (float)ini_xyz.y(), (float)ini_xyz.z(), (float)ini_t};
    float final_pos[4] = {(float)final_xyz.x(), (float)final_xyz.y(), (float)final_xyz.z(), (float)final_t};
    float ini_momentum[3] = {(float)ini_mom.x(), (float)ini_mom.y(), (float)ini_mom.z()};
    float final_momentum[3] = {(float)final_mom.x(), (float)final_mom.y(), (float)final_mom.z()};
    float kin_energy = energy - mass;
    char primary = 0;
    G4int mother_id = 0;
    if (!trj->GetParentID()) {
      primary = 1;
    } else {
      mother_id = trj->GetParentID();
    }
    _h5writer->WriteParticleInfo(_nevt, trackid, trj->GetParticleName().c_str(),
				 primary, mother_id,
				 ini_pos[0], ini_pos[1], ini_pos[2], ini_pos[3],
				 final_pos[0], final_pos[1], final_pos[2], final_pos[3],
				 ini_volume.c_str(), final_volume.c_str(),
				 ini_momentum[0], ini_momentum[1], ini_momentum[2],
				 final_momentum[0], final_momentum[1], final_momentum[2],
				 kin_energy, trj->GetCreatorProcess().c_str());

  }
}



void PersistencyManager::StoreHits(G4HCofThisEvent* hce)
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
      StoreIonizationHits(hits);
    else if (hcname == PmtSD::GetCollectionUniqueName())
      StorePmtHits(hits);
    else {
      G4String msg =
        "Collection of hits '" + sdname + "/" + hcname
        + "' is of an unknown type and will not be stored.";
      G4Exception("StoreHits()", "[PersistencyManager]", JustWarning, msg);
    }
  }

}


void PersistencyManager::StoreIonizationHits(G4VHitsCollection* hc)
{
  IonizationHitsCollection* hits =
    dynamic_cast<IonizationHitsCollection*>(hc);
  if (!hits) return;

  _hit_map.clear();

  double evt_energy = 0.;
  std::string sdname = hits->GetSDname();

  for (G4int i=0; i<hits->entries(); i++) {

    IonizationHit* hit = dynamic_cast<IonizationHit*>(hits->GetHit(i));
    if (!hit) continue;

    G4int trackid = hit->GetTrackID();

    std::vector<IonizationHit*> ihits;

    std::map<G4int, std::vector<nexus::IonizationHit*> >::iterator it =  _hit_map.find(trackid);
    if (it != _hit_map.end()) {
      ihits = it->second;
    } else {
      //ihits = std::vector<nexus::IonizationHt*>;
      _hit_map[trackid] = ihits;
    }

    ihits.push_back(hit);

    G4ThreeVector xyz = hit->GetPosition();
    _h5writer->WriteHitInfo(_nevt, trackid,  ihits.size() - 1,
			    xyz[0], xyz[1], xyz[2],
			    hit->GetTime(), hit->GetEnergyDeposit(),
			    sdname.c_str());


    //    evt_energy += hit->GetEnergyDeposit();
  }

  //  if (sdname == "ACTIVE") {
      // _event_info.second = evt_energy;
  // }

}



void PersistencyManager::StorePmtHits(G4VHitsCollection* hc)
{
  PmtHitsCollection* hits = dynamic_cast<PmtHitsCollection*>(hc);
  if (!hits) return;

  for (G4int i=0; i<hits->entries(); i++) {

    PmtHit* hit = dynamic_cast<PmtHit*>(hits->GetHit(i));
    if (!hit) continue;

    std::string sdname = hits->GetSDname();
    G4ThreeVector xyz = hit->GetPosition();
    double binsize = hit->GetBinSize();

    if (hit->GetPmtID()<1000) {
      _pmt_bin_size = binsize;
    } else {
      _sipm_bin_size = binsize;
    }

    const std::map<G4double, G4int>& wvfm = hit->GetHistogram();
    std::map<G4double, G4int>::const_iterator it;
    std::vector< std::pair<unsigned int,float> > data;
    G4double amplitude = 0.;

    for (it = wvfm.begin(); it != wvfm.end(); ++it) {
      unsigned int time_bin = (unsigned int)((*it).first/binsize+0.5);
      unsigned int charge = (unsigned int)((*it).second+0.5);

      data.push_back(std::make_pair(time_bin, charge));
      amplitude = amplitude + (*it).second;

      _h5writer->WriteSensorDataInfo(_nevt, (unsigned int)hit->GetPmtID(), time_bin, charge);
    }

    std::vector<G4int>::iterator pos_it =
      std::find(_sns_posvec.begin(), _sns_posvec.end(), hit->GetPmtID());
    if (pos_it == _sns_posvec.end()) {
      _h5writer->WriteSensorPosInfo((unsigned int)hit->GetPmtID(), (float)xyz.x(), (float)xyz.y(), (float)xyz.z());
      _sns_posvec.push_back(hit->GetPmtID());
    }

  }
}



G4bool PersistencyManager::Store(const G4Run*)
{
  // Store the number of events to be processed
  NexusApp* app = (NexusApp*) G4RunManager::GetRunManager();
  G4int num_events = app->GetNumberOfEventsToBeProcessed();

  G4String key = "num_events";
  _h5writer->WriteRunInfo(key,  std::to_string(num_events).c_str());
  key = "saved_events";
  _h5writer->WriteRunInfo(key,  std::to_string(_saved_evts).c_str());
  key = "interacting_events";
  _h5writer->WriteRunInfo(key,  std::to_string(_interacting_evts).c_str());
  key = "Pmt_time_binning";
  _h5writer->WriteRunInfo(key, (std::to_string(_pmt_bin_size/microsecond)+" mus").c_str());
  key = "SiPM_time_binning";
  _h5writer->WriteRunInfo(key, (std::to_string(_sipm_bin_size/microsecond)+" mus").c_str());

  SaveConfigurationInfo(_historyFile_init);
  SaveConfigurationInfo(_historyFile_conf);

  return true;
}

void PersistencyManager::SaveConfigurationInfo(G4String file_name)
{
  std::ifstream history(file_name, std::ifstream::in);
  while (history.good()) {

    std::string key, value;
    std::getline(history, key, ' ');
    std::getline(history, value);

    if (key != "") {
      _h5writer->WriteRunInfo(key.c_str(), value.c_str());
    }

  }

  history.close();
}
