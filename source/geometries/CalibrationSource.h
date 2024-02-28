// ----------------------------------------------------------------------------
// nexus | CalibrationSource.h
//
// This class describes a source of configurable size.
// The isotope is also configurable. It is used to simulate the small sources,
// shaped as capsules, used in Canfranc.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef CAL_SOURCE_H
#define CAL_SOURCE_H

#include "GeometryBase.h"

class G4GenericMessenger;

namespace nexus {

  class CalibrationSource: public GeometryBase {
  public:
    /// Constructor
    CalibrationSource();

    /// Destructor
    ~CalibrationSource();

    void Construct();

    G4double GetSourceDiameter();
    G4double GetSourceThickness();

    G4double GetCapsuleDiameter();
    G4double GetCapsuleThickness();

    G4double GetSourceZpos();

    void SetActiveMaterial(G4String source);

  private:

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Dimension of the Na22 source itself
    G4double source_diam_;
    G4double source_thick_;

    // Dimension of the whole support
    G4double capsule_diam_;
    G4double capsule_thick_;

    // Position of source inside its capsule
    G4double source_z_pos_;

    // Radioactive source being used
    G4String source_;
  };

  inline void CalibrationSource::SetActiveMaterial(G4String source) {source_ = source;}
}
#endif
