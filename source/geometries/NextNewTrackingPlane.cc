// -----------------------------------------------------------------------------
// nexus | NextNewTrackingPlane.cc
//
// TrackingPlane of the NEXT-WHITE detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextNewTrackingPlane.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "CylinderPointSamplerLegacy.h"
#include "BoxPointSamplerLegacy.h"

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

namespace nexus {

  using namespace CLHEP;

  NextNewTrackingPlane::NextNewTrackingPlane():
    // Dimensions
    support_plate_diam_ (63.0 * cm), //must be checked
    support_plate_thickness_ (12.0 * cm),
    support_plate_front_buffer_diam_ (53.8 * cm),
    support_plate_front_buffer_thickness_ (1. * cm),
    support_plate_tread_diam_ (54.56 *cm), //sup_diam-2* 42.2mm from drawings
    support_plate_tread_thickness_ (6. * cm), //sup_thik/2
    tracking_plane_z_pos_ (284.1 *mm),
    //    tracking_plane_z_pos_ (28.905 *cm),//el_gap_z_pos_ (25.5 *cm) From drawings + el_gap_(1.4)/2 + el_grid_thickness_ + el_to__
    //    dice_board_z_pos_ (282.25*mm), // its surface must be 2 mm away from the end of the anode plate --> pos_z_anode + anode_thickness/2. + 2.*mm = 284.1 *mm + half of DB thick
    //   z_kdb_displ_ (0.0 * cm), //distance between DB and support plate( kdb_surface at the same level as support surface)
    cable_hole_width_ (45 * mm),
    cable_hole_high_ (8 * mm),
    plug_x_ (40. *mm),
    plug_y_ (4. *mm), //two union connectors
    plug_z_ (6. *mm),
    plug_distance_from_copper_(21. * cm),

    // SiPMs per Dice Board
    SiPM_rows_ (8),
    SiPM_columns_ (8),

    // Number of Dice Boards, DB columns
    DB_columns_ (6),
    num_DBs_ (28),
    dice_side_ (79.*mm),
    dice_gap_ (1. *mm),// distance between dices

    visibility_ (1)
  {
    /// Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    msg_->DeclareProperty("tracking_plane_vis", visibility_, "Tracking Plane Visibility");

    // The Dice Board
    kapton_dice_board_ = new NextNewKDB(SiPM_rows_, SiPM_columns_);

  }

