// ----------------------------------------------------------------------------
// nexus | SiPMHamamatsuVUV.cc
//
// 6x6 mm2 VUV Hamamatsu SiPM geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SIPM_pet_VUV_new_H
#define SIPM_pet_VUV_new_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {

  class SiPMHamamatsuVUV: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMHamamatsuVUV();
    /// Destructor
    ~SiPMHamamatsuVUV();

    /// Return dimensions of the SiPM
    //G4ThreeVector GetDimensions() const;

    /// Invoke this method to build the volumes of the geometry
    void Construct();

    void SetVisibility(G4bool vis);
    void SetTimeBinning (G4double time_binning);
    void SetSensorDepth (G4int sensor_depth);
    void SetMotherDepth (G4int mother_depth);
    void SetNamingOrder (G4int naming_order);
    void SetBoxGeom (G4int box_geom);


  private:
    G4bool visibility_;

    // PDE for the sensor
    G4double eff_;

    G4int    sensor_depth_;
    G4int    mother_depth_;
    G4int    naming_order_;
    G4double time_binning_;
    G4int    box_geom_;

     // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

  inline void SiPMHamamatsuVUV::SetVisibility(G4bool vis)
  { visibility_ = vis; }
  
  inline void SiPMHamamatsuVUV::SetTimeBinning(G4double time_binning)
  { time_binning_ = time_binning; }

  inline void SiPMHamamatsuVUV::SetSensorDepth(G4int sensor_depth)
  { sensor_depth_ = sensor_depth; }

  inline void SiPMHamamatsuVUV::SetMotherDepth(G4int mother_depth)
  { mother_depth_ = mother_depth; }

  inline void SiPMHamamatsuVUV::SetNamingOrder(G4int naming_order)
  { naming_order_ = naming_order; }

  inline void SiPMHamamatsuVUV::SetBoxGeom(G4int box_geom)
  { box_geom_ = box_geom; }


} // end namespace nexus

#endif
