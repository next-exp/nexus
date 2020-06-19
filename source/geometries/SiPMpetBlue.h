#ifndef SIPM_pet_Blue_H
#define SIPM_pet_Blue_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {

  class SiPMpetBlue: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMpetBlue();
    /// Destructor
    ~SiPMpetBlue();

    /// Return dimensions of the SiPM
    //G4ThreeVector GetDimensions() const;

    /// Invoke this method to build the volumes of the geometry
    void Construct();

  private:
    G4bool visibility_;

    // Optical properties to be used for epoxy
    G4double refr_index_;

    // PDE for the sensor
    G4double eff_;

    G4double sensor_depth_, mother_depth_;
    G4double naming_order_, time_binning_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };


} // end namespace nexus

#endif
