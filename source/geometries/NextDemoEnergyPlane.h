// ----------------------------------------------------------------------------
// nexus | NextDemoEnergyPlane.h
//
// Energy plane geometry of the DEMO++ detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT_DEMO_ENERGY_PLANE_H
#define NEXT_DEMO_ENERGY_PLANE_H

#include <vector>
#include <G4Navigator.hh>

#include "PmtR11410.h"


class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  /// This is a class to place all the components of the energy plane

  class NextDemoEnergyPlane: public BaseGeometry
  {

  public:
    /// Constructor
    NextDemoEnergyPlane();

    /// Destructor
    ~NextDemoEnergyPlane();

    /// Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Sets the z position of the surface of the sapphire windows
    void SetGateSapphireSurfaceDistance(G4double z);

    /*/// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;
    */

    // Builder
    void Construct();


  private:
    void GeneratePmtPositions();

  private:

    // Dimensions
    const G4double gate_support_surface_dist_;
    const G4int num_PMTs_;
    const G4double carrier_plate_thickness_, carrier_plate_diam_;
    const G4double carrier_plate_central_hole_diam_;
    const G4double pmt_hole_length_, pmt_hole_diam_;
    const G4double pmt_flange_length_;
    const G4double sapphire_window_thickness_, sapphire_window_diam_;
    const G4double pedot_coating_thickness_;
    const G4double optical_pad_thickness_;
    const G4double pmt_base_diam_, pmt_base_thickness_;
    const G4double tpb_thickness_;

    // Visibility and verbosity
    G4bool visibility_, verbosity_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

    // PMT
    PmtR11410*  pmt_;
    std::vector<G4ThreeVector> pmt_positions_;

    // Mother logical volume of the whole energy pLane
    G4LogicalVolume* mother_logic_;

    // Position of the syrface of the sapphire windows,
    // starting from z = 0 in GATE
    G4double gate_sapphire_dist_;

  };

} //end namespace nexus
#endif
