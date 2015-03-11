// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <jmunoz@ific.uv.es>
//  Created: 25 Apr 2012
//  
//  Copyright (c) 2012 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next100TrackingPlane.h"
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

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

namespace nexus {

  using namespace CLHEP;

  Next100TrackingPlane::Next100TrackingPlane(): 
    // Dimensions
    _support_diam (106.0 * cm),
    _support_thickness (2.0 * cm),
    _el_gap_z_edge (66.895 * cm),   // This must be consistent withh Next100InnerElements:
    // el_gap_posz + el_gap_thickness/2. + grid_thickness/2.
    _z_displ (0.5 * cm),
    _hole_size (45 * mm),

    // SiPMTs per Dice Board
    _SiPM_rows (8),
    _SiPM_columns (8),

    // Number of Dice Boards, DB columns
    _DB_columns (11),
    _num_DBs (107),
    _visibility(0)
  {

    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/", "Control commands of geometry Next100.");
    _msg->DeclareProperty("tracking_plane_vis", _visibility, "Tracking Plane Visibility");

    // The Dice Board
    _dice_board = new NextElDB(_SiPM_rows, _SiPM_columns);
  }



  void Next100TrackingPlane::Construct()
  {

    // Dice Boards positions
    GenerateDBPositions();

    ///// Support plate
    G4Tubs* support_plate_nh_solid = new G4Tubs("SUPPORT_PLATE_NH", 0., _support_diam/2., _support_thickness/2., 0., twopi);

    // Making DB holes
    G4Box* support_plate_db_hole_solid = new G4Box("SUPPORT_PLATE_DB_HOLE", _hole_size/2., _hole_size/2., _support_thickness/2. + 2.*mm);
    G4SubtractionSolid* support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE", support_plate_nh_solid,
								     support_plate_db_hole_solid, 0, _DB_positions[0]);
    for (int i=1; i<_num_DBs; i++) {
      support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE", support_plate_solid,
						   support_plate_db_hole_solid, 0, _DB_positions[i]);
    }

    G4LogicalVolume* support_plate_logic = new G4LogicalVolume(support_plate_solid,
							       MaterialsList::HDPE(),
							       "SUPPORT_PLATE");


    ///// Dice Boards
    _dice_board->Construct();
    G4LogicalVolume* dice_board_logic = _dice_board->GetLogicalVolume();
    G4ThreeVector db_dimensions = _dice_board->GetDimensions();
    G4double db_thickness = db_dimensions.z();


    ///// Support Plate placement
    G4double support_plate_posz = _el_gap_z_edge + _z_displ + db_thickness + _support_thickness/2.;
    G4PVPlacement* support_plate_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,support_plate_posz), support_plate_logic,
							   "SUPPORT_PLATE", _mother_logic, false, 0);

    ///// Dice Boards placement
    G4double dice_board_posz = _el_gap_z_edge + _z_displ + db_thickness/2.;
    G4PVPlacement* dice_board_physi;
    G4ThreeVector pos;
    for (int i=0; i<_num_DBs; i++) {
      pos = _DB_positions[i];
      pos.setZ(dice_board_posz);
      dice_board_physi = new G4PVPlacement(0, pos, dice_board_logic,
					   "DICE_BOARD", _mother_logic, false, i);
    }

    // G4PVPlacement* dice_board_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), dice_board_logic,
    //  						      "DICE_BOARD", _mother_logic, false, 0);


    ////////////////////////////////////////////////
    ////// It remains the electronics to add  //////


    // SETTING VISIBILITIES   //////////

    // Dice boards always visible in dark green
    G4VisAttributes dark_green_col(G4Colour(0., .6, 0.));
    dice_board_logic->SetVisAttributes(dark_green_col);
    

    if (_visibility) {
      G4VisAttributes grey_col(G4Colour(.89, .89, .89));
      grey_col.SetForceSolid(true);
      support_plate_logic->SetVisAttributes(grey_col);
    }
    else {
      support_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }



    // VERTEX GENERATORS   //////////
    _support_gen  = new CylinderPointSampler(_support_diam/2., _support_thickness, 0.,
					     0., G4ThreeVector(0., 0., support_plate_posz));

    // Vertexes are generated in a thin surface in the backside of the board
    G4double z_dim = db_dimensions.z();
    _dice_board_gen = new BoxPointSampler(db_dimensions.x(), db_dimensions.y(), z_dim * .1,
					  0., G4ThreeVector(0., 0., dice_board_posz + z_dim*.4));
  }



  void Next100TrackingPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }



  Next100TrackingPlane::~Next100TrackingPlane()
  {
    delete _support_gen;
    delete _dice_board_gen;
  }



  G4ThreeVector Next100TrackingPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Support Plate
    if (region == "TRK_SUPPORT") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _support_gen->GenerateVertex("INSIDE");
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
	//std::cout << VertexVolume->GetName() << std::endl;
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



  void Next100TrackingPlane::GenerateDBPositions()
  {
    /// Function that computes and stores the XY positions of Dice Boards

    G4int num_rows[] = {6, 9, 10, 11, 12, 11, 12, 11, 10, 9, 6};
    G4int total_positions = 0;

    // Separation between consecutive columns / rows
    G4double x_step = (45. + 35.191) * mm;
    G4double y_step = (45. + 36.718) * mm;

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
	//G4cout << G4endl << position;
      }
    }

    // Checking
    if (total_positions != _num_DBs) {
      G4cout << "\n\nERROR: Number of DBs doesn't match with number of positions calculated\n";
      exit(0);
    }


  }

}


