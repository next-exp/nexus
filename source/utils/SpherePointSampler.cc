// ----------------------------------------------------------------------------
// nexus | SpherePointSampler.cc
//
// This class is a sampler of random uniform points in as sphere.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SpherePointSampler.h"

#include <G4RotationMatrix.hh>
#include <Randomize.hh>

#include <math.h>


namespace nexus {

  using namespace CLHEP;

  SpherePointSampler::SpherePointSampler(G4double inner_rad,
                                         G4double thickness,
                                         G4ThreeVector origin,
                                         G4RotationMatrix* rotation,
                                         G4double start_phi,
                                         G4double delta_phi,
                                         G4double start_theta,
                                         G4double delta_theta):
    inner_rad_(inner_rad),
    outer_rad_(inner_rad + thickness),
    start_phi_(start_phi),
    delta_phi_(delta_phi),
    start_theta_(start_theta),
    delta_theta_(delta_theta),
    origin_(origin),
    rotation_(rotation)
  {
    cos_start_theta_ = cos(start_theta_);
    diff_cos_thetas_ = cos_start_theta_ - cos(start_theta_ + delta_theta_);
  }



  G4ThreeVector SpherePointSampler::GenerateVertex(const G4String& region)
  {
    G4double x, y, z;
    G4ThreeVector point;

    // Default vertex
    if (region == "CENTER") {
      point = G4ThreeVector(0., 0., 0.);
    }

    // Generating in the inner surface
    else if (region == "SURFACE") {
      G4double rad = inner_rad_;
      G4double phi = GetPhi();
      G4double theta = GetTheta();
      x = rad * sin(theta) * cos(phi);
      y = rad * sin(theta) * sin(phi);
      z = rad * cos(theta);
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating between the inner and outer surfaces
    else if (region == "VOLUME") {
      G4double rad = GetRadius(inner_rad_, outer_rad_);
      G4double phi = GetPhi();
      G4double theta = GetTheta();
      x = rad * sin(theta) * cos(phi);
      y = rad * sin(theta) * sin(phi);
      z = rad * cos(theta);
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating inside
    else if (region == "INSIDE") {
      G4double rad = GetRadius(0., inner_rad_);
      G4double phi = GetPhi();
      G4double theta = GetTheta();
      x = rad * sin(theta) * cos(phi);
      y = rad * sin(theta) * sin(phi);
      z = rad * cos(theta);
      point =  RotateAndTranslate(G4ThreeVector(x, y, z));
    }


    // Unknown region
    else {
      G4Exception("[SpherePointSampler]", "GenerateVertex()", FatalException,
		              "Unknown Region!");
    }

    return point;
  }



  G4double SpherePointSampler::GetRadius(G4double inner, G4double outer)
  {
    G4double rand = G4UniformRand();
    G4double r = cbrt( (1.-rand) * inner*inner*inner + rand * outer*outer*outer);
    return r;
  }



  G4double SpherePointSampler::GetPhi()
  {
    return (start_phi_ + G4UniformRand() * delta_phi_);
  }



  G4double SpherePointSampler::GetTheta()
  {
    return acos( cos_start_theta_ - G4UniformRand() * diff_cos_thetas_ );
  }



  G4ThreeVector SpherePointSampler::RotateAndTranslate(G4ThreeVector position)
  {
    G4ThreeVector real_pos = position;

    // Rotating if needed
    if (rotation_) real_pos *= *rotation_;
    // Translating
    real_pos += origin_;

    return real_pos;
  }


} // end namespace nexus
