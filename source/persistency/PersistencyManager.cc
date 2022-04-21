// ----------------------------------------------------------------------------
// nexus | PersistencyManager.cc
//
// This class writes all the relevant information of the simulation
// to an ouput file.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PersistencyManager.h"

#include "Trajectory.h"
#include "TrajectoryMap.h"
#include "IonizationSD.h"
#include "SensorSD.h"
#include "NexusApp.h"
#include "DetectorConstruction.h"
#include "SaveAllSteppingAction.h"
#include "GeometryBase.h"
#include "HDF5Writer.h"
#include "PersistencyManagerBase.h"
#include "FactoryBase.h"

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
#include <string>

using namespace nexus;


REGISTER_CLASS(PersistencyManager, PersistencyManagerBase)


PersistencyManager::PersistencyManager():
  PersistencyManagerBase(), msg_(0), ready_(false),
  store_evt_(true), store_steps_(false),
  interacting_evt_(false), save_ie_numb_(false), event_type_("other"),
  saved_evts_(0), interacting_evts_(0), pmt_bin_size_(-1), sipm_bin_size_(-1),
  nevt_(0), start_id_(0), first_evt_(true), h5writer_(0)
{
  msg_ = new G4GenericMessenger(this, "/nexus/persistency/");
  msg_->DeclareMethod("outputFile", &PersistencyManager::OpenFile, "");
  msg_->DeclareProperty("eventType", event_type_,
                        "Type of event: bb0nu, bb2nu, background.");
  msg_->DeclareProperty("start_id", start_id_,
                        "Starting event ID for this job.");

  init_macro_ = "";
  macros_.clear();
  delayed_macros_.clear();
  secondary_macros_.clear();
}



PersistencyManager::~PersistencyManager()
{
  delete msg_;
  delete h5writer_;
}



/* void PersistencyManager::Initialize(G4String init_macro, std::vector<G4String>& macros,
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
} */


void PersistencyManager::OpenFile(G4String filename)
{
  // If the output file was not set yet, do so
  if (!h5writer_) {
    h5writer_ = new HDF5Writer();
    G4String hdf5file = filename + ".h5";
    h5writer_->Open(hdf5file, store_steps_);
    return;
  } else {
    G4Exception("[PersistencyManager]", "OpenFile()",
		JustWarning, "An output file was previously opened.");
  }
}



void PersistencyManager::CloseFile()
{
  if (!h5writer_) return;

  h5writer_->Close();
}



G4bool PersistencyManager::Store(const G4Event* event)
{
  if (interacting_evt_) {
    interacting_evts_++;
  }

  if (!store_evt_) {
    TrajectoryMap::Clear();
    if (store_steps_) {
      SaveAllSteppingAction* sa = (SaveAllSteppingAction*)
        G4RunManager::GetRunManager()->GetUserSteppingAction();
      sa->Reset();
    }
    return false;
  }

  saved_evts_++;

  if (first_evt_) {
    first_evt_ = false;
    nevt_ = start_id_;
  }

  if (store_steps_)
    StoreSteps();

  // Store the trajectories of the event
  StoreTrajectories(event->GetTrajectoryContainer());

  // Store ionization hits and sensor hits
  StoreHits(event->GetHCofThisEvent());

  nevt_++;

  TrajectoryMap::Clear();
  StoreCurrentEvent(true);

  return true;
}


