// ----------------------------------------------------------------------------
///  \file   XeSphere.h
///  \brief  A simple sphere filled with Xe.
///
///  \author   Javier Mu?oz Vidal <jmunoz@ific.uv.es>    
///  \date     27 Nov 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef _XE_SPHERE___
#define _XE_SPHERE___

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

    void Construct();

  private:
    G4bool liquid_;     ///< Whether xenon is liquid or not
    G4double pressure_; ///< Pressure (if gaseous state was selected)
    G4double radius_;   ///< Radius of the sphere

    /// Vertexes random generator
    SpherePointSampler* sphere_vertex_gen_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_; 
  };

} // end namespace nexus

#endif
