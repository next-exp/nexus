// ----------------------------------------------------------------------------
// nexus | PmtSD.cc
//
// This class is the sensitive detector that allows for the registration
// of the charge detected by a photosensor.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PmtSD.h"

#include <G4OpticalPhoton.hh>
#include <G4SDManager.hh>
#include <G4ProcessManager.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4RunManager.hh>


namespace nexus {


  PmtSD::PmtSD(G4String sdname):
    G4VSensitiveDetector(sdname),
    naming_order_(0), sensor_depth_(0), mother_depth_(0),
    boundary_(0)
  {
    // Register the name of the collection of hits
    collectionName.insert(GetCollectionUniqueName());
  }



  PmtSD::~PmtSD()
  {
  }



  G4String PmtSD::GetCollectionUniqueName()
  {
    return "PmtHitsCollection";
  }



  void PmtSD::Initialize(G4HCofThisEvent* HCE)
  {
    // Create a new collection of PMT hits
    HC_ = new PmtHitsCollection(this->GetName(), this->GetCollectionName(0));

    G4int HCID = G4SDManager::GetSDMpointer()->
      GetCollectionID(this->GetName()+"/"+this->GetCollectionName(0));

    HCE->AddHitsCollection(HCID, HC_);
  }



  G4bool PmtSD::ProcessHits(G4Step* step, G4TouchableHistory*)
  {
    // Check whether the track is an optical photon
    G4ParticleDefinition* pdef = step->GetTrack()->GetDefinition();
    if (pdef != G4OpticalPhoton::Definition()) return false;

    // Retrieve the pointer to the optical boundary process, if it has
    // not been done yet (i.e., if the pointer is not defined)
    if (!boundary_) {
      // Get the list of processes defined for the optical photon
      // and loop through it to find the optical boundary process.
      G4ProcessVector* pv = pdef->GetProcessManager()->GetProcessList();
      for (G4int i=0; i<pv->size(); i++) {
   	if ((*pv)[i]->GetProcessName() == "OpBoundary") {
   	  boundary_ = (G4OpBoundaryProcess*) (*pv)[i];
   	  break;
   	}
      }
    }

    // Check if the photon has reached a geometry boundary
    if (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {

      // Check whether the photon has been detected in the boundary
      if (boundary_->GetStatus() == Detection) {
	const G4VTouchable* touchable =
	  step->GetPostStepPoint()->GetTouchable();

	G4int pmt_id = FindPmtID(touchable);

 	PmtHit* hit = 0;
	for (G4int i=0; i<HC_->entries(); i++) {
 	  if ((*HC_)[i]->GetPmtID() == pmt_id) {
 	    hit = (*HC_)[i];
	    break;
	  }
 	}

 	// If no hit associated to this sensor exists already,
 	// create it and set main properties
 	if (!hit) {
	  //	  G4cout << "Building SD hit number " << pmt_id << G4endl;
 	  hit = new PmtHit();
 	  hit->SetPmtID(pmt_id);
 	  hit->SetBinSize(timebinning_);
 	  hit->SetPosition(touchable->GetTranslation());
	  //	  G4cout << "Sensor " << pmt_id << " in pos " << touchable->GetTranslation() << G4endl;
 	  HC_->insert(hit);
 	}

 	G4double time = step->GetPostStepPoint()->GetGlobalTime();
 	hit->Fill(time);
      }
    }

    return true;
  }



  G4int PmtSD::FindPmtID(const G4VTouchable* touchable)
  {
    G4int pmtid = touchable->GetCopyNumber(sensor_depth_);
    if (naming_order_ != 0) {
      G4int motherid = touchable->GetCopyNumber(mother_depth_);
      pmtid = naming_order_*motherid + pmtid;
    }
    return pmtid;
  }


  void PmtSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
  {
    //  int HCID = G4SDManager::GetSDMpointer()->
    //    GetCollectionID(this->GetCollectionName(0));
    //  // }
    // HCE->AddHitsCollection(HCID, _HC);


  }


} // end namespace nexus
