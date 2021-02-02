// -----------------------------------------------------------------------------
// nexus | NextNewIcs.cc
//
// Inner copper shielding of the NEXT-WHITE detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextNewIcs.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "CylinderPointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Tubs.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <Randomize.hh>
#include <G4TransportationManager.hh>
#include <G4RotationMatrix.hh>

#include <CLHEP/Units/SystemOfUnits.h>

namespace nexus {

  using namespace CLHEP;

  NextNewIcs::NextNewIcs():
    BaseGeometry(),

    // Body dimensions
    body_inner_diam_ (50.62 * cm), // inner diameter of the barrel. The design value is 50.72 cm, slightly changed to avoid overlaps
    body_length_     (72.8 * cm),
    body_thickness_  (6.0 * cm),
    body_zpos_       (2.2 * cm),

    // Tracking plane tread dimensions
//    tracking_tread_diam_   (55. * cm),  // support_plate_tread_diam_ from TP + .5
    tracking_tread_diam_   (57.6 * cm),  // support_plate_tread_diam_ from TP + .5
    tracking_tread_length_ (84.1 * mm), // support_plate_tread_thickness_ + el_tot_zone_

    lat_nozzle_in_diam_ (5. * mm),                                  // lat_nozzle_in_diam; before 30 mm
    lat_nozzle_x_pos_   (body_inner_diam_/2. + body_thickness_/2.), // lat_cathode_nozzle_xpos;
    up_small_nozzle_in_diam_  ( 5. * mm),      // The upper central nozzle
    up_big_nozzle_in_diam_    (62. * mm),      // The upper anode & cathode nozzles
    up_nozzle_y_pos_    (lat_nozzle_x_pos_),
    center_nozzle_z_pos_ (25. *mm)   //  position of the nozzles (lateral and upper side) with respect to the center of the volume 

  {
    // Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry Next100.");
    msg_->DeclareProperty("ics_vis", visibility_, "ICS Visibility");
  }

