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
    // Check whether the track is an optical photon
    G4ParticleDefinition* pdef = step->GetTrack()->GetDefinition();
    if (pdef != G4OpticalPhoton::Definition()) return false;
    
    // Retrieve the pointer to the optical boundary process.
    // We do this only once per run defining our local pointer as static.
    static G4OpBoundaryProcess* boundary = 0;

    if (!boundary) { // the pointer is not defined yet
      // Get the list of processes defined for the optical photon
      // and loop through it to find the optical boundary process.
      G4ProcessVector* pv = pdef->GetProcessManager()->GetProcessList();
      for (G4int i=0; i<pv->size(); i++) {
   	if ((*pv)[i]->GetProcessName() == "OpBoundary") {
   	  boundary = (G4OpBoundaryProcess*) (*pv)[i];
   	  break;
   	}
      }
    }
    
    
    
    if (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
      
      if (boundary->GetStatus() == Detection) {
	
	// Generates a hit and uses the postStepPoint's mother volume 
	// replica number
	//PostStepPoint because the hit is generated manually when the photon is
	//absorbed by the photocathode
	 
	//User replica number 1 since photocathode is a daughter volume
	// to the pmt which was replicated
	G4int pmt_no =
	  step->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(_depth);
	G4VPhysicalVolume* physVol =
	  step->GetPostStepPoint()->GetTouchable()->GetVolume(_depth);

	G4ThreeVector pos = physVol->GetObjectTranslation();
	
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
	  hit->SetBinSize(0.1*microsecond);
	  hit->SetPosition(physVol->GetObjectTranslation());
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
      bevt.add_true_hit(collectionName[0], bhit);
    }
  }
  
  
} // end namespace nexus

