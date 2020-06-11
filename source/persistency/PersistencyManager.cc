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
#include <G4OpticalPhoton.hh>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "CLHEP/Units/SystemOfUnits.h"


PersistencyManager::PersistencyManager(G4String init_macro,
                                       std::vector<G4String>& macros,
                                       std::vector<G4String>& delayed_macros):
  G4VPersistencyManager(), msg_(0), init_macro_(init_macro), macros_(macros),
  delayed_macros_(delayed_macros), ready_(false), store_evt_(true),
  interacting_evt_(false), event_type_("other"), saved_evts_(0),
  interacting_evts_(0), nevt_(0), start_id_(0), first_evt_(true),
  thr_charge_(0), tof_time_(50.*nanosecond), sns_only_(false),
  save_tot_charge_(true), h5writer_(0)
{
  msg_ = new G4GenericMessenger(this, "/nexus/persistency/");
  msg_->DeclareMethod("outputFile", &PersistencyManager::OpenFile, "");
  msg_->DeclareProperty("eventType", event_type_,
                        "Type of event: bb0nu, bb2nu or background.");
  msg_->DeclareProperty("start_id", start_id_,
                        "Starting event ID for this job.");
  msg_->DeclareProperty("thr_charge", thr_charge_, "Threshold for the charge saved in file.");
  msg_->DeclareProperty("sns_only", sns_only_, "If true, no true information is saved.");
  msg_->DeclareProperty("save_tot_charge", save_tot_charge_, "If true, total charge is saved.");

  G4GenericMessenger::Command& time_cmd =
    msg_->DeclareProperty("tof_time", tof_time_, "Time saved in tof table per sensor");
  time_cmd.SetUnitCategory("Time");
  time_cmd.SetParameterName("tof_time", false);
  time_cmd.SetRange("tof_time>0.");

  secondary_macros_.clear();
}



PersistencyManager::~PersistencyManager()
{
  delete _msg;
  delete _h5writer;
}


void PersistencyManager::Initialize(G4String init_macro, std::vector<G4String>& macros,
                                    std::vector<G4String>& delayed_macros)
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
  if (!current) current =
                  new PersistencyManager(init_macro, macros, delayed_macros);
}


void PersistencyManager::OpenFile(G4String filename)
{
  _h5writer = new HDF5Writer();
  G4String hdf5file = filename + ".h5";
  _h5writer->Open(hdf5file);
  return;
}



void PersistencyManager::CloseFile()
{
  _h5writer->Close();
  return;
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

  if (_sns_only == false) {
    StoreTrajectories(event->GetTrajectoryContainer());
  }

  StoreHits(event->GetHCofThisEvent());

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

    G4bool save_opt_phot = false;
    std::ifstream init_history(_historyFile_init, std::ifstream::in);

    while (init_history.good()) {
      std::string key, value;
      std::getline(init_history, key, ' ');
      std::getline(init_history, value);
      if ((key == "/Actions/RegisterTrackingAction") && (value == "OPTICAL")) {
        save_opt_phot = true;
        break;
     }
   }

    if ((trj->GetParticleDefinition() == G4OpticalPhoton::Definition()) &&
        (save_opt_phot == false)) {
      continue;
    }

    G4int trackid = trj->GetTrackID();

    G4double length = trj->GetTrackLength();

    G4ThreeVector ini_xyz = trj->GetInitialPosition();
    G4double ini_t = trj->GetInitialTime();
    G4ThreeVector final_xyz = trj->GetFinalPosition();
    G4double final_t = trj->GetFinalTime();

    G4String ini_volume = trj->GetInitialVolume();
    G4String final_volume = trj->GetFinalVolume();

    G4double mass = trj->GetParticleDefinition()->GetPDGMass();
    G4ThreeVector ini_mom = trj->GetInitialMomentum();
    G4double energy = sqrt(ini_mom.mag2() + mass*mass);
    G4ThreeVector final_mom = trj->GetFinalMomentum();

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
				 (float)ini_xyz.x(), (float)ini_xyz.y(),
                                 (float)ini_xyz.z(), (float)ini_t,
				 (float)final_xyz.x(), (float)final_xyz.y(),
                                 (float)final_xyz.z(), (float)final_t,
				 ini_volume.c_str(), final_volume.c_str(),
				 (float)ini_mom.x(), (float)ini_mom.y(),
                                 (float)ini_mom.z(), (float)final_mom.x(),
                                 (float)final_mom.y(), (float)final_mom.z(),
				 kin_energy, length,
                                 trj->GetCreatorProcess().c_str(),
				 trj->GetFinalProcess().c_str());

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

    if (hcname == IonizationSD::GetCollectionUniqueName()) {
      if (_sns_only == false) {
	StoreIonizationHits(hits);
      }
    }
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

   std::string sdname = hits->GetSDname();

   for (G4int i=0; i<hits->entries(); i++) {

     IonizationHit* hit = dynamic_cast<IonizationHit*>(hits->GetHit(i));
     if (!hit) continue;

     G4int trackid = hit->GetTrackID();
     G4ThreeVector hit_pos = hit->GetPosition();

     _h5writer->WriteHitInfo(_nevt, trackid,
			     hit_pos[0], hit_pos[1], hit_pos[2],
			     hit->GetTime(), hit->GetEnergyDeposit(),
			     sdname.c_str());
   }

 }



