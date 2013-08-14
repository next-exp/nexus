// ----------------------------------------------------------------------------
///  \file   SphereVertexGenerator.h
///  \brief  Generator of random points in a sphere.
///
///  \author   Javier Muñoz Vidal <jmunoz@ific.uv.es>
///  \date     24 November 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __SPHERE_VERTEX_GENERATOR__
#define __SPHERE_VERTEX_GENERATOR__

#include <globals.hh>
#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>


namespace nexus {

  /// Generator of random points in a sphere

  class SphereVertexGenerator
  {
  public:
    /// Constructor
    SphereVertexGenerator(G4double inner_rad, G4double thickness,
			  G4ThreeVector origin=G4ThreeVector(0.,0.,0.),
			  G4RotationMatrix* rotation=0);
    /// Destructor
    ~SphereVertexGenerator() {}


    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region);

  private:
    G4double GetRadius(G4double inner, G4double outer);
    G4double GetPhi();
    G4double GetTheta();
    G4ThreeVector RotateAndTranslate(G4ThreeVector position);
    
  private:
    
    G4double _inner_rad, _outer_rad; ///< Internal and external radius
    G4double _thickness; ///< Sphere thickness

    G4ThreeVector _origin;
    G4RotationMatrix* _rotation;

  

  };

} // namespace nexus

#endif
