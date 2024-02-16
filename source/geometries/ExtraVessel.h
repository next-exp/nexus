// ----------------------------------------------------------------------------
// nexus | ExtraVessel.h
//
// Volume used to simulate radioactive background from materials such as
// connectors, feedthroughs and adapter boards, placed outside the vessel,
// behind the tracking plane.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef EXTRA_VESSEL_H
#define EXTRA_VESSEL_H

#include "GeometryBase.h"
#include <G4ThreeVector.hh>

class G4GenericMessenger;

namespace nexus {

  class CylinderPointSamplerLegacy;

  class ExtraVessel: public GeometryBase
  {
  public:
    /// Constructor
    ExtraVessel();

    /// Destructor
    ~ExtraVessel();

    G4ThreeVector GetRelPosition();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    // Builder
    void Construct();

  private:
    // Dimensions
    G4double diameter_, thickness_;

    // Vertex generators
    CylinderPointSamplerLegacy* generic_gen_;

    G4bool visibility_;

    //Messenger for configuration parameters
    G4GenericMessenger* msg_;

  };


} // end namespace nexus

#endif
