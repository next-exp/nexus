// ----------------------------------------------------------------------------
//  $Id: IonizationSD.cc 2025 2009-07-17 11:11:39Z jmalbos $
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>
//  Created: 27 Apr 2009
//
//  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "IonizationSD.h"

#include "IonizationHit.h"
#include "Utils.h"

#include <G4SDManager.hh>
#include <G4Step.hh>

#include <bhep/bhep_svc.h>



namespace nexus {
  
  
  IonizationSD::IonizationSD(G4String name): 
    G4VSensitiveDetector(name)
  {
    collectionName.insert("IonizationHitsCollection");
  }
  
  
  
  void IonizationSD::Initialize(G4HCofThisEvent* /*HCE*/)
  {
    _HC = 
      new IonizationHitsCollection(SensitiveDetectorName, collectionName[0]);
  }
  
  
  
  G4bool IonizationSD::ProcessHits(G4Step* step, G4TouchableHistory*)
  {
    G4double edep = step->GetTotalEnergyDeposit();
    
    if (edep == 0.) return false;

    IonizationHit* hit = new IonizationHit();
    hit->SetTrackID(step->GetTrack()->GetTrackID());
    hit->SetTime(step->GetTrack()->GetGlobalTime());
    hit->SetEnergyDeposit(edep);
    hit->SetPosition(step->GetPostStepPoint()->GetPosition());
    _HC->insert(hit);

    return true;
  }
  
  

  void IonizationSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
  {
    bhep::event& bevt = bhep::bhep_svc::instance()->get_event();

    for (G4int i=0; i<_HC->entries(); i++) {
      bhep::hit* bhit = (*_HC)[i]->ToBhep();
      bevt.add_true_hit("ionization", bhit);
    }
  }
  
  
} // end namespace nexus

