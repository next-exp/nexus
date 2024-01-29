// ----------------------------------------------------------------------------
// nexus | SensorSD.cc
//
// This class is the sensitive detector that allows for the registration
// of the charge detected by a photosensor.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SensorSD.h"

#include <G4OpticalPhoton.hh>
#include <G4SDManager.hh>
#include <G4ProcessManager.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4RunManager.hh>
#include <G4RunManager.hh>


namespace nexus {


  SensorSD::SensorSD(G4String sdname):
    G4VSensitiveDetector(sdname),
    naming_order_(0), sensor_depth_(0), mother_depth_(0)
  {
    // Register the name of the collection of hits
    collectionName.insert(GetCollectionUniqueName());
  }



  SensorSD::~SensorSD()
  {
  }



  G4String SensorSD::GetCollectionUniqueName()
  {
    return "SensorHitsCollection";
  }



  void SensorSD::Initialize(G4HCofThisEvent* HCE)
  {
    // Create a new collection of PMT hits
    HC_ = new SensorHitsCollection(this->GetName(), this->GetCollectionName(0));

    G4int HCID = G4SDManager::GetSDMpointer()->
      GetCollectionID(this->GetName()+"/"+this->GetCollectionName(0));

    HCE->AddHitsCollection(HCID, HC_);
  }



  G4bool SensorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
  {
    // Check whether the track is an optical photon
    G4ParticleDefinition* pdef = step->GetTrack()->GetDefinition();
    if (pdef != G4OpticalPhoton::Definition()) return false;

    const G4VTouchable* touchable =
      step->GetPostStepPoint()->GetTouchable();

    G4int pmt_id = FindSensorID(touchable);

    SensorHit* hit = 0;
    for (size_t i=0; i<HC_->entries(); i++) {
      if ((*HC_)[i]->GetSensorID() == pmt_id) {
        hit = (*HC_)[i];
        break;
      }
    }

    // If no hit associated to this sensor exists already,
    // create it and set main properties
    if (!hit) {
      hit = new SensorHit();
      hit->SetSensorID(pmt_id);
      hit->SetBinSize(timebinning_);
      hit->SetPosition(touchable->GetTranslation());
      HC_->insert(hit);
    }

    G4double time = step->GetPostStepPoint()->GetGlobalTime();
    hit->Fill(time);

    return true;
  }



  G4int SensorSD::FindSensorID(const G4VTouchable* touchable)
  {
    G4int pmtid = touchable->GetCopyNumber(sensor_depth_);
    if (naming_order_ != 0) {
      G4int motherid = touchable->GetCopyNumber(mother_depth_);
      pmtid = naming_order_ * motherid + pmtid;
    }
    return pmtid;
  }


  void SensorSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
  {
    //  int HCID = G4SDManager::GetSDMpointer()->
    //    GetCollectionID(this->GetCollectionName(0));
    //  // }
    // HCE->AddHitsCollection(HCID, HC_);


  }


} // end namespace nexus
