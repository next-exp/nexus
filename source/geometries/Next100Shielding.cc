// ----------------------------------------------------------------------------
// nexus | Next100Shielding.cc
//
// Lead castle placed at the LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100Shielding.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "BoxPointSampler.h"

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

  Next100Shielding::Next100Shielding():
    GeometryBase{},

    // Shielding internal dimensions
    shield_x_ {158.  * cm},
    shield_y_ {166.6 * cm},
    shield_z_ {259.4 * cm},

    //Steel Structure
    beam_thickness_1     {4.   * mm},
    beam_thickness_2     {6.   * mm},
    lateral_z_separation_{1010.* mm}, //distance between the two lateral beams
    roof_z_separation_   {760. * mm}, //distance between x beams
    front_x_separation_  {156. * mm}, //distance between the two front beams

    // Box thickness
    lead_thickness_ {20.* cm},
    steel_thickness_{2. * mm},

    // Pedestal
    pedestal_x_        {1518. * mm},
    pedestal_top_x_    {1384. * mm},
    support_beam_dist_ {1027. * mm},
    support_front_dist_{732.  * mm},
    pedestal_lateral_beam_thickness_ {8. * mm},
    pedestal_front_beam_thickness_   {20.* mm},
    pedestal_roof_thickness_ {75. * mm},
    pedestal_lateral_length_ {2511. * mm},

    // Seals
    bubble_seal_thickness_ {1. * mm},
    edpm_seal_thickness_   {1. * mm},

    visibility_ {0},
    verbosity_{false}

  {
    // The shielding is made of two boxes.
    // The external one is made of lead and the internal one is made of a mix of stainless steel AISI-316Ti.
    // Besides, inside the lead we have the castle steel beams (steel S-275) that support the lead.
    // This steel beam structure is composed by tree regions: the lateral beams, the roof, and the
    // beam structure above the roof.

    msg_ = new G4GenericMessenger(this, "/Geometry/Next100/", "Control commands of geometry Next100.");

    msg_->DeclareProperty("shielding_vis", visibility_, "Shielding Visibility");
    msg_->DeclareProperty("shielding_verbosity", verbosity_, "Verbosity");

    // Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  }


  void Next100Shielding::Construct()
  {

    // LEAD BOX   ///////////
    lead_x_ = shield_x_ + 2. * steel_thickness_ + 2. * lead_thickness_;
    lead_y_ = shield_y_ + 2. * steel_thickness_ + 2. * lead_thickness_ + beam_thickness_2;
    lead_z_ = shield_z_ + 2. * steel_thickness_ + 2. * lead_thickness_;

    G4Box* lead_box_solid = new G4Box("LEAD_BOX", lead_x_/2., lead_y_/2., lead_z_/2.);

    G4LogicalVolume* lead_box_logic = new G4LogicalVolume(lead_box_solid,
							  G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb"),
							  "LEAD_BOX");
    this->SetLogicalVolume(lead_box_logic);

    //STEEL BEAM STRUCTURE
    // auxiliar positions used in translations
    G4double lat_beam_x   = shield_x_/2. + steel_thickness_ + lead_thickness_/2.;
    G4double front_beam_z = shield_z_/2. + steel_thickness_ + lead_thickness_/2.;
    G4double top_beam_y   = (shield_y_-beam_thickness_2)/2. + steel_thickness_ + beam_thickness_2
                            + lead_thickness_/2.;
    G4double lat_beam_y   = -(lead_thickness_ + beam_thickness_2)/2.;
    G4double roof_y       = (shield_y_-beam_thickness_2)/2. + steel_thickness_ + beam_thickness_2/2.;

    G4Box* roof_beam = new G4Box("STRUCT_BEAM", lead_x_/2., beam_thickness_2/2., lead_z_/2.);
    G4Box* aux_box   = new G4Box("AUX_box", shield_x_/2. + steel_thickness_, beam_thickness_2,
                                 shield_z_/2.+ steel_thickness_);
    G4SubtractionSolid* roof_beam_solid = new G4SubtractionSolid("STRUCT_BEAM", roof_beam, aux_box);

    // vertical bottom beams
    G4Box* lat_beam_solid  = new G4Box("STRUCT_BEAM",
                                       lead_thickness_/2.,
                                       (shield_y_ + 2. * steel_thickness_+ lead_thickness_)/2.,
                                       beam_thickness_1/2.);

    G4Box* front_beam_solid = new G4Box("STRUCT_BEAM",
                                        beam_thickness_2/2.,
                                        (shield_y_ + 2. * steel_thickness_+ lead_thickness_)/2.,
                                        lead_thickness_/2.);

    // horizontal top beams
    G4Box* top_xbeam_solid = new G4Box("STRUCT_BEAM",
                                       (shield_x_ + 2.*lead_thickness_ + 2.*steel_thickness_)/2.,
                                       lead_thickness_/2.,
                                       beam_thickness_1/2.);

    G4Box* top_zbeam_solid = new G4Box("STRUCT_BEAM",
                                       beam_thickness_2/2.,
                                       lead_thickness_/2.,
                                       (shield_z_ + 2.*lead_thickness_ + 2.*steel_thickness_)/2.);

    // top beams
    // x beams
    G4UnionSolid* struct_solid =
      new G4UnionSolid("STEEL_BEAM_STRUCTURE_top1", top_xbeam_solid, top_xbeam_solid, 0,
                       G4ThreeVector(0., 0., -roof_z_separation_));

    struct_solid =
      new G4UnionSolid("STEEL_BEAM_STRUCTURE_top2", struct_solid, top_xbeam_solid, 0,
                       G4ThreeVector(0., 0., -(roof_z_separation_ + lateral_z_separation_)));

    struct_solid =
      new G4UnionSolid("STEEL_BEAM_STRUCTURE_top3", struct_solid, top_xbeam_solid, 0,
                       G4ThreeVector(0., 0., -(2*roof_z_separation_ + lateral_z_separation_)));
    // z beams
    struct_solid =
      new G4UnionSolid("STEEL_BEAM_STRUCTURE_top4", struct_solid, top_zbeam_solid, 0,
      G4ThreeVector(-front_x_separation_/2., 0., -(roof_z_separation_+lateral_z_separation_/2.)));

    struct_solid =
      new G4UnionSolid("STEEL_BEAM_STRUCTURE_top5", struct_solid, top_zbeam_solid, 0,
      G4ThreeVector(front_x_separation_/2., 0., -(roof_z_separation_+lateral_z_separation_/2.)));


    G4LogicalVolume* roof_logic      = new G4LogicalVolume(roof_beam_solid,
                                           materials::Steel(), "STEEL_BEAM_ROOF");

    G4LogicalVolume* lat_beam_logic  = new G4LogicalVolume(lat_beam_solid,
                                           materials::Steel(), "STEEL_BEAM_STRUCTURE_LAT");

    G4LogicalVolume* front_beam_logic = new G4LogicalVolume(front_beam_solid,
                                            materials::Steel(), "STEEL_BEAM_STRUCTURE_FRONT");

     G4LogicalVolume* struct_logic   = new G4LogicalVolume(struct_solid,
                                           materials::Steel(), "STEEL_BEAM_STRUCTURE_TOP");

    new G4PVPlacement(0, G4ThreeVector(0., top_beam_y, roof_z_separation_ + lateral_z_separation_/2.),
                      struct_logic, "STEEL_BEAM_STRUCTURE_top", lead_box_logic, false, 0, false);

    new G4PVPlacement(0, G4ThreeVector(0., roof_y, 0.),
                      roof_logic, "STEEL_BEAM_STRUCTURE_roof", lead_box_logic, false, 0, false);

    // lateral beams
    new G4PVPlacement(0, G4ThreeVector(lat_beam_x, lat_beam_y, lateral_z_separation_/2.),
              		    lat_beam_logic, "STEEL_BEAM_STRUCTURE_lat1", lead_box_logic, false, 0, false);

    new G4PVPlacement(0, G4ThreeVector(lat_beam_x, lat_beam_y, -lateral_z_separation_/2.),
                      lat_beam_logic, "STEEL_BEAM_STRUCTURE_lat2", lead_box_logic, false, 0, false);

    new G4PVPlacement(0, G4ThreeVector(-lat_beam_x, lat_beam_y, lateral_z_separation_/2.),
                      lat_beam_logic, "STEEL_BEAM_STRUCTURE_lat3", lead_box_logic, false, 0, false);

    new G4PVPlacement(0, G4ThreeVector(-lat_beam_x, lat_beam_y, -lateral_z_separation_/2.),
                      lat_beam_logic, "STEEL_BEAM_STRUCTURE_lat4", lead_box_logic, false, 0, false);

    // front beams
    new G4PVPlacement(0, G4ThreeVector(-front_x_separation_/2., lat_beam_y, front_beam_z),
                  front_beam_logic, "STEEL_BEAM_STRUCTURE_front1", lead_box_logic, false, 0, false);

    new G4PVPlacement(0, G4ThreeVector(front_x_separation_/2., lat_beam_y, front_beam_z),
                  front_beam_logic, "STEEL_BEAM_STRUCTURE_front2", lead_box_logic, false, 0, false);

    new G4PVPlacement(0, G4ThreeVector(-front_x_separation_/2., lat_beam_y, -front_beam_z),
                  front_beam_logic, "STEEL_BEAM_STRUCTURE_front3", lead_box_logic, false, 0, false);

    new G4PVPlacement(0, G4ThreeVector(front_x_separation_/2., lat_beam_y, -front_beam_z),
                  front_beam_logic,"STEEL_BEAM_STRUCTURE_front4", lead_box_logic, false, 0, false);


    // STEEL BOX   ///////////
    G4double steel_x = shield_x_ + 2. * steel_thickness_;
    G4double steel_y = shield_y_ + 2. * steel_thickness_;
    G4double steel_z = shield_z_ + 2. * steel_thickness_;

    G4Box* steel_box_solid = new G4Box("STEEL_BOX", steel_x/2., steel_y/2., steel_z/2.);

    G4LogicalVolume* steel_box_logic = new G4LogicalVolume(steel_box_solid,
                                            materials::Steel(), "STEEL_BOX");

    new G4PVPlacement(0, G4ThreeVector(0., -beam_thickness_2/2., 0.),
                      steel_box_logic, "STEEL_BOX", lead_box_logic, false, 0);

    // AIR INSIDE
    G4Box* air_box_solid =
    new G4Box("INNER_AIR", shield_x_/2., shield_y_/2. + steel_thickness_/2., shield_z_/2.);

    air_box_logic_ = new G4LogicalVolume(air_box_solid,
                         G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "INNER_AIR");

    ////Limit the uStepMax=Maximum step length, uTrakMax=Maximum total track length,
    //uTimeMax= Maximum global time for a track, uEkinMin= Minimum remaining kinetic energy for a track
    //uRangMin=         Minimum remaining range for a track
    air_box_logic_->SetUserLimits(new G4UserLimits( DBL_MAX, DBL_MAX, DBL_MAX,100.*keV,0.));
    air_box_logic_->SetVisAttributes(G4VisAttributes::Invisible);

    new G4PVPlacement(0, G4ThreeVector(0., -steel_thickness_/2., 0.),
                      air_box_logic_, "INNER_AIR", steel_box_logic, false, 0);


    // PEDESTAL BEAMS
    // there are two kind of beams: the support T-shaped beams (support), plain "front" beams (x-direction), plain
    // "lateral" beams (z-direction) and the roof. The roof completes the semi-T shaped "front" and "lateral" beams.
    // The T-shaped beams are composed by two parts: the vertical part called "support-bottom"
    // and the horizantal top part called "support-top".
    G4double pedestal_support_bottom_thickness = 10.  * mm;
    G4double pedestal_support_top_thickness    = 15.  * mm;
    G4double pedestal_support_top_length       = 150. * mm;
    G4Box* pedestal_support_beam_bottom = new G4Box("PEDESTAL_SUPPORT_BEAM_BOTTOM",
                                                    pedestal_x_/2.,
                                                    lead_thickness_/2.,
                                                    pedestal_support_bottom_thickness/2.);

    G4Box* pedestal_support_beam_top = new G4Box("PEDESTAL_SUPPORT_BEAM_TOP",
                                                 pedestal_top_x_/2.,
                                                 pedestal_support_top_thickness/2.,
                                                 pedestal_support_top_length/2.);

    G4Box* pedestal_beam_front = new G4Box("PEDESTAL_BEAM_FRONT",
                                           pedestal_x_/2.,
                                           lead_thickness_/2.,
                                           pedestal_front_beam_thickness_/2.);

    G4Box* pedestal_beam_lateral = new G4Box("PEDESTAL_BEAM_LATERAL",
                                             pedestal_lateral_beam_thickness_/2.,
                                             lead_thickness_/2.,
                                             pedestal_lateral_length_/2.);

    G4Box* pedestal_roof_ =
      new G4Box("PEDESTAL_ROOF", pedestal_x_/2. + pedestal_lateral_beam_thickness_,
                                 pedestal_lateral_beam_thickness_/2., pedestal_lateral_length_/2.);

    G4Box* ped_aux_box    =
      new G4Box("AUX_box", pedestal_x_/2. + pedestal_lateral_beam_thickness_ - pedestal_roof_thickness_,
                           pedestal_lateral_beam_thickness_,
                           pedestal_lateral_length_/2.-pedestal_roof_thickness_);

    G4SubtractionSolid* pedestal_roof =
      new G4SubtractionSolid("PEDESTAL_ROOF", pedestal_roof_, ped_aux_box);

    G4LogicalVolume* pedestal_support_beam_bottom_logic =
      new G4LogicalVolume(pedestal_support_beam_bottom,
                          materials::Steel316Ti(), "PEDESTAL_SUPPORT_BOTTOM");

    G4LogicalVolume* pedestal_support_beam_top_logic =
      new G4LogicalVolume(pedestal_support_beam_top,
                          materials::Steel316Ti(), "PEDESTAL_SUPPORT_TOP");

    G4LogicalVolume* pedestal_beam_front_logic =
      new G4LogicalVolume(pedestal_beam_front,
                          materials::Steel316Ti(), "PEDESTAL_BEAM_FRONT");

    G4LogicalVolume* pedestal_beam_lateral_logic =
      new G4LogicalVolume(pedestal_beam_lateral,
                          materials::Steel316Ti(), "PEDESTAL_BEAM_LATERAL");

    G4LogicalVolume* pedestal_roof_logic =
      new G4LogicalVolume(pedestal_roof,
                          materials::Steel316Ti(), "PEDESTAL_ROOF");

    G4double pedestal_x_pos = pedestal_x_/2. + pedestal_lateral_beam_thickness_/2.;
    G4double pedestal_y_pos = -lead_y_/2. + lead_thickness_/2.;
    G4double pedestal_top_y_pos  = -(shield_y_/2. + steel_thickness_/2.)
                                   + pedestal_support_top_thickness/2.;  //caution: it is refered to air-box
    G4double pedestal_roof_y_pos = -(shield_y_/2. + steel_thickness_/2.)
                                   + pedestal_lateral_beam_thickness_/2.; //caution: it is refered to air-box


    new G4PVPlacement(0, G4ThreeVector(0., pedestal_y_pos, support_beam_dist_/2.),
                      pedestal_support_beam_bottom_logic,
                      "PEDESTAL_SUPPORT_BEAM_BOTTOM_1", lead_box_logic, false, 0);

    new G4PVPlacement(0, G4ThreeVector(0., pedestal_y_pos, -support_beam_dist_/2.),
                      pedestal_support_beam_bottom_logic,
                      "PEDESTAL_SUPPORT_BEAM_BOTTOM_2", lead_box_logic, false, 0);

    new G4PVPlacement(0, G4ThreeVector(0., pedestal_top_y_pos, support_beam_dist_/2.),
                      pedestal_support_beam_top_logic,
                      "PEDESTAL_SUPPORT_BEAM_TOP_1", air_box_logic_, false, 0);

    new G4PVPlacement(0, G4ThreeVector(0., pedestal_top_y_pos, -support_beam_dist_/2.),
                      pedestal_support_beam_top_logic,
                      "PEDESTAL_SUPPORT_BEAM_TOP_2", air_box_logic_, false, 0);

    new G4PVPlacement(0, G4ThreeVector(0., pedestal_y_pos, support_beam_dist_/2. + support_front_dist_),
                      pedestal_beam_front_logic,
                      "PEDESTAL_BEAM_FRONT_1", lead_box_logic, false, 0);

    new G4PVPlacement(0, G4ThreeVector(0., pedestal_y_pos, -support_beam_dist_/2. - support_front_dist_),
                      pedestal_beam_front_logic,
                      "PEDESTAL_BEAM_FRONT_2", lead_box_logic, false, 0);

    new G4PVPlacement(0, G4ThreeVector(pedestal_x_pos, pedestal_y_pos, 0.),
                      pedestal_beam_lateral_logic, "PEDESTAL_BEAM_LAT_1", lead_box_logic, false, 0);

    new G4PVPlacement(0, G4ThreeVector(-pedestal_x_pos, pedestal_y_pos, 0.),
                      pedestal_beam_lateral_logic, "PEDESTAL_BEAM_LAT_2", lead_box_logic, false, 0);

    new G4PVPlacement(0, G4ThreeVector(0., pedestal_roof_y_pos, 0.),
                      pedestal_roof_logic, "PEDESTAL_ROOF", air_box_logic_, false, 0);

    // SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes dark_grey_col = nexus::DarkGrey();
      lead_box_logic->SetVisAttributes(dark_grey_col);
      G4VisAttributes grey_col = nexus::LightGrey();
      steel_box_logic->SetVisAttributes(grey_col);
      G4VisAttributes antiox_col = nexus::BloodRed();
      //  antiox.SetForceSolid(true);
      roof_logic      ->SetVisAttributes(antiox_col);
      lat_beam_logic  ->SetVisAttributes(antiox_col);
      front_beam_logic->SetVisAttributes(antiox_col);
      struct_logic    ->SetVisAttributes(antiox_col);
    }
    else {
      lead_box_logic  ->SetVisAttributes(G4VisAttributes::Invisible);
      steel_box_logic ->SetVisAttributes(G4VisAttributes::Invisible);
      roof_logic      ->SetVisAttributes(G4VisAttributes::Invisible);
      struct_logic    ->SetVisAttributes(G4VisAttributes::Invisible);
      lat_beam_logic  ->SetVisAttributes(G4VisAttributes::Invisible);
      front_beam_logic->SetVisAttributes(G4VisAttributes::Invisible);

      pedestal_support_beam_bottom_logic->SetVisAttributes(G4VisAttributes::Invisible);
      pedestal_support_beam_top_logic   ->SetVisAttributes(G4VisAttributes::Invisible);
      pedestal_beam_front_logic         ->SetVisAttributes(G4VisAttributes::Invisible);
      pedestal_beam_lateral_logic       ->SetVisAttributes(G4VisAttributes::Invisible);
      pedestal_roof_logic               ->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // Creating the vertex generators   //////////
    //lead_gen_  = new BoxPointSampler(steel_x, steel_y, steel_z, lead_thickness_, G4ThreeVector(0.,0.,0.), 0);
    // Only shooting from the innest 5 cm.
    lead_gen_  = new BoxPointSampler(steel_x, steel_y, steel_z, 5.*cm,
                                     G4ThreeVector(0., 0., 0.), 0);

    G4double ext_offset = 1. * cm;
    external_gen_ =
      new BoxPointSampler(lead_x_ + ext_offset, lead_y_ + ext_offset, lead_z_ + ext_offset, 1. * mm,
                          G4ThreeVector(0., 0., 0.), 0);

    steel_gen_ = new BoxPointSampler(shield_x_, shield_y_, shield_z_, steel_thickness_,
                                     G4ThreeVector(0., -beam_thickness_1/2., 0.), 0);

    G4double inn_offset = .5 * cm;
    inner_air_gen_ =
      new BoxPointSampler(shield_x_ - inn_offset, shield_y_ - inn_offset, shield_z_ - inn_offset, 1. * mm,
                          G4ThreeVector(0., -beam_thickness_1/2., 0.), 0);

    // STEEL STRUCTURE GENERATORS
    lat_roof_gen_ =
      new BoxPointSampler(lead_thickness_, beam_thickness_2, shield_z_ + 2.*steel_thickness_, 0.,
                          G4ThreeVector(0., roof_y, 0.), 0);

    front_roof_gen_ =
      new BoxPointSampler(lead_x_, beam_thickness_2, lead_thickness_, 0.,
                          G4ThreeVector(0., roof_y, 0.), 0);

    struct_x_gen_ =
      new BoxPointSampler(shield_x_ + 2.*lead_thickness_ + 2.*steel_thickness_, lead_thickness_,
                          beam_thickness_1, 0.,
                          G4ThreeVector(0., top_beam_y, roof_z_separation_+lateral_z_separation_/2.), 0);

    struct_z_gen_ =
      new BoxPointSampler(beam_thickness_2, lead_thickness_,
                          shield_z_ + 2.*lead_thickness_ + 2.*steel_thickness_, 0.,
                          G4ThreeVector(-front_x_separation_/2., top_beam_y, 0.), 0);

    lat_beam_gen_ =
      new BoxPointSampler(lead_thickness_, shield_y_ + 2. * steel_thickness_+ lead_thickness_,
                          beam_thickness_1, 0.,
                          G4ThreeVector(lat_beam_x, -lead_thickness_/2., lateral_z_separation_/2.), 0);

    front_beam_gen_ =
      new BoxPointSampler(beam_thickness_2, shield_y_ + 2. * steel_thickness_+ lead_thickness_,
                          lead_thickness_, 0.,
                          G4ThreeVector(-front_x_separation_/2., -lead_thickness_/2., front_beam_z), 0);

    // Compute relative volumes
    G4double roof_vol       = roof_beam_solid->GetCubicVolume();
    G4double struct_top_vol = struct_solid   ->GetCubicVolume();
    G4double lateral_vol    = lat_beam_solid ->GetCubicVolume();
    G4double total_vol      = roof_vol + struct_top_vol + (8*lateral_vol);

    perc_roof_vol_       = roof_vol/total_vol;
    perc_front_roof_vol_ = 2*(lead_x_*beam_thickness_2*lead_thickness_)/roof_vol;
    perc_top_struct_vol_ = struct_top_vol /total_vol;

    G4double struc_beam_x_vol = (shield_x_ + 2.*lead_thickness_
                                + 2.*steel_thickness_)*lead_thickness_*beam_thickness_1;
    perc_struc_x_vol_    = 4*struc_beam_x_vol/struct_top_vol;


    // PEDESTAL GENERATORS
    ped_support_bottom_gen_ =
      new BoxPointSampler(pedestal_x_, lead_thickness_, pedestal_support_bottom_thickness, 0.,
                          G4ThreeVector(0., pedestal_y_pos, support_beam_dist_/2.), 0);

    G4double ped_gen_y_ = -lead_y_/2. + lead_thickness_ + pedestal_support_top_thickness/2.;
    ped_support_top_gen_ =
      new BoxPointSampler(pedestal_top_x_, pedestal_support_top_thickness, pedestal_support_top_length, 0.,
                          G4ThreeVector(0., ped_gen_y_, support_beam_dist_/2.), 0);

    ped_front_gen_ =
      new BoxPointSampler(pedestal_x_, lead_thickness_, pedestal_front_beam_thickness_, 0.,
                          G4ThreeVector(0., pedestal_y_pos, support_beam_dist_/2. + support_front_dist_), 0);

    ped_lateral_gen_ =
      new BoxPointSampler(pedestal_lateral_beam_thickness_, lead_thickness_, pedestal_lateral_length_, 0.,
                          G4ThreeVector(pedestal_x_pos, pedestal_y_pos, 0.), 0);

    // pedestal roof
    G4double ped_roof_gen_x = pedestal_top_x_/2. + pedestal_roof_thickness_/2.;
    G4double ped_roof_gen_y = -lead_y_/2. + lead_thickness_ + pedestal_lateral_beam_thickness_/2.;
    G4double ped_roof_gen_z = pedestal_lateral_length_/2. - pedestal_roof_thickness_/2.;
    ped_roof_lat_gen_ =
      new BoxPointSampler(pedestal_roof_thickness_, pedestal_lateral_beam_thickness_,
                          pedestal_lateral_length_, 0.,
                          G4ThreeVector(ped_roof_gen_x, ped_roof_gen_y, 0.), 0);

    ped_roof_front_gen_ =
      new BoxPointSampler(pedestal_top_x_, pedestal_lateral_beam_thickness_,
                          pedestal_roof_thickness_, 0.,
                          G4ThreeVector(0., ped_roof_gen_y, ped_roof_gen_z), 0);
    // Compute relative volumes
    G4double ped_support_bottom_vol = pedestal_support_beam_bottom->GetCubicVolume();
    G4double ped_support_top_vol    = pedestal_support_beam_top   ->GetCubicVolume();
    G4double ped_front_vol          = pedestal_beam_front         ->GetCubicVolume();
    G4double ped_lateral_vol        = pedestal_beam_lateral       ->GetCubicVolume();
    G4double ped_roof_vol           = pedestal_roof               ->GetCubicVolume();

    G4double ped_total_vol = ped_roof_vol + 2*(ped_support_bottom_vol + ped_support_top_vol
                           + ped_front_vol + ped_lateral_vol);

    perc_ped_bottom_vol_ = 2*ped_support_bottom_vol/ped_total_vol;
    perc_ped_top_vol_    = 2*ped_support_top_vol   /ped_total_vol;
    perc_ped_front_vol_  = 2*ped_front_vol         /ped_total_vol;
    perc_ped_lateral_vol_= 2*ped_lateral_vol       /ped_total_vol;
    perc_ped_roof_vol_   = ped_roof_vol            /ped_total_vol;


    // BUBBLE SEAL
    bubble_seal_front_gen_ =
      new BoxPointSampler(pedestal_x_ + 2*pedestal_lateral_beam_thickness_, bubble_seal_thickness_,
                          bubble_seal_thickness_, 0.,
                          G4ThreeVector(0., ped_gen_y_, 0.), 0);

    bubble_seal_lateral_gen_ =
      new BoxPointSampler(bubble_seal_thickness_, bubble_seal_thickness_, pedestal_lateral_length_, 0.,
                          G4ThreeVector(0., ped_gen_y_, 0.), 0);

    G4double bubble_front_vol   = 2*(pedestal_x_ + 2*pedestal_lateral_beam_thickness_)
                                   *(bubble_seal_thickness_)*(bubble_seal_thickness_);
    G4double bubble_lateral_vol = 2*(bubble_seal_thickness_)
                                   *(bubble_seal_thickness_)*(pedestal_lateral_length_);

    perc_bubble_front_vol_  = bubble_front_vol  /(bubble_front_vol + bubble_lateral_vol);
    perc_buble_lateral_vol_ = bubble_lateral_vol/(bubble_front_vol + bubble_lateral_vol);

    // EDPM SEAL
    edpm_seal_front_gen_ =
      new BoxPointSampler(edpm_seal_thickness_, shield_y_, edpm_seal_thickness_, 0.,
                          G4ThreeVector(0., -(beam_thickness_2+steel_thickness_)/2., 0.), 0);

    edpm_seal_lateral_gen_ =
      new BoxPointSampler(edpm_seal_thickness_, edpm_seal_thickness_, shield_z_, 0.,
                          G4ThreeVector(0., -(beam_thickness_2+steel_thickness_)/2., 0.), 0);

    G4double edpm_front_vol   = 2*edpm_seal_thickness_ * shield_y_ * edpm_seal_thickness_;
    G4double edpm_lateral_vol =   edpm_seal_thickness_ * edpm_seal_thickness_ * shield_z_;

    perc_edpm_front_vol_   = edpm_front_vol  /(edpm_front_vol + edpm_lateral_vol);
    perc_edpm_lateral_vol_ = edpm_lateral_vol/(edpm_front_vol + edpm_lateral_vol);


    if (verbosity_){
      std::cout<<"STEEL STRUCTURE VOLUME (m3)" <<std::endl;
      std::cout<<"ROOF BEAM     "<< roof_vol      /1.e9 <<std::endl;
      std::cout<<"TOP STRUCT    "<< struct_top_vol/1.e9 <<std::endl;
      std::cout<<"LATERAL BEAMS "<< 8*lateral_vol /1.e9 <<std::endl;
      std::cout<<"TOTAL         "<< total_vol     /1.e9 <<std::endl;

      G4double inner_vol = air_box_solid  ->GetCubicVolume();
      G4double steel_vol = steel_box_solid->GetCubicVolume() - inner_vol;
      G4double lead_vol  = lead_box_solid ->GetCubicVolume() - inner_vol - steel_vol;

      std::cout<<"INNER AIR VOLUME   (m3) "<< inner_vol/1.e9 <<std::endl;
      std::cout<<"INNER STEEL VOLUME (m3) "<< steel_vol/1.e9 <<std::endl;
      std::cout<<"LEAD VOLUME        (m3) "<< lead_vol /1.e9 <<std::endl;

      std::cout << "PEDESTAL GENERATOR PERCENTS" << std::endl;
      std::cout << "BOTTOM " << perc_ped_bottom_vol_ * 100 << std::endl;
      std::cout << "TOP "    << perc_ped_top_vol_    * 100 << std::endl;
      std::cout << "FRONT "  << perc_ped_front_vol_  * 100 << std::endl;
      std::cout << "LATERAL "<< perc_ped_lateral_vol_* 100 << std::endl;
      std::cout << "ROOF "   << perc_ped_roof_vol_   * 100 << std::endl;
    }
  }



  Next100Shielding::~Next100Shielding()
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
    delete ped_support_bottom_gen_;
    delete ped_support_top_gen_;
    delete ped_front_gen_;
    delete ped_lateral_gen_;
    delete ped_roof_lat_gen_;
    delete ped_roof_front_gen_;
    delete bubble_seal_front_gen_;
    delete bubble_seal_lateral_gen_;
    delete edpm_seal_front_gen_;
    delete edpm_seal_lateral_gen_;
  }

  G4LogicalVolume* Next100Shielding::GetAirLogicalVolume() const
  {
    return air_box_logic_;
  }

  G4ThreeVector Next100Shielding::GetDimensions() const
  {
    return G4ThreeVector(lead_x_, lead_y_, lead_z_);
  }

  G4ThreeVector Next100Shielding::GetAirDisplacement() const
  {
    return G4ThreeVector(0., -(steel_thickness_ + beam_thickness_2)/2., 0.);
  }

  G4ThreeVector Next100Shielding::GenerateVertex(const G4String& region) const
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
      G4VPhysicalVolume *VertexVolume;
      do {
          vertex = steel_gen_->GenerateVertex("WHOLE_VOL");

          G4ThreeVector glob_vtx(vertex);
          glob_vtx.rotate(pi, G4ThreeVector(0., 1., 0.));
          glob_vtx = glob_vtx + G4ThreeVector(0, 0, GetELzCoord());
          VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != "STEEL_BOX");
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
            vertex = front_roof_gen_->GenerateVertex("INSIDE");
            if (G4UniformRand() < 0.5) {
              vertex.setZ(vertex.z() + (shield_z_/2.+steel_thickness_+lead_thickness_/2.));
            }
            else{
              vertex.setZ(vertex.z() - (shield_z_/2.+steel_thickness_+lead_thickness_/2.));
            }
        	}
        	else{
              vertex = lat_roof_gen_->GenerateVertex("INSIDE");
          	  if (G4UniformRand() < 0.5) {
          	    vertex.setX(vertex.x() + (shield_x_/2.+ steel_thickness_ + lead_thickness_/2.));
          	  }
          	  else{
          	    vertex.setX(vertex.x() - (shield_x_/2.+ steel_thickness_ + lead_thickness_/2.));
          	  }
        	}
        	// VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(vertex, 0, false);
        	// } while (VertexVolume->GetName() != "STEEL_BEAM_ROOF");
        }

        else if (rand < (perc_top_struct_vol_ + perc_roof_vol_)) { //TOP BEAM STRUCTURE
            	G4double random = G4UniformRand();
            	if (random <  perc_struc_x_vol_){
            	  G4double rand_beam = int (4* G4UniformRand());
                vertex = struct_x_gen_->GenerateVertex("INSIDE");
            	  if (rand_beam == 1) {
            	    vertex.setZ(vertex.z()-roof_z_separation_);
            	  }
            	  else if (rand_beam == 2) {
            	    vertex.setZ(vertex.z()-(roof_z_separation_+lateral_z_separation_));
            	  }
            	  else if (rand_beam == 3) {
            	    vertex.setZ(vertex.z()-(2*roof_z_separation_+lateral_z_separation_));
            	  }
            	}
            	else {
                vertex = struct_z_gen_->GenerateVertex("INSIDE");
            	  if (G4UniformRand() < 0.5) {
            	    vertex.setX(vertex.x()+front_x_separation_);
            	  }
              }
        }

        else{ //LATERAL BEAM STRUCTURE
              G4double lat_prob = beam_thickness_1/(beam_thickness_1+beam_thickness_2);
              if (G4UniformRand()<lat_prob){ //lateral
              	G4double rand_beam = int (4 * G4UniformRand());
                vertex = lat_beam_gen_->GenerateVertex("INSIDE");
              	if (rand_beam ==1){
              	  vertex.setZ(vertex.z() - lateral_z_separation_);
              	}
              	else if (rand_beam ==2){
              	  vertex.setX(vertex.x() - (shield_x_ + 2*steel_thickness_ + lead_thickness_));
              	}
              	else if (rand_beam ==3){
              	  vertex.setX(vertex.x() - (shield_x_ + 2*steel_thickness_ + lead_thickness_));
              	  vertex.setZ(vertex.z() - lateral_z_separation_);
              	}
              }
              else{ // front
                G4double rand_beam = int (4 * G4UniformRand());
                vertex = front_beam_gen_->GenerateVertex("INSIDE");
              	if (rand_beam ==1){
              	  vertex.setX(vertex.x() + front_x_separation_);
              	}
              	else if (rand_beam ==2){
              	  vertex.setZ(vertex.z() - (shield_z_+2*steel_thickness_+lead_thickness_));
              	}
              	else if (rand_beam ==3){
              	  vertex.setX(vertex.x() + front_x_separation_);
              	  vertex.setZ(vertex.z() - (shield_z_+2*steel_thickness_+lead_thickness_));
              	}
              }
            }
        }

    else if(region=="PEDESTAL"){
        G4double rand = G4UniformRand();

        if (rand < perc_ped_bottom_vol_) { //SUPPORT-BOTTOM
          vertex = ped_support_bottom_gen_->GenerateVertex("INSIDE");
      	  if (G4UniformRand() < 0.5) {
      	    vertex.setZ(vertex.z() - support_beam_dist_);
      	  }
        }
        else if (rand < (perc_ped_bottom_vol_ + perc_ped_top_vol_)) { //SUPPORT-TOP
          vertex = ped_support_top_gen_->GenerateVertex("INSIDE");
          if (G4UniformRand() < 0.5) {
      	    vertex.setZ(vertex.z() - support_beam_dist_);
      	  }
        }
        else if (rand < (perc_ped_bottom_vol_ + perc_ped_top_vol_ + perc_ped_front_vol_)){ //FRONT BEAM
          vertex = ped_front_gen_->GenerateVertex("INSIDE");
          if (G4UniformRand() < 0.5) {
      	    vertex.setZ(vertex.z() - (2.*support_front_dist_ + support_beam_dist_));
      	  }
        }
        else if (rand < (perc_ped_bottom_vol_ + perc_ped_top_vol_ + perc_ped_front_vol_
                                                                  + perc_ped_lateral_vol_)){ // LATERAL BEAM
          vertex = ped_lateral_gen_->GenerateVertex("INSIDE");
          if (G4UniformRand() < 0.5) {
            vertex.setX(vertex.x() - (pedestal_x_ + pedestal_lateral_beam_thickness_));
          }
         }
        else { // ROOF
          if (G4UniformRand() < 0.5) {
            vertex = ped_roof_lat_gen_->GenerateVertex("INSIDE");
            if (G4UniformRand() < 0.5){
              vertex.setX(vertex.x() - pedestal_top_x_ - pedestal_roof_thickness_);
            }
          }
          else{
            vertex = ped_roof_front_gen_->GenerateVertex("INSIDE");
            if (G4UniformRand() < 0.5){
              vertex.setZ(vertex.z() - pedestal_lateral_length_ + pedestal_roof_thickness_);
            }
          }
         }
      }

    else if(region=="BUBBLE_SEAL"){
      G4double rand = G4UniformRand();
      if (rand<perc_bubble_front_vol_){ // front
        vertex = bubble_seal_front_gen_->GenerateVertex("INSIDE");
        if (G4UniformRand() < 0.5){
          vertex.setZ(vertex.z() + (support_beam_dist_/2. + support_front_dist_
                                 + pedestal_front_beam_thickness_/2. + bubble_seal_thickness_/2.));
        }
        else{
          vertex.setZ(vertex.z() - (support_beam_dist_/2. + support_front_dist_
                                 + pedestal_front_beam_thickness_/2. + bubble_seal_thickness_/2.));
        }
      }
      else{ // lateral
        vertex = bubble_seal_lateral_gen_->GenerateVertex("INSIDE");
        if (G4UniformRand() < 0.5){
          vertex.setX(vertex.x() + (pedestal_x_/2. + pedestal_lateral_beam_thickness_
                                 + bubble_seal_thickness_/2.));
        }
        else{
          vertex.setX(vertex.x() - (pedestal_x_/2. + pedestal_lateral_beam_thickness_
                                 + bubble_seal_thickness_/2.));
        }
      }
      }

    else if(region=="EDPM_SEAL"){
      G4double rand = G4UniformRand();
      if (rand<perc_edpm_front_vol_){ // front
        vertex = edpm_seal_front_gen_->GenerateVertex("INSIDE");
        if (G4UniformRand() < 0.5){
          vertex.setZ(vertex.z() + (shield_z_/2. - edpm_seal_thickness_/2.));
        }
        else{
          vertex.setZ(vertex.z() - (shield_z_/2. - edpm_seal_thickness_/2.));
        }
      }
      else{ // lateral
        vertex = edpm_seal_lateral_gen_->GenerateVertex("INSIDE");
        vertex.setY(vertex.y() + (shield_y_/2. - edpm_seal_thickness_/2.));
      }
    }

    else {
      G4Exception("[Next100Shielding]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }
} //end namespace nexus
