// ----------------------------------------------------------------------------
// nexus | SiPMpet.h
//
// Basic SiPM geometry with TPB coating.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SILICON_PM_pet_H
#define SILICON_PM_pet_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {


  /// Geometry of the Hamamatsu surface-mounted 1x1 mm2 MPPC (SiPM)

  class SiPMpet: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMpet();
    /// Destructor
    ~SiPMpet();

    /// Return dimensions of the SiPM
    G4ThreeVector GetDimensions() const;

    /// Invoke this method to build the volumes of the geometry
    void Construct();

  private:
    G4ThreeVector dimensions_; ///< external dimensions of the SiPMpet

    // Visibility of the tracking plane
    G4bool visibility_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    G4double time_binning_;



  };


} // end namespace nexus

#endif
