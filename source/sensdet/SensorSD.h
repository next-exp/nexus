// ----------------------------------------------------------------------------
// nexus | SensorSD.h
//
// This class is the sensitive detector that allows for the registration
// of the charge detected by a photosensor.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PMT_SD_H
#define PMT_SD_H

#include <G4VSensitiveDetector.hh>
#include "SensorHit.h"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class G4OpBoundaryProcess;


namespace nexus {

  class SensorSD: public G4VSensitiveDetector
  {
  public:
    /// Constructor providing names for the sensitive detector
    /// and the collection of hits
    SensorSD(G4String sdname);
    /// The destructor
    ~SensorSD();

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

    G4int FindSensorID(const G4VTouchable*);

    G4int naming_order_; ///< Order of the naming scheme
    G4int sensor_depth_; ///< Depth of the SD in the geometry tree
    G4int mother_depth_; ///< Depth of the SD's mother in the geometry tree

    G4double timebinning_; ///< Time bin width

    SensorHitsCollection* HC_; ///< Pointer to the collection of hits
  };

  // INLINE METHODS //////////////////////////////////////////////////

  inline void SensorSD::SetDetectorVolumeDepth(G4int d) { sensor_depth_ = d; }
  inline G4int SensorSD::GetDetectorVolumeDepth() const { return sensor_depth_; }

  inline void SensorSD::SetMotherVolumeDepth(G4int d) { mother_depth_ = d; }
  inline G4int SensorSD::GetMotherVolumeDepth() const { return mother_depth_; }

  inline void SensorSD::SetDetectorNamingOrder(G4int o) { naming_order_ = o; }
  inline G4int SensorSD::GetDetectorNamingOrder() const { return naming_order_; }

  inline G4double SensorSD::GetTimeBinning() const { return timebinning_; }
  inline void SensorSD::SetTimeBinning(G4double tb) { timebinning_ = tb; }

} // end namespace nexus

#endif
