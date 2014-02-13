// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : Javier Muñoz Vidal <jmunoz@ific.uv.es>
//  Created: 2 November 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SpherePointSampler.h"

#include <G4RotationMatrix.hh>
#include <Randomize.hh>

#include "CLHEP/Units/PhysicalConstants.h"


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
    _inner_rad(inner_rad), _thickness(thickness), _origin(origin), 
    _rotation(rotation), _outer_rad(inner_rad + thickness),
    _start_phi(start_phi), _delta_phi(delta_phi),
    _start_theta(start_theta), _delta_theta(delta_theta)
  {
    _cos_start_theta = cos(_start_theta);
    _diff_cos_thetas = _cos_start_theta - cos(_start_theta + _delta_theta);
  }
  

  
  G4ThreeVector SpherePointSampler::GenerateVertex(const G4String& region)
  {
    G4double x, y, z, origin;
    
    // Default vertex
    if (region == "CENTER") {
      return G4ThreeVector(0., 0., 0.); 
    }
    
    // Generating in the inner surface
    else if (region == "SURFACE") {
      G4double rad = _inner_rad;
      G4double phi = GetPhi();
      G4double theta = GetTheta();
      x = rad * sin(theta) * cos(phi);
      y = rad * sin(theta) * sin(phi);
      z = rad * cos(theta);
      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating between the inner and outer surfaces
    else if (region == "VOLUME") {
      G4double rad = GetRadius(_inner_rad, _outer_rad);
      G4double phi = GetPhi();
      G4double theta = GetTheta();
      x = rad * sin(theta) * cos(phi);
      y = rad * sin(theta) * sin(phi);
      z = rad * cos(theta);
      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating inside
    else if (region == "INSIDE") {
      G4double rad = GetRadius(0., _inner_rad);
      G4double phi = GetPhi();
      G4double theta = GetTheta();
      x = rad * sin(theta) * cos(phi);
      y = rad * sin(theta) * sin(phi);
      z = rad * cos(theta);
      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }


    // Unknown region
    else {
      G4Exception("[SpherePointSampler]", "GenerateVertex()", FatalException,
		  "Unknown Region!");
      //G4Exception("[SpherePointSampler] ERROR: Unknown Region!");
    }
  }
  


  G4double SpherePointSampler::GetRadius(G4double inner, G4double outer) 
  {
    G4double rand = G4UniformRand();
    G4double r = sqrt( (1.-rand) * inner*inner + rand * outer*outer);
    return r;
  }
  
  
  
  G4double SpherePointSampler::GetPhi() 
  {
    return (_start_phi + G4UniformRand() * _delta_phi);
  }



  G4double SpherePointSampler::GetTheta() 
  {
    return acos( _cos_start_theta - G4UniformRand() * _diff_cos_thetas );
  }



  G4ThreeVector SpherePointSampler::RotateAndTranslate(G4ThreeVector position) 
  {
    G4ThreeVector real_pos = position;
    
    // Rotating if needed
    if (_rotation) real_pos *= *_rotation;
    // Translating
    real_pos += _origin;

    return real_pos;
  }
  

} // end namespace nexus
