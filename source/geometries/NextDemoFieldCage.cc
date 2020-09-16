// ----------------------------------------------------------------------------
// nexus | NextDemoFieldCage.cc
//
// Field cage geometry of the DEMO++ detector. It include the elements in
// the drift and the buffer part of the detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextDemoFieldCage.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "XenonGasProperties.h"
#include "CylinderPointSampler2020.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Polyhedra.hh>
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4NistManager.hh>
#include <G4UnitsTable.hh>
#include <G4TransportationManager.hh>


namespace nexus {

  using namespace CLHEP;

  NextDemoFieldCage::NextDemoFieldCage():
    BaseGeometry(),
    mother_logic_(nullptr),
    mother_phys_(nullptr),
    gate_cathode_centre_dist_ (309.6 * mm), // distance between gate and the centre of cathode grid
    grid_thickn_ (0.1 * mm),
    gate_transparency_ (0.76),
    buffer_length_ (117.85 * mm),
    cath_grid_transparency_ (0.98),
    cathode_ring_diam_ (229. * mm),
    el_gap_length_ (9.8 * mm), // 9.8 mm when there's the plate
    elgap_ring_diam_ (232. * mm),
    light_tube_drift_start_z_ (16. * mm),
    light_tube_drift_end_z_ (304. * mm),
    ltube_thickn_ ( 8.0 * mm),
    light_tube_buff_start_z_ (317.6 * mm),
    light_tube_buff_end_z_ (420.6 * mm),
    anode_length_ (3.2 * mm),
    anode_diam_ (256.3 * mm),
    tpb_thickn_ (3 * micrometer),
    active_diam_ (194.2 * mm),
    ring_diam_ (235. * mm),
    ring_height_ (10. * mm),
    ring_thickn_ (5. * mm),
    dist_gate_first_ring_ (18.4 * mm),
    bar_start_z_ (14. * mm),
    bar_end_z_ (403.5 * mm),
    bar_width_ (19. * mm),
    bar_thickn_ (11. * mm),
    bar_addon1_length_ ( 7. * mm),
    bar_addon1_thickn_ (16. * mm),
    bar_addon2_length_ (16.5 * mm),
    bar_addon2_thickn_ ( 7. * mm),
    visibility_ (0),
    verbosity_(0),
    max_step_size_(1.*mm),
    drift_transv_diff_ (1. * mm/sqrt(cm)),
    drift_long_diff_ (.3 * mm/sqrt(cm)),
    ELtransv_diff_ (1. * mm / sqrt(cm)),
    ELlong_diff_ (0.5 * mm / sqrt(cm)),
    elfield_(0),
    ELelectric_field_ (23.2857 * kilovolt / cm)
  {
    /// Define new categories ///
    new G4UnitDefinition("kilovolt/cm","kV/cm","Electric field", kilovolt/cm);
    new G4UnitDefinition("mm/sqrt(cm)","mm/sqrt(cm)","Diffusion", mm/sqrt(cm));

    /// Initializing the geometry navigator (used in vertex generation)
    geom_navigator_ =
      G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

    /// Messenger ///
    msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/", +
                                  "Control commands of geometry NextDemo.");
    msg_->DeclareProperty("field_cage_vis", visibility_,
                          "Field cage visibility");
    msg_->DeclareProperty("field_cage_verbosity", verbosity_,
                          "Field cage verbosity");

    G4GenericMessenger::Command& max_step_size_cmd =
      msg_->DeclareProperty("max_step_size", max_step_size_,
                            "Set maximum step size.");
    max_step_size_cmd.SetUnitCategory("Length");
    max_step_size_cmd.SetParameterName("max_step_size", true);
    max_step_size_cmd.SetRange("max_step_size>0");

    G4GenericMessenger::Command& drift_transv_diff_cmd =
      msg_->DeclareProperty("drift_transv_diff", drift_transv_diff_,
                            "Tranvsersal diffusion in the drift region");
    drift_transv_diff_cmd.SetParameterName("drift_transv_diff", true);
    drift_transv_diff_cmd.SetUnitCategory("Diffusion");

