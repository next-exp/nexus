// ----------------------------------------------------------------------------
// nexus | IonizationSD.cc
//
// This class is the sensitive detector that creates ionization hits.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "IonizationSD.h"

#include "IonizationHit.h"
#include "Trajectory.h"
#include "TrajectoryMap.h"

#include <G4SDManager.hh>
#include <G4Step.hh>
#include <G4OpticalPhoton.hh>



using namespace nexus;



IonizationSD::IonizationSD(const G4String& name):
  G4VSensitiveDetector(name), include_(true)
{
  collectionName.insert(GetCollectionUniqueName());
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

  IHC_ =
    new IonizationHitsCollection(SensitiveDetectorName, collectionName[0]);

  G4int hcid =
    G4SDManager::GetSDMpointer()->GetCollectionID(SensitiveDetectorName+"/"+collectionName[0]);
  hce->AddHitsCollection(hcid, IHC_);

}



G4bool IonizationSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  G4Track* track = step->GetTrack();

  if (track->GetParticleDefinition() == G4OpticalPhoton::Definition())
    return false;

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
  IHC_->insert(hit);

  // Add energy deposit to the trajectory associated
  // to the current track
  if (include_) {
    Trajectory* trj =
      (Trajectory*) TrajectoryMap::Get(step->GetTrack()->GetTrackID());
    if (trj) {
      edep += trj->GetEnergyDeposit();
      trj->SetEnergyDeposit(edep);
    }
  }

  return true;
}



void IonizationSD::EndOfEvent(G4HCofThisEvent*)
{
}
