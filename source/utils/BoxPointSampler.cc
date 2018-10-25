// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : Javier Muñoz Vidal <jmunoz@ific.uv.es>
//  Created: 2 November 2009
//
//  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "BoxPointSampler.h"

#include <Randomize.hh>


namespace nexus {

  BoxPointSampler::BoxPointSampler(G4double inner_x,
                                   G4double inner_y, 
                        	   G4double inner_z,
                        	   G4double thickness,
				   G4ThreeVector origin,
				   G4RotationMatrix* rotation):
    _inner_x(inner_x), _inner_y(inner_y), _inner_z(inner_z),
    _thickness(thickness), _origin(origin), _rotation(rotation)
  {
    _outer_x = _inner_x + 2.*_thickness;
    _outer_y = _inner_y + 2.*_thickness;
    _outer_z = _inner_z + 2.*_thickness;

    // The Z face totally covers X and Y faces
    G4double Z_volume = _outer_x * _outer_y * _thickness;
    // The Y face totally covers X faces
    G4double Y_volume = _outer_x * _thickness * _inner_z;
    // The X face does not cover any face
    G4double X_volume = _thickness * _inner_y * _inner_z;
    
    G4double total_volume = Z_volume + Y_volume + X_volume;

    _perc_Zvol = Z_volume / total_volume;
    _perc_Yvol = Y_volume / total_volume;

    // Internal surfaces
    G4double X_surface = _inner_x * _inner_x;
    G4double Y_surface = _inner_y * _inner_y;
    G4double Z_surface = _inner_z * _inner_z;
    G4double total_surface = Z_surface + Y_surface + X_surface;

    _perc_Zsurf = Z_surface / total_surface;
    _perc_Ysurf = Y_surface / total_surface;
  }
  
  
  
  BoxPointSampler::~BoxPointSampler()
  {
  }

  
  
  G4ThreeVector BoxPointSampler::GenerateVertex(const G4String& region)
  {
    G4double x, y, z, origin;
    G4ThreeVector point;
    ///////////////////////////////////////////////////////

    // Default vertex
    if (region == "CENTER") {
      point = G4ThreeVector(0., 0., 0.); 
    }

    ///////////////////////////////////////////////////////
   
    // Generating in the endcap volume
    else if (region == "Z_VOL") {
      x = GetLength(0., _outer_x);
      y = GetLength(0., _outer_y);
      origin = -0.5 * (_inner_z + _thickness);
      z = GetLength(origin, _thickness);
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    ///////////////////////////////////////////////////////

    // Generating in the whole volume
    else if (region == "WHOLE_VOL") {
      G4double rand = G4UniformRand();
      G4double rand2 = G4UniformRand();

      // Z walls volume
      if (rand < _perc_Zvol) {
        x = GetLength(0., _outer_x);
        y = GetLength(0., _outer_y);
        // Selecting between -Z and +Z
        if (rand2 < 0.5) origin = -0.5 * (_inner_z + _thickness);
        else  origin = 0.5 * (_inner_z + _thickness);
        z = GetLength(origin, _thickness);
      }

      // Y walls volume
      else if ( (_perc_Zvol < rand) && (rand < (_perc_Zvol+_perc_Yvol))) {
        x = GetLength(0., _outer_x);
        // Selecting between -Y and +Y
        if (rand2 < 0.5) origin = -0.5 * (_inner_y + _thickness);
        else  origin = 0.5 * (_inner_y + _thickness);
        y = GetLength(origin, _thickness);
        z = GetLength(0., _inner_z);
      }

      // X walls volume
      else {
        // Selecting between -X and +X
        if (rand2 < 0.5) origin = -0.5 * (_inner_x + _thickness);
        else  origin = 0.5 * (_inner_x + _thickness);
        x = GetLength(origin, _thickness);
        y = GetLength(0., _inner_y);
        z = GetLength(0., _inner_z);
      }
      point = RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    ///////////////////////////////////////////////////////
    
    // Generating in the volume inside
    else if (region == "INSIDE") {
      x = GetLength(0., _inner_x);
      y = GetLength(0., _inner_y);
      z = GetLength(0., _inner_z);
      point =  RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    ///////////////////////////////////////////////////////

    // Generating in the endcap surface
    else if (region == "Z_SURF") {
      x = GetLength(0., _inner_x);
      y = GetLength(0., _inner_y);
      z = -0.5 * _inner_z;
      point =  RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    ///////////////////////////////////////////////////////
   
    // Generating in the whole surface
    else if (region == "WHOLE_SURF") {
      G4double rand = G4UniformRand();
      G4double rand2 = G4UniformRand();

      // Z walls surface
      if (rand < _perc_Zsurf) {
        x = GetLength(0., _inner_x);
        y = GetLength(0., _inner_y);
        // +Z wall surface
        if (rand2 < 0.5) z = 0.5 * _inner_z;
        // -Z wall surface
        else  z = -0.5 * _inner_z;
      }

      // Y walls surface
      else if ((_perc_Zsurf < rand) && (rand < (_perc_Zsurf+_perc_Ysurf))) {
        x = GetLength(0., _inner_x);
        // +Y wall surface
        if (rand2 < 0.5) y = 0.5 * _inner_y;
        // -Y wall surface
        else y = -0.5 * _inner_y;
        z = GetLength(0., _inner_z);
      }

      // X walls surface
      else {
      // +X wall surface
        if (rand2 < 0.5) x = 0.5 * _inner_x;
        // -X wall surface
        else x = -0.5 * _inner_x;
        y = GetLength(0., _inner_y);
        z = GetLength(0., _inner_z);
      }

      point =  RotateAndTranslate(G4ThreeVector(x, y, z));
    }

    ///////////////////////////////////////////////////////

    // Unknown region
    else {
      G4String err = "Unknown generation region: " + region;
      G4Exception("GenerateVertex()", "[BoxPointSampler]",
        FatalErrorInArgument, err);   
    }

    return point;

  }



  G4double BoxPointSampler::GetLength(G4double origin, G4double max_length) 
  {
    G4double rand = G4UniformRand() - 0.5;
    return origin + (rand * max_length);
  }



  G4ThreeVector BoxPointSampler::RotateAndTranslate(G4ThreeVector position) 
  {
    G4ThreeVector real_pos = position;

    // Rotating if needed
    if (_rotation) real_pos *= *_rotation;
    // Translating
    real_pos += _origin;
    return real_pos;
  }
  
} // end namespace nexus
