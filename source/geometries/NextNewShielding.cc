// ----------------------------------------------------------------------------
// nexus | NextNewShielding.cc
//
// Lead castle placed at the LSC to be used with the NEW geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextNewShielding.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "BoxPointSamplerLegacy.h"

#include <G4GenericMessenger.hh>
#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4UnionSolid.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>
#include <G4RotationMatrix.hh>
#include <G4UserLimits.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  NextNewShielding::NextNewShielding():
    GeometryBase(),

    // Shielding internal dimensions
    shield_x_ (155.  * cm),
    shield_y_ (225.6 * cm),
    shield_z_ (258.5 * cm), // 253.0 * cm before May 12, 2017

    //Steel Structure
    beam_base_thickness_ (4. *mm),
    lateral_z_separation_ (1006. *mm), //distance between the two lateral beams
    roof_z_separation_ (755. *mm), //distance between 1st and 2nd roof beams
    front_x_separation_ (154.*mm), //distance between the two front beams
    // Box thickness
    lead_thickness_ (20. * cm),
    steel_thickness_ (6. * mm),
    visibility_ (0)

  {

    /// Shielding is made of two boxes, the external made of lead,
    /// and the internal made of a mix of steel & titanium
    /// The steel beam structure is placed inside the lead


    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    msg_->DeclareProperty("shielding_vis", visibility_, "Shielding Visibility");

    // Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  }



  void NextNewShielding::Construct()
  {
    // LEAD BOX   ///////////
    lead_x_ = shield_x_ + 2. * steel_thickness_ + 2. * lead_thickness_;
    lead_y_ = shield_y_ + 2. * steel_thickness_ + 2. * lead_thickness_;
    lead_z_ = shield_z_ + 2. * steel_thickness_ + 2. * lead_thickness_;

    G4Box* lead_box_solid = new G4Box("LEAD_BOX", lead_x_/2., lead_y_/2., lead_z_/2.);

    G4LogicalVolume* lead_box_logic = new G4LogicalVolume(lead_box_solid,
							  G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"),
							  "LEAD_BOX");
    this->SetLogicalVolume(lead_box_logic);

    //STEEL BEAM STRUCTURE
    //auxiliar positions
    G4double lat_beam_x = shield_x_/2.+ steel_thickness_ + lead_thickness_/2.;
    G4double front_beam_z = shield_z_/2.+steel_thickness_+lead_thickness_/2.;
    G4double top_beam_y = shield_y_/2.+ steel_thickness_ + lead_thickness_/2.;

    G4Box* roof_beam = new G4Box("STRUCT_BEAM",lead_x_/2.,beam_base_thickness_/2.,lead_z_/2.);
    G4Box* aux_box = new G4Box("AUX_box",shield_x_/2.+ steel_thickness_,beam_base_thickness_,shield_z_/2.+ steel_thickness_);
    G4SubtractionSolid* roof_beam_solid = new G4SubtractionSolid("STRUCT_BEAM",roof_beam,aux_box,0,G4ThreeVector(0.,0.,0.));

    G4Box* lat_beam_solid = new G4Box("STRUCT_BEAM",
				      lead_thickness_/2.,
				      shield_y_/2.+steel_thickness_/2.,
				      beam_base_thickness_/2.);
    G4Box* top_xbeam_solid = new G4Box("STRUCT_BEAM",
				      shield_x_/2.+lead_thickness_+steel_thickness_,
				      lead_thickness_/2.,
				      beam_base_thickness_/2.);
    G4Box* top_zbeam_solid = new G4Box("STRUCT_BEAM",
				       beam_base_thickness_/2.,
				       lead_thickness_/2. -1.*mm,
				       shield_z_/2.+lead_thickness_+steel_thickness_);
    G4UnionSolid* struct_solid = new G4UnionSolid("STEEL_BEAM1_STRUCTURE",top_xbeam_solid,top_zbeam_solid,
						  0,G4ThreeVector(-front_x_separation_/2.,0.,
								  -(roof_z_separation_+lateral_z_separation_/2.)));
    struct_solid = new G4UnionSolid("STEEL_BEAM2_STRUCTURE",struct_solid,top_xbeam_solid,
      				    0,G4ThreeVector(0.,0., -roof_z_separation_));
    struct_solid = new G4UnionSolid("STEEL_BEAM3_STRUCTURE",struct_solid,top_xbeam_solid,
       				    0,G4ThreeVector(0.,0., -(roof_z_separation_+lateral_z_separation_)));
    struct_solid = new G4UnionSolid("STEEL_BEAM4_STRUCTURE",struct_solid,top_xbeam_solid,
       				    0, G4ThreeVector(0.,0., -(2*roof_z_separation_+lateral_z_separation_)));
    struct_solid = new G4UnionSolid("STEEL_BEAM5_STRUCTURE",struct_solid,top_zbeam_solid,
     				    0,G4ThreeVector(front_x_separation_/2.,0.,
      						    -(roof_z_separation_+lateral_z_separation_/2.)));


    G4LogicalVolume* roof_logic = new G4LogicalVolume(roof_beam_solid,
      						      materials::Steel(),
  						      "STEEL_BEAM_ROOF");
    G4LogicalVolume* lat_beam_logic = new G4LogicalVolume(lat_beam_solid,
     							  materials::Steel(),
 							  "STEEL_BEAM_STRUCTURE_LAT");
    G4LogicalVolume* struct_logic = new G4LogicalVolume(struct_solid,
							materials::Steel(),
							"STEEL_BEAM_STRUCTURE_TOP");

    new G4PVPlacement(0,G4ThreeVector(0.,shield_y_/2.+steel_thickness_/2.,0.),roof_logic,"STEEL_BEAM_STRUCTURE_roof",
		      lead_box_logic,false,0,false);
    new G4PVPlacement(0,G4ThreeVector
		      (lat_beam_x,-beam_base_thickness_/2.,lateral_z_separation_/2.)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat1",
		      lead_box_logic,false,0,false);
    new G4PVPlacement(0,G4ThreeVector
		      (lat_beam_x,-beam_base_thickness_/2.,-lateral_z_separation_/2.)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat2",
		      lead_box_logic,false,0,false);
    new G4PVPlacement(0,G4ThreeVector
		      (-lat_beam_x,-beam_base_thickness_/2.,lateral_z_separation_/2.)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat3",
		      lead_box_logic,false,0,false);
    new G4PVPlacement(0,G4ThreeVector
		      (-lat_beam_x,-beam_base_thickness_/2.,-lateral_z_separation_/2.)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat4",
		      lead_box_logic,false,0,false);
    // Rotate the beams
    G4RotationMatrix* rot_beam = new G4RotationMatrix();
    rot_beam->rotateY(pi/2.);
    new G4PVPlacement(rot_beam,G4ThreeVector
		      (-front_x_separation_/2.,-beam_base_thickness_/2.,front_beam_z)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat5",
		      lead_box_logic,false,0,false);
    new G4PVPlacement(rot_beam,G4ThreeVector
		      (front_x_separation_/2.,-beam_base_thickness_/2.,front_beam_z)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat6",
		      lead_box_logic,false,0,false);
    new G4PVPlacement(rot_beam,G4ThreeVector
		      (-front_x_separation_/2.,-beam_base_thickness_/2.,-front_beam_z)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat7",
		      lead_box_logic,false,0,false);
    new G4PVPlacement(rot_beam,G4ThreeVector
		      (front_x_separation_/2.,-beam_base_thickness_/2.,-front_beam_z)
		      ,lat_beam_logic,"STEEL_BEAM_STRUCTURE_lat8",
		      lead_box_logic,false,0,false);

    new G4PVPlacement(0,G4ThreeVector(0.,top_beam_y,roof_z_separation_+lateral_z_separation_/2.)
		      ,struct_logic,"STEEL_BEAM_STRUCTURE_top", lead_box_logic,false,0,false);


    // STEEL BOX   ///////////
    G4double steel_x = shield_x_ + 2. * steel_thickness_;
    G4double steel_y = shield_y_ + 2. * steel_thickness_;
    G4double steel_z = shield_z_ + 2. * steel_thickness_;

    G4Box* steel_box_solid =
      new G4Box("STEEL_BOX", steel_x/2., steel_y/2., steel_z/2.);

    G4LogicalVolume* steel_box_logic = new G4LogicalVolume(steel_box_solid,
							   materials::Steel(),
							   "STEEL_BOX");

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), steel_box_logic,
		      "STEEL_BOX", lead_box_logic, false, 0);

    // AIR INSIDE
    G4Box* air_box_solid =
      new G4Box("INNER_AIR", shield_x_/2., shield_y_/2., shield_z_/2.);

    air_box_logic_ =
      new G4LogicalVolume(air_box_solid,
			  G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),
			  "INNER_AIR");

    ////Limit the uStepMax=Maximum step length, uTrakMax=Maximum total track length,
    //uTimeMax= Maximum global time for a track, uEkinMin= Minimum remaining kinetic energy for a track
    //uRangMin=         Minimum remaining range for a track
    air_box_logic_->SetUserLimits(new G4UserLimits( DBL_MAX, DBL_MAX, DBL_MAX,100.*keV,0.));
    air_box_logic_->SetVisAttributes(G4VisAttributes::GetInvisible());

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), air_box_logic_,
		      "INNER_AIR", steel_box_logic, false, 0);



    // SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes dark_grey_col = nexus::DarkGrey();
      lead_box_logic->SetVisAttributes(dark_grey_col);
      G4VisAttributes grey_col = nexus::LightGrey();
      steel_box_logic->SetVisAttributes(grey_col);
      G4VisAttributes antiox_col = nexus::BloodRed();
      //  antiox.SetForceSolid(true);
      roof_logic->SetVisAttributes(antiox_col);
      lat_beam_logic->SetVisAttributes(antiox_col);
      struct_logic->SetVisAttributes(antiox_col);
    }
    else {
      lead_box_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      steel_box_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      roof_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      struct_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      lat_beam_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }


    // Creating the vertex generators   //////////
    // Only shooting from the innest 5 cm.
    lead_gen_  =
      new BoxPointSamplerLegacy(steel_x, steel_y, steel_z, 5.*cm, G4ThreeVector(0.,0.,0.), 0);

    G4double ext_offset = 1. * cm;
    external_gen_ =
      new BoxPointSamplerLegacy(lead_x_ + ext_offset, lead_y_ + ext_offset,
                                lead_z_ + ext_offset, 1. * mm, G4ThreeVector(0.,0.,0.), 0);


    steel_gen_ =
      new BoxPointSamplerLegacy(shield_x_, shield_y_, shield_z_, steel_thickness_, G4ThreeVector(0.,0.,0.), 0);

    G4double inn_offset = .5 * cm;
    inner_air_gen_ =
      new BoxPointSamplerLegacy(shield_x_ - inn_offset, shield_y_ - inn_offset,
                                shield_z_ - inn_offset, 1. * mm, G4ThreeVector(0.,0.,0.), 0);


    lat_roof_gen_ =
      new BoxPointSamplerLegacy(lead_thickness_, beam_base_thickness_, shield_z_, 0.,
                                G4ThreeVector(0.,shield_y_/2.+steel_thickness_/2.,0.), 0);
    front_roof_gen_ =
      new BoxPointSamplerLegacy(lead_x_, beam_base_thickness_,lead_thickness_, 0.,
                                G4ThreeVector(0.,shield_y_/2.+steel_thickness_/2., 0.), 0);
    struct_x_gen_ =
      new BoxPointSamplerLegacy(shield_x_+2*lead_thickness_+2*steel_thickness_,
                                lead_thickness_, beam_base_thickness_, 0.,
                                G4ThreeVector(0.,top_beam_y,roof_z_separation_+lateral_z_separation_/2), 0);
    struct_z_gen_ =
      new BoxPointSamplerLegacy(beam_base_thickness_, lead_thickness_ -1.*mm,
                                shield_z_+2*lead_thickness_+2*steel_thickness_, 0.,
                                G4ThreeVector(-front_x_separation_/2.,top_beam_y, 0.), 0);
    lat_beam_gen_ =
      new BoxPointSamplerLegacy(lead_thickness_, shield_y_+steel_thickness_, beam_base_thickness_, 0.,
                                G4ThreeVector(lat_beam_x,-beam_base_thickness_/2.,lateral_z_separation_/2.), 0);
    front_beam_gen_ =
      new BoxPointSamplerLegacy(beam_base_thickness_, shield_y_+steel_thickness_, lead_thickness_,
                                0., G4ThreeVector(-front_x_separation_/2., -beam_base_thickness_/2., front_beam_z), 0);



    // Calculating some probs
    G4double roof_vol = roof_beam_solid->GetCubicVolume();
    //std::cout<<"ROOF BEAM VOLUME "<<roof_vol<<std::endl;
    G4double struct_top_vol = struct_solid->GetCubicVolume();
    //std::cout<<"TOP STRUCT VOLUME "<<struct_top_vol<<std::endl;
    G4double lateral_vol = lat_beam_solid->GetCubicVolume();
    //std::cout<<"LAT BEAM STRUCT VOLUME "<<lateral_vol<<"\t TOTAL LATERAL BEAMS VOL "<<8*lateral_vol<<std::endl;
    G4double total_vol = roof_vol+struct_top_vol+(8*lateral_vol);
    //std::cout<<"TOTAL STRUCTURE VOLUME "<<total_vol<<std::endl;

    perc_roof_vol_ = roof_vol/total_vol;
    perc_front_roof_vol_ = 2*(lead_x_*beam_base_thickness_*lead_thickness_) /roof_vol;
    perc_top_struct_vol_ = struct_top_vol /total_vol;
    perc_struc_x_vol_ = 4*((shield_x_+2*lead_thickness_+2*steel_thickness_)*lead_thickness_*beam_base_thickness_)/struct_top_vol;

    // std::cout<<"SHIELDING LEAD VOLUME:\t"<<lead_box_solid->GetCubicVolume()<<std::endl;
    // std::cout<<"SHIELDING STEEL VOLUME:\t"<<steel_box_solid->GetCubicVolume()<<std::endl;
    // std::cout<<"VOLUME INSIDE THE SHIELDING CASTLE:\t"<<shielding_box_solid->GetCubicVolume()<<std::endl;

  }



  NextNewShielding::~NextNewShielding()
  {
    delete lead_gen_;
    delete external_gen_;
    delete steel_gen_;
    delete inner_air_gen_;
    delete lat_roof_gen_;
    delete front_roof_gen_;
    delete struct_x_gen_;
    delete struct_z_gen_;
    delete lat_beam_gen_;
    delete front_beam_gen_;
  }

  G4LogicalVolume* NextNewShielding::GetAirLogicalVolume() const
  {
    return air_box_logic_;
  }

  G4ThreeVector NextNewShielding::GetDimensions() const
  {
    return G4ThreeVector(lead_x_, lead_y_, lead_z_);
  }

  G4ThreeVector NextNewShielding::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "SHIELDING_LEAD") {
      G4VPhysicalVolume *VertexVolume;
      do {
	vertex = lead_gen_->GenerateVertex("WHOLE_VOL");
	// To check its volume, one needs to rotate and shift the vertex
	// because the check is done using global coordinates
	G4ThreeVector glob_vtx(vertex);
	// First rotate, then shift
	glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "LEAD_BOX");
    }

    else if (region == "SHIELDING_STEEL") {
      vertex = steel_gen_->GenerateVertex("WHOLE_VOL");
    }

    else if (region == "INNER_AIR") {
      vertex = inner_air_gen_->GenerateVertex("WHOLE_VOL");
    }

    else if (region == "EXTERNAL") {
      vertex = external_gen_->GenerateVertex("WHOLE_VOL");
    }
    else if(region=="SHIELDING_STRUCT"){
      G4double rand = G4UniformRand();

      if (rand < perc_roof_vol_) { //ROOF BEAM STRUCTURE
      	// G4VPhysicalVolume *VertexVolume;
      	// do {
      	if (G4UniformRand() <  perc_front_roof_vol_){
      	  if (G4UniformRand() < 0.5) {
      	    vertex = front_roof_gen_->GenerateVertex("INSIDE");
      	    vertex.setZ(vertex.z() + (shield_z_/2.+steel_thickness_+lead_thickness_/2.));
	    // std::cout<<"frontal +"<<std::endl;
      	  }
      	  else{
      	    vertex = front_roof_gen_->GenerateVertex("INSIDE");
      	    vertex.setZ(vertex.z() - (shield_z_/2.+steel_thickness_+lead_thickness_/2.));
      	    // std::cout<<"frontal -"<<std::endl;
      	  }
      	}
      	else{
      	  if (G4UniformRand() < 0.5) {
      	    vertex = lat_roof_gen_->GenerateVertex("INSIDE");
      	    vertex.setX(vertex.x() + ( shield_x_/2.+ steel_thickness_ + lead_thickness_/2.));
      	    // std::cout<<"lateral +"<<std::endl;
      	  }
      	  else{
      	    vertex = lat_roof_gen_->GenerateVertex("INSIDE");
      	    vertex.setX(vertex.x() - ( shield_x_/2.+ steel_thickness_ + lead_thickness_/2.));
      	    // std::cout<<"lateral -"<<std::endl;
      	  }
      	}
      }

      else if (rand < perc_top_struct_vol_) { //TOP BEAM STRUCTURE
	G4double random = G4UniformRand();
	if (random <  perc_struc_x_vol_){
	  G4double rand_beam = int (4* G4UniformRand());
	  if (rand_beam == 0) {
	    vertex = struct_x_gen_->GenerateVertex("INSIDE");
	  }
	  else if (rand_beam == 1) {
	    vertex = struct_x_gen_->GenerateVertex("INSIDE");
	    vertex.setZ(vertex.z()-roof_z_separation_);
	  }
	  else if (rand_beam == 2) {
	    vertex = struct_x_gen_->GenerateVertex("INSIDE");
	    vertex.setZ(vertex.z()-(roof_z_separation_+lateral_z_separation_));
	  }
	  else if (rand_beam == 3) {
	    vertex = struct_x_gen_->GenerateVertex("INSIDE");
	    vertex.setZ(vertex.z()-(2*roof_z_separation_+lateral_z_separation_));
	  }
	}
	else {
	  if (G4UniformRand() < 0.5) {
	    vertex = struct_z_gen_->GenerateVertex("INSIDE");
	  }
	  else {
	    vertex = struct_z_gen_->GenerateVertex("INSIDE");
	    vertex.setX(vertex.x()+front_x_separation_);
	  }
	}
      }

      else{    //LATERAL BEAM STRUCTURE
	G4double rand_beam = int (8 * G4UniformRand());
	// std::cout<< "beam number "<<rand_beam<<std::endl; //0-7
	if (rand_beam == 0) {
	  //lat_1 (lat_beam_x,-beam_base_thickness_/2.,lateral_z_separation_/2.)
	  vertex = lat_beam_gen_->GenerateVertex("INSIDE");
	}
	else if (rand_beam ==1){
	  // //lat_2 (lat_beam_x,-beam_base_thickness_/2.,-lateral_z_separation_/2.)
	  vertex = lat_beam_gen_->GenerateVertex("INSIDE");
	  vertex.setZ(vertex.z() -lateral_z_separation_);
	}
	else if (rand_beam ==2){
	  // //lat_3 	(-lat_beam_x,-beam_base_thickness_/2.,lateral_z_separation_/2.)
	  vertex = lat_beam_gen_->GenerateVertex("INSIDE");
	  vertex.setX(vertex.x() -(shield_x_ + 2*steel_thickness_ + lead_thickness_ ));
	}
	else if (rand_beam ==3){
	  // //lat_4 (-lat_beam_x,-beam_base_thickness_/2.,-lateral_z_separation_/2.)
	  vertex = lat_beam_gen_->GenerateVertex("INSIDE");
	  vertex.setX(vertex.x() -(shield_x_ + 2*steel_thickness_ + lead_thickness_ ));
	  vertex.setZ(vertex.z() -lateral_z_separation_);
	}
	else if (rand_beam ==4){
	  // //lat_5 front_beam (-front_x_separation_/2.,-beam_base_thickness_/2.,front_beam_z)
	  vertex = front_beam_gen_->GenerateVertex("INSIDE");
	}
	else if (rand_beam ==5){
	  // //lat_6 front_beam (front_x_separation_/2.,-beam_base_thickness_/2.,front_beam_z)
	  vertex = front_beam_gen_->GenerateVertex("INSIDE");
	  vertex.setX(vertex.x() + front_x_separation_);
	}
	else if (rand_beam ==6){
	  // //lat_7 front_beam (-front_x_separation_/2.,-beam_base_thickness_/2.,-front_beam_z)
	  vertex = front_beam_gen_->GenerateVertex("INSIDE");
	  vertex.setZ(vertex.z() -(shield_z_+2*steel_thickness_+lead_thickness_));
	}
	else if (rand_beam ==7){
	  //lat_8 front_beam (front_x_separation_/2.,-beam_base_thickness_/2.,-front_beam_z)
	  vertex = front_beam_gen_->GenerateVertex("INSIDE");
	  vertex.setX(vertex.x() + front_x_separation_);
	  vertex.setZ(vertex.z() -(shield_z_+2*steel_thickness_+lead_thickness_));
	}
      }

    }
    else {
      G4Exception("[NextNewShielding]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }


} //end namespace nexus
