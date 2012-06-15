// ----------------------------------------------------------------------------
///  \file   CylindricChamber.h
///  \brief  A simple cylindric tracking chamber.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     29 Mar 2009
///  \version  $Id$ 
///
///  Copyright (c) 2009-2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __CYLINDRIC_CHAMBER__
#define __CYLINDRIC_CHAMBER__

#include "BaseGeometry.h"


namespace nexus {

  class CylinderPointSampler;

  
  /// Geometry of a cylindric chamber filled with liquid xenon
  
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
    /// Read user configuration parameters
    void SetParameters();
    /// Define geometry volumes
    void BuildGeometry();

  private:
    G4double _radius; ///< Chamber internal radius
    G4double _length; ///< Chamber internal length
    G4double _thickn; ///< Chamber thickness

    G4double _gxe_pressure; ///< Gas pressure

    CylinderPointSampler* _chamber_vertex_gen; ///< Vertices generator
  };

} // end namespace nexus

#endif
