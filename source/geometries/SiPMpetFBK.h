// ----------------------------------------------------------------------------
// nexus | SiPMpetFBK.h
//
// Basic SiPM geometry without TPB coating.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SIPM_pet_FBK_H
#define SIPM_pet_FBK_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {

  class SiPMpetFBK: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMpetFBK();
    /// Destructor
    ~SiPMpetFBK();

    /// Return dimensions of the SiPM
    //G4ThreeVector GetDimensions() const;

    /// Invoke this method to build the volumes of the geometry
    void Construct();

    /// Setting for correct ID numbering
    void SetSensorDepth (G4int sensor_depth);
    void SetMotherDepth (G4int mother_depth);
    void SetNamingOrder (G4int naming_order);


  private:
    //G4ThreeVector _dimensions; ///< external dimensions of the SiPMpet

    // Visibility of the tracking plane
    G4bool visibility_;

    // Optical properties to be used for epoxy
    G4double refr_index_;

    // PDE for the sensor
    G4double eff_;

    G4double time_binning_, sipm_size_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Numbering
    G4int sensor_depth_;
    G4int mother_depth_;
    G4int naming_order_;

  };

  inline void SiPMpetFBK::SetSensorDepth(G4int sensor_depth)
  { sensor_depth_ = sensor_depth; }

  inline void SiPMpetFBK::SetMotherDepth(G4int mother_depth)
  { mother_depth_ = mother_depth; }

  inline void SiPMpetFBK::SetNamingOrder(G4int naming_order)
  { naming_order_ = naming_order; }



} // end namespace nexus

#endif
