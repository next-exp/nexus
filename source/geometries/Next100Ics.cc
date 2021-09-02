// ----------------------------------------------------------------------------
// nexus | Next100Ics.cc
//
// Inner copper shielding of the NEXT-100 detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100Ics.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "CylinderPointSampler.h"
#include "SpherePointSampler.h"
#include "BoxPointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4Cons.hh>
#include <G4Sphere.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  Next100Ics::Next100Ics():
    GeometryBase(),

    // Body dimensions
    body_in_rad_ (56.0  * cm),
    body_length_ (160.0 * cm),
    body_thickness_ (12.0 * cm),

    // Tracking plane dimensions  (thin version without substractions)
    tracking_length_ (10.0 * cm),

    //KDB plugs constructed here because the copper tracking plane is divided in two parts,
    // one hosted in the Next100Trackingplane class (support) and the proper shielding hosted here.
    plug_x_ (40. *mm),
    plug_y_ (4. *mm), //two union conectors
    plug_z_ (6. *mm),
    // Number of Dice Boards, DB columns
    DB_columns_ (11),
    num_DBs_ (107),

    visibility_ (0)
  {

    // Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/Next100/", "Control commands of geometry Next100.");
    msg_->DeclareProperty("ics_vis", visibility_, "ICS Visibility");

  }

  void Next100Ics::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }


  void Next100Ics::Construct()
  {

    // Dice Boards holes positions
    GenerateDBPositions();

    // ICS SOLIDS  ///////////
    // Body
    G4Tubs* ics_solid = new G4Tubs("ICS_BODY", body_in_rad_, body_in_rad_ + body_thickness_,
					body_length_/2., 0.*deg, 360.*deg);

    // Unions of parts
    G4double ics_tracking_zpos = body_length_/2. - tracking_length_/2.;

    G4LogicalVolume* ics_logic =
      new G4LogicalVolume(ics_solid,
			  G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "ICS");

    //this->SetLogicalVolume(ics_logic);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), ics_logic, "ICS", mother_logic_, false, 0, false);


    ///// DB plugs placement
    G4Box* plug_solid = new G4Box("DB_PLUG", plug_x_/2., plug_y_/2., plug_z_/2.);
    G4LogicalVolume* plug_logic = new G4LogicalVolume(plug_solid,  materials::PEEK(), "DB_PLUG");
     plug_posz_ = ics_tracking_zpos + tracking_length_/2. + plug_z_ ;
    G4ThreeVector pos;
    for (int i=0; i<num_DBs_; i++) {
      pos = DB_positions_[i];
      pos.setY(pos.y()- 10.*mm);
      pos.setZ(plug_posz_);
      new G4PVPlacement(0, pos, plug_logic,
    			"DB_PLUG", mother_logic_, false, i, false);
    }


    // SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes copper_col = nexus::CopperBrown();
      //copper_col.SetForceSolid(true);
      ics_logic->SetVisAttributes(copper_col);
      G4VisAttributes dirty_white_col =nexus::DirtyWhite();
      dirty_white_col.SetForceSolid(true);
      plug_logic->SetVisAttributes(dirty_white_col);
    }
    else {
      ics_logic->SetVisAttributes(G4VisAttributes::Invisible);
      plug_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }


    // // VERTEX GENERATORS   //////////
    // body_gen_ =
    //   new CylinderPointSampler(body_in_rad_, body_length_, body_thickness_, 0.);
    //
    // tracking_gen_ =
    //   new CylinderPointSampler(0.*cm, tracking_length_, tracking_orad_, 0.,
		// 			     G4ThreeVector(0., 0., ics_tracking_zpos));
    //
    // energy_cyl_gen_ =
    //   new CylinderPointSampler(body_in_rad_, energy_cyl_length_, energy_thickness_, 0.,  G4ThreeVector(0., 0., energy_cyl_zpos));
    //
    // energy_sph_gen_ = new SpherePointSampler(energy_orad_ - energy_thickness_, energy_thickness_, G4ThreeVector(0., 0., energy_sph_zpos_),
		// 			     0,	0., twopi, 180.*deg - energy_theta_, energy_theta_);
    //
    //
    // plug_gen_ = new BoxPointSampler(plug_x_, plug_y_, plug_z_,0.,
		// 		    G4ThreeVector(0.,0.,0.),0);
    //
    //
    // // Calculating some probs
    // G4double body_vol = ics_body_solid->GetCubicVolume();
    // G4double tracking_vol = ics_tracking_solid->GetCubicVolume();
    // G4double energy_cyl_vol = ics_energy_cyl_solid->GetCubicVolume();
    // G4double energy_sph_vol = ics_energy_sph_solid->GetCubicVolume();
    // G4double total_vol = body_vol + tracking_vol + energy_cyl_vol + energy_sph_vol;
    //
    // perc_body_vol_ = body_vol / total_vol;
    // perc_tracking_vol_ = (body_vol + tracking_vol) / total_vol;
    // perc_energy_cyl_vol_ = (body_vol + tracking_vol + energy_cyl_vol) / total_vol;
  }



  Next100Ics::~Next100Ics()
  {
    delete body_gen_;
    delete plug_gen_;
  }



  G4ThreeVector Next100Ics::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

  //   // Vertex in the whole ICS volume
  //   if (region == "ICS") {
  //
  //     G4double rand = G4UniformRand();
  //
  //     if (rand < perc_body_vol_){
	// vertex = body_gen_->GenerateVertex("BODY_VOL");        // Body
  //     }
  //
  //
  //     else if  (rand < perc_tracking_vol_){
	// G4VPhysicalVolume *VertexVolume;
	// do {
	//   vertex = tracking_gen_->GenerateVertex("BODY_VOL");    // Tracking plane
	//   // To check its volume, one needs to rotate and shift the vertex
	//   // because the check is done using global coordinates
	//   G4ThreeVector glob_vtx(vertex);
	//   // First rotate, then shift
	//   glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	//   glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	//   VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	// } while (VertexVolume->GetName() != "ICS");
  //     }
  //
  //     else if  (rand < perc_energy_cyl_vol_)
	// vertex = energy_cyl_gen_->GenerateVertex("BODY_VOL");  // Energy plane, cylindric section
  //
  //     else {
	// G4VPhysicalVolume *VertexVolume;
	// do {
	//   vertex = energy_sph_gen_->GenerateVertex("VOLUME");     // Energy plane, spherical section
	//   // To check its volume, one needs to rotate and shift the vertex
	// // because the check is done using global coordinates
	// G4ThreeVector glob_vtx(vertex);
	// // First rotate, then shift
	// glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
	// glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
	//   VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
	// } while (VertexVolume->GetName() != "ICS");
  //     }
  //   }
  //
  //   // PIGGY TAIL PLUG
  //   else if (region == "DB_PLUG") {
  //     G4ThreeVector ini_vertex = plug_gen_->GenerateVertex("INSIDE");
  //     G4double rand = num_DBs_ * G4UniformRand();
  //     G4ThreeVector db_pos = DB_positions_[int(rand)];
  //     vertex = ini_vertex + db_pos;
  //     vertex.setY(vertex.y()- 10.*mm);
  //     vertex.setZ(vertex.z() + plug_posz_);
  //   }
  //
  //
  //   else {
  //     G4Exception("[Next100Ics]", "GenerateVertex()", FatalException,
	// 	  "Unknown vertex generation region!");
  //   }


    return vertex;
  }


  void Next100Ics::GenerateDBPositions()
  {
    /// Function that computes and stores the XY positions of Dice Boards

    G4int num_rows[] = {6, 9, 10, 11, 12, 11, 12, 11, 10, 9, 6};
    G4int total_positions = 0;

    // Separation between consecutive columns / rows
    G4double x_step = (45. + 35.191) * mm;
    G4double y_step = (45. + 36.718) * mm;

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
	//G4cout << G4endl << position;
      }
    }

    // Checking
    if (total_positions != num_DBs_) {
      G4cout << "\n\nERROR: Number of DBs doesn't match with number of positions calculated\n";
      exit(0);
    }


  }


} //end namespace nexus
