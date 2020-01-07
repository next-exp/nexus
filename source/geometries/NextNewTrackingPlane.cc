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
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

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
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  NextNewTrackingPlane::NextNewTrackingPlane(): 
    // Dimensions
    _support_plate_diam (63.0 * cm), //must be checked
    _support_plate_thickness (12.0 * cm),
    _support_plate_front_buffer_diam (53.8 * cm),
    _support_plate_front_buffer_thickness (1. * cm),
    _support_plate_tread_diam (54.56 *cm), //sup_diam-2* 42.2mm from drawings
    _support_plate_tread_thickness (6. * cm), //sup_thik/2
    _tracking_plane_z_pos (284.1 *mm),
    //    _tracking_plane_z_pos (28.905 *cm),//_el_gap_z_pos (25.5 *cm) From drawings + _el_gap(1.4)/2 + _el_grid_thickness + _el_to_
    //    _dice_board_z_pos (282.25*mm), // its surface must be 2 mm away from the end of the anode plate --> pos_z_anode + anode_thickness/2. + 2.*mm = 284.1 *mm + half of DB thick
    //   _z_kdb_displ (0.0 * cm), //distance between DB and support plate( kdb_surface at the same level as support surface)
    _cable_hole_width (45 * mm),
    _cable_hole_high (8 * mm),
    _plug_x (40. *mm),
    _plug_y (4. *mm), //two union connectors
    _plug_z (6. *mm),
    _plug_distance_from_copper(21. * cm),

    // SiPMs per Dice Board
    _SiPM_rows (8),
    _SiPM_columns (8),

    // Number of Dice Boards, DB columns   
    _DB_columns (6),
    _num_DBs (28),
    _dice_side (79.*mm),
    _dice_gap (1. *mm),// distance between dices
   
    _visibility (1)
  {
    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("tracking_plane_vis", _visibility, "Tracking Plane Visibility");

    // The Dice Board
    _kapton_dice_board = new NextNewKDB(_SiPM_rows, _SiPM_columns);
      
  }

  void NextNewTrackingPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }
  
  void NextNewTrackingPlane::Construct()
  {
    GenerateDBPositions();
          
    ///// SUPPORT PLATE /////
    G4Tubs* support_plate_nh_solid = 
      new G4Tubs("SUPPORT_PLATE_NH", 0., _support_plate_diam/2., _support_plate_thickness/2., 0., twopi);
    //Making front buffer
    G4Tubs* support_plate_front_buffer_solid =
      new G4Tubs("SUPPORT_PLATE_FBUF_SOLID",0., _support_plate_front_buffer_diam/2.,
		 (_support_plate_front_buffer_thickness+1.*mm)/2.,0.,twopi);
    G4SubtractionSolid* support_plate_solid =
      new G4SubtractionSolid("SUPPORT_PLATE", support_plate_nh_solid,support_plate_front_buffer_solid,0,
			     G4ThreeVector(0.,0.,-_support_plate_thickness/2. + _support_plate_front_buffer_thickness/2.-.5*mm));
    //Making tread
    G4Tubs* support_plate_tread_solid = 
      new G4Tubs("SUPPORT_PLATE_TREAD",_support_plate_tread_diam/2.,_support_plate_diam/2.+1.*mm,
		 (_support_plate_tread_thickness+1.*mm)/2.,0.,twopi);
    support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE",support_plate_solid,support_plate_tread_solid,0,
						 G4ThreeVector(0.,0.,-_support_plate_thickness/2. + _support_plate_tread_thickness/2.-.5*mm));
    // Making DB cables holes through  
    G4Box* support_plate_cable_hole_solid = 
      new G4Box("SUPPORT_PLATE_HOLE", _cable_hole_width/2., _cable_hole_high/2., _support_plate_thickness/2. + 1.*mm);
    G4ThreeVector pos;
    for (unsigned int i=0; i<_DB_positions.size(); i++) {
      pos = _DB_positions[i];
      support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE", support_plate_solid,
     						   support_plate_cable_hole_solid, 0, pos);
    }
 
    G4LogicalVolume* support_plate_logic = new G4LogicalVolume(support_plate_solid,
							       G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
							       "SUPPORT_PLATE"); 
    ///// Support Plate placement
    G4double support_plate_z_pos =  _tracking_plane_z_pos + _support_plate_thickness/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,support_plate_z_pos), support_plate_logic,
		      "SUPPORT_PLATE", _mother_logic, false, 0, false);  
    /////  DICE BOARDS  ///// 
    _kapton_dice_board->Construct();
    _kdb_dimensions = _kapton_dice_board->GetDimensions();
    G4LogicalVolume* dice_board_logic = _kapton_dice_board->GetLogicalVolume();
    G4double db_thickness =_kdb_dimensions.z();
    ////Dice Boards placement
    //_dice_board_z_pos = support_plate_z_pos -_support_plate_thickness/2. -_z_kdb_displ +db_thickness/2.;
    _dice_board_z_pos = _tracking_plane_z_pos + db_thickness/2. + 3. * mm;
    
    G4ThreeVector post;
    for (int i=0; i<_num_DBs; i++) {
      post = _DB_positions[i];
      post.setZ(_dice_board_z_pos);
      new G4PVPlacement(0, post, dice_board_logic,
			"DICE_BOARD", _mother_logic, false, i+1, false);
    }

    // PrintAbsoluteSiPMPos(G4ThreeVector(0., 0., 275.), pi);
    
    //PIGGY TAIL PLUG/////////////////////////////////////////////////////
    G4Box* plug_solid = new G4Box("DB_CONNECTOR", _plug_x/2., _plug_y/2., _plug_z/2.);
    G4LogicalVolume* plug_logic = new G4LogicalVolume(plug_solid,  MaterialsList::PEEK(), "DB_PLUG");
    //G4PVPlacement * plug_physi;  
    G4ThreeVector positn;
    for (int i=0; i<_num_DBs; i++) {
      positn = _DB_positions[i];
      positn.setY(positn.y()- 10.*mm);
      positn.setZ(_dice_board_z_pos + _support_plate_front_buffer_thickness
                  + _support_plate_thickness + _plug_distance_from_copper);
      new G4PVPlacement(0, positn, plug_logic,"DB_PLUG",
			_mother_logic, false, i+1, false);
    }
    
   
   //// SETTING VISIBILITIES   //////////    
    if (_visibility) {
      G4VisAttributes light_brown_col = nexus::CopperBrown();
      light_brown_col.SetForceSolid(true);
      support_plate_logic->SetVisAttributes(light_brown_col);
      G4VisAttributes dirty_white_col =nexus::DirtyWhite();
      dirty_white_col.SetForceSolid(true);
      plug_logic->SetVisAttributes(dirty_white_col);    
    }
    else {
      support_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
      plug_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // VERTEX GENERATORS   //////////
    _support_body_gen  =
      new CylinderPointSampler(0., _support_plate_thickness-_support_plate_front_buffer_thickness,
			                         _support_plate_tread_diam/2., 0.,
                               G4ThreeVector(0., 0., support_plate_z_pos));
    _support_flange_gen  =
      new CylinderPointSampler(_support_plate_tread_diam/2., _support_plate_thickness/2.,
			                         (_support_plate_diam - _support_plate_tread_diam)/2., 0., 
                               G4ThreeVector(0., 0., support_plate_z_pos+_support_plate_thickness/4.));
    _support_buffer_gen  =
      new CylinderPointSampler(_support_plate_front_buffer_diam/2.,
                               _support_plate_front_buffer_thickness/2.,
                               (_support_plate_tread_diam-_support_plate_front_buffer_diam)/2.,
                               0., G4ThreeVector(0., 0., support_plate_z_pos -_support_plate_thickness/2.
                               +_support_plate_front_buffer_thickness/2.));
    _plug_gen =
      new BoxPointSampler(_plug_x, _plug_y, _plug_z,0., G4ThreeVector(0.,0.,0.),0);
			  // G4ThreeVector(0.,0.,_dice_board_z_pos + _support_plate_front_buffer_thickness + _support_plate_thickness),0);

     // Getting the support  volume over total
    G4double body_vol = 
      (_support_plate_thickness-_support_plate_front_buffer_thickness)*pi*(_support_plate_tread_diam/2.)*(_support_plate_tread_diam/2.);
    G4double flange_vol = 
      (_support_plate_thickness/2.)*pi*((_support_plate_diam/2.)*(_support_plate_diam/2.)-( _support_plate_tread_diam/2.)*( _support_plate_tread_diam/2.));
    G4double buffer_vol = 
      (_support_plate_front_buffer_thickness/2.)*pi*((_support_plate_tread_diam/2.)*(_support_plate_tread_diam/2.)-(_support_plate_front_buffer_diam/2.)*(_support_plate_front_buffer_diam/2.));
     G4double total_vol = body_vol + flange_vol + buffer_vol;
     _body_perc = body_vol / total_vol;
     _flange_perc =  (flange_vol + body_vol) / total_vol;
     // std::cout<<"SUPPORT PLATE (TP) VOLUME: \t"<<total_vol<<std::endl;
  }

  NextNewTrackingPlane::~NextNewTrackingPlane()
  {
    delete _support_body_gen;
    delete _support_flange_gen;
    delete _support_buffer_gen;
    delete _plug_gen;
  }

  G4ThreeVector NextNewTrackingPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Support Plate
    if (region == "SUPPORT_PLATE") {
      G4double rand1 = G4UniformRand();
      //Generating in the body
      if (rand1 < _body_perc) {
        // As it is full of holes, let's get sure vertexes are in the right volume
        G4VPhysicalVolume *VertexVolume;
        do {
          vertex = _support_body_gen->GenerateVertex("BODY_VOL");
          // To check its volume, one needs to rotate and shift the vertex
          // because the check is done using global coordinates
          G4ThreeVector glob_vtx(vertex);
          // First rotate, then shift
          glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
          glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
          VertexVolume = _geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false);
        } while (VertexVolume->GetName() != "SUPPORT_PLATE");
      }
      // Generating in the flange
      else if (rand1 < _flange_perc){
       	vertex = _support_flange_gen->GenerateVertex("BODY_VOL");
      }
      // Generating in the buffer
      else {
       	vertex = _support_buffer_gen->GenerateVertex("BODY_VOL");
      }
    } 

    // Dice Boards
    else if (region == "DICE_BOARD") {
      G4ThreeVector ini_vertex = _kapton_dice_board->GenerateVertex(region);
      G4double rand = _num_DBs * G4UniformRand();
      G4ThreeVector db_pos = _DB_positions[int(rand)];
      vertex = ini_vertex + db_pos;
      vertex.setZ(vertex.z() +_dice_board_z_pos);     
    }

    // PIGGY TAIL PLUG
    else if (region == "DB_PLUG") {
      G4ThreeVector ini_vertex = _plug_gen->GenerateVertex("INSIDE");
      G4double rand = _num_DBs * G4UniformRand();
      G4ThreeVector db_pos = _DB_positions[int(rand)];
      vertex = ini_vertex + db_pos;
      vertex.setY(vertex.y() - 10.*mm);
      vertex.setZ(vertex.z() +_dice_board_z_pos + _support_plate_front_buffer_thickness
                  + _support_plate_thickness + _plug_distance_from_copper);
    }

    else {
      G4Exception("[NextNewTrackingPlane]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");     
    }
    
    return vertex;
  }


  
  void NextNewTrackingPlane::GenerateDBPositions()
  {
    //  std::cout<< "Generating DB positions"<<std::endl;
    /// Function that computes and stores the XY positions of Dice Boards
    G4int num_rows[] = {3, 5, 6, 6, 5, 3};
    G4int total_positions = 0;
    // Separation between consecutive columns / rows _dice_gap
    G4double x_step = _dice_side +_dice_gap;
    G4double y_step = _dice_side +_dice_gap;
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

  void NextNewTrackingPlane::PrintAbsoluteSiPMPos(G4ThreeVector displ, G4double rot_angle)
  {
    // Print the absolute positions of SiPMs in gas, for possible checks
    const std::vector<std::pair<int, G4ThreeVector> > SiPM_positions = 
      _kapton_dice_board->GetPositions();
    G4ThreeVector post;
    for (int i=0; i<_num_DBs; i++) {
      post = _DB_positions[i];
      post.setZ(_dice_board_z_pos);
      for (unsigned int si=0; si< SiPM_positions.size(); si++) {
	G4ThreeVector mypos =  SiPM_positions[si].second;
	std::pair<int, G4ThreeVector> abs_pos;
	abs_pos.first = (i+1)*1000+ SiPM_positions[si].first ;
	//	abs_pos.second =
	G4ThreeVector pos_tot =
	  G4ThreeVector(post.getX()+mypos.getX(), post.getY()+mypos.getY(), post.getZ()+mypos.getZ());
	pos_tot.rotate(rot_angle, G4ThreeVector(0., 1., 0.));
	pos_tot = pos_tot + displ;
	abs_pos.second = pos_tot;
		  
	_absSiPMpos.push_back(abs_pos);
      }
    }
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
  
}//end namespace nexus
