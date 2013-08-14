// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <jmunoz@ific.uv.es>
///  \date     25 Apr 2012
///  \version  $Id$
///
///  Copyright (c) 2012 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXT100_TRACKING_PLANE__
#define __NEXT100_TRACKING_PLANE__


#include <G4LogicalVolume.hh>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

#include "CylinderPointSampler.h"
#include "BoxPointSampler.h"
#include "NextElDB.h"


class G4Material;
class G4LogicalVolume;


namespace nexus {

  /// This is a geometry formed by all the components of the tracking plane

  class Next100TrackingPlane
  {

  public:
    /// Constructor
    Next100TrackingPlane(G4LogicalVolume* mother_logic);

    /// Destructor
    ~Next100TrackingPlane();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;


  private:
    void ReadParameters();
    void GenerateDBPositions();


  private:

    // Dice board
    NextElDB* _dice_board;

    // Dimensions
    G4double _support_diam, _support_thickness;
    G4double _el_gap_z_edge, _z_displ;
    G4double _hole_size;

    G4int _SiPM_rows, _SiPM_columns;
    G4int _DB_columns, _num_DBs;
    std::vector<G4ThreeVector> _DB_positions;

   
    // Visibility of the tracking plane
    G4int _visibility;


    // Vertex generators
    CylinderPointSampler* _support_gen;
    BoxPointSampler* _dice_board_gen;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

  };

} //end namespace nexus
#endif
