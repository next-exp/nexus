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

#include <TList.h>

#include <GATE/Run.h>
#include <GATE/Event.h>
#include <GATE/Particle.h>
#include <GATE/Track.h>
#include <GATE/RootWriter.h>
#include <GATE/Environment.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;


PersistencyManager::PersistencyManager(G4String historyFile_init, G4String historyFile_conf): 
  G4VPersistencyManager(), _msg(0),
  _ready(false), _store_evt(true),  event_type_("other"), _writer(0),
  _saved_evts(0), _nevt(0), _start_id(0), _first_evt(true), _hdf5dump(false),
  _h5writer(0)
{

  _historyFile_init = historyFile_init;
  _historyFile_conf = historyFile_conf;

  _msg = new G4GenericMessenger(this, "/nexus/persistency/");
  _msg->DeclareMethod("outputFile", &PersistencyManager::OpenFile, "");
  _msg->DeclareProperty("eventType", event_type_, "Type of event: bb0nu, bb2nu or background.");
  _msg->DeclareProperty("start_id", _start_id, "Starting event ID for this job.");
  _msg->DeclareProperty("hdf5", _hdf5dump, "Generate hdf5 output.");
}



PersistencyManager::~PersistencyManager()
{
  delete _writer;
  delete _msg;
  if (_hdf5dump) delete _h5writer;
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

  if (_hdf5dump) {
    _h5writer = new HDF5Writer();
    G4String hdf5file = filename + ".h5";
    _h5writer->Open(hdf5file);
  }
}



void PersistencyManager::CloseFile()
{
  if (!_writer || !_writer->IsOpen()) return;

  _writer->Close();
  if (_hdf5dump) _h5writer->Close();
}



