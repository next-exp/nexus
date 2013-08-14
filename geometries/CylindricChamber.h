// ----------------------------------------------------------------------------
///  \file   CylindricChamber.h
///  \brief  A simple cylindric tracking chamber.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     29 Mar 2009
///  \version  $Id$ 
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __CYLINDRIC_CHAMBER__
#define __CYLINDRIC_CHAMBER__

#include "BaseGeometry.h"

class G4Material;


namespace nexus {

  class CylinderVertexGenerator;

  
  /// Geometry of a cylindric tracking chamber.
  
  class CylindricChamber: public BaseGeometry
  {
  public:
    /// Constructor
    CylindricChamber();
    /// Destructor
    ~CylindricChamber();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    void SetParameters();
    void BuildGeometry();

  private:
    // Dimensions
    G4double _radius;  ///< Chamber internal radius
    G4double _length;  ///< Chamber internal length
    G4double _thickn;  ///< Chamber thickness

    // Materials
    G4Material* _chamber_mat;  ///< Material the chamber is made of
    G4Material* _tracking_mat; ///< Material used as tracker

    /// Vertices generator
    CylinderVertexGenerator* _chamber_vertex_gen;
  };

} // end namespace nexus

#endif
