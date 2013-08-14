// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>
//  Created: 15 Apr 2009
//
//  Copyright (c) 2009-2012 NEXT Collaboration. All rights reserved.
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
    
  
  DefaultEventAction::DefaultEventAction(): _nevt(0), _nupd(10)
  {
  }
  
  
  
  DefaultEventAction::~DefaultEventAction()
  {
  }
  
  
  
  void DefaultEventAction::BeginOfEventAction(const G4Event* event)
  {
    // Print out event number info
    if ((_nevt%_nupd) == 0) {
      G4cout << " >> Event no. " << _nevt << G4endl;
      if (_nevt == (10*_nupd))
	_nupd *= 10;
    }

    // get bhep transient event from singleton
    bhep::event& bevt = bhep::bhep_svc::instance()->get_event();

    // be sure the event is empty
    bevt.clear();
    
    // set event number
    bevt.set_event_number(_nevt);
    G4int event_id = event->GetEventID();
    bevt.add_property("G4EventID", event_id);
  }
  
  
  
  void DefaultEventAction::EndOfEventAction(const G4Event* event)
  {
    // save transient bhep event in file
    bhep::event& bevt = bhep::bhep_svc::instance()->get_event();
    //bevt.add_property("", );
    bhep::bhep_svc::instance()->get_writer_root().write(bevt);

    // clear the event and increase the event number
    bevt.clear();
    _nevt++;

    // draw tracks in visual mode
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
