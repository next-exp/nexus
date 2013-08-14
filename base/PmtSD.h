// ----------------------------------------------------------------------------
///  \file   PmtSD.h
///  \brief  Sensitive detector for light sensors.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     15 Feb 2020  
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __PMT_SD__
#define __PMT_SD__

#include <G4VSensitiveDetector.hh>
#include "PmtHit.h"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


namespace nexus {
  
  /// Sensitive detector for light sensors.

  class PmtSD: public G4VSensitiveDetector
  {
  public:
    /// Constructor
    PmtSD(G4String sd_name, G4String hc_name);
    /// Destructor
    ~PmtSD();
    
    /// Method invoked at the beginning of every event. 
    /// The collection of PMT hits is created here and attached to 
    /// the G4HCofThisEvent object.
    void Initialize(G4HCofThisEvent*);
    
    /// Method invoked at the end of every event.
    void EndOfEvent(G4HCofThisEvent*);

    /// Sets the depth of the sensitive detector in the volume hierarchy
    void SetDetectorVolumeDepth(G4int);
    /// Returns the depth of the sensitive detector in the volume hierarchy
    G4int GetDetectorVolumeDepth();

  private:
    ///
    G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    
  private:
    PmtHitsCollection* _HC; ///< Collection of PmtHits
    G4int _depth;
  };
  

  inline void PmtSD::SetDetectorVolumeDepth(G4int d) { _depth = d; }

  inline G4int PmtSD::GetDetectorVolumeDepth() { return _depth; }

} // end namespace nexus

#endif
