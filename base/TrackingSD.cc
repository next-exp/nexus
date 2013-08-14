//
//  TrackingSD.cc
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>
//     Created: 27 Apr 2009
//     Updated: 29 Apr 2009
//
//  Copyright (c) 2009 -- IFIC Neutrino Group 
//

#include "TrackingSD.h"
#include "TrackingHit.h"
#include <G4SDManager.hh>
#include <G4Step.hh>


namespace nexus {


  TrackingSD::TrackingSD(G4String name): G4VSensitiveDetector(name)
  {
    G4String HCname;
    collectionName.insert(HCname="trackingCollection");
  }



  void TrackingSD::Initialize(G4HCofThisEvent* HCE)
  {
    _trackingCollection = 
      new TrackingHitsCollection(SensitiveDetectorName, collectionName[0]); 

    static G4int HCID = -1;
    if (HCID < 0) { 
      HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); 
    }
    
    HCE->AddHitsCollection(HCID, _trackingCollection); 
  }



  G4bool TrackingSD::ProcessHits(G4Step* step, G4TouchableHistory*)
  {
    G4double edep = step->GetTotalEnergyDeposit();
    
    if (edep == 0.) return false;

    TrackingHit* hit = new TrackingHit();
    hit->SetTrackID(step->GetTrack()->GetTrackID());
    hit->SetEnergyDeposit(edep);
    hit->SetPosition(step->GetPostStepPoint()->GetPosition());
    _trackingCollection->insert(hit);
    
    return true;
  }
  

  void TrackingSD::EndOfEvent(G4HCofThisEvent*)
  {
    //   if (verboseLevel>0) { 
    //     G4int NbHits = trackerCollection->entries();
    //     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
    //   	   << " hits in the tracker chambers: " << G4endl;
    //     for (G4int i=0;i<NbHits;i++) (*trackerCollection)[i]->Print();
    //   } 
  }


} // end namespace nexus