    G4GenericMessenger::Command& drift_long_diff_cmd =
      msg_->DeclareProperty("drift_long_diff", drift_long_diff_,
                            "Longitudinal diffusion in the drift region");
    drift_long_diff_cmd.SetParameterName("drift_long_diff", true);
    drift_long_diff_cmd.SetUnitCategory("Diffusion");

    G4GenericMessenger::Command&  ELtransv_diff_cmd =
      msg_->DeclareProperty("ELtransv_diff", ELtransv_diff_,
                            "Tranvsersal diffusion in the EL region");
    ELtransv_diff_cmd.SetParameterName("ELtransv_diff", true);
    ELtransv_diff_cmd.SetUnitCategory("Diffusion");

    G4GenericMessenger::Command&  ELlong_diff_cmd =
      msg_->DeclareProperty("ELlong_diff", ELlong_diff_,
                            "Longitudinal diffusion in the EL region");
    ELlong_diff_cmd.SetParameterName("ELlong_diff", true);
    ELlong_diff_cmd.SetUnitCategory("Diffusion");

    msg_->DeclareProperty("elfield", elfield_,
                          "True if the EL field is on (full simulation), false if it's not (parametrized simulation).");

    G4GenericMessenger::Command& El_field_intensity_cmd =
      msg_->DeclareProperty("EL_field_intensity", ELelectric_field_,
                            "Electric field in the EL region");
    El_field_intensity_cmd.SetParameterName("EL_field_intensity", true);
    El_field_intensity_cmd.SetUnitCategory("Electric field");
  }


  NextDemoFieldCage::~NextDemoFieldCage()
  {
    //delete msg_;
  }


  void NextDemoFieldCage::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }


  void NextDemoFieldCage::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
  {
    mother_phys_ = mother_phys;
  }


  void NextDemoFieldCage::Construct()
  {
    /// Calculate lengths of active region
    active_length_ = gate_cathode_centre_dist_ - grid_thickn_/2.;

    /// Calculate derived positions in mother volume
    active_zpos_       = GetELzCoord() + active_length_/2.;
    cathode_grid_zpos_ = GetELzCoord() + gate_cathode_centre_dist_;
    el_gap_zpos_       = active_zpos_ - active_length_/2. - el_gap_length_/2.;
    ltube_drift_length_ = light_tube_drift_end_z_ - light_tube_drift_start_z_;
    ltube_buff_length_  = light_tube_buff_end_z_ - light_tube_buff_start_z_;
    bar_length_  = bar_end_z_ - bar_start_z_;

    if (verbosity_) {
      G4cout << "Active length = " << active_length_/mm << " mm" << G4endl;
      G4cout << "Buffer length = " << buffer_length_/mm << " mm" << G4endl;
      G4cout << G4endl;
    }

    /// Define materials to be used
    DefineMaterials();

    /// Build the different parts of the field cage
    BuildActive();
    BuildCathodeGrid();
    BuildBuffer();
    BuildELRegion();
    BuildFieldCage();

  }

  void NextDemoFieldCage::DefineMaterials()
  {
    /// Read gas properties from mother volume
    gas_         = mother_logic_->GetMaterial();
    pressure_    = gas_->GetPressure();
    temperature_ = gas_->GetTemperature();

    aluminum_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");

    /// Teflon for the light tube
    teflon_ =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

    /// Quartz
    quartz_ =  MaterialsList::FusedSilica();
    quartz_->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());

    /// TPB coating
    tpb_ = MaterialsList::TPB();
    tpb_->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
  }

  void NextDemoFieldCage::BuildActive()
  {
    // Position of z planes
    G4double zplane[2] = {-active_length_/2., active_length_/2.};
    // Inner radius
    G4double rinner[2] = {0., 0.};
    // Outer radius
    G4double router[2] = {active_diam_/2., active_diam_/2.};

    G4Polyhedra* active_solid =
      new G4Polyhedra("ACTIVE", 0., twopi, 10, 2, zplane, rinner, router);

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, gas_, "ACTIVE");

    new G4PVPlacement(0, G4ThreeVector(0.,0., active_zpos_),
                      active_logic, "ACTIVE", mother_logic_, false, 0, false);

    // Limit the step size in this volume for better tracking precision
    active_logic->SetUserLimits(new G4UserLimits(max_step_size_));

    // Set the volume as an ionization sensitive detector
    IonizationSD* ionisd = new IonizationSD("/DEMO/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    //Define a drift field for this volume
    UniformElectricDriftField* field = new UniformElectricDriftField();
    G4double global_active_zpos = active_zpos_ - GetELzCoord();
    field->SetCathodePosition(global_active_zpos + active_length_/2.);
    field->SetAnodePosition(global_active_zpos - active_length_/2.);
    field->SetDriftVelocity(1.*mm/microsecond);
    field->SetTransverseDiffusion(drift_transv_diff_);
    field->SetLongitudinalDiffusion(drift_long_diff_);
    G4Region* drift_region = new G4Region("DRIFT");
    drift_region->SetUserInformation(field);
    drift_region->AddRootLogicalVolume(active_logic);

    active_gen_ =
      new CylinderPointSampler2020(0., active_diam_/2., active_length_/2.,
                                   0., twopi, nullptr,
                                   G4ThreeVector(0., 0., active_zpos_));

    active_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }


