// ----------------------------------------------------------------------------
// nexus | SpherePointSampler.h
//
// This class is a sampler of random uniform points in as sphere.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SPHERE_POINT_SAMPLER
#define SPHERE_POINT_SAMPLER

#include "RandomUtils.h"

#include <globals.hh>
#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <G4VPhysicalVolume.hh>

#include <CLHEP/Units/PhysicalConstants.h>

namespace nexus {

  using namespace CLHEP;

  /// Generator of random points in a sphere
  class SpherePointSampler
  {
  public:
    /// Constructor
    SpherePointSampler(G4double min_rad, G4double max_rad, G4double start_phi = 0.,
                       G4double delta_phi = twopi, G4double start_theta = 0.,
                       G4double delta_theta = pi, G4ThreeVector origin=G4ThreeVector(0.,0.,0.),
                       G4RotationMatrix* rotation=0);

    SpherePointSampler (G4VPhysicalVolume* physVolume);

    /// Destructor
    ~SpherePointSampler() {}


    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const vtx_region& region);

  private:
    G4double GetRadius(G4double inner, G4double outer);
    G4double GetPhi();
    G4double GetTheta();
    G4ThreeVector RotateAndTranslate(G4ThreeVector position);

  private:
    // Sphere dimensions
    G4double inner_rad_, outer_rad_;
    G4double start_phi_, delta_phi_;
    G4double start_theta_, delta_theta_;

    // Needed constants
    G4double cos_start_theta_, diff_cos_thetas_;

    // Origin & Rotation
    G4ThreeVector     origin_;
    G4RotationMatrix* rotation_;

  };

} // namespace nexus

#endif
