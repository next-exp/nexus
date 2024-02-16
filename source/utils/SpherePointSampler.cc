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
#include <G4LogicalVolume.hh>
#include <G4Sphere.hh>

#include <math.h>


namespace nexus {

  using namespace CLHEP;

  SpherePointSampler::SpherePointSampler(G4double min_rad, G4double max_rad,
                                         G4double start_phi, G4double delta_phi,
                                         G4double start_theta, G4double delta_theta,
                                         G4ThreeVector origin, G4RotationMatrix* rotation):
    inner_rad_(min_rad),
    outer_rad_(max_rad),
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


  // Constructor via Geant4 Physical Volume
  SpherePointSampler::SpherePointSampler (G4VPhysicalVolume* physVolume):
    origin_(physVolume->GetObjectTranslation()),    
    rotation_(physVolume->GetObjectRotation())
  {
    G4Sphere* solidVolume = dynamic_cast<G4Sphere*> (physVolume->GetLogicalVolume()->GetSolid());
    inner_rad_   = solidVolume->GetInnerRadius();
    outer_rad_   = solidVolume->GetOuterRadius();
    start_phi_   = solidVolume->GetStartPhiAngle();
    delta_phi_   = solidVolume->GetDeltaPhiAngle();
    start_theta_ = solidVolume->GetStartThetaAngle();
    delta_theta_ = solidVolume->GetDeltaThetaAngle();
  }



  G4ThreeVector SpherePointSampler::GenerateVertex(const vtx_region& region)
  {
    G4double x = 0.;
    G4double y = 0.;
    G4double z = 0.;
    G4ThreeVector point;

    // Default vertex
    if (region == CENTER) {
      x = y = z = 0.;
    }

    // Generating inside the sphere between the inner and outer surfaces
    else if (region == VOLUME) {
      G4double rad = GetRadius(inner_rad_, outer_rad_);
      G4double phi = GetPhi();
      G4double theta = GetTheta();
      x = rad * sin(theta) * cos(phi);
      y = rad * sin(theta) * sin(phi);
      z = rad * cos(theta);      
    }

    // Generating in the inner surface
    else if (region == INNER_SURF) {
      G4double rad = inner_rad_;
      G4double phi = GetPhi();
      G4double theta = GetTheta();
      x = rad * sin(theta) * cos(phi);
      y = rad * sin(theta) * sin(phi);
      z = rad * cos(theta);      
    }

    // Generating in the outer surface
    else if (region == OUTER_SURF) {
      G4double rad = outer_rad_;
      G4double phi = GetPhi();
      G4double theta = GetTheta();
      x = rad * sin(theta) * cos(phi);
      y = rad * sin(theta) * sin(phi);
      z = rad * cos(theta);      
    }

    // Unknown region
    else {
      G4Exception("[SpherePointSampler]", "GenerateVertex()", FatalException,
		              "Unknown Region! Possible are VOLUME, INNER_SURF and OUTER_SURF");
    }

    return RotateAndTranslate(G4ThreeVector(x, y, z));
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
