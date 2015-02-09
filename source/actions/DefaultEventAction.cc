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


    PersistencyManager* pm = dynamic_cast<PersistencyManager*>
      (G4VPersistencyManager::GetPersistencyManager());
    pm->StoreCurrentEvent(false);

    G4HCofThisEvent* hc = event->GetHCofThisEvent();
    if (hc) {
      
      G4SDManager* sdmgr = G4SDManager::GetSDMpointer();     
      G4HCtable* hct = sdmgr->GetHCtable();

      G4double total_energy = 0.;
      for (int i=0; i<hct->entries(); i++) {
	G4String hcname = hct->GetHCname(i);
	G4String sdname = hct->GetSDname(i);
	if (sdname == "ACTIVE") {
	  int active_id = sdmgr->GetCollectionID(sdname+"/"+hcname);
	  G4VHitsCollection* hits = hc->GetHC(active_id);

	  IonizationHitsCollection* ioni_hits = 
	    dynamic_cast<IonizationHitsCollection*>(hits);
	  if (!ioni_hits) return;
	  
	  if (_energy_threshold != 0.) { // only need to loop over the hits if I have an actual threshold
	    for (G4int ihit=0; ihit<ioni_hits->entries(); ihit++) {
	      IonizationHit* hit = dynamic_cast<IonizationHit*>(ioni_hits->GetHit(ihit));
	      total_energy +=  hit->GetEnergyDeposit();
	    }
	    if (total_energy > _energy_threshold) {
	      pm->StoreCurrentEvent(true);
	    }
	  } else {
	    if (ioni_hits->entries() != 0)
	      pm->StoreCurrentEvent(true);
	  }
	}
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


} // end namespace nexus
