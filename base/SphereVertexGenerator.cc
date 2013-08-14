// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : Javier Muñoz Vidal <jmunoz@ific.uv.es>
//  Created: 2 November 2009
//
//  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SphereVertexGenerator.h"

#include <G4RotationMatrix.hh>
#include <Randomize.hh>


namespace nexus {


  SphereVertexGenerator::SphereVertexGenerator(G4double inner_rad, 
					       G4double thickness,
					       G4ThreeVector origin, 
					       G4RotationMatrix* rotation):
    _inner_rad(inner_rad), _thickness(thickness), _origin(origin), 
    _rotation(rotation), _outer_rad(inner_rad + thickness)
  {
  }
  

  
  G4ThreeVector SphereVertexGenerator::GenerateVertex(const G4String& region)
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


    else if (region == "VOLUME") {
      G4double rad = GetRadius(_inner_rad, _outer_rad);
      G4double phi = GetPhi();
      G4double theta = GetTheta();
      x = rad * sin(theta) * cos(phi);
      y = rad * sin(theta) * sin(phi);
      z = rad * cos(theta);
      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }


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
      G4Exception("[SphereVertexGenerator] ERROR: Unknown Region!");
    }
  }
  


  G4double SphereVertexGenerator::GetRadius(G4double inner, G4double outer) 
  {
    G4double rand = G4UniformRand();
    G4double r = sqrt( (1.-rand) * inner*inner + rand * outer*outer);
    return r;
  }
  
  
  
  G4double SphereVertexGenerator::GetPhi() 
  {
    return (G4UniformRand() * twopi);
  }



  G4double SphereVertexGenerator::GetTheta() 
  {
    return (G4UniformRand() * halfpi);
  }



  G4ThreeVector SphereVertexGenerator::RotateAndTranslate(G4ThreeVector position) 
  {
    G4ThreeVector real_pos = position;
    
    // Rotating if needed
    if (_rotation) real_pos *= *_rotation;
    // Translating
    real_pos += _origin;

    return real_pos;
  }
  

} // end namespace nexus
