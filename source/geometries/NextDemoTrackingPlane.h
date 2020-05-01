// ----------------------------------------------------------------------------
///  \file   
///  \brief  
///
///  \author   <jmunoz@ific.uv.es>
///  \date     25 Apr 2012
///  \version  $Id$
///
///  Copyright (c) 2012 NEXT Collaboration
//
//  Updated to NextDemo++  by  Ruth Weiss Babai  <ruty.wb@gmail.com>
//  From:   Next100TrackingPlane.h
//  Date:       Aug 2019
// ----------------------------------------------------------------------------

#ifndef __NEXT_DEMO_TRACKING_PLANE__
#define __NEXT_DEMO_TRACKING_PLANE__


#include <G4LogicalVolume.hh>
#include <G4Navigator.hh>
#include <G4TransportationManager.hh>

#include "CylinderPointSampler.h"
#include "BoxPointSampler.h"
//#include "NextNewKDB.h"
#include "NextDemoKDB.h"


class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  /// This is a geometry formed by all the components of the tracking plane

  class NextDemoTrackingPlane: public BaseGeometry
  {

  public:
    /// Constructor
    NextDemoTrackingPlane();
    //NextDemoTrackingPlane(const G4double el_gap_z_edge);

    /// Destructor
    ~NextDemoTrackingPlane();

    // Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    void SetAnodeZCoord(G4double z);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();


  private:
    void GenerateDBPositions();
    void PrintAbsoluteSiPMPos();


  private:

    // Logical Volume where whole Tracking Plane is placed
    G4LogicalVolume* _mother_logic;

    // Dice board
    //   NextElDB* _dice_board;
    //  NextNewKDB* _dice_board;
    NextDemoKDB* _dice_board;

    // Dimensions
    // G4double _support_diam, _support_thickness;
    G4double _support_side, _support_thickness;
    G4double _el_gap_z_edge, _z_displ;
    G4double _hole_size;

    G4int _SiPM_rows, _SiPM_columns;
    G4int _DB_columns, _num_DBs;                // Ruty
    G4int _dice_side_x, _dice_side,_dice_gap;   // Ruty
    std::vector<G4ThreeVector> _DB_positions;

    // Visibility of the tracking plane
    G4bool _visibility, _verbosity, _verb_sipmPos;

    // Vertex generators
    //CylinderPointSampler* _support_gen;
    BoxPointSampler* _support_gen;
    BoxPointSampler* _dice_board_gen;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 

    // Container to store the absolute position of SiPMs in gas
    std::vector<std::pair<int, G4ThreeVector> > _absSiPMpos;
  };

} //end namespace nexus
#endif
