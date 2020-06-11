// ----------------------------------------------------------------------------
// nexus | IonizationSD.h
//
// This class is the sensitive detector that creates ionization hits.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef IONIZATION_SD_H
#define IONIZATION_SD_H

#include <G4VSensitiveDetector.hh>
#include "IonizationHit.h"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


namespace nexus {

  /// Sensitive detector to create ionization hits

  class IonizationSD: public G4VSensitiveDetector
  {
  public:
    /// Constructor
    IonizationSD(const G4String& sdname);
    /// Destructor
    virtual ~IonizationSD();

    /// A hit collection created by this sensitive detector is attached
    /// in this method to the G4HCofThisEvent object.
    virtual void Initialize(G4HCofThisEvent*);

    void EndOfEvent(G4HCofThisEvent*);

    /// Return the unique name of the hits collection created
    /// by this sensitive detector. This will be used by the persistency
    /// manager to fetch the collection from the G4HCofThisEvent object.
    static G4String GetCollectionUniqueName();

    void IncludeInTotalEnergyDeposit(G4bool);

  private:
    ///
    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  private:
    IonizationHitsCollection* IHC_;
    G4String det_name_;
    G4bool include_;
  };

  inline void IonizationSD::IncludeInTotalEnergyDeposit(G4bool inc)
  { include_ = inc; }

} // end namespace nexus

#endif
