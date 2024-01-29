// ----------------------------------------------------------------------------
// nexus | MagBox.h
//
// Detector with magnetic field.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef MAGBOX_H
#define MAGBOX_H

#include "GeometryBase.h"

class G4LogicalVolume;
class G4GenericMessenger;
class G4Material;


namespace nexus {

  class BoxPointSamplerLegacy;

  class MagBox: public GeometryBase {
  public:
    /// Constructor
    MagBox();

    /// Destructor
    ~MagBox();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    void Construct();

  private:
    // Detector dimensions
    const G4double detector_size_; /// Size of the Xe box

    // ACTIVE gas Xenon
    G4Material* gas_;

    // Parameters
    G4double max_step_size_;  /// Maximum Step Size
    G4String gas_name_;       /// Gas name
    G4double pressure_;       /// Pressure Gas Xenon
    G4double mag_intensity_;  /// Magnetic Field Intensity

    //Vertex genrator
    BoxPointSamplerLegacy* active_gen_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

} // end namespace nexus

#endif
