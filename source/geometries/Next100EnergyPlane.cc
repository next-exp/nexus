// ----------------------------------------------------------------------------
// nexus | Next100EnergyPlane.cc
//
// Energy plane geometry of the NEXT-100 detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100EnergyPlane.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "CylinderPointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>
#include <G4VPhysicalVolume.hh>
#include <G4TransportationManager.hh>
#include <Randomize.hh>

namespace nexus {

  using namespace CLHEP;

  Next100EnergyPlane::Next100EnergyPlane():

    num_PMTs_ (60),

    // copper plate
    copper_plate_thickn_ (120 * mm),
    copper_plate_diam_ (1340. * mm),

    // copper plate holes
    hole_diam_front_  (80. * mm),
    hole_diam_rear_   (62. * mm),
    hole_length_front_(49.5 * mm),
    hole_length_rear_ (copper_plate_thickn_ - hole_length_front_),

    // huts
    hut_int_diam_     (64. * mm),
    hut_diam_         (76. * mm), // hut external diam
    hut_hole_length_  (45. * mm),
    hut_length_long_  (120.* mm),
    hut_length_medium_(100.* mm),
    hut_length_short_ (60. * mm - 1 * mm), // 60 from drawings, subtraction needed to solve small overlap
    last_hut_long_   (17),
    last_hut_medium_ (35),

    sapphire_window_thickn_ (6. * mm),
    optical_pad_thickn_ (1.0 * mm),
    tpb_thickn_ (1.*micrometer),

    pmt_stand_out_ (2. * mm), // length that PMTs stand out of copper, in the front
    pmt_base_diam_ (46.8 * mm),
    pmt_base_thickn_ (0.5 * mm),

    visibility_(1),
    verbosity_(0)
  {
    /// HOW THIS GEOMETRY IS BUILT ///
    /// 1. Central hole for the gas flow is created in the copper plate.
    /// 2. The copper huts, which provide shielding behind the PMTs,
    ///    are glued to the copper plate.
    /// 3. Since the sapphire windows that protect the PMTs and part of the
    ///    PMTs themselves stand out the copper plate, holes are made in the
    ///    copper volume to host the elements related with the PMTs.
    /// 4. A vacuum volume that encapsulates the sapphire window,
    ///    the optical pad, the PMT and the internal part of the base is made;
    ///    this volume fits exactly the hole previously done in the copper.
    /// 5. This volume is replicated by the number of the PMts and placed
    ///    in the gas volume, inside the holes excavated in the copper.


    /// Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ =
      G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
				  "Control commands of geometry Next100.");
    msg_->DeclareProperty("energy_plane_vis", visibility_, "Energy Plane visibility");
    msg_->DeclareProperty("energy_plane_verbosity", verbosity_, "Energy Plane verbosity");

