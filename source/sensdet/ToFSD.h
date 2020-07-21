// ----------------------------------------------------------------------------
// nexus | ToFSD.h
//
// This class is the sensitive detector used for PETALO.
// Each time a photoelectron is detected by a sensor, two PmtHit instances
// are created (if needed): one to store the full response with large time
// binning and the other one with a fine time binning, which stores only
// the first part of the waveform.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef TOF_SD_H
#define TOF_SD_H

#include <G4VSensitiveDetector.hh>
#include "PmtHit.h"

class G4Step;
class G4HCofThisEvent;
class G4VTouchable;
class G4TouchableHistory;
class G4OpBoundaryProcess;


namespace nexus {

  class ToFSD: public G4VSensitiveDetector
  {
  public:
    /// Constructor providing names for the sensitive detector
    /// and the collection of hits
    ToFSD(G4String sdname);
    /// The destructor
    ~ToFSD();

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

    /// Set the depth of the SD's grandmother volume in the geometry hierarchy
    void SetGrandMotherVolumeDepth(G4int);
    /// Return the depth of the SD's grandmother volume in the geometry hierarchy
    G4int GetGrandMotherVolumeDepth() const;

    /// Return the time binning chosen for the pmt hits
    G4double GetTimeBinning() const;
    /// Set a time binning for the pmt hits
    void SetTimeBinning(G4double);

    /// Set the box geometry parameter
    void SetBoxGeom(G4bool);

    /// Return the unique name of the hits collection created
    /// by this sensitive detector. This will be used by the
    /// persistency manager to select the collection.
    static G4String GetCollectionUniqueName();

  private:

    G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    G4int FindPmtID(const G4VTouchable*);

    G4int naming_order_; ///< Order of the naming scheme
    G4int sensor_depth_; ///< Depth of the SD in the geometry tree
    G4int mother_depth_; ///< Depth of the SD's mother in the geometry tree
    G4int grandmother_depth_; ///< Depth of the SD's grandmother in the geometry tree

    G4double timebinning_; ///< Time bin width

    G4OpBoundaryProcess* boundary_; ///< Pointer to the optical boundary process

    G4bool box_geom_; ///< Boolean required to change the naming_order_ for the case of the BoxSetup

    PmtHitsCollection* HC_; ///< Pointer to the collection of hits
  };

  // INLINE METHODS //////////////////////////////////////////////////

  inline void ToFSD::SetDetectorVolumeDepth(G4int d) { sensor_depth_ = d; }
  inline G4int ToFSD::GetDetectorVolumeDepth() const { return sensor_depth_; }

  inline void ToFSD::SetMotherVolumeDepth(G4int d) { mother_depth_ = d; }
  inline G4int ToFSD::GetMotherVolumeDepth() const { return mother_depth_; }

  inline void ToFSD::SetDetectorNamingOrder(G4int o) { naming_order_ = o; }
  inline G4int ToFSD::GetDetectorNamingOrder() const { return naming_order_; }

  inline void ToFSD::SetGrandMotherVolumeDepth(G4int d) { grandmother_depth_ = d; }
  inline G4int ToFSD::GetGrandMotherVolumeDepth() const { return grandmother_depth_; }

  inline G4double ToFSD::GetTimeBinning() const { return timebinning_; }
  inline void ToFSD::SetTimeBinning(G4double tb) { timebinning_ = tb; }

  inline void ToFSD::SetBoxGeom(G4bool bg) { box_geom_ = bg; }

} // end namespace nexus

#endif
