// ----------------------------------------------------------------------------
// nexus | HexagonMeshTools.h
//
// Functions for helping with hexagonal mesh creation
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef HEXAGON_MESH_TOOLS_H
#define HEXAGON_MESH_TOOLS_H

#include <G4ExtrudedSolid.hh>
#include <G4VPhysicalVolume.hh>

namespace nexus {

    /// Construct a hexagon with a given thickness
    G4ExtrudedSolid* CreateHexagon(G4double half_thickness, G4double circumradius);

    /// Place hexagons inside a disk to create a mesh
    void PlaceHexagons(G4int n_hole, G4double in_radius, G4double thickness, G4LogicalVolume* disk_logical, G4LogicalVolume* hex_logical, G4double mesh_diam);

} // namespace nexus

#endif
