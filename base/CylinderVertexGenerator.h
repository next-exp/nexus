// ----------------------------------------------------------------------------
///  \file   CylinderVertexGenerator.h
///  \brief  Generator of vertices in a cylindric geometry.
///
///  \author   Javier Muñoz Vidal <jmunoz@ific.uv.es>
///  \date     2 November 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __CYLINDER_VERTEX_GENERATOR__
#define __CYLINDER_VERTEX_GENERATOR__

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <globals.hh>


namespace nexus {


  class CylinderVertexGenerator
  {
  public:
    /// Constructor
    CylinderVertexGenerator(G4double inner_rad, G4double inner_length,
			    G4double body_thickness, G4double endcaps_thickness,
			    G4ThreeVector origin=G4ThreeVector(0.,0.,0.),
			    G4RotationMatrix* rotation=0);
    /// Destructor
    ~CylinderVertexGenerator();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region);

  private:
    G4double GetRadius(G4double inner, G4double outer);
    G4double GetPhi();
    G4double GetLength(G4double origin, G4double max_length);
    G4ThreeVector RotateAndTranslate(G4ThreeVector position);

  private:
    /// Default constructor hidden
    CylinderVertexGenerator();

  private:
    G4double _inner_length, _inner_radius;        ///< Internal dimensions
    G4double _body_thickness, _endcaps_thickness; ///< Thicknesses
    G4double _outer_radius;                       ///< External radius

    G4double _perc_body_surf, _perc_body_vol; ///< Cylinder body percentages

    G4ThreeVector _origin;
    G4RotationMatrix* _rotation;
  };

  
} // namespace nexus

#endif