void PersistencyManager::StorePmtHits(G4VHitsCollection* hc)
{
  std::map<G4int, PmtHit*> mapOfHits;

  PmtHitsCollection* hits = dynamic_cast<PmtHitsCollection*>(hc);
  if (!hits) return;

  std::vector<G4int > sensor_ids;
  for (G4int i=0; i<hits->entries(); i++) {

    PmtHit* hit = dynamic_cast<PmtHit*>(hits->GetHit(i));
    if (!hit) continue;

    int s_id  = hit->GetPmtID();
    mapOfHits[s_id] = hit;

    const std::map<G4double, G4int>& wvfm = hit->GetHistogram();
    std::map<G4double, G4int>::const_iterator it;
    double binsize = hit->GetBinSize();

    G4double amplitude = 0.;
    for (it = wvfm.begin(); it != wvfm.end(); ++it) {
      amplitude = amplitude + (*it).second;
    }
    if (hit->GetPmtID() >= 0) {
      G4int sens_id;
      sens_id = hit->GetPmtID();

      if (amplitude > _thr_charge){
        sensor_ids.push_back(sens_id);
      }
      if (hit->GetPmtID() >= 1000) {
	_bin_size = binsize;
      }
    } else if (hit->GetPmtID()<0) {
      _tof_bin_size = binsize;
    }
  }

  for (G4int s_id: sensor_ids){
    std::string sdname = hits->GetSDname();

    PmtHit* hit = mapOfHits[s_id];
    G4ThreeVector xyz = hit->GetPosition();
    double binsize = hit->GetBinSize();

    const std::map<G4double, G4int>& wvfm = hit->GetHistogram();
    std::map<G4double, G4int>::const_iterator it;
    std::vector< std::pair<unsigned int, float> > data;

    G4double amplitude = 0.;
    if (_save_tot_charge == true) {
      for (it = wvfm.begin(); it != wvfm.end(); ++it) {
        amplitude = amplitude + (*it).second;
        unsigned int time_bin = (unsigned int)((*it).first/binsize+0.5);
        unsigned int charge = (unsigned int)((*it).second+0.5);
        data.push_back(std::make_pair(time_bin, charge));
        _h5writer->WriteSensorDataInfo(_nevt, (unsigned int)hit->GetPmtID(), time_bin, charge);
      }
    }

    if (hit->GetPmtID() >= 0) {
      std::vector<G4int>::iterator pos_it =
	std::find(_sns_posvec.begin(), _sns_posvec.end(), hit->GetPmtID());
      if (pos_it == _sns_posvec.end()) {
	_h5writer->WriteSensorPosInfo((unsigned int)hit->GetPmtID(), sdname.c_str(),
                                      (float)xyz.x(), (float)xyz.y(), (float)xyz.z());
	_sns_posvec.push_back(hit->GetPmtID());
      }
    }


    // TOF
    PmtHit* hitTof = mapOfHits[-s_id];
    const std::map<G4double, G4int>& wvfmTof = hitTof->GetHistogram();

    double binsize_tof = hitTof->GetBinSize();

    for (it = wvfmTof.begin(); it != wvfmTof.end(); ++it) {

      if (((*it).first) <= _tof_time){
        unsigned int time_bin_tof = (unsigned int)((*it).first/binsize_tof+0.5);
        unsigned int charge_tof = (unsigned int)((*it).second+0.5);
        _h5writer->WriteSensorTofInfo(_nevt, hitTof->GetPmtID(), time_bin_tof, charge_tof);
      }
      else {
        break;
      }
    }

    /*
    const std::map<G4double, G4double>&  wvls= hit->GetWavelengths();
    std::map<G4double, G4double>::const_iterator w;

    std::vector<double > value;
    int count =0;
    std::ostringstream strs;
    strs << hit->GetPmtID();
    std::string sens_id = strs.str();
    //   G4cout << "Longitud = " << wvls.size() << G4endl;
    for (w = wvls.begin(); w != wvls.end(); ++w) {
      if (count < 100) {
	value.clear();
	std::ostringstream strs2;
	strs2 << count;
	std::string order = strs2.str();
	std::string key = sens_id + '_' + order;
	value.push_back(w->first/CLHEP::picosecond);
	value.push_back(w->second/CLHEP::nanometer);
	//	G4cout << key << ", " << value[0] << ", " << value[1] << G4endl;
	ievt->fstore(key, value);
	count++;
      }
    }
    */
  }
}



