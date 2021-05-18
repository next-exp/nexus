// -----------------------------------------------------------------------------
//  nexus | Next100SiPM.h
//
//  Geometry of the Hamamatsu MPPC S13372-1350TE, the model of
//  silicon photomultiplier (SiPM) used in the NEXT-100 detector.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT100_SIPM_H
#define NEXT100_SIPM_H

#include "GeometryBase.h"

#include <G4ThreeVector.hh>

namespace nexus {

  class Next100SiPM: public GeometryBase
  {
  public:
    // Constructor
    Next100SiPM();
    /// Destructor
    ~Next100SiPM();

    // Return dimensions of the SiPM
    G4ThreeVector GetDimensions() const;

    // Invoke this method to build the volumes of the geometry
    void Construct() override;

    // Needed settings for correct numbering
    void SetSensorDepth (G4int sensor_depth);
    void SetMotherDepth (G4int mother_depth);
    void SetNamingOrder (G4int naming_order);
    void SetTimeBinning (G4double time_binning);
    void SetVisibility  (G4bool visibility);

  private:
    G4ThreeVector dimensions_;

    G4int    sensor_depth_;
    G4int    mother_depth_;
    G4int    naming_order_;
    G4double time_binning_;

    G4bool visibility_;

  };

  inline void Next100SiPM::SetTimeBinning(G4double time_binning)
  { time_binning_ = time_binning; }

  inline void Next100SiPM::SetSensorDepth(G4int sensor_depth)
  { sensor_depth_ = sensor_depth; }

  inline void Next100SiPM::SetMotherDepth(G4int mother_depth)
  { mother_depth_ = mother_depth; }

  inline void Next100SiPM::SetNamingOrder(G4int naming_order)
  { naming_order_ = naming_order; }

  inline void Next100SiPM::SetVisibility(G4bool visibility)
  { visibility_ = visibility; }

}

#endif
