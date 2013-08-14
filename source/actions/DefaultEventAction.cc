// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>
//  Created: 15 Apr 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DefaultEventAction.h"

#include "ConfigService.h"

#include <G4Run.hh>
#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>

#include <bhep/bhep_svc.h>
#include <bhep/store.h>


namespace nexus {
  
  
  DefaultEventAction::DefaultEventAction(): _evt_no(0)
  {
  }
  
  
  
  void DefaultEventAction::BeginOfEventAction(const G4Event* event)
  {
    // get bhep transient event from singleton
    bhep::event& bevt = bhep::bhep_svc::instance()->get_event();

    // be sure the event is empty
    bevt.clear();
    
    // set event number
    bevt.set_event_number(_evt_no);
    G4int event_id = event->GetEventID();
    bevt.add_property("G4EventID", event_id);
  }
  
  
  
  void DefaultEventAction::EndOfEventAction(const G4Event* event)
  {
    // save transient bhep event in file
    bhep::event& bevt = bhep::bhep_svc::instance()->get_event();
    bhep::bhep_svc::instance()->get_writer_root().write(bevt);

    // clear the event and increase the event number
    bevt.clear();
    _evt_no++;

    // visualization of tracks
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
  
} // end namespace nexus
