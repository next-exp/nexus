// ----------------------------------------------------------------------------
///  \file   SquareChamber.h
///  \brief  A simple square tracking chamber.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     1 August 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __SQUARE_CHAMBER__
#define __SQUARE_CHAMBER__

#include "BaseGeometry.h"


namespace nexus {

  class BoxVertexGenerator;
  

  /// Geometry of a square tracking chamber

  class SquareChamber: public BaseGeometry
  {
  public:
    /// Constructor
    SquareChamber();
    /// Destructor
    ~SquareChamber();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    void SetParameters();
    void BuildGeometry();

  private:

    G4double _width, _height, _length, _thickn;
    G4double _pressure;

    // Vertex Generator
    BoxVertexGenerator* _chamber_vertex_gen;
  };

} // end namespace nexus

#endif
