// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <jmunoz@ific.uv.es>
//  Created: 25 Apr 2012
//
//  Copyright (c) 2012 NEXT Collaboration
//
//  Updated to NextDemo++  by  Ruth Weiss Babai <ruty.wb@gmail.com>
//  From:   Next100TrackingPlane.cc
//  Date:       Aug 2019
// ----------------------------------------------------------------------------

#include "NextDemoTrackingPlane.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
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
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>


namespace nexus {


  NextDemoTrackingPlane::NextDemoTrackingPlane():

    // Dimensions
    support_side_ (16.0 * cm),  // <-> from STEP file
    support_thickness_ (1.2 * cm),  // <-> from STEP file  //  Next100:(2.0 * cm)
    //
    z_displ_ (5.79 * mm),  // From Neus Drawing (5.79 * mm)
    hole_size_ (49 * mm), // <-> from STEP file // For Next100::(45 * mm),

    // SiPMTs per Dice Board
    SiPM_rows_ (8),
    SiPM_columns_ (8),

    // Number of Dice Boards, DB columns
    DB_columns_ (2),
    num_DBs_ (4),
    dice_side_x_ (85.*mm),  // From NextNewTrackingPlane: & From Drawing "0000-00 ASSEMBLY NEXT-DEMO++.pdf"
    dice_side_ (79.*mm),
    dice_gap_ (1. *mm),// distance between dices // From NextNewTrackingPlane: & From Drawing "0000-00 ASSEMBLY NEXT-DEMO++.pdf"
    visibility_(0),
    verbosity_(0),
    verb_sipmPos_(0)
  {


    /// Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/", "Control commands of geometry NextDemo.");
    msg_->DeclareProperty("tracking_plane_vis", visibility_, "Tracking Plane Visibility");
    msg_->DeclareProperty("tracking_plane_verbosity", verbosity_, "Tracking Plane verbosity");
    msg_->DeclareProperty("SiPMPos_verbosity", verb_sipmPos_, "SiPMPos verbosity");

    // The Dice Board
    // _dice_board = new NextElDB(_SiPM_rows, _SiPM_columns);
    //_dice_board = new NextNewKDB(_SiPM_rows, _SiPM_columns);
    dice_board_ = new NextDemoKDB(SiPM_rows_, SiPM_columns_);
  }



