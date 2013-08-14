// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>
//  Created: 9 Apr 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "FastSimFiltersEventAction.h"

#include "ConfigService.h"
#include "IonizationHit.h"

#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4Gamma.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>
#include <G4SDManager.hh>
#include <G4HCofThisEvent.hh>
#include <G4RunManager.hh>
#include <bhep/bhep_svc.h>
#include <G4Run.hh>

namespace nexus {
  
  
  FastSimFiltersEventAction::FastSimFiltersEventAction():
    _gamma_filter(0), _hit_filter(0), _evt_no(0), _filtered(false),
    _proc_events(0)
  {
    _gamma_min_energy = 
      ConfigService::Instance().Actions().GetDParam("gamma_min_energy");

    _job_events =
      ConfigService::Instance().Job().GetIParam("number_events");
  }
  
  
  
  FastSimFiltersEventAction::~FastSimFiltersEventAction()
  {
  }
  
  
  
  void FastSimFiltersEventAction::BeginOfEventAction(const G4Event* event)
  {
    _filtered = GammaFilter(event);

    // get bhep transient event from singleton and reset it
    bhep::event& bevt = bhep::bhep_svc::instance()->get_event();
    bevt.clear();

    if (!_filtered) {
      
      //
      _gamma_filter++;
      
      // set event number
      bevt.set_event_number(_evt_no);
      G4int event_id = event->GetEventID();
      bevt.add_property("G4EventID", event_id);
      
    }
    else {
      G4RunManager::GetRunManager()->AbortEvent();
    }

    _proc_events++;
  }
  
  
  
  void FastSimFiltersEventAction::EndOfEventAction(const G4Event* event)
  {
    if (!_filtered) {
      bhep::event& bevt = bhep::bhep_svc::instance()->get_event();
      
      vector<bhep::hit*> bhits = bevt.true_hits("ionization");
      
      if (bhits.size() > 0) {
	bhep::bhep_svc::instance()->get_writer_root().write(bevt);
	_hit_filter++;
	_evt_no++;
      }
    }
     
    if (_proc_events == _job_events) {
      bhep::bhep_svc::instance()->get_run_info().
	add_property("STATS_gamma_filter", _gamma_filter);
      
      bhep::bhep_svc::instance()->get_run_info().
	add_property("STATS_hit_filter", _hit_filter);
    }
    
    

   
    // visualization of tracks .......................................

    if (G4VVisManager::GetConcreteInstance()) {
      G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
      if (tc) {
	for (size_t i=0; i<tc->size(); i++) {
	  G4Trajectory* trj = (G4Trajectory*) (*tc)[i];
	  trj->DrawTrajectory();
	}
      }
    }
    
  }
  
  
  
  G4bool FastSimFiltersEventAction::GammaFilter(const G4Event* event)
  {
    if (_gamma_min_energy <= 0) return false;

    G4double total_energy = 0.;
    
    // get a pointer to the first vertex in the event
    G4PrimaryVertex* vertex = event->GetPrimaryVertex(0);

    // loop over vertices in the event
    while (vertex) {

      // get a pointer to the first particle in the vertex
      G4PrimaryParticle* particle = vertex->GetPrimary(0);

      // loop over primary particles in the vertex
      while (particle) {
	
	// if particle is a gamma, add its energy to the total
	if (particle->GetG4code() == G4Gamma::Definition())
	  total_energy += particle->GetMomentum().mag();
	
	// get a pointer to the following particle in the vertex
	particle = particle->GetNext();
      }

      // get a pointer to the following vertex in the event
      vertex = vertex->GetNext();
    }

    if (total_energy < _gamma_min_energy) 
      return true;
    else 
      return false;
  }
  
  
} // end namespace nexus
