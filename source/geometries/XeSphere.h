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
class G4GenericMessenger;
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
    G4bool _liquid;     ///< Whether xenon is liquid or not
    G4double _pressure; ///< Pressure (if gaseous state was selected)
    G4double _radius;   ///< Radius of the sphere

    /// Vertexes random generator
    SpherePointSampler* _sphere_vertex_gen;

    /// Messenger for the definition of control commands
    G4GenericMessenger* _msg; 
  };

} // end namespace nexus

#endif
