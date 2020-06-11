// ----------------------------------------------------------------------------
// nexus | SpherePointSampler.h
//
// This class is a sampler of random uniform points in as sphere.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SPHERE_POINT_SAMPLER_H
#define SPHERE_POINT_SAMPLER_H

#include <globals.hh>
#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>

#include <CLHEP/Units/PhysicalConstants.h>

namespace nexus {

  using namespace CLHEP;

  /// Generator of random points in a sphere
  class SpherePointSampler
  {
  public:
    /// Constructor
    SpherePointSampler(G4double inner_rad, G4double thickness,
		       G4ThreeVector origin=G4ThreeVector(0.,0.,0.),
                       G4RotationMatrix* rotation=0,
		       G4double start_phi = 0., G4double delta_phi = twopi,
		       G4double start_theta = 0., G4double delta_theta = pi);
    /// Destructor
    ~SpherePointSampler() {}


    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region);

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