void NextDemoFieldCage::BuildCathodeGrid()
  {
    G4Material* grid_mat =
      MaterialsList::FakeDielectric(gas_, "cath_grid_mat");
    grid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(pressure_, temperature_, cath_grid_transparency_, grid_thickn_));

    G4Tubs* cathode_grid_solid =
      new G4Tubs("CATHODE_GRID", 0., cathode_ring_diam_/2., grid_thickn_/2.,
                 0, twopi);

    G4LogicalVolume* cathode_grid_logic =
      new G4LogicalVolume(cathode_grid_solid, grid_mat, "CATHODE_GRID");

    new G4PVPlacement(0, G4ThreeVector(0., 0., cathode_grid_zpos_),
		      cathode_grid_logic, "CATHODE_GRID", mother_logic_,
		      false, 0, false);

    /// Visibilities
    if (visibility_) {
      G4VisAttributes grey = nexus::LightGrey();
      cathode_grid_logic->SetVisAttributes(grey);
    } else {
      cathode_grid_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  void NextDemoFieldCage::BuildBuffer()
  {
    G4double buffer_zpos =
      active_zpos_ + active_length_/2. + grid_thickn_ + buffer_length_/2.;

    /// Position of z planes
    G4double zplane[2] = {-buffer_length_/2., buffer_length_/2.};
    /// Inner radius
    G4double rinner[2] = {0., 0.};
    /// Outer radius
    G4double router[2] = {active_diam_/2., active_diam_/2.};

    G4Polyhedra* buffer_solid =
      new G4Polyhedra("BUFFER", 0., twopi, 10, 2, zplane, rinner, router);

    G4LogicalVolume* buffer_logic =
      new G4LogicalVolume(buffer_solid, gas_, "BUFFER");
    new G4PVPlacement(0, G4ThreeVector(0., 0., buffer_zpos), buffer_logic,
		      "BUFFER", mother_logic_, false, 0, false);


    /// Set the volume as an ionization sensitive detector
    IonizationSD* buffsd = new IonizationSD("/NEXT100/BUFFER");
    buffsd->IncludeInTotalEnergyDeposit(false);
    buffer_logic->SetSensitiveDetector(buffsd);
    G4SDManager::GetSDMpointer()->AddNewDetector(buffsd);

    /// Visibilities
    buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }


  void NextDemoFieldCage::BuildELRegion()
  {
    G4Tubs* elgap_solid = new G4Tubs("EL_GAP", 0., elgap_ring_diam_/2.,
                                     el_gap_length_/2., 0, twopi);

    G4LogicalVolume* elgap_logic =
      new G4LogicalVolume(elgap_solid, gas_, "EL_GAP");   // gxe_

    G4double el_gap_zpos = GetELzCoord() - el_gap_length_/2.;

    new G4PVPlacement(0, G4ThreeVector(0., 0., el_gap_zpos),
                      elgap_logic, "EL_GAP", mother_logic_, false, 0, false);

    if (elfield_) {
      UniformElectricDriftField* el_field = new UniformElectricDriftField();
      G4double global_el_gap_zpos = el_gap_zpos_ - GetELzCoord();
      el_field->SetCathodePosition(global_el_gap_zpos + el_gap_length_/2.);
      el_field->SetAnodePosition(global_el_gap_zpos - el_gap_length_/2.);
      el_field->SetDriftVelocity(2.5*mm/microsecond);
      el_field->SetTransverseDiffusion(ELtransv_diff_);
      el_field->SetLongitudinalDiffusion(ELlong_diff_);
      XenonGasProperties xgp(pressure_, temperature_);
      // el_field->SetLightYield(xgp.ELLightYield(24.8571*kilovolt/cm));//value for E that gives Y=1160 photons per ie- in normal conditions
      el_field->SetLightYield(xgp.ELLightYield(ELelectric_field_));
      G4Region* el_region = new G4Region("EL_REGION");
      el_region->SetUserInformation(el_field);
      el_region->AddRootLogicalVolume(elgap_logic);
    }

    G4Material* grid_mat = MaterialsList::FakeDielectric(gas_, "grid_mat");
    grid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(pressure_, temperature_, gate_transparency_, grid_thickn_));

    G4Tubs* gate_grid_solid =
      new G4Tubs("GRID_GATE", 0., elgap_ring_diam_/2., grid_thickn_/2.,
                 0, twopi);

    G4LogicalVolume* gate_grid_logic =
      new G4LogicalVolume(gate_grid_solid, grid_mat, "GATE_GRID");
    G4double grid_zpos = el_gap_length_/2. - grid_thickn_/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., grid_zpos),
                      gate_grid_logic, "GATE_GRID",
                      elgap_logic, false, 0, false);

    G4Tubs* anode_quartz_solid =
      new G4Tubs("ANODE_PLATE", 0., anode_diam_/2. , anode_length_/2.,
                 0, twopi);

    G4LogicalVolume* anode_logic =
      new G4LogicalVolume(anode_quartz_solid, quartz_, "ANODE_PLATE");

    G4double anode_zpos = GetELzCoord() - el_gap_length_ - anode_length_/2.;

    new G4PVPlacement(0, G4ThreeVector(0., 0., anode_zpos), anode_logic,
                      "ANODE_PLATE", mother_logic_, false, 0, false);

    /// Visibilities
    if (visibility_) {
      G4VisAttributes light_blue = nexus::LightBlue();
      elgap_logic->SetVisAttributes(light_blue);
      G4VisAttributes grey = nexus::LightGrey();
      gate_grid_logic->SetVisAttributes(grey);
      G4VisAttributes yellow = nexus::Yellow();
      anode_logic->SetVisAttributes(yellow);
    } else {
      elgap_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

  }

  void NextDemoFieldCage::BuildFieldCage()
  {
    /// Drift light tube
    G4double zplane[2] = {-ltube_drift_length_/2., ltube_drift_length_/2.};
    G4double rinner[2] = {active_diam_/2., active_diam_/2.};
    G4double router[2] =
      {active_diam_/2. + ltube_thickn_, active_diam_/2. + ltube_thickn_};

    G4cout << rinner[0] << ", " << router[0] << ", " << zplane[0] << G4endl;

    G4Polyhedra* ltube_drift_solid =
      new G4Polyhedra("LIGHT_TUBE_DRIFT", 0., twopi, 10, 2, zplane,
                      rinner, router);

    G4LogicalVolume* ltube_drift_logic =
      new G4LogicalVolume(ltube_drift_solid, teflon_, "LIGHT_TUBE_DRIFT");

    G4double lt_drift_zpos =
      GetELzCoord() + light_tube_drift_start_z_ + ltube_drift_length_/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., lt_drift_zpos),
                      ltube_drift_logic, "LIGHT_TUBE_DRIFT", mother_logic_,
                      false, 0, false);

    G4double router_tpb[2] =
      {active_diam_/2. + tpb_thickn_, active_diam_/2. + tpb_thickn_};

    G4Polyhedra* tpb_drift_solid =
      new G4Polyhedra("DRIFT_TPB", 0., twopi, 10, 2, zplane,
		      rinner, router_tpb);
    G4LogicalVolume* tpb_drift_logic =
      new G4LogicalVolume(tpb_drift_solid, tpb_, "DRIFT_TPB");
    G4VPhysicalVolume* tpb_drift_phys =
      new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), tpb_drift_logic,
		      "DRIFT_TPB", ltube_drift_logic, false, 0, false);

    /// Buffer light tube
    G4double buff_zplane[2] = {-ltube_buff_length_/2., ltube_buff_length_/2.};
    G4Polyhedra* ltube_buff_solid =
      new G4Polyhedra("LIGHT_TUBE_BUFFER", 0., twopi, 10, 2, buff_zplane,
                      rinner, router);

    G4LogicalVolume* ltube_buff_logic =
      new G4LogicalVolume(ltube_buff_solid, teflon_, "LIGHT_TUBE_BUFFER");

    G4double lt_buffer_zpos =
      GetELzCoord() + light_tube_buff_start_z_ + ltube_buff_length_/2.;

    new G4PVPlacement(0, G4ThreeVector(0., 0., lt_buffer_zpos),
                      ltube_buff_logic, "LIGHT_TUBE_BOTTOM", mother_logic_,
                      false, 0, false);

    /// Optical surfaces
    G4OpticalSurface* lt_drift_opsur = new G4OpticalSurface("LIGHT_TUBE_DRIFT");
    lt_drift_opsur->SetType(dielectric_metal);
    lt_drift_opsur->SetModel(unified);
    lt_drift_opsur->SetFinish(ground);
    lt_drift_opsur->SetSigmaAlpha(0.1);
    lt_drift_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
    new G4LogicalSkinSurface("LIGHT_TUBE_DRIFT",
                             ltube_drift_logic, lt_drift_opsur);

    G4OpticalSurface* gas_tpb_teflon_surf =
      new G4OpticalSurface("GAS_TPB_TEFLON_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);

    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", tpb_drift_phys,
                               mother_phys_, gas_tpb_teflon_surf);
    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", mother_phys_,
                               tpb_drift_phys, gas_tpb_teflon_surf);


    G4OpticalSurface* lt_buff_opsur = new G4OpticalSurface("LIGHT_TUBE_BUFFER");
    lt_buff_opsur->SetType(dielectric_metal);
    lt_buff_opsur->SetModel(unified);
    lt_buff_opsur->SetFinish(ground);
    lt_buff_opsur->SetSigmaAlpha(0.1);
    lt_buff_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
    new G4LogicalSkinSurface("LIGHT_TUBE_BUFFER",
                             ltube_buff_logic, lt_buff_opsur);


    /// Visibilities
    if (visibility_) {
      G4VisAttributes titanium_col = nexus::TitaniumGrey();
      titanium_col.SetForceSolid(true);
      ltube_drift_logic->SetVisAttributes(titanium_col);
      ltube_buff_logic->SetVisAttributes(titanium_col);
    } else {
      ltube_drift_logic->SetVisAttributes(G4VisAttributes::Invisible);
      ltube_buff_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    /// Field shaping rings
    G4Tubs* ring_solid = new G4Tubs("FIELD_RING", ring_diam_/2.,
                                    (ring_diam_/2.+ring_thickn_),
                                    ring_height_/2., 0, twopi);

    G4LogicalVolume* ring_logic =
      new G4LogicalVolume(ring_solid, aluminum_, "FIELD_RING");

    G4int num_rings = 19;

    G4double posz = GetELzCoord() + dist_gate_first_ring_ + ring_height_/2.;
    for (G4int i=0; i<num_rings; i++) {
      // new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
      //                   "FIELD_RING", mother_logic_, false, i, true);

      posz = posz + ring_height_ + 15. * mm;
    }

    /// Buffer part
    //  posz = posz + ring_height_ + 5.0 * mm - ring_up_bt_gap_;
    // for (G4int i=19; i<23; i++) {
    //   new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
    //                     "FIELD_RING", mother_logic_, false, i, false);
    //   posz = posz - ring_height_ - 10. * mm;
    // }

    /// Visibilities
    if (visibility_) {
      G4VisAttributes yellow_col =nexus::Yellow();
      yellow_col.SetForceSolid(true);
      ring_logic->SetVisAttributes(yellow_col);
    }
    else {
      ring_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    /*
    /// SUPPORT BARS - DRIFT
    G4Box* bar_base = new G4Box("SUPPORT_BAR", bar_thickn_/2.,
                                bar_width_/2., bar_length_/2.);

    G4Box* addon1 = new G4Box("SUPPORT_BAR", bar_addon1_thickn_/2.,
                              bar_width_/2., bar_addon1_length_/2.);

    G4Box* addon2 = new G4Box("SUPPORT_BAR", bar_addon2_thickn_/2.,
                              bar_width_/2., bar_addon2_length_/2.);

    ///  UNIONS for support bar (pillar) and add_on
    G4UnionSolid* bar_solid =
      new G4UnionSolid("SUPPORT_BAR", bar_base, addon1, 0,
                       G4ThreeVector((bar_thickn_ + bar_addon1_thickn_)/2., 0.,
                                     -(bar_length_ - bar_addon1_length_)/2.)); // + or -

    bar_solid =
      new G4UnionSolid("SUPPORT_BAR", bar_solid, addon2, 0,
                       G4ThreeVector(bar_thickn_/2. + bar_addon1_thickn_ - bar_addon2_thickn_/2., 0,
                                     -(bar_length_ + bar_addon2_length_)/2.));


    G4LogicalVolume* bar_logic =
      new G4LogicalVolume(bar_solid, MaterialsList::PEEK(), "SUPPORT_BAR");

    G4double bar_rpos = active_diam_/2. + ltube_thickn_ + bar_thickn_/2.;
    G4double bar_zpos = GetELzCoord() + bar_start_z_ + bar_length_/2.;

    G4RotationMatrix rotbar;
    for (G4int i=0; i<10; i++) {
    rotbar.rotateZ((i*36.+90)*deg);     // 360/10.
    G4double x = bar_rpos * cos((i*36.+90)*deg);
    G4double y = bar_rpos * sin((i*36.+90)*deg);

    // new G4PVPlacement(G4Transform3D(rotbar, G4ThreeVector(x, y, bar_zpos)),
    //      	      bar_logic, "SUPPORT_BAR", mother_logic_, false, i, true);
    }

    if (visibility_) {
      G4VisAttributes lilla_col =nexus::Lilla();
      lilla_col.SetForceSolid(true);
      bar_logic->SetVisAttributes(lilla_col);
    }
    else {
      bar_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
    */
  }


  G4ThreeVector NextDemoFieldCage::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

     if (region == "ACTIVE") {
       G4VPhysicalVolume *VertexVolume;
       do {
         vertex = active_gen_->GenerateVertex("VOLUME");
         G4ThreeVector glob_vtx(vertex);
         glob_vtx = glob_vtx + G4ThreeVector(0, 0, -GetELzCoord());
         VertexVolume =
           geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
       } while (VertexVolume->GetName() != region);
     }
     else {
      G4Exception("[NextDemoFieldCage]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }
}
