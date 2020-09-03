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
    // Constructor
    SiPMSensl();
    /// Destructor
    ~SiPMSensl();

    // Return dimensions of the SiPM
    G4ThreeVector GetDimensions() const;

    // Invoke this method to build the volumes of the geometry
    void Construct();

    // Needed settings for correct numbering
    void SetSensorDepth (G4int sensor_depth);
    void SetMotherDepth (G4int mother_depth);
    void SetNamingOrder (G4int naming_order);


  private:
    G4ThreeVector dimensions_; ///< external dimensions of the SiPMSensl

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Time binning of sensors
    G4double binning_;

    // Numbering
    G4int sensor_depth_;
    G4int mother_depth_;
    G4int naming_order_;

    // Visibility of the tracking plane
    G4bool visibility_;

  };

  inline void SiPMSensl::SetSensorDepth(G4int sensor_depth)
  { sensor_depth_ = sensor_depth; }

  inline void SiPMSensl::SetMotherDepth(G4int mother_depth)
  { mother_depth_ = mother_depth; }

  inline void SiPMSensl::SetNamingOrder(G4int naming_order)
  { naming_order_ = naming_order; }

} // end namespace nexus

#endif
