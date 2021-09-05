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

    in_rad_   (56.0 * cm),
    thickness_(12.0 * cm),

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

    G4double gate_zpos = GetELzCoord();
    length_ = gate_tp_distance_ + field_cage_length_;

    G4double offset = 1.* mm;

    // Dice Boards holes positions
    GenerateDBPositions();

    // ICS
    G4double ics_z_pos = gate_zpos + length_/2. - gate_tp_distance_;

    G4Tubs* ics_body = new G4Tubs("ICS", in_rad_, in_rad_ + thickness_,
                                  length_/2., 0.*deg, 360.*deg);
    G4SubtractionSolid* ics_solid;

    // Port holes
    G4double port_hole_rad = 9. * mm;
    G4double port_x = (in_rad_ + thickness_/2.)/sqrt(2.);
    G4double port_y = port_x;

    G4RotationMatrix* port_a_Rot = new G4RotationMatrix;
    port_a_Rot->rotateX( 90. * deg);
    port_a_Rot->rotateY(-45. * deg);

    G4RotationMatrix* port_b_Rot = new G4RotationMatrix;
    port_b_Rot->rotateX( 90. * deg);
    port_b_Rot->rotateY( 45. * deg);

    G4Tubs* port_hole_solid = new G4Tubs("PORT_HOLE", 0., port_hole_rad,
                                         (thickness_ + offset)/2., 0.*deg, 360.*deg);

    ics_solid = new G4SubtractionSolid("ICS", ics_body, port_hole_solid,
                port_a_Rot, G4ThreeVector(port_x, port_y, port_z_1a_-ics_z_pos));

    ics_solid = new G4SubtractionSolid("ICS", ics_solid, port_hole_solid,
                port_a_Rot, G4ThreeVector(port_x, port_y, port_z_2a_-ics_z_pos));

    ics_solid = new G4SubtractionSolid("ICS", ics_solid, port_hole_solid,
                port_b_Rot, G4ThreeVector(-port_x, port_y, port_z_1b_-ics_z_pos));

    ics_solid = new G4SubtractionSolid("ICS", ics_solid, port_hole_solid,
                port_b_Rot, G4ThreeVector(-port_x, port_y, port_z_2b_-ics_z_pos));

    // Upper holes




    G4LogicalVolume* ics_logic =
      new G4LogicalVolume(ics_solid,
        G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "ICS");

    new G4PVPlacement(0, G4ThreeVector(0., 0., ics_z_pos),
                      ics_logic, "ICS", mother_logic_, false, 0, false);


    ///// DB plugs placement
    G4double ics_tracking_zpos = length_/2. - tracking_length_/2.;
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


    // VERTEX GENERATORS   //////////
  }


  Next100Ics::~Next100Ics()
  {
  }


  G4ThreeVector Next100Ics::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

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

  void Next100Ics::SetPortZpositions(G4double port_positions[])
  {
    port_z_1a_ = port_positions[0];
    port_z_2a_ = port_positions[1];
    port_z_1b_ = port_positions[2];
    port_z_2b_ = port_positions[3];
  }

} //end namespace nexus
