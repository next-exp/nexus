// ----------------------------------------------------------------------------
// nexus | Honeycomb.h
//
// Support structure to the EP copper plate.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef HONEYCOMB_H
#define HONEYCOMB_H

#include "GeometryBase.h"

namespace nexus {
  class Honeycomb: public GeometryBase
  {
  public:
    /// Constructor
    Honeycomb();

    /// Destructor
    ~Honeycomb();

    void Construct();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;


  private:

    // Beam rotation angle
    G4double angle_, beam_dist_, beam_thickn_; 
  };

} // end namespace nexus

#endif
