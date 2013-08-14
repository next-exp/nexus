// ----------------------------------------------------------------------------
///  \file   XeSphere.h
///  \brief  A simple sphere filled with Xe.
///
///  \author   Javier Muñoz Vidal <jmunoz@ific.uv.es>    
///  \date     27 Nov 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __XE_SPHERE__
#define __XE_SPHERE__

#include "BaseGeometry.h"

//class G4LogicalVolume;
class G4Material;


namespace nexus {
  
  class SphereVertexGenerator;
  
  
  /// Geometry of a sphere filled with Xe
  
  class XeSphere: public BaseGeometry
  {
  public:
    /// Constructor
    XeSphere();
    /// Destructor
    ~XeSphere();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region) const;

  private:
    void SetParameters();
    void BuildGeometry();

  private:
    // Dimensions
    G4double _radius;  ///< Sphere internal radius
    G4double _thickn;  ///< Sphere thickness

    // Materials
    G4Material* _sphere_mat;   ///< Material the sphere is made of
    G4Material* _tracking_mat; ///< Material used as tracker

    // Vertex Generator
    SphereVertexGenerator* _sphere_vertex_gen;
  };

} // end namespace nexus

#endif
