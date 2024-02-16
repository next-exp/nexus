// -----------------------------------------------------------------------------
// nexus | NextNewPedestal.h
//
// Table supporting the NEXT-WHITE vessel.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXTNEW_PEDESTAL_H
#define NEXTNEW_PEDESTAL_H

#include "GeometryBase.h"

#include <G4Navigator.hh>

class G4GenericMessenger;

namespace nexus {

  class BoxPointSamplerLegacy;

  class NextNewPedestal: public GeometryBase
  {
  public:
    /// Constructor
    NextNewPedestal();

    /// Destructor
    ~NextNewPedestal();

    /// Sets the logical volume where the pedestal will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();

    /// Setters and getters
    void SetPosition(G4double pos);
    G4ThreeVector GetDimensions() const;

  private:
    // Mother Logical Volume of the ICS
    G4LogicalVolume* mother_logic_;
    // Dimensions
    G4double  table_x_, table_y_, table_z_, y_pos_;

    // Visibility of the shielding
    G4bool visibility_;

    // Vertex generators
    BoxPointSamplerLegacy* table_gen_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

} // end namespace nexus

#endif
