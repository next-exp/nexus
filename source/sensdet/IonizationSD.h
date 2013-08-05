// ----------------------------------------------------------------------------
///  \file   IonizationSD.h
///  \brief  Sensitive detector to create ionization hits.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     27 Apr 2009
///  \version  $Id: IonizationSD.h 2017 2009-07-15 21:55:44Z jmalbos $ 
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __IONIZATION_SD__
#define __IONIZATION_SD__

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
  
    /// TODO. METHOD DESCRIPTION    
    void EndOfEvent(G4HCofThisEvent*);

    /// Return the unique name of the hits collection created
    /// by this sensitive detector. This will be used by the persistency
    /// manager to fetch the collection from the G4HCofThisEvent object.
    static G4String GetCollectionUniqueName();

  private:
    /// 
    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    
  private:
    IonizationHitsCollection* _IHC;
    G4String _det_name;
  };
  
} // end namespace nexus

#endif
