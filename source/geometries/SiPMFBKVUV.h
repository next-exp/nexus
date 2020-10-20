// ----------------------------------------------------------------------------
// nexus | SiPMFBKVUV.cc
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

  class SiPMFBKVUV: public BaseGeometry
  {
  public:
    /// Constructor
    SiPMFBKVUV();
    /// Destructor
    ~SiPMFBKVUV();

    /// Invoke this method to build the volumes of the geometry
    void Construct();

    void SetVisibility(G4bool vis);
    void SetPDE(G4double eff);
    void SetTimeBinning (G4double time_binning);
    void SetSensorDepth (G4int sensor_depth);
    void SetMotherDepth (G4int mother_depth);
    void SetNamingOrder (G4int naming_order);
    void SetBoxGeom (G4int box_geom);


  private:
    G4bool visibility_;

    // PDE for the sensor
    G4double eff_;

    G4double sensor_depth_, mother_depth_;
    G4double naming_order_, time_binning_;
    G4int box_geom_;

  };

  inline void SiPMFBKVUV::SetVisibility(G4bool vis)
  { visibility_ = vis; }

  inline void SiPMFBKVUV::SetPDE(G4double eff)
  { eff_ = eff; }

  inline void SiPMFBKVUV::SetTimeBinning(G4double time_binning)
  { time_binning_ = time_binning; }

  inline void SiPMFBKVUV::SetSensorDepth(G4int sensor_depth)
  { sensor_depth_ = sensor_depth; }

  inline void SiPMFBKVUV::SetMotherDepth(G4int mother_depth)
  { mother_depth_ = mother_depth; }

  inline void SiPMFBKVUV::SetNamingOrder(G4int naming_order)
  { naming_order_ = naming_order; }

  inline void SiPMFBKVUV::SetBoxGeom(G4int box_geom)
  { box_geom_ = box_geom; }


} // end namespace nexus

#endif
