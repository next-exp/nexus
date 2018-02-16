#ifndef __TOF_SD__
#define __TOF_SD__

#include <G4VSensitiveDetector.hh>
#include "PmtHit.h"

class G4Step;
class G4HCofThisEvent;
class G4VTouchable;
class G4TouchableHistory;
class G4OpBoundaryProcess;


namespace nexus {

  /// General-purpose sensitive detector for photosensors

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
    G4int _grandmother_depth; ///< Depth of the SD's grandmother in the geometry tree

    G4double _timebinning; ///< Time bin width

    G4OpBoundaryProcess* _boundary; ///< Pointer to the optical boundary process

    PmtHitsCollection* _HC; ///< Pointer to the collection of hits
  };

  // INLINE METHODS //////////////////////////////////////////////////

  inline void ToFSD::SetDetectorVolumeDepth(G4int d) { _sensor_depth = d; }
  inline G4int ToFSD::GetDetectorVolumeDepth() const { return _sensor_depth; }

  inline void ToFSD::SetMotherVolumeDepth(G4int d) { _mother_depth = d; }
  inline G4int ToFSD::GetMotherVolumeDepth() const { return _mother_depth; }

  inline void ToFSD::SetDetectorNamingOrder(G4int o) { _naming_order = o; }
  inline G4int ToFSD::GetDetectorNamingOrder() const { return _naming_order; }

  inline void ToFSD::SetGrandMotherVolumeDepth(G4int d) { _grandmother_depth = d; }
  inline G4int ToFSD::GetGrandMotherVolumeDepth() const { return _grandmother_depth; }

  inline G4double ToFSD::GetTimeBinning() const { return _timebinning; }
  inline void ToFSD::SetTimeBinning(G4double tb) { _timebinning = tb; }

} // end namespace nexus

#endif
