// ----------------------------------------------------------------------------
// nexus | HexagonMeshTools.h
//
// This class provides the functions needed to create a hexagonal mesh
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef HEXAGON_MESH_TOOLS_H
#define HEXAGON_MESH_TOOLS_H

#include <G4ExtrudedSolid.hh>
#include <G4MultiUnion.hh>

namespace nexus {

  class HexagonMeshTools
  {
  public:

    /// Destructor
    ~HexagonMeshTools();

    /// Construct a hexagon with a given thickness
    G4ExtrudedSolid* CreateHexagon(G4double Thickness, G4double circumradius);

    /// Create a union of hexagons
    void CreateHexUnion(G4int nHole, G4double InRadius, G4double Thickness, G4MultiUnion* MeshUnion, G4ExtrudedSolid* hexagon);

  private:
    /// Default constructor is hidden
    HexagonMeshTools();

  };

  // inline methods ..................................................

  inline HexagonMeshTools::~HexagonMeshTools() { }

} // namespace nexus

#endif
