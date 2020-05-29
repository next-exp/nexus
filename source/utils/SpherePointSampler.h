// ----------------------------------------------------------------------------
///  \file   SpherePointSampler.h
///  \brief  Generator of random points in a sphere.
///
///  \author   Javier Munyoz Vidal <jmunoz@ific.uv.es>
///  \date     24 November 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SPHERE_POINT_SAMPLER
#define SPHERE_POINT_SAMPLER

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
		       G4ThreeVector origin=G4ThreeVector(0.,0.,0.), G4RotationMatrix* rotation=0,
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
    G4double _inner_rad, _outer_rad;
    G4double _start_phi, _delta_phi;
    G4double _start_theta, _delta_theta;

    // Needed constants
    G4double _cos_start_theta, _diff_cos_thetas;

    // Origin & Rotation
    G4ThreeVector     _origin;
    G4RotationMatrix* _rotation;

  };

} // namespace nexus

#endif
