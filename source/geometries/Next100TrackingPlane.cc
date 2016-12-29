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
#include "Visibilities.h"
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
#include <stdexcept>

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
    // _dice_board = new NextElDB(_SiPM_rows, _SiPM_columns);
    _dice_board = new NextNewKDB(_SiPM_rows, _SiPM_columns);
  }



  void Next100TrackingPlane::Construct()
  {

    // Dice Boards positions
    _dice_board->Construct();
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
							        G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
							       "SUPPORT_PLATE");


    ///// Dice Boards
 
    G4LogicalVolume* dice_board_logic = _dice_board->GetLogicalVolume();
    G4ThreeVector db_dimensions = _dice_board->GetDimensions();
    G4double db_thickness = db_dimensions.z();


    ///// Support Plate placement
    G4double support_plate_posz = _el_gap_z_edge + _z_displ + db_thickness + _support_thickness/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,support_plate_posz), support_plate_logic,  "SUPPORT_PLATE", _mother_logic, false, 0);

    ///// Dice Boards placement
    G4double dice_board_posz = _el_gap_z_edge + _z_displ + db_thickness/2.;

     const std::vector<std::pair<int, G4ThreeVector> > SiPM_positions = 
      _dice_board->GetPositions();
    
    G4ThreeVector pos;
    for (int i=0; i<_num_DBs; i++) {
      pos = _DB_positions[i];
      pos.setZ(dice_board_posz);
      new G4PVPlacement(0, pos, dice_board_logic,
			"DICE_BOARD", _mother_logic, false, i+1, false);
      // Store the absolute positions of SiPMs in gas, for possible checks
      for (unsigned int si=0; si< SiPM_positions.size(); si++) {
	G4ThreeVector mypos =  SiPM_positions[si].second;
	std::pair<int, G4ThreeVector> abs_pos;
	abs_pos.first = (i+1)*1000+ SiPM_positions[si].first ;
	abs_pos.second = 
	  G4ThreeVector(pos.getX()+mypos.getX(), pos.getY()+mypos.getY(), pos.getZ()+mypos.getZ());

	_absSiPMpos.push_back(abs_pos);
      }
    }

    // PrintAbsoluteSiPMPos();

    ////////////////////////////////////////////////
    ////// It remains the electronics to add  //////


    // SETTING VISIBILITIES   //////////

    // Dice boards always visible in dark green
    G4VisAttributes dark_green_col = nexus::DarkGreen();
    dice_board_logic->SetVisAttributes(dark_green_col);
    

    if (_visibility) {
      G4VisAttributes light_brown_col = nexus::CopperBrown();
      support_plate_logic->SetVisAttributes(light_brown_col);
      light_brown_col.SetForceSolid(true);
      support_plate_logic->SetVisAttributes(light_brown_col);
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
	// To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false);
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
    else {
      G4Exception("[Next100TrackingPlane]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");     
    }

    return vertex;

  }



  void Next100TrackingPlane::GenerateDBPositions()
  {
    /// Function that computes and stores the XY positions of Dice Boards

    G4int num_rows[] = {6, 9, 10, 11, 12, 11, 12, 11, 10, 9, 6};
   
    G4int total_positions = 0;

    // Separation between consecutive columns / rows
    // G4double x_step = (45. + 35.191) * mm;
    // G4double y_step = (45. + 36.718) * mm;
 
    G4double x_step = _dice_board->GetDimensions().getX() + 1.*mm;
    G4double y_step = _dice_board->GetDimensions().getY() + 1.*mm;

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

   void Next100TrackingPlane::PrintAbsoluteSiPMPos()
  {
    G4cout << "----- Absolute position of SiPMs in gas volume -----" << G4endl;
    G4cout <<  G4endl;
    for (unsigned int i=0; i<_absSiPMpos.size(); i++) {
      std::pair<int, G4ThreeVector> abs_pos = _absSiPMpos[i];
      G4cout << "ID number: " << _absSiPMpos[i].first << ", position: " 
      	     << _absSiPMpos[i].second.getX() << ", "
      	     << _absSiPMpos[i].second.getY() << ", "
      	     << _absSiPMpos[i].second.getZ()  << G4endl;
    }
    G4cout <<  G4endl;
    G4cout << "---------------------------------------------------" << G4endl;
  }

}


