// ----------------------------------------------------------------------------
///  \file   NextNewTrackingPlane.h
///  \brief  
///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     17 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_TRACKING_PLANE__
#define __NEXTNEW_TRACKING_PLANE__

#include <G4LogicalVolume.hh>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

#include "CylinderPointSampler.h"
#include "NextNewKDB.h"

class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;

namespace nexus {

  /// This is a geometry formed by all the components of the tracking plane
  class NextNewTrackingPlane
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

  private:

    // Logical Volume where whole Tracking Plane is placed
    G4LogicalVolume* _mother_logic;
    // Dice board
    NextNewKDB* _kapton_dice_board;
    // Dimensions
    const G4double _support_plate_diam, _support_plate_thickness, _support_plate_front_buffer_diam,
      _support_plate_front_buffer_thickness, _support_plate_tread_diam, _support_plate_tread_thickness;
    const G4double _tracking_plane_z_pos;
    G4double _el_gap_z_edge, _z_kdb_displ;////????????
    const G4double _cable_hole_width, _cable_hole_high;

    const G4int _SiPM_rows, _SiPM_columns;
    const G4int _DB_columns, _num_DBs, _dice_side, _dice_gap;
    G4ThreeVector _kdb_dimensions;
    std::vector<G4ThreeVector> _DB_positions;

    G4double _dice_board_z_pos;

    // Visibility of the tracking plane
    G4bool _visibility;

    // Vertex generators
    CylinderPointSampler* _support_body_gen;
    CylinderPointSampler* _support_flange_gen;
    CylinderPointSampler* _support_buffer_gen;
    
    G4double _body_perc;
    G4double _flange_perc;
    
    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} //end namespace nexus
#endif