  void NextNewTrackingPlane::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }

  void NextNewTrackingPlane::Construct()
  {
    GenerateDBPositions();

    ///// SUPPORT PLATE /////
    G4Tubs* support_plate_nh_solid =
      new G4Tubs("SUPPORT_PLATE_NH", 0., support_plate_diam_/2., support_plate_thickness_/2., 0., twopi);
    //Making front buffer
    G4Tubs* support_plate_front_buffer_solid =
      new G4Tubs("SUPPORT_PLATE_FBUF_SOLID",0., support_plate_front_buffer_diam_/2.,
		 (support_plate_front_buffer_thickness_+1.*mm)/2.,0.,twopi);
    G4SubtractionSolid* support_plate_solid =
      new G4SubtractionSolid("SUPPORT_PLATE", support_plate_nh_solid,support_plate_front_buffer_solid,0,
			     G4ThreeVector(0.,0.,-support_plate_thickness_/2. + support_plate_front_buffer_thickness_/2.-.5*mm));

    //Making tread
    G4Tubs* support_plate_tread_solid =
      new G4Tubs("SUPPORT_PLATE_TREAD",support_plate_tread_diam_/2.,support_plate_diam_/2.+1.*mm,
		 (support_plate_tread_thickness_+1.*mm)/2.,0.,twopi);
    support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE",support_plate_solid,support_plate_tread_solid,0,
						 G4ThreeVector(0.,0.,-support_plate_thickness_/2. + support_plate_tread_thickness_/2.-.5*mm));

    // Making DB cables holes through
    G4Box* support_plate_cable_hole_solid =
      new G4Box("SUPPORT_PLATE_HOLE", cable_hole_width_/2., cable_hole_high_/2., support_plate_thickness_/2. + 1.*mm);
    G4ThreeVector pos;
    for (unsigned int i=0; i<DB_positions_.size(); i++) {
      pos = DB_positions_[i];
      support_plate_solid = new G4SubtractionSolid("SUPPORT_PLATE", support_plate_solid,
     						   support_plate_cable_hole_solid, 0, pos);
    }

    G4Material* copper_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    copper_mat->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* support_plate_logic = new G4LogicalVolume(support_plate_solid,
							       copper_mat,
							       "SUPPORT_PLATE");

    ///// Support Plate placement
    G4double support_plate_z_pos =  tracking_plane_z_pos_ + support_plate_thickness_/2.;
    new G4PVPlacement(0, G4ThreeVector(0.,0.,support_plate_z_pos), support_plate_logic,
		      "SUPPORT_PLATE", mother_logic_, false, 0, false);

    /////  DICE BOARDS  /////
    kapton_dice_board_->SetMotherLogicalVolume(mother_logic_);
    kapton_dice_board_->Construct();
    kdb_dimensions_ = kapton_dice_board_->GetDimensions();
    G4LogicalVolume* dice_board_logic = kapton_dice_board_->GetLogicalVolume();
    G4double db_thickness =kdb_dimensions_.z();
    ////Dice Boards placement
    //dice_board_z_pos_ = support_plate_z_pos -support_plate_thickness_/2. -z_kdb_displ_ +db_thickness/2.;
    dice_board_z_pos_ = tracking_plane_z_pos_ + db_thickness/2. + 3. * mm - 1.35 * mm;

    G4ThreeVector post;
    for (int i=0; i<num_DBs_; i++) {
      post = DB_positions_[i];
      post.setZ(dice_board_z_pos_);
      new G4PVPlacement(0, post, dice_board_logic,
			"DICE_BOARD", mother_logic_, false, i+1, false);
    }

    // PrintAbsoluteSiPMPos(G4ThreeVector(0., 0., 275.), pi);

    //PIGGY TAIL PLUG/////////////////////////////////////////////////////
    G4Box* plug_solid = new G4Box("DB_CONNECTOR", plug_x_/2., plug_y_/2., plug_z_/2.);

    G4Material* peek_mat = materials::PEEK();
    peek_mat->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* plug_logic = new G4LogicalVolume(plug_solid,  peek_mat, "DB_PLUG");
    //G4PVPlacement * plug_physi;
    G4ThreeVector positn;
    for (int i=0; i<num_DBs_; i++) {
      positn = DB_positions_[i];
      positn.setY(positn.y()- 10.*mm);
      positn.setZ(dice_board_z_pos_ + support_plate_front_buffer_thickness_
                  + support_plate_thickness_ + plug_distance_from_copper_);
      new G4PVPlacement(0, positn, plug_logic,"DB_PLUG",
			mother_logic_, false, i+1, false);
    }


   //// SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes light_brown_col = nexus::CopperBrown();
      light_brown_col.SetForceSolid(true);
      support_plate_logic->SetVisAttributes(light_brown_col);
      G4VisAttributes dirty_white_col =nexus::DirtyWhite();
      dirty_white_col.SetForceSolid(true);
      plug_logic->SetVisAttributes(dirty_white_col);
    }
    else {
      support_plate_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      plug_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }


    // VERTEX GENERATORS   //////////
    support_body_gen_  =
      new CylinderPointSamplerLegacy(0., support_plate_thickness_-support_plate_front_buffer_thickness_,
			                         support_plate_tread_diam_/2., 0.,
                               G4ThreeVector(0., 0., support_plate_z_pos));
    support_flange_gen_  =
      new CylinderPointSamplerLegacy(support_plate_tread_diam_/2., support_plate_thickness_/2.,
			                         (support_plate_diam_ - support_plate_tread_diam_)/2., 0.,
                               G4ThreeVector(0., 0., support_plate_z_pos+support_plate_thickness_/4.));
    support_buffer_gen_  =
      new CylinderPointSamplerLegacy(support_plate_front_buffer_diam_/2.,
                               support_plate_front_buffer_thickness_/2.,
                               (support_plate_tread_diam_-support_plate_front_buffer_diam_)/2.,
                               0., G4ThreeVector(0., 0., support_plate_z_pos -support_plate_thickness_/2.
                               +support_plate_front_buffer_thickness_/2.));
    plug_gen_ =
      new BoxPointSamplerLegacy(plug_x_, plug_y_, plug_z_,0., G4ThreeVector(0.,0.,0.),0);
			  // G4ThreeVector(0.,0.,dice_board_z_pos_ + support_plate_front_buffer_thickness_ + support_plate_thickness_),0);

     // Getting the support  volume over total
    G4double body_vol =
      (support_plate_thickness_-support_plate_front_buffer_thickness_)*pi*(support_plate_tread_diam_/2.)*(support_plate_tread_diam_/2.);
    G4double flange_vol =
      (support_plate_thickness_/2.)*pi*((support_plate_diam_/2.)*(support_plate_diam_/2.)-( support_plate_tread_diam_/2.)*( support_plate_tread_diam_/2.));
    G4double buffer_vol =
      (support_plate_front_buffer_thickness_/2.)*pi*((support_plate_tread_diam_/2.)*(support_plate_tread_diam_/2.)-(support_plate_front_buffer_diam_/2.)*(support_plate_front_buffer_diam_/2.));
     G4double total_vol = body_vol + flange_vol + buffer_vol;
     body_perc_ = body_vol / total_vol;
     flange_perc_ =  (flange_vol + body_vol) / total_vol;
     // std::cout<<"SUPPORT PLATE (TP) VOLUME: \t"<<total_vol<<std::endl;
  }

  NextNewTrackingPlane::~NextNewTrackingPlane()
  {
    delete support_body_gen_;
    delete support_flange_gen_;
    delete support_buffer_gen_;
    delete plug_gen_;
  }

  G4ThreeVector NextNewTrackingPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Support Plate
    if (region == "SUPPORT_PLATE") {
      G4double rand1 = G4UniformRand();
      //Generating in the body
      if (rand1 < body_perc_) {
        // As it is full of holes, let's get sure vertexes are in the right volume
        G4VPhysicalVolume *VertexVolume;
        do {
          vertex = support_body_gen_->GenerateVertex("BODY_VOL");
          // To check its volume, one needs to rotate and shift the vertex
          // because the check is done using global coordinates
          G4ThreeVector glob_vtx(vertex);
          // First rotate, then shift
          glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
          glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
          VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
        } while (VertexVolume->GetName() != "SUPPORT_PLATE");
      }
      // Generating in the flange
      else if (rand1 < flange_perc_){
       	vertex = support_flange_gen_->GenerateVertex("BODY_VOL");
      }
      // Generating in the buffer
      else {
       	vertex = support_buffer_gen_->GenerateVertex("BODY_VOL");
      }
    }

    // Dice Boards
    else if (region == "DICE_BOARD") {
      G4ThreeVector ini_vertex = kapton_dice_board_->GenerateVertex(region);
      G4double rand = num_DBs_ * G4UniformRand();
      G4ThreeVector db_pos = DB_positions_[int(rand)];
      vertex = ini_vertex + db_pos;
      vertex.setZ(vertex.z() +dice_board_z_pos_);
    }

    // PIGGY TAIL PLUG
    else if (region == "DB_PLUG") {
      G4ThreeVector ini_vertex = plug_gen_->GenerateVertex("INSIDE");
      G4double rand = num_DBs_ * G4UniformRand();
      G4ThreeVector db_pos = DB_positions_[int(rand)];
      vertex = ini_vertex + db_pos;
      vertex.setY(vertex.y() - 10.*mm);
      vertex.setZ(vertex.z() +dice_board_z_pos_ + support_plate_front_buffer_thickness_
                  + support_plate_thickness_ + plug_distance_from_copper_);
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
    // Separation between consecutive columns / rows dice_gap_
    G4double x_step = dice_side_ +dice_gap_;
    G4double y_step = dice_side_ +dice_gap_;
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

  void NextNewTrackingPlane::PrintAbsoluteSiPMPos(G4ThreeVector displ, G4double rot_angle)
  {
    // Print the absolute positions of SiPMs in gas, for possible checks
    const std::vector<std::pair<int, G4ThreeVector> > SiPM_positions =
      kapton_dice_board_->GetPositions();
    G4ThreeVector post;
    for (int i=0; i<num_DBs_; i++) {
      post = DB_positions_[i];
      post.setZ(dice_board_z_pos_);
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

	absSiPMpos_.push_back(abs_pos);
      }
    }
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

}//end namespace nexus
