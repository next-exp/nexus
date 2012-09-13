// ----------------------------------------------------------------------------
///  \file   CylindricChamber.h
///  \brief  A simple cylindric tracking chamber
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     29 Mar 2009
///  \version  $Id$ 
///
///  Copyright (c) 2009-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __CYLINDRIC_CHAMBER__
#define __CYLINDRIC_CHAMBER__

#include "BaseGeometry.h"
#include <vector>


namespace nexus {

  class CylinderPointSampler;

  
  /// A cylindric gaseous tracking chamber filled with xenon
  
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
    ///
    void CalculateELTableVertices(double) const;

  private:
    G4double _radius; ///< Chamber internal radius
    G4double _length; ///< Chamber internal length
    G4double _thickn; ///< Chamber thickness

    G4double _elgap_length; ///< Length of the electroluminescence gap

    G4double _gxe_pressure; ///< Gas pressure

    mutable std::vector<G4ThreeVector> _table_vertices;
    mutable std::vector<G4ThreeVector>::iterator _vtx;

    CylinderPointSampler* _chamber_vertex_gen; ///< Vertices generator
  };

} // end namespace nexus

#endif
