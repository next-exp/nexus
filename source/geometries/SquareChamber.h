// ----------------------------------------------------------------------------
///  \file   SquareChamber.h
///  \brief  A simple square tracking chamber.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     1 August 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __SQUARE_CHAMBER__
#define __SQUARE_CHAMBER__

#include "BaseGeometry.h"


namespace nexus {

  class BoxPointSampler;
  

  /// Geometry of a square tracking chamber filled with gaseous xenon

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

    BoxPointSampler* _chamber_vertex_gen; ///< Vertices random generator
  };

} // end namespace nexus

#endif
