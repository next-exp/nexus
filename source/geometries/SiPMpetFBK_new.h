// ----------------------------------------------------------------------------
// nexus | SiPMpetFBK_new.cc
//
// 3x3 mm2 FBK SiPM geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SIPM_pet_FBK_new_H
#define SIPM_pet_FBK_new_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {

  class SiPMpetFBK_new: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMpetFBK_new();
    /// Destructor
    ~SiPMpetFBK_new();
    
    /// Return dimensions of the SiPM
    //G4ThreeVector GetDimensions() const;
    
    /// Invoke this method to build the volumes of the geometry
    void Construct();

    void SetTimeBinning (G4double time_binning);
    void SetSensorDepth (G4int sensor_depth);
    void SetMotherDepth (G4int mother_depth);
    void SetNamingOrder (G4int naming_order);

    
  private:
    //G4ThreeVector _dimensions; ///< external dimensions of the SiPMpet

    // Visibility of the tracking plane
    G4bool visibility_;

    // PDE for the sensor
    G4double eff_;

    G4double sensor_depth_, mother_depth_;
    G4double naming_order_, time_binning_;
    G4bool   box_geom_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

  };

  inline void SiPMpetFBK_new::SetTimeBinning(G4double time_binning)
  { time_binning_ = time_binning; }

  inline void SiPMpetFBK_new::SetSensorDepth(G4int sensor_depth)
  { sensor_depth_ = sensor_depth; }

  inline void SiPMpetFBK_new::SetMotherDepth(G4int mother_depth)
  { mother_depth_ = mother_depth; }

  inline void SiPMpetFBK_new::SetNamingOrder(G4int naming_order)
  { naming_order_ = naming_order; }


} // end namespace nexus

#endif
