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
#include "NextNewKDB.h"


class G4Material;
class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  /// This is a geometry formed by all the components of the tracking plane

  class Next100TrackingPlane: public BaseGeometry
  {

  public:
    /// Constructor
    Next100TrackingPlane();

    /// Destructor
    ~Next100TrackingPlane();

    // Sets the Logical Volume where Inner Elements will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

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
    NextNewKDB* _dice_board;

    // Dimensions
    G4double _support_diam, _support_thickness;
    G4double _el_gap_z_edge, _z_displ;
    G4double _hole_size;

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

    // Container to store the absolute position of SiPMs in gas
    std::vector<std::pair<int, G4ThreeVector> > _absSiPMpos;
  };

} //end namespace nexus
#endif
