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
#include "BoxPointSampler.h"
#include "NextElDB.h"

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
    NextElDB* _dice_board;
    // Dimensions
    G4double _support_diam, _support_thickness;
    G4double _tracking_plane_z_pos;
    G4double _el_gap_z_edge, _z_displ;////????????
    G4double _cable_hole_width, _cable_hole_high;

    G4int _SiPM_rows, _SiPM_columns;
    G4int _DB_columns, _num_DBs;
    std::vector<G4ThreeVector> _DB_positions;

    // Visibility of the tracking plane
    G4bool _visibility;

    // Vertex generators
    CylinderPointSampler* _support_gen;
    BoxPointSampler* _dice_board_gen;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

  };

} //end namespace nexus
#endif


