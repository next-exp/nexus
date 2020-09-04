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
    G4LogicalVolume* mother_logic_;

    // Dice board
    //   NextElDB* _dice_board;
    //  NextNewKDB* _dice_board;
    NextDemoKDB* dice_board_;

    // Dimensions
    // G4double _support_diam, _support_thickness;
    G4double support_side_, support_thickness_;
    G4double el_gap_z_edge_, z_displ_;
    G4double hole_size_;

    G4int SiPM_rows_, SiPM_columns_;
    G4int DB_columns_, num_DBs_;                // Ruty
    G4int dice_side_x_, dice_side_,dice_gap_;   // Ruty
    std::vector<G4ThreeVector> DB_positions_;

    // Visibility of the tracking plane
    G4bool visibility_, verbosity_, verb_sipmPos_;

    // Vertex generators
    //CylinderPointSampler* _support_gen;
    BoxPointSampler* support_gen_;
    BoxPointSampler* dice_board_gen_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

    // Container to store the absolute position of SiPMs in gas
    std::vector<std::pair<int, G4ThreeVector> > absSiPMpos_;
  };

} //end namespace nexus
#endif
