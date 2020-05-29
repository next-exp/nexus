// ----------------------------------------------------------------------------
// nexus | PmtR11410.h
//
// Geometry of the Hamamatsu R11410 photomultiplier.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PMT_R11410_H
#define PMT_R11410_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>

class G4OpticalSurface;
class G4GenericMessenger;

namespace nexus {

  class CylinderPointSampler;


  /// Geometry model for the Hamamatsu R11410-10 photomultiplier

  class PmtR11410: public BaseGeometry
  {
  public:
    /// Constructor
    PmtR11410();

    /// Destructor
    ~PmtR11410();

    G4ThreeVector GetRelPosition();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    // Builder
    void Construct();


  private:
    G4OpticalSurface* GetPhotOptSurf();

  private:
    // Dimensions
    G4double front_body_diam_, front_body_length_;
    G4double rear_body_diam_, rear_body_length_;
    G4double body_thickness_;
    G4double window_diam_, window_thickness_;
    G4double photocathode_diam_, photocathode_thickness_;

    // Vertex generators
    CylinderPointSampler* front_body_gen_;
    CylinderPointSampler* medium_body_gen_;
    CylinderPointSampler* rear_body_gen_;
    CylinderPointSampler* rear_cap_gen_;
    CylinderPointSampler* front_cap_gen_;

    G4double front_body_perc_, fr_med_body_perc_, fr_med_re_body_perc_;
    G4double front_perc_, fr_med_perc_, fr_med_re_perc_, fr_med_re_cap_perc_;

    G4bool visibility_;
    G4int sd_depth_;

    //Messenger for configuration parameters
    G4GenericMessenger* msg_;

    G4double binning_;

  };


} // end namespace nexus

#endif
