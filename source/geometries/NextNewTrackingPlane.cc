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
#include <G4RotationMatrix.hh>

namespace nexus {

  NextNewTrackingPlane::NextNewTrackingPlane(): 
    // Dimensions
    _support_plate_diam (63.0 * cm), //must be checked
    _support_plate_thickness (12.0 * cm),
    _support_plate_front_buffer_diam (48.2 * cm),
    _support_plate_front_buffer_thickness (5 * mm),
    _support_plate_tread_diam (54. *cm), //sup_diam-2* ics/2 from drawings ???
    _support_plate_tread_thickness (6. * cm), //sup_thik/2

    _z_displ (0.5 * cm), //distance between DB and suport plate
    //??????????????????????????
    _tracking_plane_z_pos (28 *cm),//_el_gap_z_pos (25.5 *cm) From drawings + _el_gap(1.4)/2 + _el_grid_thickness + _el_to_DB  
    //???????????????????????????????
    // This must be consistent withh Next100InnerElements
    _cable_hole_width (45 * mm),//??????
    _cable_hole_high (5 * mm),//??????

    // SiPMs per Dice Board
    _SiPM_rows (8),
    _SiPM_columns (8),

    // Number of Dice Boards, DB columns
    _num_DBs (28),
    _DB_columns (6),
    _dice_gap (2 *mm)// distance between dices
  {
    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("tracking_plane_vis", _visibility, "Tracking Plane Visibility");

    // The Dice Board
    _kapton_dice_board = new NextNewKDB(_SiPM_rows, _SiPM_columns);
    _kdb_dimensions = _kapton_dice_board->GetDimensions();
   
  }

