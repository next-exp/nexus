// ----------------------------------------------------------------------------
///  \file   PmtSD.h
///  \brief  Sensitive detector for photosensors
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     15 Feb 2020  
///  \version  $Id$
///
///  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __PMT_SD__
#define __PMT_SD__

#include <G4VSensitiveDetector.hh>
#include "PmtHit.h"

class G4Step;
class G4HCofThisEvent;
class G4VTouchable;
class G4TouchableHistory;
class G4OpBoundaryProcess;


namespace nexus {
  
  /// General-purpose sensitive detector for photosensors

  class PmtSD: public G4VSensitiveDetector
  {
  public:
    /// Constructor providing names for the sensitive detector
    /// and the collection of hits
    PmtSD(G4String sdname, G4String hcname);
    /// The destructor
    ~PmtSD();
    
    /// Initialization of the sensitive detector. Invoked at the beginning 
    /// of every event. The collection of hits is created here and registered
    /// in the event (so that it can be retrieved thru the G4HCofThisEvent object).
    void Initialize(G4HCofThisEvent*);
    
    /// Method invoked at the end of every event
    void EndOfEvent(G4HCofThisEvent*);

    /// Set the depth of the sensitive detector in the geometry hierarchy
    void SetDetectorVolumeDepth(G4int);
    /// Return the depth of the sensitive detector in the volume hierarchy
    G4int GetDetectorVolumeDepth() const;
    /// Set the depth of the SD's mother volume in the geometry hierarchy
    void SetMotherVolumeDepth(G4int);
    /// Return the depth of the SD's mother volume in the geometry hierarchy
    G4int GetMotherVolumeDepth() const;
    /// Set the naming order (typically, a multiple of 10) of the SD     
    void SetDetectorNamingOrder(G4int);
    /// Return the naming order of the SD
    G4int GetDetectorNamingOrder() const;

    /// Return the time binning chosen for the pmt hits
    G4double GetTimeBinning() const;
    /// Set a time binning for the pmt hits
    void SetTimeBinning(G4double);

    /// Return the unique name of the hits collection created
    /// by this sensitive detector. This will be used by the 
    /// persistency manager to select the collection.
    static G4String GetCollectionUniqueName();

  private:

    G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    G4int FindPmtID(const G4VTouchable*);

  private:
    G4int _hcid;

    G4int _naming_order; ///< Order of the naming scheme
    G4int _sensor_depth; ///< Depth of the SD in the geometry tree
    G4int _mother_depth; ///< Depth of the SD's mother in the geometry tree
    
    G4double _timebinning; ///< Time bin width
    
    G4OpBoundaryProcess* _boundary; ///< Pointer to the optical boundary process

    PmtHitsCollection* _HC; ///< Pointer to the collection of hits
  };
  
  // INLINE METHODS //////////////////////////////////////////////////

  inline void PmtSD::SetDetectorVolumeDepth(G4int d) { _sensor_depth = d; }
  inline G4int PmtSD::GetDetectorVolumeDepth() const { return _sensor_depth; }

  inline void PmtSD::SetMotherVolumeDepth(G4int d) { _mother_depth = d; }
  inline G4int PmtSD::GetMotherVolumeDepth() const { return _mother_depth; }

  inline void PmtSD::SetDetectorNamingOrder(G4int o) { _naming_order = o; }
  inline G4int PmtSD::GetDetectorNamingOrder() const { return _naming_order; }

  inline G4double PmtSD::GetTimeBinning() const { return _timebinning; }
  inline void PmtSD::SetTimeBinning(G4double tb) { _timebinning = tb; }

} // end namespace nexus

#endif
