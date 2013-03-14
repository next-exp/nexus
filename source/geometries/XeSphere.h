// ----------------------------------------------------------------------------
///  \file   XeSphere.h
///  \brief  A simple sphere filled with Xe.
///
///  \author   Javier Mu?oz Vidal <jmunoz@ific.uv.es>    
///  \date     27 Nov 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __XE_SPHERE__
#define __XE_SPHERE__

#include "BaseGeometry.h"

class G4Material;
namespace nexus { class SpherePointSampler; }


namespace nexus {
  
  /// Spherical chamber filled with xenon (liquid or gas)
  
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
    void Construct();

  private:
    // Dimensions
    G4double _radius;  ///< Internal radius of the sphere


    // Materials
    G4Material* _xenon;   ///< Material the sphere is made of


    // Vertex Generator
    SpherePointSampler* _sphere_vertex_gen;
  };

} // end namespace nexus

#endif
