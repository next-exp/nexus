//
//  TrackingSD.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>
//     Created: 27 Apr 2009
//     Updated: 
//
//  Copyright (c) 2009 -- IFIC Neutrino Group 
//

#ifndef __TRACKING_SD__
#define __TRACKING_SD__

#include <G4VSensitiveDetector.hh>
#include "TrackingHit.h"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


namespace nexus {

  class TrackingSD: public G4VSensitiveDetector
  {
  public:
    /// Constructor
    TrackingSD(G4String);
    /// Destructor
    ~TrackingSD() {}

    void Initialize(G4HCofThisEvent*);
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    void EndOfEvent(G4HCofThisEvent*);

  private:
    TrackingHitsCollection* _trackingCollection;
  };
  
} // end namespace nexus

#endif
