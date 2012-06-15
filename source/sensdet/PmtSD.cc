// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 15 Feb 2010
//
//  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "PmtSD.h"

#include <G4OpticalPhoton.hh>
#include <G4SDManager.hh>
#include <G4ProcessManager.hh>
#include <G4OpBoundaryProcess.hh>

#include <bhep/bhep_svc.h>


namespace nexus {
  
  
  PmtSD::PmtSD(G4String sdname, G4String hcname): 
    G4VSensitiveDetector(sdname), 
    _naming_order(0), _sensor_depth(0), _mother_depth(0),
    _HC(0), _boundary(0)
  {
    // Register the name of the collection of hits
    collectionName.insert(hcname);
  }
  
  
  
  PmtSD::~PmtSD()
  {
  }
  
  
  
  void PmtSD::Initialize(G4HCofThisEvent* HCE)
  {
    // Create a new collection of PMT hits
    _HC = new PmtHitsCollection(this->GetName(), this->GetCollectionName(0));
    
    // Register the collection in the event
    static G4int HCID = -1;
    if (HCID < 0) {
      HCID = G4SDManager::GetSDMpointer()->
	GetCollectionID(this->GetCollectionName(0)); 
    }
    HCE->AddHitsCollection(HCID, _HC);
  }
  
  
    
  G4bool PmtSD::ProcessHits(G4Step* step, G4TouchableHistory*)
  {
    // Check whether the track is an optical photon
    G4ParticleDefinition* pdef = step->GetTrack()->GetDefinition();
    if (pdef != G4OpticalPhoton::Definition()) return false;
    
    // Retrieve the pointer to the optical boundary process, if it has
    // not been done yet (i.e., if the pointer is not defined)
    if (!_boundary) {
      // Get the list of processes defined for the optical photon
      // and loop through it to find the optical boundary process.
      G4ProcessVector* pv = pdef->GetProcessManager()->GetProcessList();
      for (G4int i=0; i<pv->size(); i++) {
   	if ((*pv)[i]->GetProcessName() == "OpBoundary") {
   	  _boundary = (G4OpBoundaryProcess*) (*pv)[i];
   	  break;
   	}
      }
    }
    
    // Check if the photon has reached a geometry boundary
    if (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
      
      // Check whether the photon has been detected in the boundary
      if (_boundary->GetStatus() == Detection) {

	const G4VTouchable* touchable =
	  step->GetPreStepPoint()->GetTouchable();

	G4int pmt_id = FindPmtID(touchable);
	
 	PmtHit* hit = 0;
	for (G4int i=0; i<_HC->entries(); i++) {
 	  if ((*_HC)[i]->GetPmtID() == pmt_id) {
 	    hit = (*_HC)[i];
	    break;
	  }
 	}

 	// If no hit associated to this sensor exists already,
 	// create it and set main properties
 	if (!hit) {
 	  hit = new PmtHit();
 	  hit->SetPmtID(pmt_id);
 	  hit->SetBinSize(_timebinning);
 	  hit->SetPosition(touchable->GetTranslation());
 	  _HC->insert(hit);
 	}
	
 	G4double time = step->GetPostStepPoint()->GetGlobalTime();
 	hit->Fill(time);
      }
    }
  }
  


  G4int PmtSD::FindPmtID(const G4VTouchable* touchable)
  {
    G4int pmtid = touchable->GetCopyNumber(_sensor_depth);
    if (_naming_order != 0) {
      G4int motherid = touchable->GetCopyNumber(_mother_depth);
      pmtid = _naming_order * motherid + pmtid;
    } 
    return pmtid;
  }
  
  
  void PmtSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
  {
    bhep::event& bevt = bhep::bhep_svc::instance()->get_event();
    for (G4int i=0; i<_HC->entries(); i++) {
      bhep::hit* bhit = (*_HC)[i]->ToBhep();
      bevt.add_true_hit(collectionName[0], bhit);
    }
  }
  
  
} // end namespace nexus

