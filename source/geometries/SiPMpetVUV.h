// ----------------------------------------------------------------------------
// nexus | SiPMpetVUV.h
//
// Basic 3x3 mm2 SiPM geometry without TPB coating.
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

    /// Return dimensions of the SiPM
    //G4ThreeVector GetDimensions() const;

    /// Invoke this method to build the volumes of the geometry
    void Construct();

  private:
    //G4ThreeVector _dimensions; ///< external dimensions of the SiPMpet

    // Visibility of the tracking plane
    G4bool visibility_;

    // Optical properties to be used for epoxy
    G4double refr_index_;

    // PDE for the sensor
    G4double eff_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    G4double time_binning_;

  };


} // end namespace nexus

#endif
