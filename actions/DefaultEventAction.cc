//
//  DefaultEventAction.cc
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>
//     Created: 15 Apr 2009
//     Updated: 
//
//  Copyright (c) 2009 -- IFIC Neutrino Group 
//

#include "DefaultEventAction.h"
#include "DefaultTrackingAction.h"
#include "TrackingHit.h"

#include <G4Event.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>
#include <G4VVisManager.hh>
#include <G4SDManager.hh>

#include <bhep/gstore.h>
#include <bhep/EventManager2.h>


namespace nexus {
  
  
  DefaultEventAction::DefaultEventAction()
  {
    Initialize();
  }
  
  
  
  void DefaultEventAction::Initialize()
  {
    bhep::gstore store;
    bhep::vstring idst_files, odst_files;
    
    odst_files.push_back("nexus.dst.txt");
    store.store("idst_files", idst_files);
    store.store("odst_files", odst_files);
    _evtMgr = new EventManager2(store, bhep::MUTE);
    _evtMgr->initialize();
  }



  DefaultEventAction::~DefaultEventAction()
  {
    _evtMgr->finalize();
    delete _evtMgr;
  }
  
  
  
  void DefaultEventAction::BeginOfEventAction(const G4Event* event)
  {
    G4int event_id = event->GetEventID();
    
    bhep::event& bevt = bhep::bhep_svc::instance().get_event();
    bevt.add_property("G4EventId", bhep::to_string(event_id));
  }



  void DefaultEventAction::EndOfEventAction(const G4Event* event)
  {
    /// Hits in Sensitive Detectors ---------------------------------
    G4HCofThisEvent* HCE = event->GetHCofThisEvent();
    ProcessHits(HCE);

    /// Visualization of tracks -------------------------------------
    G4TrajectoryContainer* trajectoryContainer = 
      event->GetTrajectoryContainer();
    
    G4int number_trajectories;
    if (trajectoryContainer)
      number_trajectories = trajectoryContainer->entries();
    
    if (G4VVisManager::GetConcreteInstance()) {
      for (G4int i=0; i<number_trajectories; i++) {
	G4Trajectory* trj = 
	  (G4Trajectory*) ((*(event->GetTrajectoryContainer()))[i]);
	trj->DrawTrajectory();
      }
    }

    /// Event is written in DST -------------------------------------
    bhep::event& bevt = bhep::bhep_svc::instance().get_event();
    _evtMgr->write(bevt);
    
    bevt.clear();
  }
  
  
  
  void DefaultEventAction::ProcessHits(G4HCofThisEvent* HCE)
  {
    bhep::event& bevt = bhep::bhep_svc::instance().get_event();

    G4SDManager* SDman  = G4SDManager::GetSDMpointer();
    G4int collection_id = SDman->GetCollectionID("trackingCollection");

    TrackingHitsCollection* THC =
      (TrackingHitsCollection*)(HCE->GetHC(collection_id));

    for (G4int i=0; i<(THC->entries()); i++) {
      
      bhep::hit* bhit = new bhep::hit("tracking");
      
      G4ThreeVector xyz = (*THC)[i]->GetPosition();
      bhit->set_point(bhep::Point3D(xyz.x(), xyz.y(), xyz.z()));
      
      G4double energy_dep = (*THC)[i]->GetEnergyDeposit();
      bhit->add_property("EnergyDep", bhep::to_string(energy_dep));
      
      G4int ID = (*THC)[i]->GetTrackID();
      bhep::particle& bpart = DefaultTrackingAction::GetBParticle(ID);
      bhit->set_mother_particle(bpart);
      bpart.add_hit("tracking", bhit);
    }
  }
  

} // end namespace nexus
