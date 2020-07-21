// ----------------------------------------------------------------------------
// nexus | SiPMpetTPB.h
//
// Basic 3x3 mm2 SiPM geometry with TPB coating.
// The decay time of TPB is configurable.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SIPM_pet_TPB_H
#define SIPM_pet_TPB_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {


  /// Geometry of 3x3 mm2 active surface SiPMs

  class SiPMpetTPB: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMpetTPB();
    /// Destructor
    ~SiPMpetTPB();

    /// Invoke this method to build the volumes of the geometry
    void Construct();

  private:

    // Visibility of the tracking plane
    G4bool visibility_;

    // Optical properties to be used for epoxy
    G4double refr_index_;

    G4double decay_time_; ///< decay time of TPB

    G4bool phys_;

    G4double time_binning_;

    G4bool   box_geom_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;



  };


} // end namespace nexus

#endif
