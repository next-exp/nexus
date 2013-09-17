// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>
//  Created: 10 September 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "FastSimEventAction.h"

#include "Trajectory.h"
#include "PersistencyManager.h"

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>
#include <G4GenericMessenger.hh>


using namespace nexus;



FastSimEventAction::FastSimEventAction(): 
  G4UserEventAction(), _msg(0), _nevt(0), _nupdate(10), _energy_threshold(0.)
{
  _msg = new G4GenericMessenger(this, "/Actions/FastSimEventAction/");
  _msg->DeclareProperty("energy_threshold", _energy_threshold, "");
}
  
  
  
FastSimEventAction::~FastSimEventAction()
{
  delete _msg;
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

  G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
  if (tc) {

    G4double total_energy = 0.;

    for (G4int i=0; i<tc->entries(); ++i) {
      Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
      if (!trj) continue;
      total_energy = total_energy + trj->GetEnergyDeposit();
    }

    if (total_energy > _energy_threshold) {
      PersistencyManager* pm = dynamic_cast<PersistencyManager*>
        (G4VPersistencyManager::GetPersistencyManager());
      pm->StoreCurrentEvent(true);
    }
  }


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