    /// The PMT
    pmt_ = new PmtR11410();
  }


  void Next100EnergyPlane::Construct()
  {

    G4double offset = 1. * cm;

    GeneratePositions();

    /// Copper Plate ///
    G4Tubs* copper_plate_origin_solid =
      new G4Tubs("COPPER_PLATE_ORIGIN", 0., copper_plate_diam_/2.,
                 copper_plate_thickn_/2., 0., twopi);

    /// Hole for gas flow ///
    G4double gas_hole_diam   = 12.  * mm;
    G4double gas_hole_length = 103. * mm;
    G4Tubs* copper_plate_gas_hole_solid =
      new G4Tubs("COPPER_PLATE_CENTRAL_HOLE", 0., gas_hole_diam/2.,
		             (gas_hole_length + offset)/2., 0., twopi);

    G4ThreeVector gas_hole_pos =
      G4ThreeVector(0., 0., -copper_plate_thickn_/2. + gas_hole_length/2.);
    G4SubtractionSolid* copper_plate_hole_solid =
      new G4SubtractionSolid("EP_COPPER_PLATE", copper_plate_origin_solid,
                             copper_plate_gas_hole_solid, 0, gas_hole_pos);

    /// Glue together the different kinds of huts to the copper plate ///
    G4ThreeVector hut_pos;

    G4double hut_length = hut_hole_length_ + hut_length_short_;
    G4double transl_z = copper_plate_thickn_/2. + hut_length/2 - offset/2.;
    G4Tubs* short_hut_solid =
      new G4Tubs("SHORT_HUT", 0., hut_diam_/2., (hut_length + offset)/2., 0., twopi);
    hut_pos = short_hut_pos_[0];
    hut_pos.setZ(transl_z);
    G4UnionSolid* copper_plate_hut_solid =
      new G4UnionSolid("EP_COPPER_PLATE", copper_plate_hole_solid,
                       short_hut_solid, 0, hut_pos);

    for (unsigned int i=1; i<short_hut_pos_.size(); i++) {
      hut_pos = short_hut_pos_[i];
      hut_pos.setZ(transl_z);
      copper_plate_hut_solid =
        new G4UnionSolid("EP_COPPER_PLATE", copper_plate_hut_solid,
                         short_hut_solid, 0, hut_pos);
    }

    hut_length = hut_hole_length_ + hut_length_medium_;
    transl_z = copper_plate_thickn_/2. + hut_length/2 - offset/2.;
    G4Tubs* medium_hut_solid = new G4Tubs("MEDIUM_HUT", 0., hut_diam_/2.,
                                          (hut_length + offset)/2., 0., twopi);

    for (unsigned int i=0; i<medium_hut_pos_.size(); i++) {
      hut_pos = medium_hut_pos_[i];
      hut_pos.setZ(transl_z);
      copper_plate_hut_solid =
        new G4UnionSolid("EP_COPPER_PLATE", copper_plate_hut_solid,
                         medium_hut_solid, 0, hut_pos);
    }

    hut_length = hut_hole_length_ + hut_length_long_;
    transl_z = copper_plate_thickn_/2. + hut_length/2 - offset/2.;
    G4Tubs* long_hut_solid = new G4Tubs("LONG_HUT", 0., hut_diam_/2.,
                                        (hut_length + offset)/2., 0., twopi);

    for (unsigned int i=0; i<long_hut_pos_.size(); i++) {
      hut_pos = long_hut_pos_[i];
      hut_pos.setZ(transl_z);
      copper_plate_hut_solid =
        new G4UnionSolid("EP_COPPER_PLATE", copper_plate_hut_solid,
                         long_hut_solid, 0, hut_pos);
    }


    /// Holes in copper ///
    G4Tubs* hole_front_solid =
      new G4Tubs("HOLE_FRONT", 0., hole_diam_front_/2.,
                 (hole_length_front_ + offset)/2., 0., twopi);

    G4Tubs* hole_rear_solid =
      new G4Tubs("HOLE_REAR", 0., hole_diam_rear_/2.,
                 (hole_length_rear_ + offset)/2., 0., twopi);

    transl_z = (hole_length_front_ + offset)/2. + hole_length_rear_/2.;
    G4UnionSolid* hole_solid =
      new G4UnionSolid("HOLE", hole_front_solid, hole_rear_solid,
                       0, G4ThreeVector(0., 0., transl_z));

    G4Tubs* hole_hut_solid = new G4Tubs("HOLE_HUT", 0., hut_int_diam_/2.,
                                        hut_hole_length_/2., 0., twopi);

    transl_z = (hole_length_front_ + offset)/2. + hole_length_rear_ + hut_hole_length_/2.;
    hole_solid = new G4UnionSolid("HOLE", hole_solid, hole_hut_solid,
                                  0, G4ThreeVector(0., 0., transl_z));

    G4ThreeVector hole_pos = pmt_positions_[0];
    transl_z = - copper_plate_thickn_/2. + hole_length_front_/2. - offset/2.;
    hole_pos.setZ(transl_z);
    G4SubtractionSolid* copper_plate_solid =
      new G4SubtractionSolid("EP_COPPER_PLATE", copper_plate_hut_solid, hole_solid, 0, hole_pos);

    for (G4int i=1; i<num_PMTs_; i++) {
      hole_pos = pmt_positions_[i];
      hole_pos.setZ(transl_z);
      copper_plate_solid =
        new G4SubtractionSolid("EP_COPPER_PLATE", copper_plate_solid,
                               hole_solid, 0, hole_pos);

    }

    G4Material* copper = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    // In Geant4 11.0.0, a bug in treating the OpBoundaryProcess produced in
    // the surface makes the code fail. This is avoided by setting
    // an empty G4MaterialPropertiesTable of the G4Material.
    copper->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    G4LogicalVolume* copper_plate_logic =
      new G4LogicalVolume(copper_plate_solid, copper, "EP_COPPER_PLATE");

    G4double stand_out_length =
      sapphire_window_thickn_ + tpb_thickn_ + optical_pad_thickn_ + pmt_stand_out_;

    copper_plate_posz_ = GetCoordOrigin().z()
      + gate_sapphire_wdw_dist_ + stand_out_length + copper_plate_thickn_/2.;

    // Add optical surface
    G4OpticalSurface* gas_copperplate_opsur = new G4OpticalSurface("GAS_COPPER_PLATE_OPSURF");
    gas_copperplate_opsur->SetType(dielectric_metal);
    gas_copperplate_opsur->SetModel(unified);
    gas_copperplate_opsur->SetFinish(ground);
    gas_copperplate_opsur->SetSigmaAlpha(0.0);
    gas_copperplate_opsur->SetMaterialPropertiesTable(opticalprops::Copper());
    new G4LogicalSkinSurface("GAS_COPPER_PLATE_OPSURF",
                            copper_plate_logic, gas_copperplate_opsur);

    new G4PVPlacement(0, G4ThreeVector(0., 0., copper_plate_posz_),
                      copper_plate_logic, "EP_COPPER_PLATE", mother_logic_, false, 0, false);

    /// Assign optical properties to materials ///
    G4Material* sapphire = materials::Sapphire();
    sapphire->SetMaterialPropertiesTable(opticalprops::Sapphire());
    G4Material* tpb = materials::TPB();
    tpb->SetMaterialPropertiesTable(opticalprops::TPB());
    G4Material* vacuum =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    vacuum->SetMaterialPropertiesTable(opticalprops::Vacuum());
    G4Material* optical_coupler = materials::OpticalSilicone();
    optical_coupler->SetMaterialPropertiesTable(opticalprops::OptCoupler());


    /// Vacuum volume that encapsulates all elements related to PMTs. ///
    G4double vacuum_front_length =
      hole_length_front_ + pmt_stand_out_ + optical_pad_thickn_
      + sapphire_window_thickn_ + tpb_thickn_;
    G4Tubs* vacuum_front_solid =
      new G4Tubs("HOLE_FRONT", 0., hole_diam_front_/2., vacuum_front_length/2., 0., twopi);

    G4Tubs* vacuum_rear_solid =
      new G4Tubs("HOLE_REAR", 0., hole_diam_rear_/2., (hole_length_rear_+offset)/2., 0., twopi);

    G4Tubs* vacuum_hut_solid =
      new G4Tubs("HOLE_HUT", 0., hut_int_diam_/2., hut_hole_length_/2., 0., twopi);

    G4UnionSolid* vacuum_solid =
      new G4UnionSolid("EP_HOLE", vacuum_front_solid, vacuum_rear_solid, 0,
                      G4ThreeVector(0., 0., (vacuum_front_length+hole_length_rear_)/2.));

    vacuum_solid =
      new G4UnionSolid("EP_HOLE", vacuum_solid, vacuum_hut_solid, 0,
                       G4ThreeVector(0., 0., vacuum_front_length/2.+hole_length_rear_+hut_hole_length_/2.));

    G4LogicalVolume* vacuum_logic = new G4LogicalVolume(vacuum_solid, vacuum, "EP_HOLE");

    /// Sapphire window ///
    G4Tubs* sapphire_window_solid =
      new G4Tubs("SAPPHIRE_WINDOW", 0., hole_diam_front_/2.,
                 (sapphire_window_thickn_ + tpb_thickn_)/2., 0., twopi);

    G4LogicalVolume* sapphire_window_logic
      = new G4LogicalVolume(sapphire_window_solid, sapphire, "SAPPHIRE_WINDOW");

    G4double window_posz = -vacuum_front_length/2. + (sapphire_window_thickn_ + tpb_thickn_)/2.;

    G4VPhysicalVolume* sapphire_window_phys =
      new G4PVPlacement(0, G4ThreeVector(0., 0., window_posz),
                        sapphire_window_logic,
                        "SAPPHIRE_WINDOW", vacuum_logic, false, 0, false);

    /// TPB coating on sapphire window ///
    G4Tubs* tpb_solid =
      new G4Tubs("SAPPHIRE_WDW_TPB", 0., hole_diam_front_/2, tpb_thickn_/2.,
                 0., twopi);

    G4LogicalVolume* tpb_logic = new G4LogicalVolume(tpb_solid, tpb, "SAPPHIRE_WDW_TPB");

    G4double tpb_posz = - (sapphire_window_thickn_ + tpb_thickn_)/2. + tpb_thickn_/2.;

    new G4PVPlacement(0, G4ThreeVector(0., 0., tpb_posz), tpb_logic,
                      "SAPPHIRE_WDW_TPB", sapphire_window_logic, false,
                      0, false);


    /// Optical surface on TPB to model roughness ///
    G4OpticalSurface* tpb_surf =
      new G4OpticalSurface("tpb_sapphire_surf",
                           glisur, ground, dielectric_dielectric, .01);
    new G4LogicalSkinSurface("tpb_sapphire_surf", tpb_logic, tpb_surf);


    /// Optical pad ///
    G4Tubs* optical_pad_solid =
      new G4Tubs("OPTICAL_PAD", 0., hole_diam_front_/2., optical_pad_thickn_/2., 0., twopi);

    G4LogicalVolume* optical_pad_logic =
      new G4LogicalVolume(optical_pad_solid, optical_coupler, "OPTICAL_PAD");

    G4double pad_posz =
      -vacuum_front_length/2. + sapphire_window_thickn_ + tpb_thickn_ +
      optical_pad_thickn_/2.;

    G4VPhysicalVolume* optical_pad_phys =
      new G4PVPlacement(0, G4ThreeVector(0., 0., pad_posz), optical_pad_logic,
                        "OPTICAL_PAD", vacuum_logic, false, 0, false);

    /// PMT ///
    pmt_->SetSensorDepth(3);
    pmt_->Construct();
    G4LogicalVolume* pmt_logic = pmt_->GetLogicalVolume();
    G4double pmt_rel_posz = pmt_->GetRelPosition().z();
    pmt_zpos_ =
      -vacuum_front_length/2. + sapphire_window_thickn_ + tpb_thickn_ +
      optical_pad_thickn_ + pmt_rel_posz;
    G4ThreeVector pmt_pos = G4ThreeVector(0., 0., pmt_zpos_);

    pmt_rot_ = new G4RotationMatrix();
    rot_angle_ = pi;
    pmt_rot_->rotateY(rot_angle_);
    new G4PVPlacement(G4Transform3D(*pmt_rot_, pmt_pos), pmt_logic,
                      "PMT", vacuum_logic, false, 0, false);


    /// Part of the PMT bases with pins and resistors ///
    G4Tubs* pmt_base_solid = new G4Tubs("PMT_BASE", 0., pmt_base_diam_/2.,
                                        pmt_base_thickn_/2., 0., twopi);

    G4LogicalVolume* pmt_base_logic =
      new G4LogicalVolume(pmt_base_solid,
                          G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"),
                          "PMT_BASE");

    G4double pmt_base_posz = vacuum_front_length/2. + hole_length_rear_ + hut_hole_length_/2.;

    G4VPhysicalVolume* pmt_base_phys =
       new G4PVPlacement(0, G4ThreeVector(0., 0., pmt_base_posz),
                         pmt_base_logic,
                         "PMT_BASE", vacuum_logic, false, 0, false);

    /// Placing the encapsulating volume with all internal components in place
    vacuum_posz_ = copper_plate_posz_ - copper_plate_thickn_/2.
      + vacuum_front_length/2. - sapphire_window_thickn_ - tpb_thickn_
      - optical_pad_thickn_ - pmt_stand_out_;

    G4ThreeVector pos;
    for (int i=0; i<num_PMTs_; i++) {
      pos = pmt_positions_[i];
      pos.setZ(vacuum_posz_);
      new G4PVPlacement(0, pos, vacuum_logic, "EP_HOLE", mother_logic_,
                        false, i, false);
    }


    if (verbosity_) {
      G4cout << "Copper plate starts in z = "
	     << copper_plate_posz_ - copper_plate_thickn_/2.
	     << " mm and ends in z = "
	     << copper_plate_posz_ + copper_plate_thickn_/2.
	     << G4endl;
      G4cout << "Sapphire windows starts in z = "
	     << vacuum_posz_ + window_posz - sapphire_window_thickn_/2.
	     << " mm and ends in z = "
	     << vacuum_posz_ + window_posz + sapphire_window_thickn_/2.
	     << G4endl;
    }

    //////////////////////
    ///  VISIBILITIES  ///
    //////////////////////

    vacuum_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    pmt_base_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    if (visibility_) {
      G4VisAttributes copper_col = CopperBrownAlpha();
      copper_col.SetForceSolid(true);
      copper_plate_logic->SetVisAttributes(copper_col);
      G4VisAttributes sapphire_col = nexus::LillaAlpha();
      sapphire_col.SetForceSolid(true);
      sapphire_window_logic->SetVisAttributes(sapphire_col);
      G4VisAttributes pad_col = nexus::LightGreenAlpha();
      pad_col.SetForceSolid(true);
      optical_pad_logic->SetVisAttributes(pad_col);
      G4VisAttributes tpb_col = nexus::LightBlueAlpha();
      tpb_col.SetForceSolid(true);
      tpb_logic->SetVisAttributes(tpb_col);

    } else {
      copper_plate_logic   ->SetVisAttributes(G4VisAttributes::GetInvisible());
      sapphire_window_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      optical_pad_logic    ->SetVisAttributes(G4VisAttributes::GetInvisible());
      tpb_logic            ->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    //////////////////////////
    /// VERTEX GENERATORS  ///
    //////////////////////////

    G4double full_copper_length = copper_plate_thickn_ + hut_hole_length_ +
                                  hut_length_long_;
    G4double full_copper_posz   = copper_plate_posz_ + copper_plate_thickn_/2. +
                                  hut_hole_length_   + hut_length_long_ -
                                  full_copper_length/2.;
    copper_gen_ =
      new CylinderPointSampler(0., copper_plate_diam_/2.,
                                   full_copper_length/2.,
                                   0., twopi, nullptr,
                                   G4ThreeVector(0., 0., full_copper_posz));

    sapphire_window_gen_ = new CylinderPointSampler(sapphire_window_phys);
    optical_pad_gen_     = new CylinderPointSampler(optical_pad_phys);
    pmt_base_gen_        = new CylinderPointSampler(pmt_base_phys);

  }


  Next100EnergyPlane::~Next100EnergyPlane()
  {
    delete copper_gen_;
    delete sapphire_window_gen_;
    delete optical_pad_gen_;
    delete pmt_base_gen_;
  }


  G4ThreeVector Next100EnergyPlane::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    // Copper plate
    // As it is full of holes, let's get sure vertices are in the right volume
    if (region == "EP_COPPER_PLATE") {
      G4VPhysicalVolume *VertexVolume;
      do {
        vertex = copper_gen_->GenerateVertex(VOLUME);
        G4ThreeVector glob_vtx(vertex);
        glob_vtx = glob_vtx - GetCoordOrigin();
        VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != region);
    }

    // Sapphire windows
    else if (region == "SAPPHIRE_WINDOW") {
      G4VPhysicalVolume *VertexVolume;
      do {
        vertex = sapphire_window_gen_->GenerateVertex(VOLUME);
        G4double rand = num_PMTs_ * G4UniformRand();
        G4ThreeVector sapphire_pos = pmt_positions_[int(rand)];
        vertex += sapphire_pos;
        G4double z_translation = vacuum_posz_;
        vertex.setZ(vertex.z() + z_translation);
        G4ThreeVector glob_vtx(vertex);
        glob_vtx = glob_vtx - GetCoordOrigin();
        VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
      } while (VertexVolume->GetName() != region);
    }

    // Optical pads
    else if (region == "OPTICAL_PAD") {
      vertex = optical_pad_gen_->GenerateVertex(VOLUME);
      G4double rand = num_PMTs_ * G4UniformRand();
      G4ThreeVector optical_pad_pos = pmt_positions_[int(rand)];
      vertex += optical_pad_pos;
      G4double z_translation = vacuum_posz_;
      vertex.setZ(vertex.z() + z_translation);
    }

    // PMTs (What to do with them ?? Should we update to the new vertex generators??)
    else  if (region == "PMT" || region == "PMT_BODY") {
      G4ThreeVector ini_vertex = pmt_->GenerateVertex(region);
      ini_vertex.rotate(rot_angle_, G4ThreeVector(0., 1., 0.));
      G4double rand = num_PMTs_ * G4UniformRand();
      G4ThreeVector pmt_pos = pmt_positions_[int(rand)];
      vertex = ini_vertex + pmt_pos;
      G4double z_translation = vacuum_posz_ + pmt_zpos_;
      vertex.setZ(vertex.z() + z_translation);
    }

    // PMT bases
    else if (region == "PMT_BASE") {
      vertex = pmt_base_gen_->GenerateVertex(VOLUME);
      G4double rand = num_PMTs_ * G4UniformRand();
      G4ThreeVector pmt_base_pos = pmt_positions_[int(rand)];
      vertex += pmt_base_pos;
      G4double z_translation = vacuum_posz_;
      vertex.setZ(vertex.z() + z_translation);
    }

    else {
      G4Exception("[Next100EnergyPlane]", "GenerateVertex()", FatalException,
     		  "Unknown vertex generation region!");
    }

    return vertex;
  }


  void Next100EnergyPlane::GeneratePositions()
  {
    /// Function that computes and stores the XY positions of PMTs in the copper plate

    G4int num_conc_circles = 4;
    G4int num_inner_pmts = 6;
    G4double x_pitch = 125 * mm;
    G4double y_pitch = 108.3 * mm;
    G4int total_positions = 0;
    G4ThreeVector position(0.,0.,0.);

    for (G4int circle=1; circle<=num_conc_circles; circle++) {
      G4double rad     = circle * x_pitch;
      G4double step    = 360.0/num_inner_pmts;

      for (G4int place=0; place<num_inner_pmts; place++) {
      	G4double angle = place * step;
      	position.setX(rad * cos(angle*deg));
      	position.setY(rad * sin(angle*deg));
      	pmt_positions_.push_back(position);
      	total_positions++;
      }

      for (G4int i=1; i<circle; i++) {
      	G4double start_x = (circle-(i*0.5))*x_pitch;
      	G4double start_y = i*y_pitch;
      	rad  = std::sqrt(std::pow(start_x, 2) + std::pow(start_y, 2));
      	G4double start_angle = std::atan2(start_y, start_x)/deg;

      	for (G4int place=0; place<num_inner_pmts; place++) {
      	  G4double angle = start_angle + place * step;
      	  position.setX(rad * cos(angle*deg));
      	  position.setY(rad * sin(angle*deg));
      	  pmt_positions_.push_back(position);
      	  total_positions++;
	       }
      }
    }

    long_hut_pos_ =
      std::vector<G4ThreeVector>(pmt_positions_.begin(),
                                 pmt_positions_.begin()+last_hut_long_+1);
    medium_hut_pos_ =
      std::vector<G4ThreeVector>(pmt_positions_.begin()+last_hut_long_+1,
                                 pmt_positions_.begin()+last_hut_medium_+1);
    short_hut_pos_ =
      std::vector<G4ThreeVector>(pmt_positions_.begin()+last_hut_medium_+1,
                                 pmt_positions_.end());

    // Checking
    if (total_positions != num_PMTs_) {
      G4cout << "\nERROR: Number of PMTs doesn't match with number of positions calculated\n";
      G4cout << "Number of PMTs = " << num_PMTs_ << ", number of positions = "
	     << total_positions << G4endl;
      exit(0);
    }
  }

  void Next100EnergyPlane::PrintPMTPositions() const
  {
    G4cout << "*** PMT xyz positions ***" << G4endl;
    G4cout << G4endl;
    for (size_t i=0; i<pmt_positions_.size(); i++) {
      G4cout << "PMTR11410 " << i << "  " << pmt_positions_[i] << G4endl;
    }
  }

}
