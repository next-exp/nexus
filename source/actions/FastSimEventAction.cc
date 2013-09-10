// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>
//  Created: 10 September 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "FastSimEventAction.h"

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>


using namespace nexus;



FastSimEventAction::FastSimEventAction(): 
  G4UserEventAction(), _nevt(0), _nupdate(10)
{
}
  
  
  
FastSimEventAction::~FastSimEventAction()
{
}
  
  
  
void FastSimEventAction::BeginOfEventAction(const G4Event* event)
{
  // Print out event number info
  if ((_nevt % _nupdate) == 0) {
    G4cout << " >> Event no. " << _nevt << G4endl;
    if (_nevt == (10 * _nupdate)) _nupdate *= 10;
  }
}



void FastSimEventAction::EndOfEventAction(const G4Event* event)
{
  _nevt++;

  G4cout << "EndOfEventAction" << G4endl;

  //event->GetHCofThisEvent()->


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


