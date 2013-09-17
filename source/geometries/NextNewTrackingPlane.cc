// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 17 Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewTrackingPlane.h"
#include "MaterialsList.h"
#include <G4GenericMessenger.hh>
#include "OpticalMaterialProperties.h"

#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>
#include <Randomize.hh>

namespace nexus {

  NextNewTrackingPlane::NextNewTrackingPlane(): 
    // Dimensions
    _support_diam (63.0 * cm), //must be checked
    _support_thickness (12.0 * cm),
    _z_displ (0.5 * cm), //distance between DB and suport plate
    //??????????????????????????
    _tracking_plane_z_pos (28 *cm),//_el_gap_z_pos (25.5 *cm) From drawings + _el_gap(1.4)/2 + _el_grid_thickness + _el_to_DB  
    //???????????????????????????????
    // This must be consistent withh Next100InnerElements
    _cable_hole_width (45 * mm),//??????
    _cable_hole_high (15 * mm),//??????

    // SiPMs per Dice Board
    _SiPM_rows (8),
    _SiPM_columns (8),

    // Number of Dice Boards, DB columns
    _num_DBs (28),
    _DB_columns (6)
  {
    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("tracking_plane_vis", _visibility, "Tracking Plane Visibility");

    // The Dice Board
    _dice_board = new NextElDB(_SiPM_rows, _SiPM_columns);
  }

  void NextNewTrackingPlane::Construct()
  {
    // Dice Boards positions
    GenerateDBPositions();

    ///// Support plate
    G4Tubs* support_plate_nh_solid = new G4Tubs("SUPPORT_PLATE_NH", 0., _support_diam/2., _support_thickness/2., 0., twopi);
    // Making DB cables holes through  
    G4Box* support_plate_cable_hole_solid = new G4Box("SUPPORT_PLATE_HOLE", _cable_hole_width/2., _cable_hole_high/2., _support_thickness/2. + 2.*mm);
    G4SubtractionSolid* support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE", support_plate_nh_solid,
								     support_plate_cable_hole_solid, 0, _DB_positions[0]);
    for (int i=1; i<_num_DBs; i++) {
      support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE", support_plate_solid,
						   support_plate_cable_hole_solid, 0, _DB_positions[i]);
    }
    G4LogicalVolume* support_plate_logic = new G4LogicalVolume(support_plate_solid,
							       G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
							       "SUPPORT_PLATE");
    
    ///// Dice Boards
    _dice_board->Construct();
    G4LogicalVolume* dice_board_logic = _dice_board->GetLogicalVolume();
    G4ThreeVector db_dimensions = _dice_board->GetDimensions();
    G4double db_thickness = db_dimensions.z();

    ///// Support Plate placement
    G4double support_plate_z_pos =  _tracking_plane_z_pos + db_thickness + _support_thickness/2.;
    G4PVPlacement* support_plate_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,-support_plate_z_pos), support_plate_logic,
							   "SUPPORT_PLATE", _mother_logic, false, 0);

    ///// Dice Boards placement
    G4double dice_board_z_pos = _tracking_plane_z_pos + db_thickness/2.;
    G4PVPlacement* dice_board_physi;
    G4ThreeVector pos;
    for (int i=0; i<_num_DBs; i++) {
      pos = _DB_positions[i];
      pos.setZ(-dice_board_z_pos);
      dice_board_physi = new G4PVPlacement(0, pos, dice_board_logic,
					   "DICE_BOARD", _mother_logic, false, i);
    }

    // //// SETTING VISIBILITIES   //////////

    // // Dice boards always visible in dark green
    // G4VisAttributes dark_green_col(G4Colour(0., .6, 0.));
    // dice_board_logic->SetVisAttributes(dark_green_col);
    

    // if (_visibility) {
    //   G4VisAttributes grey_col(G4Colour(.89, .89, .89));
    //   grey_col.SetForceSolid(true);
    //   support_plate_logic->SetVisAttributes(grey_col);
    // }
    // else {
    //   support_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
    // }

    // VERTEX GENERATORS   //////////
    _support_gen  = new CylinderPointSampler(_support_diam/2., _support_thickness, 0.,
					     0., G4ThreeVector(0., 0., -support_plate_z_pos));
    // Vertexes are generated in a thin surface in the backside of the board
    G4double z_dim = db_dimensions.z();
    _dice_board_gen = new BoxPointSampler(db_dimensions.x(), db_dimensions.y(), z_dim * .1,
					  0., G4ThreeVector(0., 0., -dice_board_z_pos - z_dim*.4));
  }

  void NextNewTrackingPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  NextNewTrackingPlane::~NextNewTrackingPlane()
  {
    delete _support_gen;
    delete _dice_board_gen;
  }

G4ThreeVector NextNewTrackingPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    // Support Plate
    if (region == "TRK_SUPPORT") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _support_gen->GenerateVertex(INSIDE);
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      } while (VertexVolume->GetName() != "SUPPORT_PLATE");
    }
    // Dice Boards
    else if (region == "DICE_BOARD") {
      vertex = _dice_board_gen->GenerateVertex("INSIDE");
      G4double rand = _num_DBs * G4UniformRand();
      G4ThreeVector db_pos = _DB_positions[int(rand)];
      vertex += db_pos;
    }

    return vertex;

  }


void NextNewTrackingPlane::GenerateDBPositions()
  {
    /// Function that computes and stores the XY positions of Dice Boards
    G4int num_rows[] = {3, 5, 6, 6, 5, 3};
    G4int total_positions = 0;
    // Separation between consecutive columns / rows
    G4double x_step =2*mm ;//(45. + 35.191) * mm ???????????
    G4double y_step =2*mm ;//(45. + 36.718) * mm ???????????
    G4double x_dim = x_step * (_DB_columns -1);
    G4ThreeVector position(0.,0.,0.);
    // For every column
    for (G4int col=0; col<_DB_columns; col++) {
      G4double pos_x = x_dim/2. - col * x_step;
      G4int rows = num_rows[col];
      G4double y_dim = y_step * (rows-1);
      // For every row
      for (G4int row=0; row<rows; row++) {
	G4double pos_y = y_dim/2. - row * y_step;
	position.setX(pos_x);
	position.setY(pos_y);
	_DB_positions.push_back(position);
	total_positions++;
      }
    }
    // Checking
    if (total_positions != _num_DBs) {
      G4cout << "\n\nERROR: Number of DBs doesn't match with number of positions calculated\n";
      exit(0);
    }
  }
}//end namespace nexus
