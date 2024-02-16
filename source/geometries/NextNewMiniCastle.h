// -----------------------------------------------------------------------------
// nexus | NextNewMiniCastle.h
//
// Copper castle placed inside the lead castle at LSC.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXTNEW_MINI_CASTLE_H
#define NEXTNEW_MINI_CASTLE_H

#include "GeometryBase.h"

#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

class G4GenericMessenger;

namespace nexus {

  class BoxPointSamplerLegacy;

  class NextNewMiniCastle: public GeometryBase
  {
  public:
    /// Constructor
    NextNewMiniCastle();

    /// Destructor
    ~NextNewMiniCastle();

    /// Sets the Logical Volume where Radon tube will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();

    /// The position of the surface of the pedestal is needed
    /// for the castle to lay on it.
    void SetPedestalSurfacePosition(G4double ped_surf_pos);


  private:
    // Mother logical volume
    G4LogicalVolume* mother_logic_;

    // Dimensions
    G4double  x_, y_, z_, thickness_, open_space_z_;
    G4double steel_thickn_;

    // Vertex generators
    BoxPointSamplerLegacy* mini_castle_box_gen_;
    BoxPointSamplerLegacy* mini_castle_external_surf_gen_;
    BoxPointSamplerLegacy* steel_box_gen_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Position of the pedestal surface in y
    G4double pedestal_surf_y_;

    // Visibility
    G4bool visibility_;

    G4GenericMessenger* msg_;

  };

} // end namespace nexus

#endif
