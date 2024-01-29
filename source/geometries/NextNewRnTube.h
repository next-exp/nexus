// -----------------------------------------------------------------------------
// nexus | NextNewRnTube.h
//
// Tube around the NEXT-WHITE vessel used to shoot backgrounds coming
// from the air, mainly radon in the air attached to the vessel walls.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXTNEW_RN_TUBE_H
#define NEXTNEW_RN_TUBE_H

#include "GeometryBase.h"

#include <G4Navigator.hh>

class G4GenericMessenger;

namespace nexus {

  class CylinderPointSamplerLegacy;

  class NextNewRnTube: public GeometryBase
  {
  public:
    /// Constructor
    NextNewRnTube();

    /// Destructor
    ~NextNewRnTube();

    /// Sets the Logical Volume where Radon tube will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();


  private:
    // Mother Logical Volume of the ICS
    G4LogicalVolume* mother_logic_;
    // Dimensions
    G4double  inner_diam_, length_, thickness_;

    // Visibility of the shielding
    G4bool visibility_;

    // Vertex generators
    CylinderPointSamplerLegacy* tube_gen_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

} // end namespace nexus

#endif
