// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>
//  Created: 15 Apr 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "DefaultEventAction.h"
#include "Trajectory.h"
#include "PersistencyManager.h"
#include "IonizationHit.h"

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>
#include <G4GenericMessenger.hh>
#include <G4HCofThisEvent.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>
#include <globals.hh>


namespace nexus {


  DefaultEventAction::DefaultEventAction(): 
    G4UserEventAction(), _nevt(0), _nupdate(10), _energy_threshold(0.)
  {
    _msg = new G4GenericMessenger(this, "/Actions/DefaultEventAction/");
    
    G4GenericMessenger::Command& thresh_cmd =
       _msg->DeclareProperty("energy_threshold", _energy_threshold, "");
    thresh_cmd.SetParameterName("energy_threshold", true);
    thresh_cmd.SetUnitCategory("Energy");
  }
  
  
  
  DefaultEventAction::~DefaultEventAction()
  {
  }
  
  
  
  void DefaultEventAction::BeginOfEventAction(const G4Event* /*event*/)
  {
    // Print out event number info
    if ((_nevt % _nupdate) == 0) {
      G4cout << " >> Event no. " << _nevt << G4endl;
      if (_nevt == (10 * _nupdate)) _nupdate *= 10;
    }
  }



  void DefaultEventAction::EndOfEventAction(const G4Event* event)
  {
    _nevt++;

    // Determine whether total energy deposit in ionization sensitive 
    // detectors is above threshold
    if (_energy_threshold >= 0.) {

      // Get the trajectories stored for this event and loop through them
      // to calculate the total energy deposit

      G4double edep = 0.;

      G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
      if (tc) {
        for (unsigned int i=0; i<tc->size(); ++i) {
          Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
          edep += trj->GetEnergyDeposit();
          // Draw tracks in visual mode
          if (G4VVisManager::GetConcreteInstance()) trj->DrawTrajectory();
        }
      }

      PersistencyManager* pm = dynamic_cast<PersistencyManager*>
        (G4VPersistencyManager::GetPersistencyManager());
 
      // if (edep > _energy_threshold) pm->StoreCurrentEvent(true);
      // else pm->StoreCurrentEvent(false);
      if (!event->IsAborted() && edep > _energy_threshold) {
	pm->StoreCurrentEvent(true);
      } else {
	pm->StoreCurrentEvent(false);
      }

    }
  }


} // end namespace nexus
