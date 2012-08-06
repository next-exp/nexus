// ----------------------------------------------------------------------------
///  \file   SquareChamber.h
///  \brief  A box-shaped chamber filled with xenon.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     1 August 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __SQUARE_CHAMBER__
#define __SQUARE_CHAMBER__

#include "BaseGeometry.h"


namespace nexus {

  class BoxPointSampler;
  

  /// Geometry of a box-shaped chamber filled with xenon

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
    /// Read user configuration parameters
    void SetParameters();
    /// Define the volumes of the geometry
    void BuildGeometry();
    
  private:
    G4double _width, _height, _length, _thickn; ///< Chamber dimensions
    G4double _elgap_length; ///< Length of the EL gap
    G4double _pitch; /// Distance between sensors
    G4double _gxe_pressure; ///< Xenon gas pressure

    BoxPointSampler* _chamber_vertex_gen; ///< Random generator of vertices
  };

} // end namespace nexus

#endif