void PersistencyManager::StoreTrajectories(G4TrajectoryContainer* tc)
{
  // If the pointer is null, no trajectories were stored in this event
  if (!tc) return;

  // Loop through the trajectories stored in the container
  for (size_t i=0; i<tc->entries(); ++i) {
    Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
    if (!trj) continue;

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
    h5writer_->WriteParticleInfo(nevt_, trackid, trj->GetParticleName().c_str(),
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
  for (auto i=0; i<hct->entries(); i++) {

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
    else if (hcname == SensorSD::GetCollectionUniqueName()) {
      StoreSensorHits(hits);
    } else {
      G4String msg =
        "Collection of hits '" + sdname + "/" + hcname
        + "' is of an unknown type and will not be stored.";
      G4Exception("[PersistencyManager]", "StoreHits()", JustWarning, msg);
    }
  }

}


void PersistencyManager::StoreIonizationHits(G4VHitsCollection* hc)
{
  IonizationHitsCollection* hits =
    dynamic_cast<IonizationHitsCollection*>(hc);
  if (!hits) return;

  hit_map_.clear();

  double evt_energy = 0.;
  std::string sdname = hits->GetSDname();

  for (size_t i=0; i<hits->entries(); i++) {

    IonizationHit* hit = dynamic_cast<IonizationHit*>(hits->GetHit(i));
    if (!hit) continue;

    G4int trackid = hit->GetTrackID();

    std::vector<G4int>* ihits = nullptr;
    std::map<G4int, std::vector<G4int>* >::iterator it = hit_map_.find(trackid);
    if (it != hit_map_.end()) {
      ihits = it->second;
    } else {
       ihits = new std::vector<G4int>;
      hit_map_[trackid] = ihits;
    }

    ihits->push_back(1);

    G4ThreeVector xyz = hit->GetPosition();
    h5writer_->WriteHitInfo(nevt_, trackid,  ihits->size() - 1,
			    xyz[0], xyz[1], xyz[2],
			    hit->GetTime(), hit->GetEnergyDeposit(),
			    sdname.c_str());

    evt_energy += hit->GetEnergyDeposit();
  }
}



void PersistencyManager::StoreSensorHits(G4VHitsCollection* hc)
{
  SensorHitsCollection* hits = dynamic_cast<SensorHitsCollection*>(hc);
  if (!hits) return;

  std::string sdname = hits->GetSDname();

  std::map<G4String, G4double>::const_iterator sensdet_it = sensdet_bin_.find(sdname);
  if (sensdet_it == sensdet_bin_.end()) {
    for (size_t j=0; j<hits->entries(); j++) {
      SensorHit* hit = dynamic_cast<SensorHit*>(hits->GetHit(j));
      if (!hit) continue;
      G4double bin_size = hit->GetBinSize();
      sensdet_bin_[sdname] = bin_size;
      break;
    }
  }

  for (size_t i=0; i<hits->entries(); i++) {

    SensorHit* hit = dynamic_cast<SensorHit*>(hits->GetHit(i));
    if (!hit) continue;

    G4ThreeVector xyz = hit->GetPosition();
    G4double binsize = hit->GetBinSize();

    const std::map<G4double, G4int>& wvfm = hit->GetHistogram();
    std::map<G4double, G4int>::const_iterator it;
    std::vector< std::pair<unsigned int,float> > data;
    G4double amplitude = 0.;

    for (it = wvfm.begin(); it != wvfm.end(); ++it) {
      unsigned int time_bin = (unsigned int)((*it).first/binsize+0.5);
      unsigned int charge = (unsigned int)((*it).second+0.5);

      data.push_back(std::make_pair(time_bin, charge));
      amplitude = amplitude + (*it).second;

      h5writer_->WriteSensorDataInfo(nevt_, (unsigned int)hit->GetPmtID(),
                                     time_bin, charge);
    }

    std::vector<G4int>::iterator pos_it =
      std::find(sns_posvec_.begin(), sns_posvec_.end(), hit->GetPmtID());
    if (pos_it == sns_posvec_.end()) {
      h5writer_->WriteSensorPosInfo((unsigned int)hit->GetPmtID(), sdname.c_str(),
				    (float)xyz.x(), (float)xyz.y(), (float)xyz.z());
      sns_posvec_.push_back(hit->GetPmtID());
    }

  }
}


void PersistencyManager::StoreSteps()
{
  SaveAllSteppingAction* sa = (SaveAllSteppingAction*)
    G4RunManager::GetRunManager()->GetUserSteppingAction();

  StepContainer<G4String> initial_volumes = sa->get_initial_volumes();
  StepContainer<G4String>   final_volumes = sa->get_final_volumes  ();
  StepContainer<G4String>      proc_names = sa->get_proc_names     ();

  StepContainer<G4ThreeVector> initial_poss = sa->get_initial_poss();
  StepContainer<G4ThreeVector>   final_poss = sa->get_final_poss  ();

  for (auto it = initial_volumes.begin(); it != initial_volumes.end(); ++it) {
    std::pair<G4int, G4String> key           = it->first;
    G4int                      track_id      = key.first;
    G4String                   particle_name = key.second;

    for (size_t step_id=0; step_id < it->second.size(); ++step_id) {
      h5writer_->WriteStep(nevt_, track_id, particle_name, step_id,
                           initial_volumes[key][step_id],
                             final_volumes[key][step_id],
                                proc_names[key][step_id],
                           initial_poss   [key][step_id].x(),
                           initial_poss   [key][step_id].y(),
                           initial_poss   [key][step_id].z(),
                             final_poss   [key][step_id].x(),
                             final_poss   [key][step_id].y(),
                             final_poss   [key][step_id].z());
    }
  }
  sa->Reset();
}

G4bool PersistencyManager::Store(const G4Run*)
{
  // Store the event type
  G4String key = "event_type";
  h5writer_->WriteRunInfo(key, event_type_.c_str());

  // Store the number of events to be processed
  NexusApp* app = (NexusApp*) G4RunManager::GetRunManager();
  G4int num_events = app->GetNumberOfEventsToBeProcessed();

  key = "num_events";
  h5writer_->WriteRunInfo(key,  std::to_string(num_events).c_str());
  key = "saved_events";
  h5writer_->WriteRunInfo(key,  std::to_string(saved_evts_).c_str());

  if (save_ie_numb_) {
    key = "interacting_events";
    h5writer_->WriteRunInfo(key,  std::to_string(interacting_evts_).c_str());
  }

  std::map<G4String, G4double>::const_iterator it;
  for (it = sensdet_bin_.begin(); it != sensdet_bin_.end(); ++it) {
    h5writer_->WriteRunInfo((it->first + "_binning").c_str(),
                           (std::to_string(it->second/microsecond)+" mus").c_str());
  }

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

    G4String line;
    std::getline(history, line);
    if (line[0] == '#')
      continue;

    std::stringstream ss(line);
    G4String key, value;
    std::getline(ss, key, ' ');
    std::getline(ss, value);

    if (key != "") {
      auto found_binning = key.find("binning");
      auto found_other_macro = key.find("/control/execute");
      if ((found_binning == std::string::npos) &&
          (found_other_macro == std::string::npos)) {
        if (key[0] == '\n') {
          key.erase(0, 1);
        }
	h5writer_->WriteRunInfo(key.c_str(), value.c_str());
      }

      if (found_other_macro != std::string::npos)
        secondary_macros_.push_back(value);
    }

  }

  history.close();
}
