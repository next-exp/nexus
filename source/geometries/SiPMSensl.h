// ----------------------------------------------------------------------------
// nexus | SiPMSensl.h
//
// Geometry of the SensL SiPM used in the NEXT-WHITE detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SILICON_PM_SENSL_H
#define SILICON_PM_SENSL_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {


  /// Geometry of the Hamamatsu surface-mounted 1x1 mm2 MPPC (SiPM)
  
  class SiPMSensl: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMSensl();
    /// Destructor
    ~SiPMSensl();
    
    /// Return dimensions of the SiPM
    G4ThreeVector GetDimensions() const;
    
    /// Invoke this method to build the volumes of the geometry
    void Construct();
    
  private:
    G4ThreeVector dimensions_; ///< external dimensions of the SiPMSensl

    // Visibility of the tracking plane
    G4bool visibility_;

    // Time binning of sensors
    G4double binning_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

  };


} // end namespace nexus

#endif
