// ----------------------------------------------------------------------------
// nexus | SiPMpetVUV.h
//
// Variable size SiPM geometry with no wavelength shifter
// and a window with perfect transparency and configurable refractive index.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SIPM_pet_VUV_H
#define SIPM_pet_VUV_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {


  /// Geometry of the Hamamatsu surface-mounted 1x1 mm2 MPPC (SiPM)

  class SiPMpetVUV: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMpetVUV();
    /// Destructor
    ~SiPMpetVUV();

    /// Invoke this method to build the volumes of the geometry
    void Construct();

    void SetSensorDepth (G4int sensor_depth);
    void SetMotherDepth (G4int mother_depth);
    void SetNamingOrder (G4int naming_order);

  private:

    // Visibility of the tracking plane
    G4bool visibility_;

    // Optical properties to be used for epoxy
    G4double refr_index_;

    // PDE for the sensor
    G4double eff_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    G4double time_binning_;
    G4double sipm_size_;
    G4int sensor_depth_, mother_depth_, naming_order_;

  };

  inline void SiPMpetVUV::SetSensorDepth(G4int sensor_depth)
  { sensor_depth_ = sensor_depth; }

  inline void SiPMpetVUV::SetMotherDepth(G4int mother_depth)
  { mother_depth_ = mother_depth; }

  inline void SiPMpetVUV::SetNamingOrder(G4int naming_order)
  { naming_order_ = naming_order; }

} // end namespace nexus

#endif