G4bool PersistencyManager::Store(const G4Run*)
{

  // Store the number of events to be processed
  NexusApp* app = (NexusApp*) G4RunManager::GetRunManager();
  G4int num_events = app->GetNumberOfEventsToBeProcessed();

  G4String key = "num_events";
  _h5writer->WriteRunInfo(key, std::to_string(num_events).c_str());
  key = "saved_events";
  _h5writer->WriteRunInfo(key, std::to_string(_saved_evts).c_str());
  key = "bin_size";
  _h5writer->WriteRunInfo(key, (std::to_string(_bin_size/microsecond)+" mus").c_str());
  key = "tof_bin_size";
  _h5writer->WriteRunInfo(key, (std::to_string(_tof_bin_size/picosecond)+" ps").c_str());
  key = "interacting_events";
  _h5writer->WriteRunInfo(key,  std::to_string(_interacting_evts).c_str());

  SaveConfigurationInfo(init_macro_);
  for (unsigned long i=0; i<macros_.size(); i++) {
    SaveConfigurationInfo(macros_[i]);
  }
  for (unsigned long i=0; i<delayed_macros_.size(); i++) {
    SaveConfigurationInfo(delayed_macros_[i]);
  }
  for (unsigned long i=0; i<secondary_macros_.size(); i++) {
    SaveConfigurationInfo(secondary_macros_[i]);
  }

  return true;
}

void PersistencyManager::SaveConfigurationInfo(G4String file_name)
{
  std::ifstream history(file_name, std::ifstream::in);
  while (history.good()) {

    G4String key, value;
    std::getline(history, key, ' ');
    std::getline(history, value);

    if (key != "") {
      auto found_binning = key.find("binning");
      auto found_comment = key.find("#");
      if ((found_binning == std::string::npos) && (found_comment == std::string::npos))
	h5writer_->WriteRunInfo(key.c_str(), value.c_str());

      auto found_other_macro = key.find("/control/execute");
      if (found_other_macro != std::string::npos)
        secondary_macros_.push_back(value);
    }

  }

  history.close();
}
