// ----------------------------------------------------------------------------
///  \file   IonizationSD.h
///  \brief  Sensitive detector to create ionization hits.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     27 Apr 2009
///  \version  $Id: IonizationSD.h 2017 2009-07-15 21:55:44Z jmalbos $ 
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __IONIZATION_SD__
#define __IONIZATION_SD__

#include <G4VSensitiveDetector.hh>
#include "IonizationHit.h"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


namespace nexus {
  
  /// Sensitive detector to create ionization hits.
  
  class IonizationSD: public G4VSensitiveDetector
  {
  public:
    /// Constructor
    IonizationSD(const G4String& name);
    /// Destructor
    ~IonizationSD() {}
    
    /// A hit collection created by this sensitive detector is attached 
    /// in this method to the G4HCofThisEvent object.
    void Initialize(G4HCofThisEvent*);

    /// Hits are stored in the transient bhep event
    void EndOfEvent(G4HCofThisEvent*);

  private:
    
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    
  private:
    IonizationHitsCollection* _IHC;
  };
  
} // end namespace nexus

#endif
