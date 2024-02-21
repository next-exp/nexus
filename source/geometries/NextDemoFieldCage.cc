// ----------------------------------------------------------------------------
// nexus | NextDemoFieldCage.cc
//
// Geometry of the DEMO++ field cage. It includes the elements in
// the drift and the buffer part of the detector + EL region.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextDemoFieldCage.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "XenonProperties.h"
#include "CylinderPointSampler.h"
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
    GeometryBase(),
    config_ (""),
    mother_logic_(nullptr),
    mother_phys_(nullptr),
    gate_cathode_centre_dist_ (309.6 * mm), // distance between gate and the centre of cathode grid
    grid_thickn_ (0.1 * mm),
    gate_transparency_    (0.83),
    anode_transparency_   (0.83),
    cathode_transparency_ (0.98),
    buffer_length_ (117.85 * mm),
    el_gap_length_plate_ (9.8 * mm), // when there's the plate (run 5)
    el_gap_length_mesh_ (5. * mm),   // when there's the mesh  (run 7)
    elgap_ring_diam_ (232. * mm),
    light_tube_drift_start_z_ (16. * mm),
    light_tube_drift_end_z_ (304. * mm),
    light_tube_thickn_ ( 8.0 * mm),
    light_tube_buff_start_z_ (317.6 * mm),
    light_tube_buff_end_z_ (420.6 * mm),
    anode_length_ (3.2 * mm),
    anode_diam_ (256.3 * mm),
    tpb_thickn_ (3 * micrometer),
    ito_thickness_ (15. * nanometer),
    active_diam_ (194.2 * mm),
    num_drift_rings_ (19),
    num_rings_ (23),
    ring_diam_ (235. * mm),
    ring_height_ (10. * mm),
    ring_thickn_ (5. * mm),
    ring_drift_buff_gap_ (27.2 * mm),
    dist_drift_ring_centres_ (15. * mm),
    dist_buff_ring_centres_ (20. * mm),
    dist_gate_first_ring_ (18.4 * mm),
    bar_start_z_ (14. * mm),
    bar_end_z_ (403.5 * mm), // check if it's 413.5 or 403.5 mm
    bar_width_ (19. * mm),
    bar_thickn_ (11. * mm),
    visibility_ (0),
    verbosity_(0),
    max_step_size_(1.*mm),
    drift_transv_diff_ (1. * mm/sqrt(cm)),
    drift_long_diff_ (.3 * mm/sqrt(cm)),
    ELtransv_diff_ (1. * mm / sqrt(cm)),
    ELlong_diff_ (0.5 * mm / sqrt(cm)),
    elfield_(0),
    ELelectric_field_ (23.2857 * kilovolt / cm),
    // EL gap generation disk parameters
    el_gap_gen_disk_diam_(0.),
    el_gap_gen_disk_x_(0.), el_gap_gen_disk_y_(0.),
    el_gap_gen_disk_zmin_(0.), el_gap_gen_disk_zmax_(1.)
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

    // el gap generator params
    G4GenericMessenger::Command& el_gap_gen_disk_diam_cmd =
      msg_->DeclareProperty("el_gap_gen_disk_diam", el_gap_gen_disk_diam_,
                            "Diameter of the EL gap vertex generation disk.");
    el_gap_gen_disk_diam_cmd.SetUnitCategory("Length");
    el_gap_gen_disk_diam_cmd.SetParameterName("el_gap_gen_disk_diam", false);
    el_gap_gen_disk_diam_cmd.SetRange("el_gap_gen_disk_diam>=0.");

    G4GenericMessenger::Command& el_gap_gen_disk_x_cmd =
      msg_->DeclareProperty("el_gap_gen_disk_x", el_gap_gen_disk_x_,
                            "X position of the center of the EL gap vertex generation disk.");
    el_gap_gen_disk_x_cmd.SetUnitCategory("Length");
    el_gap_gen_disk_x_cmd.SetParameterName("el_gap_gen_disk_x", false);

    G4GenericMessenger::Command& el_gap_gen_disk_y_cmd =
      msg_->DeclareProperty("el_gap_gen_disk_y", el_gap_gen_disk_y_,
                            "Y position of the center of the EL gap vertex generation disk.");
    el_gap_gen_disk_y_cmd.SetUnitCategory("Length");
    el_gap_gen_disk_y_cmd.SetParameterName("el_gap_gen_disk_y", false);

    G4GenericMessenger::Command& el_gap_gen_disk_zmin_cmd =
      msg_->DeclareProperty("el_gap_gen_disk_zmin", el_gap_gen_disk_zmin_,
                            "Minimum Z range of the EL gap vertex generation disk.");
    el_gap_gen_disk_zmin_cmd.SetParameterName("el_gap_gen_disk_zmin", false);
    el_gap_gen_disk_zmin_cmd.SetRange("el_gap_gen_disk_zmin>=0.0 && el_gap_gen_disk_zmin<=1.0");

    G4GenericMessenger::Command& el_gap_gen_disk_zmax_cmd =
      msg_->DeclareProperty("el_gap_gen_disk_zmax", el_gap_gen_disk_zmax_,
                            "Maximum Z range of the EL gap vertex generation disk.");
    el_gap_gen_disk_zmax_cmd.SetParameterName("el_gap_gen_disk_zmax", false);
    el_gap_gen_disk_zmax_cmd.SetRange("el_gap_gen_disk_zmax>=0.0 && el_gap_gen_disk_zmax<=1.0");
  }


  NextDemoFieldCage::~NextDemoFieldCage()
  {
    delete msg_;
  }


  void NextDemoFieldCage::Construct()
  {

    /// Calculate derived lengths of specific volumes
    active_length_      = gate_cathode_centre_dist_ - grid_thickn_/2.;
    light_tube_drift_length_ = light_tube_drift_end_z_ - light_tube_drift_start_z_;
    light_tube_buff_length_  = light_tube_buff_end_z_ - light_tube_buff_start_z_;
    bar_length_         = bar_end_z_ - bar_start_z_;

    /// Calculate derived positions in mother volume
    active_zpos_       = GetELzCoord() + active_length_/2.;
    cathode_grid_zpos_ = GetELzCoord() + gate_cathode_centre_dist_;

    if (verbosity_) {
      G4cout << "Active length = " << active_length_/mm << " mm" << G4endl;
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
    sc_yield_    = gas_->GetMaterialPropertiesTable()->GetConstProperty("SCINTILLATIONYIELD");
    e_lifetime_  = gas_->GetMaterialPropertiesTable()->GetConstProperty("ATTACHMENT");

    aluminum_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
    aluminum_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    /// Teflon for the light tube
    teflon_ =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
    teflon_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

    /// Quartz
    quartz_ =  materials::FusedSilica();
    quartz_->SetMaterialPropertiesTable(opticalprops::FusedSilica());

    /// TPB coating
    tpb_ = materials::TPB();
    tpb_->SetMaterialPropertiesTable(opticalprops::TPB());

    //ITO coating
    ito_ = materials::ITO();
    ito_->SetMaterialPropertiesTable(opticalprops::ITO());
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
    field->SetLifetime(e_lifetime_);
    G4Region* drift_region = new G4Region("DRIFT");
    drift_region->SetUserInformation(field);
    drift_region->AddRootLogicalVolume(active_logic);

    active_gen_ =
      new CylinderPointSampler(0., active_diam_/2., active_length_/2.,
                               0., twopi, nullptr,
                               G4ThreeVector(0., 0., active_zpos_));

    active_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }


  void NextDemoFieldCage::BuildCathodeGrid()
  {
    G4Material* cathode_mat =
      materials::FakeDielectric(gas_, "cathode_mat");
    cathode_mat->SetMaterialPropertiesTable(opticalprops::FakeGrid(pressure_,
                                                                   temperature_,
                                                                   cathode_transparency_,
                                                                   grid_thickn_));

    G4double cath_zplane[2] = {-grid_thickn_/2., grid_thickn_/2.};
    G4double rinner[2]      = {0., 0.};
    G4double router[2]      = {active_diam_/2., active_diam_/2.};

    G4Polyhedra* cathode_grid_solid =
      new G4Polyhedra("CATHODE_GRID", 0., twopi, 10, 2, cath_zplane, rinner, router);

    G4LogicalVolume* cathode_grid_logic =
      new G4LogicalVolume(cathode_grid_solid, cathode_mat, "CATHODE_GRID");

    new G4PVPlacement(0, G4ThreeVector(0., 0., cathode_grid_zpos_),
                      cathode_grid_logic, "CATHODE_GRID", mother_logic_,
                      false, 0, false);

    /// Visibilities
    if (visibility_) {
      G4VisAttributes grey = nexus::LightGrey();
      cathode_grid_logic->SetVisAttributes(grey);
    } else {
      cathode_grid_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }
  }


  void NextDemoFieldCage::BuildBuffer()
  {
    /// Position of z planes
    // The length of the BUFFER sensitive detector is shorter by the same amount
    // as the stand-out of copper rings around sapphire windows
    G4double buffer_sd_length = buffer_length_ - 1.5 * mm;
    G4double zplane[2] = {-buffer_sd_length/2., buffer_sd_length/2.};
    /// Inner radius
    G4double rinner[2] = {0., 0.};
    /// Outer radius
    G4double router[2] = {active_diam_/2., active_diam_/2.};

    G4Polyhedra* buffer_solid =
      new G4Polyhedra("BUFFER", 0., twopi, 10, 2, zplane, rinner, router);

    G4LogicalVolume* buffer_logic =
      new G4LogicalVolume(buffer_solid, gas_, "BUFFER");

    G4double buffer_zpos =
      active_zpos_ + active_length_/2. + grid_thickn_ + buffer_sd_length/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., buffer_zpos), buffer_logic,
                      "BUFFER", mother_logic_, false, 0, false);

    /// Set the volume as an ionization sensitive detector
    IonizationSD* buffsd = new IonizationSD("/NEXT100/BUFFER");
    buffsd->IncludeInTotalEnergyDeposit(false);
    buffer_logic->SetSensitiveDetector(buffsd);
    G4SDManager::GetSDMpointer()->AddNewDetector(buffsd);

    /// Visibilities
    buffer_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }


  void NextDemoFieldCage::BuildELRegion()
  {
    G4double el_gap_length = 0. * mm;
    if (config_ == "run5") {
      el_gap_length = el_gap_length_plate_;
    }
    else {
      el_gap_length = el_gap_length_mesh_;
    }

    G4Tubs* elgap_solid = new G4Tubs("EL_GAP", 0., elgap_ring_diam_/2.,
                                     el_gap_length/2., 0, twopi);
    G4LogicalVolume* elgap_logic =
      new G4LogicalVolume(elgap_solid, gas_, "EL_GAP");

    G4double el_gap_zpos = GetELzCoord() - el_gap_length/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., el_gap_zpos),
                      elgap_logic, "EL_GAP", mother_logic_, false, 0, false);

    if (elfield_) {
      UniformElectricDriftField* el_field = new UniformElectricDriftField();
      G4double global_el_gap_zpos = el_gap_zpos - GetELzCoord();
      el_field->SetCathodePosition(global_el_gap_zpos + el_gap_length/2.);
      el_field->SetAnodePosition(global_el_gap_zpos - el_gap_length/2.);
      el_field->SetDriftVelocity(2.5*mm/microsecond);
      el_field->SetTransverseDiffusion(ELtransv_diff_);
      el_field->SetLongitudinalDiffusion(ELlong_diff_);
      el_field->SetLightYield(XenonELLightYield(ELelectric_field_, pressure_));
      G4Region* el_region = new G4Region("EL_REGION");
      el_region->SetUserInformation(el_field);
      el_region->AddRootLogicalVolume(elgap_logic);
    }

    // Building the GATE
    G4Material* gate_mat = materials::FakeDielectric(gas_, "gate_mat");
    gate_mat->SetMaterialPropertiesTable(opticalprops::FakeGrid(pressure_,
                                                                temperature_,
                                                                gate_transparency_,
                                                                grid_thickn_,
                                                                sc_yield_,
                                                                1000*ms));

    G4Tubs* gate_grid_solid =
      new G4Tubs("GATE_GRID", 0., elgap_ring_diam_/2., grid_thickn_/2.,
                 0, twopi);

    G4LogicalVolume* gate_grid_logic =
      new G4LogicalVolume(gate_grid_solid, gate_mat, "GATE_GRID");

    G4double grid_zpos = el_gap_length/2. - grid_thickn_/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., grid_zpos), gate_grid_logic,
                      "GATE_GRID", elgap_logic, false, 0, false);

    /// Visibilities
    if (visibility_) {
      elgap_logic->SetVisAttributes(nexus::LightBlue());
      gate_grid_logic->SetVisAttributes(nexus::LightGrey());
    } else {
      elgap_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      gate_grid_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }


    // Building the ANODE plate corresponding to "run5" configuration
    if (config_ == "run5") {
      G4Tubs* quartz_anode_solid =
        new G4Tubs("ANODE_PLATE", 0., anode_diam_/2., anode_length_/2.,
                   0, twopi);
      G4LogicalVolume* quartz_anode_logic =
        new G4LogicalVolume(quartz_anode_solid, quartz_, "ANODE_PLATE");

      // A tiny offset is needed because EL is produced only if the PostStepVolume is GAS material.
      G4double anode_zpos = GetELzCoord() - el_gap_length - anode_length_/2. - 0.1*mm;
      new G4PVPlacement(0, G4ThreeVector(0., 0., anode_zpos), quartz_anode_logic,
                        "ANODE_PLATE", mother_logic_, false, 0, false);

      // Add TPB
      G4Tubs* tpb_anode_solid =
        new G4Tubs("TPB_ANODE", 0., anode_diam_/2., tpb_thickn_/2., 0, twopi);
      G4LogicalVolume* tpb_anode_logic =
        new G4LogicalVolume(tpb_anode_solid, tpb_, "TPB_ANODE");
      new G4PVPlacement(nullptr, G4ThreeVector(0., 0., anode_length_/2.-tpb_thickn_/2.),
                        tpb_anode_logic, "TPB_ANODE", quartz_anode_logic, false, 0, false);

      // Optical surface around TPB
      G4OpticalSurface* tpb_anode_surf =
        new G4OpticalSurface("TPB_ANODE_OPSURF", glisur, ground,
                             dielectric_dielectric, .01);
      new G4LogicalSkinSurface("TPB_ANODE_OPSURF", tpb_anode_logic, tpb_anode_surf);

      // Add ITO
      G4Tubs* ito_anode_solid =
        new G4Tubs("ITO_ANODE", 0., anode_diam_/2., ito_thickness_/2., 0, twopi);
      G4LogicalVolume* ito_anode_logic =
        new G4LogicalVolume(ito_anode_solid, ito_, "ITO_ANODE");
      new G4PVPlacement(nullptr, G4ThreeVector(0., 0., anode_length_/2.-tpb_thickn_-ito_thickness_/2.),
                        ito_anode_logic, "ITO_ANODE", quartz_anode_logic, false, 0, false);

      // Run5 Visibilities
      tpb_anode_logic   ->SetVisAttributes(G4VisAttributes::GetInvisible());
      ito_anode_logic   ->SetVisAttributes(G4VisAttributes::GetInvisible());
      if (visibility_) quartz_anode_logic->SetVisAttributes(nexus::Yellow());
      else             quartz_anode_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    // Building the ANODE grid corresponding to "run7" and "run8" configuration
    else {
      G4Material* anode_mat = materials::FakeDielectric(gas_, "anode_mat");
      anode_mat->SetMaterialPropertiesTable(opticalprops::FakeGrid(pressure_,
                                                                   temperature_,
                                                                   anode_transparency_,
                                                                   grid_thickn_,
                                                                   sc_yield_,
                                                                   1000*ms));
      G4Tubs* anode_grid_solid =
        new G4Tubs("ANODE_GRID", 0., elgap_ring_diam_/2., grid_thickn_/2., 0, twopi);

      G4LogicalVolume* anode_grid_logic =
        new G4LogicalVolume(anode_grid_solid, anode_mat, "ANODE_GRID");

      new G4PVPlacement(0, G4ThreeVector(0., 0., -grid_zpos), anode_grid_logic,
                        "ANODE_GRID", elgap_logic, false, 0, false);

      // Run7-Run8 Visibilities
      if (visibility_) anode_grid_logic->SetVisAttributes(nexus::LightGrey());
      else             anode_grid_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    // Vertex generator
    if (el_gap_gen_disk_zmin_ > el_gap_gen_disk_zmax_)
      G4Exception("[NextDemoFieldCage]", "NextDemoFieldCage()", FatalErrorInArgument,
                  "Error in configuration of EL gap generator: zmax < zmin");

    G4double el_gap_gen_disk_thickn =
      el_gap_length * (el_gap_gen_disk_zmax_ - el_gap_gen_disk_zmin_);

    G4double el_gap_gen_disk_z = el_gap_zpos + el_gap_length/2.
      - el_gap_length * el_gap_gen_disk_zmin_ - el_gap_gen_disk_thickn/2.;

    G4ThreeVector el_gap_gen_pos(el_gap_gen_disk_x_,
                                 el_gap_gen_disk_y_,
                                 el_gap_gen_disk_z);

    el_gap_gen_ = new CylinderPointSampler(0., el_gap_gen_disk_diam_/2.,
                                           el_gap_gen_disk_thickn/2., 0., twopi,
                                           nullptr, el_gap_gen_pos);
  }


  void NextDemoFieldCage::BuildFieldCage()
  {
    /// Drift light tube
    G4double drift_zplane[2] = {-light_tube_drift_length_/2., light_tube_drift_length_/2.};
    G4double rinner[2] = {active_diam_/2., active_diam_/2.};
    G4double router[2] =
      {active_diam_/2. + light_tube_thickn_, active_diam_/2. + light_tube_thickn_};

    G4Polyhedra* light_tube_drift_solid =
      new G4Polyhedra("LIGHT_TUBE_DRIFT", 0., twopi, 10, 2, drift_zplane,
                      rinner, router);
    G4LogicalVolume* light_tube_drift_logic =
      new G4LogicalVolume(light_tube_drift_solid, teflon_, "LIGHT_TUBE_DRIFT");

    G4double lt_drift_zpos =
      GetELzCoord() + light_tube_drift_start_z_ + light_tube_drift_length_/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., lt_drift_zpos),
                      light_tube_drift_logic, "LIGHT_TUBE_DRIFT", mother_logic_,
                      false, 0, false);

    G4double router_tpb[2] =
      {active_diam_/2. + tpb_thickn_, active_diam_/2. + tpb_thickn_};

    G4Polyhedra* tpb_drift_solid =
      new G4Polyhedra("DRIFT_TPB", 0., twopi, 10, 2, drift_zplane,
		      rinner, router_tpb);
    G4LogicalVolume* tpb_drift_logic =
      new G4LogicalVolume(tpb_drift_solid, tpb_, "DRIFT_TPB");
    G4VPhysicalVolume* tpb_drift_phys =
      new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), tpb_drift_logic,
		      "DRIFT_TPB", light_tube_drift_logic, false, 0, false);

    /// Buffer light tube
    G4double buff_zplane[2] = {-light_tube_buff_length_/2., light_tube_buff_length_/2.};
    G4Polyhedra* light_tube_buff_solid =
      new G4Polyhedra("LIGHT_TUBE_BUFFER", 0., twopi, 10, 2, buff_zplane,
                      rinner, router);
    G4LogicalVolume* light_tube_buff_logic =
      new G4LogicalVolume(light_tube_buff_solid, teflon_, "LIGHT_TUBE_BUFFER");

    G4double lt_buffer_zpos =
      GetELzCoord() + light_tube_buff_start_z_ + light_tube_buff_length_/2.;
    new G4PVPlacement(0, G4ThreeVector(0., 0., lt_buffer_zpos),
                      light_tube_buff_logic, "LIGHT_TUBE_BOTTOM", mother_logic_,
                      false, 0, false);

    G4Polyhedra* tpb_buff_solid =
      new G4Polyhedra("BUFFER_TPB", 0., twopi, 10, 2, buff_zplane,
		      rinner, router_tpb);
    G4LogicalVolume* tpb_buff_logic =
      new G4LogicalVolume(tpb_buff_solid, tpb_, "BUFFER_TPB");
    G4VPhysicalVolume* tpb_buff_phys =
      new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), tpb_buff_logic,
		      "BUFFER_TPB", light_tube_buff_logic, false, 0, false);

    /// Optical surfaces
    G4OpticalSurface* lt_drift_opsur = new G4OpticalSurface("LIGHT_TUBE_DRIFT");
    lt_drift_opsur->SetType(dielectric_metal);
    lt_drift_opsur->SetModel(unified);
    lt_drift_opsur->SetFinish(ground);
    lt_drift_opsur->SetSigmaAlpha(0.1);
    lt_drift_opsur->SetMaterialPropertiesTable(opticalprops::PTFE());
    new G4LogicalSkinSurface("LIGHT_TUBE_DRIFT",
                             light_tube_drift_logic, lt_drift_opsur);

    G4OpticalSurface* lt_buff_opsur = new G4OpticalSurface("LIGHT_TUBE_BUFFER");
    lt_buff_opsur->SetType(dielectric_metal);
    lt_buff_opsur->SetModel(unified);
    lt_buff_opsur->SetFinish(ground);
    lt_buff_opsur->SetSigmaAlpha(0.1);
    lt_buff_opsur->SetMaterialPropertiesTable(opticalprops::PTFE());
    new G4LogicalSkinSurface("LIGHT_TUBE_BUFFER",
                             light_tube_buff_logic, lt_buff_opsur);

    G4OpticalSurface* gas_tpb_teflon_surf =
      new G4OpticalSurface("GAS_TPB_TEFLON_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);

    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", tpb_drift_phys,
                               mother_phys_, gas_tpb_teflon_surf);
    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", mother_phys_,
                               tpb_drift_phys, gas_tpb_teflon_surf);
    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", tpb_buff_phys,
                               mother_phys_, gas_tpb_teflon_surf);
    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", mother_phys_,
                               tpb_buff_phys, gas_tpb_teflon_surf);


    /// Visibilities
    tpb_drift_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    tpb_buff_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    if (visibility_) {
      G4VisAttributes teflon_col = nexus::White();
      teflon_col.SetForceSolid(true);
      light_tube_drift_logic->SetVisAttributes(teflon_col);
      light_tube_buff_logic->SetVisAttributes(teflon_col);
    } else {
      light_tube_drift_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      light_tube_buff_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

    /// Field shaping rings
    G4Tubs* ring_solid = new G4Tubs("FIELD_RING", ring_diam_/2.,
                                    (ring_diam_/2.+ring_thickn_),
                                    ring_height_/2., 0, twopi);
    G4LogicalVolume* ring_logic =
      new G4LogicalVolume(ring_solid, aluminum_, "FIELD_RING");

    G4double posz = GetELzCoord() + dist_gate_first_ring_ + ring_height_/2.;
    for (G4int i=0; i<num_drift_rings_; i++) {
      new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
                        "FIELD_RING", mother_logic_, false, i, false);

      posz = posz + dist_drift_ring_centres_;
    }

    /// Buffer part
    posz = posz - dist_drift_ring_centres_ + ring_height_/2. +
      ring_drift_buff_gap_ + ring_height_/2.;
    for (G4int i=num_drift_rings_; i<num_rings_; i++) {
      new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
                        "FIELD_RING", mother_logic_, false, i, false);
      posz = posz + dist_buff_ring_centres_;
    }

    /// Visibilities
    if (visibility_) {
      G4VisAttributes yellow_col =nexus::Yellow();
      ring_logic->SetVisAttributes(yellow_col);
    }
    else {
      ring_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }


    /// SUPPORT BARS
    // This volume is simplified to a parallelepiped made of polyethilene.
    G4Box* bar_solid = new G4Box("SUPPORT_BAR", bar_width_/2.,
                                 bar_thickn_/2., bar_length_/2.);
    G4LogicalVolume* bar_logic =
      new G4LogicalVolume(bar_solid, materials::HDPE(), "SUPPORT_BAR");

    G4double bar_rpos = active_diam_/2. + light_tube_thickn_ + bar_thickn_/2.;
    G4double bar_zpos = GetELzCoord() + bar_start_z_ + bar_length_/2.;

    G4RotationMatrix rotbar;
    G4ThreeVector bar_pos(0., bar_rpos, bar_zpos);

    G4int n_bars = 10;
    G4double step = 360 / n_bars;
    for (G4int i=0; i<n_bars; i++) {
      G4double angle = (i*step)*deg;
      if (i!=0) rotbar.rotateZ(step*deg);
      bar_pos.setX(-bar_rpos * sin(angle));
      bar_pos.setY(bar_rpos * cos(angle));
      new G4PVPlacement(G4Transform3D(rotbar, bar_pos),
                        bar_logic, "SUPPORT_BAR", mother_logic_, false, i, false);
    }

    if (visibility_) {
      G4VisAttributes lilla_col =nexus::Lilla();
      lilla_col.SetForceSolid(true);
      bar_logic->SetVisAttributes(lilla_col);
    }
    else {
      bar_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }

  }


  G4ThreeVector NextDemoFieldCage::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

     if (region == "ACTIVE") {
       G4VPhysicalVolume *VertexVolume;
       do {
         vertex = active_gen_->GenerateVertex(VOLUME);
         G4ThreeVector glob_vtx(vertex);
         glob_vtx = glob_vtx + G4ThreeVector(0, 0, -GetELzCoord());
         VertexVolume =
           geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
       } while (VertexVolume->GetName() != region);
     }
     else if (region == "EL_GAP") {
       vertex = el_gap_gen_->GenerateVertex(VOLUME);
     }
     else {
      G4Exception("[NextDemoFieldCage]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }
}
