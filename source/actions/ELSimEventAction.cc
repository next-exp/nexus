// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : P Ferrario <paolafer@ific.uv.es>
//  Created: 23 Jan 2015
//
//  Copyright (c) 2009-2015 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "ELSimEventAction.h"

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>


namespace nexus {


  ELSimEventAction::ELSimEventAction(): 
    G4UserEventAction(), _nevt(0), _nupdate(10)
  {
  
  }
  
  
  
  ELSimEventAction::~ELSimEventAction()
  {
  }
  
  
  
  void ELSimEventAction::BeginOfEventAction(const G4Event* /*event*/)
  {
    // Print out event number info
    if ((_nevt % _nupdate) == 0) {
      G4cout << " >> Event no. " << _nevt << G4endl;
      if (_nevt == (10 * _nupdate)) _nupdate *= 10;
    }
  }



  void ELSimEventAction::EndOfEventAction(const G4Event* event)
  {
    _nevt++;

    
    // draw tracks in visual mode
    if (G4VVisManager::GetConcreteInstance()) {
      G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
      if (tc) {
        for (size_t i=0; i<tc->size(); i++) {
          G4VTrajectory* trj = (*tc)[i];
          trj->DrawTrajectory();
        }
      }
    }
  }


} // end namespace nexus
