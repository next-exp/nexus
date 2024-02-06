// ----------------------------------------------------------------------------
// nexus | Cigar.h
//
// Box-shaped box of material with a coating.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef Cigar_H
#define Cigar_H

#include "GeometryBase.h"
#include "GenericWLSFiber.h"
#include "PmtR11410.h"
#include "BoxPointSampler.h"
#include "MaterialsList.h"

class G4Material;
class G4GenericMessenger;
namespace nexus { class SpherePointSampler; }


namespace nexus {

  /// Spherical chamber filled with xenon (liquid or gas)

  class Cigar: public GeometryBase
  {
  public:
    /// Constructor
    Cigar();
    /// Destructor
    ~Cigar();

    /// Return vertex within region <region> of the chamber
    G4ThreeVector GenerateVertex(const G4String& region) const;

    void Construct();

  private:
    G4double world_z_;             // World dimensions
    G4double world_xy_;
    G4double cigar_length_;
    G4double cigar_width_;
    G4double fiber_diameter_;
    G4String gas_;
    G4double pressure_;
    G4String coating_;
    G4String fiber_type_;
    G4bool coated_;

    GenericWLSFiber* fiber_;
    BoxPointSampler* inside_cigar_;
    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;
  };

} // end namespace nexus

#endif