  void NextNewTrackingPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    std::cout<< "--------0--------"<<std::endl;
    _mother_logic = mother_logic;
  }
  
  void NextNewTrackingPlane::Construct()
  {
    GenerateDBPositions();
    std::cout<< "--------1--------"<<std::endl;
          
    ///// SUPPORT PLATE /////
    G4Tubs* support_plate_nh_solid = 
      new G4Tubs("SUPPORT_PLATE_NH", 0., _support_plate_diam/2., _support_plate_thickness/2., 0., twopi);
   
    std::cout<< "--------2--------"<<std::endl;
    //Making front buffer
    G4Tubs* support_plate_front_buffer_solid =
      new G4Tubs("SUPPORT_PLATE_FBUF_SOLID",0., _support_plate_front_buffer_diam/2.,(_support_plate_front_buffer_thickness+1.*mm)/2.,0.,twopi);
    G4SubtractionSolid* support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE", support_plate_nh_solid,support_plate_front_buffer_solid,0,G4ThreeVector(0.,0.,-_support_plate_thickness/2. + _support_plate_front_buffer_thickness/2.-.5*mm));
   
    //Making tread
    G4Tubs* support_plate_tread_solid = 
      new G4Tubs("SUPPORT_PLATE_TREAD",_support_plate_tread_diam/2.,_support_plate_diam/2.+1.*mm,(_support_plate_tread_thickness+1.*mm)/2.,0.,twopi);
    
    support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE",support_plate_solid,support_plate_tread_solid,0,G4ThreeVector(0.,0.,-_support_plate_thickness/2. + _support_plate_tread_thickness/2.-.5*mm));

    // Making DB cables holes through  
    G4Box* support_plate_cable_hole_solid = new G4Box("SUPPORT_PLATE_HOLE", _cable_hole_width/2., _cable_hole_high/2., _support_plate_thickness/2. + 1.*mm);
    
    //Rotate the cable holes to vertical to aboid crash while try to subtract the holes in the corner
    G4ThreeVector pos;
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateZ(pi/2.);
    for (int i=0; i<_DB_positions.size(); i++) {
      pos = _DB_positions[i];
      //pos.setZ(_support_plate_thickness/2.);
      std::cout<< pos <<std::endl;
      support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE", support_plate_solid,
     						   support_plate_cable_hole_solid, rot, pos);
    }
    delete rot;

    std::cout<< "--------3--------"<<std::endl;
    G4LogicalVolume* support_plate_logic = new G4LogicalVolume(support_plate_solid,
							       G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
							       "SUPPORT_PLATE");
    std::cout<< "--------4--------"<<std::endl;
    ///// Support Plate placement
    G4double support_plate_z_pos =  _tracking_plane_z_pos;// + db_thickness + _support_thickness/2.;
    G4PVPlacement* support_plate_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,support_plate_z_pos), support_plate_logic,
    "SUPPORT_PLATE", _mother_logic, false, 0);
    std::cout<< "--------5--------"<<std::endl;
    
    ///// Dice Boards
    _kapton_dice_board->Construct();
    G4LogicalVolume* dice_board_logic = _kapton_dice_board->GetLogicalVolume();
    G4ThreeVector db_dimensions = _kapton_dice_board->GetDimensions();
    G4double db_thickness = db_dimensions.z();
    std::cout<< "--------6--------"<<std::endl;
    
    ///// Dice Boards placement
    G4double dice_board_z_pos = _tracking_plane_z_pos -db_thickness/2.;
    G4PVPlacement* dice_board_physi;
    G4ThreeVector post;
    for (int i=0; i<_num_DBs; i++) {
      post = _DB_positions[i];
      post.setZ(dice_board_z_pos);
      dice_board_physi = new G4PVPlacement(0, post, dice_board_logic,
      "DICE_BOARD", _mother_logic, false, i);
    }
    std::cout<< "--------7--------"<<std::endl;
    //// SETTING VISIBILITIES   //////////
    // Dice boards always visible in dark green
    G4VisAttributes dark_green_col(G4Colour(0., .6, 0.));
    dice_board_logic->SetVisAttributes(dark_green_col);
    

    // if (_visibility) {
    G4VisAttributes Copper_col(G4Colour(.72, .45, .20));
    //Copper_col.SetForceSolid(true);
    support_plate_logic->SetVisAttributes(Copper_col);
    // }
    // else {
    //   support_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
    // }

    // VERTEX GENERATORS   //////////
    _support_gen  = new CylinderPointSampler(_support_plate_diam/2., _support_plate_thickness, 0.,
					     0., G4ThreeVector(0., 0., _tracking_plane_z_pos));//-------!!!!!!

  std::cout<< "--------8--------"<<std::endl;   
  }

  NextNewTrackingPlane::~NextNewTrackingPlane()
  {
    std::cout<< "--------d--------"<<std::endl;
    delete _support_gen;
    std::cout<< "--------del--------"<<std::endl;
  }

  G4ThreeVector NextNewTrackingPlane::GenerateVertex(const G4String& region) const
  {
    std::cout<< "--------9--------"<<std::endl;
    G4ThreeVector vertex(0., 0., 0.);
    /*  // Support Plate
    if (region == "TRK_SUPPORT") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = _support_gen->GenerateVertex(INSIDE);
	VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(vertex, 0, false);
      } while (VertexVolume->GetName() != "SUPPORT_PLATE");
    }
    // Dice Boards
    else if (region == "DICE_BOARD") {
      vertex = _kapton_dice_board->GenerateVertex(region);
      G4double rand = _num_DBs * G4UniformRand();
      G4ThreeVector db_pos = _DB_positions[int(rand)];
      vertex += db_pos;
      }*/
    return vertex;
  }
  
  void NextNewTrackingPlane::GenerateDBPositions()
  {
    std::cout<< "--------10--------"<<std::endl;
    /// Function that computes and stores the XY positions of Dice Boards
    G4int num_rows[] = {3, 5, 6, 6, 5, 3};
    G4int total_positions = 0;
    // Separation between consecutive columns / rows _dice_gap
    G4double x_step = 79*mm+2*mm;//_dice_gap;//(45. + 35.191) 
    G4double y_step =79*mm+2*mm; //_dice_gap;//(45. + 36.718) * mm ???????????
   // G4double x_dim =_kdb_dimension.x();
   // G4double y_dim =_kdb_dimension.y();
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
	std::cout<<pos_x<<std::endl;
	position.setX(pos_x);
	std::cout<<pos_y<<std::endl;
	position.setY(pos_y);
	_DB_positions.push_back(position);
	total_positions++;
      }
    }
    // // Checking
    // if (total_positions != _num_DBs) {
    //   G4cout << "\n\nERROR: Number of DBs doesn't match with number of positions calculated\n";
    //   exit(0);
    // }
  }
}//end namespace nexus