  void NextDemoTrackingPlane::Construct()
  {

    // Dice Boards positions
    dice_board_->Construct();
    GenerateDBPositions();

    ///// Support plate  // from STEP file : changing from Tubs to Box  //
    // G4Tubs* support_plate_nh_solid = new G4Tubs("SUPPORT_PLATE_NH", 0., _support_diam/2., _support_thickness/2., 0., twopi);
    G4Box* support_plate_nh_solid = new G4Box("SUPPORT_PLATE_NH",  support_side_/2.,  support_side_/2., support_thickness_/2.);

    // Making DB holes
    G4Box* support_plate_db_hole_solid = new G4Box("SUPPORT_PLATE_DB_HOLE", hole_size_/2., hole_size_/2., support_thickness_/2. + 2.*mm);
    G4SubtractionSolid* support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE", support_plate_nh_solid,
								     support_plate_db_hole_solid, 0, DB_positions_[0]);
    for (int i=1; i<num_DBs_; i++) {
      support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE", support_plate_solid,
						   support_plate_db_hole_solid, 0, DB_positions_[i]);
    }

    G4LogicalVolume* support_plate_logic = new G4LogicalVolume(support_plate_solid,
							        G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"),
							       "SUPPORT_PLATE");


    ///// Dice Boards

    G4LogicalVolume* dice_board_logic = dice_board_->GetLogicalVolume();
    G4ThreeVector db_dimensions = dice_board_->GetDimensions();
    G4double db_thickness = db_dimensions.z();


    ///// Support Plate placement

    G4double support_plate_posz =
             el_gap_z_edge_ + z_displ_ + db_thickness + support_thickness_/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,support_plate_posz),
               support_plate_logic,  "SUPPORT_PLATE", mother_logic_, false, 0);

    ///// Dice Boards placement
    G4double dice_board_posz = el_gap_z_edge_ + z_displ_ + db_thickness/2.;
   if (verbosity_) {
    G4cout << "  ****************************  " << G4endl;
    G4cout << " from GET  el_gap_z_edge_ " <<  el_gap_z_edge_  << G4endl;
    G4cout << " dice_board_posz: " << dice_board_posz  <<  G4endl;
    G4cout << "  ****************************  " << G4endl;
   }

     const std::vector<std::pair<int, G4ThreeVector> > SiPM_positions =
      dice_board_->GetPositions();

    G4ThreeVector pos;
    for (int i=0; i<num_DBs_; i++) {
      pos = DB_positions_[i];
      pos.setZ(dice_board_posz);
      new G4PVPlacement(0, pos, dice_board_logic,
			"DICE_BOARD", mother_logic_, false, i+1, false);
      // Store the absolute positions of SiPMs in gas, for possible checks
      for (unsigned int si=0; si< SiPM_positions.size(); si++) {
	G4ThreeVector mypos =  SiPM_positions[si].second;
	std::pair<int, G4ThreeVector> abs_pos;
	abs_pos.first = (i+1)*1000+ SiPM_positions[si].first ;
	abs_pos.second =
	  G4ThreeVector(pos.getX()+mypos.getX(), pos.getY()+mypos.getY(), pos.getZ()+mypos.getZ());

	absSiPMpos_.push_back(abs_pos);
      }
    }

   if (verb_sipmPos_) {
     PrintAbsoluteSiPMPos();
   }

    ////////////////////////////////////////////////
    ////// It remains the electronics to add  //////


    // SETTING VISIBILITIES   //////////

    // Dice boards always visible in dark green
    G4VisAttributes dark_green_col = nexus::DarkGreen();
    dice_board_logic->SetVisAttributes(dark_green_col);


    if (visibility_) {
      G4VisAttributes light_brown_col = nexus::CopperBrown();
      support_plate_logic->SetVisAttributes(light_brown_col);
      light_brown_col.SetForceSolid(true);
      support_plate_logic->SetVisAttributes(light_brown_col);
    }
    else {
      support_plate_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }



    // VERTEX GENERATORS   //////////
    //_support_gen  = new CylinderPointSampler(_support_diam/2., _support_thickness, 0.,
	//				     0., G4ThreeVector(0., 0., support_plate_posz));
    support_gen_  = new BoxPointSampler(support_side_/2., support_side_/2., support_thickness_,
					     0., G4ThreeVector(0., 0., support_plate_posz));

    // Vertexes are generated in a thin surface in the backside of the board
    G4double z_dim = db_dimensions.z();
    dice_board_gen_ = new BoxPointSampler(db_dimensions.x(), db_dimensions.y(), z_dim * .1,
					  0., G4ThreeVector(0., 0., dice_board_posz + z_dim*.4));
  }



  void NextDemoTrackingPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }

  void NextDemoTrackingPlane::SetAnodeZCoord(G4double z)
  {
    el_gap_z_edge_ = z;
  }


  NextDemoTrackingPlane::~NextDemoTrackingPlane()
  {
    delete support_gen_;
    delete dice_board_gen_;
  }



  G4ThreeVector NextDemoTrackingPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Support Plate
    if (region == "TRK_SUPPORT") {
      //R//do {G4VPhysicalVolume *VertexVolume;
      //R//do {
	vertex = support_gen_->GenerateVertex("INSIDE");
	/*// To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	// glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	//std::cout << VertexVolume->GetName() << std::endl;
      } while (VertexVolume->GetName() != "SUPPORT_PLATE");
      */
    }

    // Dice Boards
    else if (region == "DICE_BOARD") {
      vertex = dice_board_gen_->GenerateVertex("INSIDE");
      G4double rand = num_DBs_ * G4UniformRand();
      G4ThreeVector db_pos = DB_positions_[int(rand)];
      vertex += db_pos;
    }
    else {
      G4Exception("[NextDemoTrackingPlane]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;

  }



  void NextDemoTrackingPlane::GenerateDBPositions()
  {
    /// Function that computes and stores the XY positions of Dice Boards

    G4int num_rows[] = {2, 2}; // From NextNewTrackingPlane: & From Drawing "0000-00 ASSEMBLY NEXT-DEMO++.pdf"

    G4int total_positions = 0;


    G4double x_step = dice_board_->GetDimensions().getX() + 1.*mm; // Next100
    G4double y_step = dice_board_->GetDimensions().getY() + 1.*mm; // Next100

     // Separation between consecutive columns / rows _dice_gap
     // G4double x_step = _dice_side_x +_dice_gap;    // Ruty
     // G4double y_step = _dice_side +_dice_gap;

    G4double x_dim = x_step * (DB_columns_ -1);

    G4ThreeVector position(0.,0.,0.);

    // For every column
    for (G4int col=0; col<DB_columns_; col++) {
      G4double pos_x = x_dim/2. - col * x_step;
      G4int rows = num_rows[col];
      G4double y_dim = y_step * (rows-1);
      // For every row
      for (G4int row=0; row<rows; row++) {
	G4double pos_y = y_dim/2. - row * y_step;

	position.setX(pos_x);
	position.setY(pos_y);
	DB_positions_.push_back(position);
	total_positions++;
      }
    }

    // Checking
    if (total_positions != num_DBs_) {
      G4cout << "\n\nERROR: Number of DBs doesn't match with number of positions calculated\n";
      exit(0);
    }


  }

   void NextDemoTrackingPlane::PrintAbsoluteSiPMPos()
  {
    G4cout << "----- Absolute position of SiPMs in gas volume -----" << G4endl;
    G4cout <<  G4endl;
    for (unsigned int i=0; i<absSiPMpos_.size(); i++) {
      std::pair<int, G4ThreeVector> abs_pos = absSiPMpos_[i];
      G4cout << "ID number: " << absSiPMpos_[i].first << ", position: "
      	     << absSiPMpos_[i].second.getX() << ", "
      	     << absSiPMpos_[i].second.getY() << ", "
      	     << absSiPMpos_[i].second.getZ()  << G4endl;
    }
    G4cout <<  G4endl;
    G4cout << "---------------------------------------------------" << G4endl;
  }

}
