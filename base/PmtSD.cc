// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J. Martin-Albo <jmalbos@ific.uv.es>
//  Created: 15 Feb 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PmtSD.h"

#include <G4OpticalPhoton.hh>
#include <G4SDManager.hh>
#include <G4ProcessManager.hh>
#include <G4OpBoundaryProcess.hh>

#include <bhep/bhep_svc.h>


namespace nexus {
  
  
  PmtSD::PmtSD(G4String sd_name, G4String hc_name): 
    G4VSensitiveDetector(sd_name)
  {
    collectionName.insert(hc_name);
  }
  
  
  
  PmtSD::~PmtSD()
  {
  }
  
  
  
  void PmtSD::Initialize(G4HCofThisEvent* HCE)
  {
    // create a new collection of PMT hits
    _HC = new PmtHitsCollection(SensitiveDetectorName, collectionName[0]);

    G4int ID = -1;
    if (ID < 0) {
      ID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); 
    }

    HCE->AddHitsCollection(ID, _HC);
  }
  
  
    
  G4bool PmtSD::ProcessHits(G4Step* step, G4TouchableHistory*)
  {
    // Check if particle is an optical photon
    G4ParticleDefinition* ptype = step->GetTrack()->GetDefinition();
    if (ptype != G4OpticalPhoton::Definition()) return false;
    
    // Retrieve the optical boundary process only once
    static G4OpBoundaryProcess* boundary = 0;

    if (!boundary) {
      // get the process manager
      G4ProcessManager* pm = 
   	step->GetTrack()->GetDefinition()->GetProcessManager();
      // get the list of processes defined for the particle
      G4ProcessVector* pv = pm->GetProcessList();
    
      // loop over the processes list to find the opt boundary process
      for (G4int i=0; i<pv->size(); i++) {
   	if ((*pv)[i]->GetProcessName() == "OpBoundary") {
   	  boundary = (G4OpBoundaryProcess*) (*pv)[i];
   	  break;
   	}
      }
    }

    // Check if the optical photon is actually at a boundary
    // or still inside the volume.
    G4StepPoint* post_point = step->GetPostStepPoint();  
    if (post_point->GetStepStatus() == fGeomBoundary) {
      
      G4OpBoundaryProcessStatus status = boundary->GetStatus();
      
      if (status == Detection) {

	//Generates a hit and uses the postStepPoint's mother volume replica number
	//PostStepPoint because the hit is generated manually when the photon is
	//absorbed by the photocathode
	 
	//User replica number 1 since photocathode is a daughter volume
	// to the pmt which was replicated
	G4int pmt_no =
	  step->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(_depth);
	G4VPhysicalVolume* physVol =
	  step->GetPostStepPoint()->GetTouchable()->GetVolume(_depth);
	
	// find the correct hit collection
	PmtHit* hit = 0;
	for (G4int i=0; i<_HC->entries(); i++) {
	  if ((*_HC)[i]->GetPmtID() == pmt_no) {
	    hit = (*_HC)[i];
	  }
	}

	if (hit == 0) {
	  hit = new PmtHit();
	  hit->SetPmtID(pmt_no);
	  hit->SetBinSize(1.*microsecond);
	  _HC->insert(hit);
	}
	
	G4double time = step->GetPostStepPoint()->GetGlobalTime();

	hit->Fill(time);
      }
    }
  }
  
  
  
  
  void PmtSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
  {
    bhep::event& bevt = bhep::bhep_svc::instance()->get_event();
    for (G4int i=0; i<_HC->entries(); i++) {
      bhep::hit* bhit = (*_HC)[i]->ToBhep();
      bevt.add_true_hit("pmt", bhit);
    }
  }
  
  
} // end namespace nexus

