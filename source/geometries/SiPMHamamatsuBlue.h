// ----------------------------------------------------------------------------
// nexus | SiPMHamamatsuBlue.cc
//
// 6x6 mm2 Hamamatsu Blue SiPM geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SIPM_pet_Blue_H
#define SIPM_pet_Blue_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {

  class SiPMHamamatsuBlue: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMHamamatsuBlue();
    /// Destructor
    ~SiPMHamamatsuBlue();

    /// Return dimensions of the SiPM
    //G4ThreeVector GetDimensions() const;

    /// Invoke this method to build the volumes of the geometry
    void Construct();

    void SetTimeBinning (G4double time_binning);
    void SetSensorDepth (G4int sensor_depth);
    void SetMotherDepth (G4int mother_depth);
    void SetNamingOrder (G4int naming_order);
    void SetBoxGeom (G4int box_geom);


  private:
    G4bool visibility_;

    // Optical properties to be used for epoxy
    G4double refr_index_;

    // PDE for the sensor
    G4double eff_;

    G4int    sensor_depth_;
    G4int    mother_depth_;
    G4int    naming_order_;
    G4double time_binning_;
    G4int   box_geom_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

  inline void SiPMHamamatsuBlue::SetTimeBinning(G4double time_binning)
  { time_binning_ = time_binning; }

  inline void SiPMHamamatsuBlue::SetSensorDepth(G4int sensor_depth)
  { sensor_depth_ = sensor_depth; }

  inline void SiPMHamamatsuBlue::SetMotherDepth(G4int mother_depth)
  { mother_depth_ = mother_depth; }

  inline void SiPMHamamatsuBlue::SetNamingOrder(G4int naming_order)
  { naming_order_ = naming_order; }

  inline void SiPMHamamatsuBlue::SetBoxGeom(G4int box_geom)
  { box_geom_ = box_geom; }


} // end namespace nexus

#endif
