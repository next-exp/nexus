// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : Javier Muñoz Vidal <jmunoz@ific.uv.es>
//  Created: 2 November 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "CylinderPointSampler.h"

#include <Randomize.hh>


namespace nexus {

  
  CylinderPointSampler::CylinderPointSampler
  (G4double inner_radius, G4double inner_length, 
   G4double body_thickness, G4double endcaps_thickness, 
   G4ThreeVector origin, G4RotationMatrix* rotation):
    _inner_radius(inner_radius), _inner_length(inner_length),
    _body_thickness(body_thickness), _endcaps_thickness(endcaps_thickness),
    _outer_radius(_inner_radius + _body_thickness),
    _origin(origin), _rotation(rotation)
  {
    // Calculate surface ratio between body and endcaps
    G4double body_surf = twopi * _inner_radius * _inner_length;
    G4double endcaps_surf = 2. * pi * _inner_radius * _inner_radius;
    _perc_body_surf = body_surf / (body_surf + endcaps_surf);

    // Calculate volume ratio between body and endcaps
    G4double body_vol = pi * _inner_length * 
      (_outer_radius*_outer_radius - _inner_radius*_inner_radius);
    G4double endcaps_vol = 
      2. * (pi * _outer_radius*_outer_radius) * _endcaps_thickness;
    _perc_body_vol = body_vol / (body_vol + endcaps_vol);
  }
  
  
  
  CylinderPointSampler::~CylinderPointSampler()
  {
  }
  

  
  G4ThreeVector CylinderPointSampler::GenerateVertex(const G4String& region)
  {
    G4double x, y, z;
 
    // Center of the chamber
    if (region == "CENTER") {
      return RotateAndTranslate(G4ThreeVector(0., 0., 0.)); 
    }
    
    // Generating in the endcap volume
    else if (region == "ENDCAP_VOL") {
      G4double phi = GetPhi();
      G4double rad = GetRadius(0., _outer_radius);
      x = rad * cos(phi);
      y = rad * sin(phi);
      G4double origin = -0.5 * (_inner_length + _endcaps_thickness);
      z = GetLength(origin, _endcaps_thickness);
      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating in the body volume
    else if (region == "BODY_VOL") {
      G4double phi = GetPhi();
      G4double rad = GetRadius(_inner_radius, _outer_radius);
      x = rad * cos(phi);
      y = rad * sin(phi);
      G4double origin = 0.;
      z = GetLength(origin, _inner_length);
      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating in the whole volume
    else if (region == "WHOLE_VOL") {

      G4double rand = G4UniformRand();

      if (rand < _perc_body_vol) {
        // Generating in the body volume
        G4double phi = GetPhi();
        G4double rad = GetRadius(_inner_radius, _outer_radius);
        x = rad * cos(phi);
        y = rad * sin(phi);
        G4double origin = 0.;
        z = GetLength(origin, _inner_length);
      }
      else {
        // Generating in the endcaps volume
        G4double phi = GetPhi();
        G4double rad = GetRadius(0., _outer_radius);
        x = rad * cos(phi);
        y = rad * sin(phi);
        G4double origin = -0.5 * (_inner_length + _endcaps_thickness);
        z = GetLength(origin, _endcaps_thickness);
      }

      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }
    
    // Generating in the volume inside
    else if (region == "INSIDE") {
      G4double phi = GetPhi();
      G4double rad = GetRadius(0., _outer_radius);
      x = rad * cos(phi);
      y = rad * sin(phi);
      G4double origin = 0.;
      z = GetLength(origin, _inner_length);
      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Generating in the endcap surface
    else if (region == "ENDCAP_SURF") {
      G4double phi = GetPhi();
      G4double rad = GetRadius(0., _inner_radius);
      x = rad * cos(phi);
      y = rad * sin(phi);
      z = -0.5 * _inner_length;
      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }
   
    // Generating in the body surface
    else if (region == "BODY_SURF") {
      G4double phi = GetPhi();
      x = _inner_radius * cos(phi);
      y = _inner_radius * sin(phi);
      G4double origin = 0.;
      z = GetLength(origin, _inner_length);
      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }
   
    // Generating in the whole surface
    else if (region == "WHOLE_SURF") {

      G4double rand = G4UniformRand();

      if (rand < _perc_body_surf) {
        // Generating in the body surface
        G4double phi = GetPhi();
        x = _inner_radius * cos(phi);
        y = _inner_radius * sin(phi);
        G4double origin = 0.;
        z = GetLength(origin, _inner_length);
      }
      else {
        // Generating in the endcaps surface
        G4double phi = GetPhi();
        G4double rad = GetRadius(0., _inner_radius);
        x = rad * cos(phi);
        y = rad * sin(phi);
        z = -0.5 * _inner_length;
      }

      return RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    // Unknown region
    else {
      return G4ThreeVector();
      //G4Exception("[CylinderPointSampler] ERROR: Unknown region!");
    }
  }



  G4double CylinderPointSampler::GetRadius(G4double inner, G4double outer) 
  {
    G4double rand = G4UniformRand();
    G4double r = sqrt( (1.-rand) * inner*inner + rand * outer*outer);
    return r;
  }


  
  G4double CylinderPointSampler::GetPhi() 
  {
    return (G4UniformRand() * twopi);
  }


  
  G4double CylinderPointSampler::GetLength(G4double origin, 
					      G4double max_length) 
  {
    return (origin + (G4UniformRand() - 0.5) * max_length);
  }

  

  G4ThreeVector 
  CylinderPointSampler::RotateAndTranslate(G4ThreeVector position) 
  {
    G4ThreeVector real_pos = position;
    
    // Rotating if needed
    if (_rotation) real_pos *= *_rotation;
    // Translating
    real_pos += _origin;
    
    return real_pos;
  }
  

} // end namespace nexus
