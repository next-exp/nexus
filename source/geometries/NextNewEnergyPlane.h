// -----------------------------------------------------------------------------
// nexus | NextNewEnergyPlane.h
//
// Energy plane of the NEXT-WHITE detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXTNEW_ENERGY_PLANE_H
#define NEXTNEW_ENERGY_PLANE_H

#include <vector>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

#include "NextNewPmtEnclosure.h"


class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  class CylinderPointSamplerLegacy;

  /// This is a class to place all the components of the energy plane
  class NextNewEnergyPlane: public GeometryBase
  {

  public:
    /// Constructor
    NextNewEnergyPlane();

    /// Destructor
    ~NextNewEnergyPlane();

    /// Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    // Builder
    void Construct();

  private:

    void GeneratePMTsPositions();
    void GenerateGasHolePositions();

    // Mother Logical Volume of the whole Energy PLane
    G4LogicalVolume* mother_logic_;

    const G4int num_PMTs_;
    const G4int num_gas_holes_;
    // Dimensions
    const G4double energy_plane_z_pos_;
    const G4double carrier_plate_front_buffer_thickness_,carrier_plate_front_buffer_diam_ ;
    const G4double carrier_plate_thickness_, carrier_plate_diam_,enclosure_hole_diam_,gas_hole_diam_,gas_hole_pos_ ;
    const G4double tpb_thickness_;

    const G4double axial_port_hole_diam_,  axial_port_thickn_;


    NextNewPmtEnclosure*  enclosure_;
    G4double enclosure_z_pos_;
    std::vector<G4ThreeVector> pmt_positions_;
    std::vector<G4ThreeVector> gas_hole_positions_;

    // Visibility of the energy  plane
    G4bool visibility_;

    // Vertex generators
    CylinderPointSamplerLegacy* carrier_gen_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;
    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;
  };

} //end namespace nexus
#endif