G4bool PersistencyManager::Store(const G4Event* event)
{
  if (!_store_evt) {
    TrajectoryMap::Clear();    
    return false;
  }

  _saved_evts++;

  if (_first_evt) {
    _first_evt = false;
    _nevt = _start_id;
  }

  G4cout << "Event number " << _nevt << G4endl;
  // Create a new GATE event
  gate::Event ievt;
  ievt.SetEventID(_nevt);
  if (_hdf5dump) {
    _event_info.first = _nevt;
  }
 
  if (event_type_ == "bb0nu") {
    ievt.SetMCEventType(gate::BB0NU);
  } else if (event_type_ == "bb2nu") {
    ievt.SetMCEventType(gate::BB2NU);
  } else if (event_type_ == "background") {
    ievt.SetMCEventType(gate::BKG);
  } else {
    ievt.SetMCEventType(gate::NOETYPE);
  }

  // Store the trajectories of the event as Gate particles
  StoreTrajectories(event->GetTrajectoryContainer(), &ievt);

  StoreHits(event->GetHCofThisEvent(), &ievt);
  if (_hdf5dump) {
    //Save event-related information in hdf5 file
    _h5writer->WriteEventInfo(_event_info.first, _event_info.second);
    //Save relationships among tables
    _h5writer->WriteEventExtentInfo(_nevt, _h5writer->GetSnsDataIndex(),
                                    _h5writer->GetHitIndex(), _h5writer->GetParticleIndex());
  }

  // Add event to the tree
  _writer->Write(ievt);
  _nevt++;

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
    ipart->SetLabel(trj->GetParticleName());

    
    G4int trackid = trj->GetTrackID();
    ipart->SetID(trj->GetTrackID());
    _iprtmap[trackid] = ipart;

    ipart->SetPathLength(trj->GetTrackLength());
    G4ThreeVector ini_xyz = trj->GetInitialPosition();
    G4double ini_t = trj->GetInitialTime(); 
    ipart->SetInitialVtx(gate::Vector4D(ini_xyz.x(), ini_xyz.y(), ini_xyz.z(), ini_t));
    G4ThreeVector xyz = trj->GetFinalPosition();
    G4double t = trj->GetFinalTime();
    ipart->SetFinalVtx(gate::Vector4D(xyz.x(), xyz.y(), xyz.z(), t));
    
    G4String ini_volume = trj->GetInitialVolume();
    ipart->SetInitialVol(ini_volume);
    G4String volume = trj->GetDecayVolume();
    ipart->SetFinalVol(volume);

    G4double mass = trj->GetParticleDefinition()->GetPDGMass();
    G4ThreeVector mom = trj->GetInitialMomentum();
    G4double energy = sqrt(mom.mag2() + mass*mass);
    ipart->SetInitialMom(mom.x(), mom.y(), mom.z(), energy);
    ipart->SetFinalMom(0, 0, 0, mass);
  //  std::cout << "After: "<< ipart->GetInitialVtx4D().GetT() << std::endl;
    ievent->AddMCParticle(ipart);

    if (_hdf5dump) {
      float ini_pos[4] = {(float)ini_xyz.x(), (float)ini_xyz.y(), (float)ini_xyz.z(), (float)ini_t};
      float final_pos[4] = {(float)xyz.x(), (float)xyz.y(), (float)xyz.z(), (float)t};
      float momentum[3] = {(float)mom.x(), (float)mom.y(), (float)mom.z()};
      float kin_energy = energy - mass;
      char primary = 0;
      G4int mother_id = -1;
      if (!trj->GetParentID()) {
	primary = 1;
      } else {
	mother_id = trj->GetParentID();
      }
      _h5writer->WriteParticleInfo(trackid, trj->GetParticleName().c_str(), primary, mother_id,
                                   &ini_pos[0], 4, &final_pos[0], 4, ini_volume.c_str(), volume.c_str(),
                                   &momentum[0], 3, kin_energy, trj->GetCreatorProcess().c_str());
    }
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

  double evt_energy = 0.;
  std::string sdname = hits->GetSDname();

  for (G4int i=0; i<hits->entries(); i++) {
    
    IonizationHit* hit = dynamic_cast<IonizationHit*>(hits->GetHit(i));
    if (!hit) continue;

    G4int trackid = hit->GetTrackID();

    gate::MCTrack* itrk = 0;

    std::map<G4int, gate::MCTrack*>::iterator it = _itrkmap.find(trackid);
    if (it != _itrkmap.end()) {
      itrk = it->second;
    } else {  
      itrk = new gate::MCTrack();
      itrk->SetLabel(sdname);
      _itrkmap[trackid] = itrk;
      itrk->SetParticle(_iprtmap[trackid]);
      _iprtmap[trackid]->AddTrack(itrk);
      ievt->AddMCTrack(itrk);
    }

    G4ThreeVector xyz = hit->GetPosition();
    gate::MCHit* ghit = new gate::MCHit();
    ghit->SetLabel(sdname);

    ghit->SetPosition(gate::Point3D(xyz.x(), xyz.y(), xyz.z()));
    ghit->SetTime(hit->GetTime());
    ghit->SetAmplitude(hit->GetEnergyDeposit());
    evt_energy += hit->GetEnergyDeposit();
    itrk->AddHit(ghit);
    ievt->AddMCHit(ghit);
  }

  if (sdname == "ACTIVE") {
    ievt->SetMCEnergy(evt_energy);
    if (_hdf5dump) {
      _event_info.second = evt_energy;
    }
  }

  if (_hdf5dump) {
    for (std::map<G4int, gate::MCTrack*>::iterator it=_itrkmap.begin(); it!=_itrkmap.end(); ++it) {
      gate::MCTrack* mytrack = it->second;
      const std::vector<gate::BHit*> myhits = mytrack->GetHits();
      for (unsigned int h=0; h<myhits.size(); ++h) {
        gate::BHit* hit = myhits[h];
        gate::Point3D pos = hit->GetPosition();
        float hit_pos[3] = {(float)pos.x(), (float)pos.y(), (float)pos.z()};
        _h5writer->WriteHitInfo(it->first, h, &hit_pos[0], 3, hit->GetTime(), hit->GetAmplitude(), hit->GetLabel().c_str());      
      }

    }
  }

  for (unsigned int tr=0; tr<ievt->GetMCTracks().size(); ++tr) {
    G4double tot_energy = 0.;
    gate::MCTrack* mytrack =  ievt->GetMCTracks()[tr];
    const std::vector<gate::BHit*> myhits = mytrack->GetHits();
    for (unsigned int h=0; h<myhits.size(); ++h) {
      tot_energy += myhits[h]->GetAmplitude();
      myhits[h]->SetID(h);
    }
    mytrack->SetExtremes(0, myhits.size()-1);
    mytrack->SetEnergy(tot_energy);
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
    std::vector< std::pair<unsigned int, float> > data;

    G4double amplitude = 0.;
    
    for (it = wvfm.begin(); it != wvfm.end(); ++it) {
      unsigned int time_bin = (unsigned int)((*it).first/binsize+0.5);
      unsigned int charge = (unsigned int)((*it).second+0.5);

      data.push_back(std::make_pair(time_bin, charge));
      amplitude = amplitude + (*it).second;

      if (_hdf5dump) {
        _h5writer->WriteSensorDataInfo((unsigned int)hit->GetPmtID(), time_bin, charge);
      }
    }
    wf->SetData(data);
    isnr->SetAmplitude(amplitude);

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
    ievt->AddMCSensHit(isnr);    
  }
}



G4bool PersistencyManager::Store(const G4Run*)
{
   
  gate::Run grun = gate::Run();

  // Store the number of events to be processed 
  NexusApp* app = (NexusApp*) G4RunManager::GetRunManager();
  G4int num_events = app->GetNumberOfEventsToBeProcessed();

  if (_hdf5dump) {
    G4String key = "num_events";
    _h5writer->WriteRunInfo(key,  std::to_string(num_events).c_str());
    key = "saved_events";
    _h5writer->WriteRunInfo(key,  std::to_string(_saved_evts).c_str());
  }
  SaveConfigurationInfo(_historyFile_init, grun);
  SaveConfigurationInfo(_historyFile_conf, grun);

  std::stringstream ss;
  ss << num_events;


  grun.store("num_events", ss.str());
  grun.SetNumEvents((int)_saved_evts);

  _writer->WriteRunInfo(grun);

  // if (_hdf5dump) {
  //   _h5writer->WriteRunInfo(-1);
  // }

  return true;
}

void PersistencyManager::SaveConfigurationInfo(G4String file_name, gate::Run& grun)
{
  std::ifstream history(file_name, std::ifstream::in);
  while (history.good()) {

    std::string key, value;
    std::getline(history, key, ' ');
    std::getline(history, value);

    if (key != "") {
      grun.fstore(key,value);
      if (_hdf5dump) {
        _h5writer->WriteRunInfo(key.c_str(), value.c_str());
      }
    }

  }
  
  history.close();
}
