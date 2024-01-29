// -----------------------------------------------------------------------------
// nexus | NextNewTrackingPlane.h
//
// TrackingPlane of the NEXT-WHITE detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXTNEW_TRACKING_PLANE_H
#define NEXTNEW_TRACKING_PLANE_H

#include <G4LogicalVolume.hh>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

#include "NextNewKDB.h"

class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;

namespace nexus {

  class CylinderPointSamplerLegacy;
  class BoxPointSamplerLegacy;

  /// This is a geometry formed by all the components of the tracking plane
  class NextNewTrackingPlane: public GeometryBase
  {

  public:
    /// Constructor
    NextNewTrackingPlane();
    /// Destructor
    ~NextNewTrackingPlane();

    // Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();



  private:
    void GenerateDBPositions();
    void PrintAbsoluteSiPMPos(G4ThreeVector displ, G4double rot_angle);

  private:

    // Logical Volume where whole Tracking Plane is placed
    G4LogicalVolume* mother_logic_;
    // Dice board
    NextNewKDB* kapton_dice_board_;
    // Dimensions
    const G4double support_plate_diam_, support_plate_thickness_, support_plate_front_buffer_diam_,
      support_plate_front_buffer_thickness_, support_plate_tread_diam_, support_plate_tread_thickness_;
    const G4double tracking_plane_z_pos_;
    //  G4double el_gap_z_edge_, z_kdb_displ_;////????????
    const G4double cable_hole_width_, cable_hole_high_;
    const G4double plug_x_,plug_y_,plug_z_;
    const G4double plug_distance_from_copper_;

    const G4int SiPM_rows_, SiPM_columns_;
    const G4int DB_columns_, num_DBs_, dice_side_, dice_gap_;
    G4ThreeVector kdb_dimensions_;
    std::vector<G4ThreeVector> DB_positions_;

    G4double dice_board_z_pos_;

    // Visibility of the tracking plane
    G4bool visibility_;

    // Vertex generators
    CylinderPointSamplerLegacy* support_body_gen_;
    CylinderPointSamplerLegacy* support_flange_gen_;
    CylinderPointSamplerLegacy* support_buffer_gen_;
    BoxPointSamplerLegacy* plug_gen_;

    G4double body_perc_;
    G4double flange_perc_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Container to store the absolute position of SiPMs in gas
    std::vector<std::pair<int, G4ThreeVector> > absSiPMpos_;

  };

} //end namespace nexus
#endif
