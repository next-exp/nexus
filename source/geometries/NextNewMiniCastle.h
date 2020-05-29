// -----------------------------------------------------------------------------
// nexus | NextNewMiniCastle.h
//
// Copper castle placed inside the lead castle at LSC.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXTNEW_MINI_CASTLE
#define NEXTNEW_MINI_CASTLE

#include "BaseGeometry.h"

#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

class G4GenericMessenger;

namespace nexus {

  class BoxPointSampler;

  class NextNewMiniCastle: public BaseGeometry
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
    BoxPointSampler* mini_castle_box_gen_;
    BoxPointSampler* mini_castle_external_surf_gen_;
    BoxPointSampler* steel_box_gen_;
    
    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Position of the pedestal surface in y
    G4double pedestal_surf_y_;

  };

} // end namespace nexus

#endif
