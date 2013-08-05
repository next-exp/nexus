// ----------------------------------------------------------------------------
//  $Id: IonizationSD.cc 2025 2009-07-17 11:11:39Z jmalbos $
//
//  Author : <justo.martin-albo@ific.uv.es>
//  Created: 27 Apr 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "IonizationSD.h"

#include "IonizationHit.h"


#include <G4SDManager.hh>
#include <G4Step.hh>


using namespace nexus;


  
IonizationSD::IonizationSD(const G4String& name):
  G4VSensitiveDetector(name)
{
  collectionName.insert(GetCollectionUniqueName());
  //collectionName.insert("IonizationHitsCollection");
}
  


IonizationSD::~IonizationSD()
{
}



G4String IonizationSD::GetCollectionUniqueName()
{
  G4String name = "IonizationHitsCollection";
  return name;
}


  
void IonizationSD::Initialize(G4HCofThisEvent* hce)
{
  // Create a collection of ionization hits and add it to 
  // the collection of hits of the event

  _IHC = 
    new IonizationHitsCollection(SensitiveDetectorName, collectionName[0]);

  G4int hcid = 
    G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection(hcid, _IHC);
}
  
  
  
G4bool IonizationSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  G4double edep = step->GetTotalEnergyDeposit();
    
  // Discard steps where no energy was deposited in the detector
  if (edep <= 0.) return false;

  // Create a hit and set its properties
  IonizationHit* hit = new IonizationHit();
  hit->SetTrackID(step->GetTrack()->GetTrackID());
  hit->SetTime(step->GetTrack()->GetGlobalTime());
  hit->SetEnergyDeposit(edep);
  hit->SetPosition(step->GetPostStepPoint()->GetPosition());
    
  // Add hit to collection
  _IHC->insert(hit);

  return true;
}
  
  

void IonizationSD::EndOfEvent(G4HCofThisEvent*)
{
}