  void NextNewIcs::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }

  void NextNewIcs::SetNozzlesZPosition(const G4double lat_nozzle_z_pos, const G4double up_nozzle_z_pos)
  {
    lat_nozzle_z_pos_ = lat_nozzle_z_pos;
    up_nozzle_z_pos_  = up_nozzle_z_pos;
  }

  void NextNewIcs::Construct()
  {
    ////// INNER COPPER SHIELDING BARREL  ///////////
    G4Tubs* ics_body_nh_solid =
      new G4Tubs("ICS_BODY", body_inner_diam_/2.,
                 body_inner_diam_/2. + body_thickness_,
                 body_length_/2., 0., twopi);

    G4Tubs* ics_tracking_tread_solid =
      new G4Tubs("ICS_TRACKING_TREAD", body_inner_diam_/2. - 2. * mm,
                 tracking_tread_diam_/2., tracking_tread_length_/2., 0., twopi);

    G4SubtractionSolid* ics_solid =
      new G4SubtractionSolid("ICS", ics_body_nh_solid, ics_tracking_tread_solid, 0,
                             G4ThreeVector(0., 0., body_length_/2. -tracking_tread_length_/2.));

    // LATERAL NOZZLES HOLES
    G4Tubs* lateral_nozzle_hole_solid =
      new G4Tubs("LAT_NOZZLE_HOLE", 0., lat_nozzle_in_diam_/2.,
                 body_thickness_/2. + 2.*mm, 0., twopi);

    // Rotate the nozzles holes
    G4RotationMatrix* roty = new G4RotationMatrix();
    roty->rotateY(pi/2.);

    // Lateral anode nozzle
    ics_solid = new G4SubtractionSolid("ICS", ics_solid, lateral_nozzle_hole_solid, roty,
   				                             G4ThreeVector(lat_nozzle_x_pos_, 0., center_nozzle_z_pos_ - lat_nozzle_z_pos_));
    // Lateral cathode nozzle
    ics_solid = new G4SubtractionSolid("ICS", ics_solid, lateral_nozzle_hole_solid, roty,
   				                             G4ThreeVector(lat_nozzle_x_pos_, 0., center_nozzle_z_pos_ + lat_nozzle_z_pos_));

    // UPPER NOZZLES HOLES
    G4Tubs* up_small_nozzle_hole_solid =
      new G4Tubs("UP_SMALL_NOZZLE_HOLE", 0., up_small_nozzle_in_diam_/2.,
                 body_thickness_/2. + 5. * mm, 0., twopi);

    G4Tubs* up_big_nozzle_hole_solid =
      new G4Tubs("UP_BIG_NOZZLE_HOLE", 0., up_big_nozzle_in_diam_/2.,
                 body_thickness_/2. + 5. * mm, 0., twopi);

    // Rotate the nozzles holes to vertical
    G4RotationMatrix* rotx = new G4RotationMatrix();
    rotx->rotateX(pi/2.);

    // Upper anode nozzle
    ics_solid = new G4SubtractionSolid("ICS", ics_solid, up_big_nozzle_hole_solid, rotx,
				                               G4ThreeVector( 0., up_nozzle_y_pos_, center_nozzle_z_pos_ -up_nozzle_z_pos_));
    // Upper central nozzle
    ics_solid = new G4SubtractionSolid("ICS", ics_solid, up_small_nozzle_hole_solid, rotx,
				                               G4ThreeVector( 0., up_nozzle_y_pos_, center_nozzle_z_pos_));
    // Upper cathode nozzle
    ics_solid = new G4SubtractionSolid("ICS", ics_solid, up_big_nozzle_hole_solid, rotx,
				                               G4ThreeVector( 0., up_nozzle_y_pos_, center_nozzle_z_pos_ +up_nozzle_z_pos_));


    G4LogicalVolume* ics_logic =
      new G4LogicalVolume(ics_solid,
     						          G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "ICS");

    new G4PVPlacement(0, G4ThreeVector(0.,0.,-body_zpos_), ics_logic, "ICS", mother_logic_, false, 0, false);


    // SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes copper_col = nexus::CopperBrown();
      copper_col.SetForceSolid(true);
      ics_logic->SetVisAttributes(copper_col);
    }
    else {
      ics_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // VERTEX GENERATORS   //////////
    body_gen_ = new CylinderPointSampler(body_inner_diam_/2.,
                                         body_length_-tracking_tread_length_,
                                         body_thickness_, 0.,
                                         G4ThreeVector(0., 0., -body_zpos_ - tracking_tread_length_/2.));

    G4double body_outer_diam = body_inner_diam_ + 2 * body_thickness_;
    tread_gen_ = new CylinderPointSampler(tracking_tread_diam_/2., tracking_tread_length_,
                                          (body_outer_diam - tracking_tread_diam_)/2., 0.,
                                          G4ThreeVector(0.,0., -body_zpos_ + body_length_/2. - tracking_tread_length_/2.));

    // Calculating some probs
    G4double body_vol = (body_length_-tracking_tread_length_) * pi *
      (pow(body_inner_diam_/2.+body_thickness_, 2) - pow(body_inner_diam_/2., 2));
    G4double tread_vol = tracking_tread_length_ * pi *
      (pow(body_inner_diam_/2.+body_thickness_, 2) - pow(tracking_tread_diam_/2., 2));
    G4double total_vol = body_vol + tread_vol;
    body_perc_ = body_vol/total_vol;
  }

  NextNewIcs::~NextNewIcs()
  {
    delete body_gen_;
    delete tread_gen_;
  }



  G4ThreeVector NextNewIcs::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "ICS") {
      G4double rand = G4UniformRand();

      //Generating in the body
      if (rand < body_perc_) {
        // As it is full of holes, let's get sure vertices are in the right volume
        G4VPhysicalVolume *VertexVolume;
        do {
          vertex = body_gen_->GenerateVertex("BODY_VOL");
          // To check its volume, one needs to rotate and shift the vertex
          // because the check is done using global coordinates
          G4ThreeVector glob_vtx(vertex);
          // First rotate, then shift
          glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
          glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
          VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
        } while (VertexVolume->GetName() != "ICS");
      }
      // Generating in the tread
      else {
        G4VPhysicalVolume *VertexVolume;
        do {
          vertex = tread_gen_->GenerateVertex("BODY_VOL");
          G4ThreeVector glob_vtx(vertex);
          glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
          glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
          VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
        } while (VertexVolume->GetName() != "ICS");
      }
    } else {
      G4Exception("[NextNewIcs]", "GenerateVertex()", FatalException,
		   "Unknown Region!");
    }
    return vertex;
  }

} //end namespace nexus
