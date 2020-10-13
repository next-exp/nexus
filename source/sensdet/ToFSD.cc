// ----------------------------------------------------------------------------
// nexus | ToFSD.cc
//
// This class is the sensitive detector used for PETALO.
// Each time a photoelectron is detected by a sensor, two PmtHit instances
// are created (if needed): one to store the full response with large time
// binning and the other one with a fine time binning, which stores only
// the first part of the waveform.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ToFSD.h"

#include <G4OpticalPhoton.hh>
#include <G4SDManager.hh>
#include <G4ProcessManager.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4RunManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>


namespace nexus {

  using namespace CLHEP;

  ToFSD::ToFSD(G4String sdname):
    G4VSensitiveDetector(sdname),
    naming_order_(0), sensor_depth_(0), mother_depth_(0),
    boundary_(0)
  {
    // Register the name of the collection of hits
    collectionName.insert(GetCollectionUniqueName());
  }



  ToFSD::~ToFSD()
  {
  }


  G4String ToFSD::GetCollectionUniqueName()
  {
    return "PmtHitsCollection";
  }


  void ToFSD::Initialize(G4HCofThisEvent* HCE)
  {
    // Create a new collection of PMT hits
    HC_ = new PmtHitsCollection(this->GetName(), this->GetCollectionName(0));

    G4int HCID = G4SDManager::GetSDMpointer()->
      GetCollectionID(this->GetName()+"/"+this->GetCollectionName(0));

    HCE->AddHitsCollection(HCID, HC_);
  }



  G4bool ToFSD::ProcessHits(G4Step* step, G4TouchableHistory*)
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
        PmtHit* hit_tof = 0;
	for (G4int i=0; i<HC_->entries(); i++) {
 	  if ((*HC_)[i]->GetPmtID() == pmt_id) {
 	    hit = (*HC_)[i];
	    break;
	  }
 	}

        for (G4int i=0; i<HC_->entries(); i++) {
 	  if ((*HC_)[i]->GetPmtID() == - pmt_id) {
 	    hit_tof = (*HC_)[i];
	    break;
	  }
 	}

 	// If no hit associated to this sensor exists already,
 	// create it and set main properties
 	if (!hit) {
 	  hit = new PmtHit();
 	  hit->SetPmtID(pmt_id);
 	  hit->SetBinSize(timebinning_);
 	  hit->SetPosition(touchable->GetTranslation());
 	  HC_->insert(hit);

          hit_tof = new PmtHit();
          hit_tof->SetPmtID(-pmt_id);
 	  hit_tof->SetBinSize(5 * picosecond);
 	  hit_tof->SetPosition(touchable->GetTranslation());
 	  HC_->insert(hit_tof);
 	}

 	G4double time = step->GetPostStepPoint()->GetGlobalTime();
 	hit->Fill(time);
        hit_tof->Fill(time);
      }
    }

    return true;
  }



  G4int ToFSD::FindPmtID(const G4VTouchable* touchable)
  {
    G4int pmtid = touchable->GetCopyNumber(sensor_depth_);
    if (naming_order_ != 0) {
      G4int motherid = touchable->GetCopyNumber(mother_depth_);
      pmtid = naming_order_*motherid + pmtid;
    }
    if (box_geom_) {
      std::vector<G4int> init_ids ({ 0, 4, 32, 36, 64, 68, 96, 100 });
      G4int motherid = touchable->GetCopyNumber(mother_depth_);
      G4int first_id = (init_ids)[motherid];
      pmtid = first_id + pmtid;
    }
    return pmtid;
  }



  void ToFSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
  {
    //  int HCID = G4SDManager::GetSDMpointer()->
    //    GetCollectionID(this->GetCollectionName(0));
    //  // }
    // HCE->AddHitsCollection(HCID, _HC);
  }


} // end namespace nexus
