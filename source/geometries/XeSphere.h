// ----------------------------------------------------------------------------
// nexus | XeSphere.h
//
// Sphere filled with xenon.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef XE_SPHERE_H
#define XE_SPHERE_H

#include "GeometryBase.h"

class G4Material;
class G4GenericMessenger;
namespace nexus { class SpherePointSampler; }


namespace nexus {

  /// Spherical chamber filled with xenon (liquid or gas)

  class XeSphere: public GeometryBase
  {
  public:
    /// Constructor
    XeSphere();
    /// Destructor
    ~XeSphere();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& /*region*/) const;

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
