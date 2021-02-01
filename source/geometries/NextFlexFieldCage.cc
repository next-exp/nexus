// -----------------------------------------------------------------------------
//  nexus | NextFlexFieldCage.cc
//
//  NEXT-Flex Field Cage geometry for performance studies.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextFlexFieldCage.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "XenonGasProperties.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "CylinderPointSampler2020.h"
#include "GenericPhotosensor.h"
#include "PmtSD.h"
#include "Visibilities.h"

#include <G4UnitsTable.hh>
#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4SDManager.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UserLimits.hh>
#include <G4Transform3D.hh>


using namespace nexus;


NextFlexFieldCage::NextFlexFieldCage():
  BaseGeometry(),
  mother_logic_            (nullptr),
  verbosity_               (false),
  visibility_              (false),
  fiber_sensor_visibility_ (false),
  msg_                     (nullptr),
  fc_with_fibers_          (true), 
  active_diam_             (984. * mm),          // Same as NEXT100 (something btwn 1000 & 984 mm)
  active_length_           (116. * cm),          // Distance GATE - CATHODE (meshes not included)
  drift_transv_diff_       (1. * mm/sqrt(cm)),   // Drift field transversal diffusion
  drift_long_diff_         (.3 * mm/sqrt(cm)),   // Drift field longitudinal diffusion
  cathode_transparency_    (0.95),               // Cathode transparency
  buffer_length_           (280. * mm),          // Distance CATHODE - sapphire window surfaces
  el_gap_length_           (10. * mm),           // Distance ANODE - GATE (meshes included)
  el_field_on_             (false),              // EL field ON-OFF
  el_field_int_            (16.0 * kilovolt/cm), // EL field intensity
  el_transv_diff_          (0. * mm/sqrt(cm)),   // EL field transversal diffusion
  el_long_diff_            (0. * mm/sqrt(cm)),   // EL field longitudinal diffusion
  anode_transparency_      (0.95),               // Anode transparency
  gate_transparency_       (0.95),               // Gate transparency
  photoe_prob_             (0),                  // OpticalPhotoElectric Probability
  fiber_claddings_         (2),                  // Number of fiber claddings (0, 1 or 2)
  fiber_sensor_binning_    (100. * ns),          // Size of fiber sensors time binning
  wls_mat_name_            ("TPB"),              // UV wls material name
  fiber_mat_name_          ("EJ280"),            // Fiber core material name
  el_gap_gen_disk_diam_    (0.),                 // EL_GAP generator diameter
  el_gap_gen_disk_x_       (0.),                 // EL_GAP generator x centre
  el_gap_gen_disk_y_       (0.),                 // EL_GAP generator y centre
  el_gap_gen_disk_zmin_    (0.),                 // EL_GAP generator initial z fraction 
  el_gap_gen_disk_zmax_    (1.)                  // EL_GAP generator final   z fraction
{

  // Messenger
  msg_ = new G4GenericMessenger(this, "/Geometry/NextFlex/",
                                "Control commands of the NextFlex geometry.");

  // Define new categories
  new G4UnitDefinition("kilovolt/cm", "kV/cm", "Electric field", kilovolt/cm);
  new G4UnitDefinition("mm/sqrt(cm)", "mm/sqrt(cm)", "Diffusion", mm/sqrt(cm));

  // Parametrized dimensions
  DefineConfigurationParameters();

  // Hard-wired dimensions
  cathode_thickness_      = 0.1  * mm;
  anode_thickness_        = 0.1  * mm;
  gate_thickness_         = 0.1  * mm;

  light_tube_thickness_   = 5.   * mm;
  wls_thickness_          = 1.   * um;

  fiber_thickness_        = 2.  * mm;
  cladding_perc_          = 0.02;       // Fiber thickness perc devoted to each cladding
  fiber_extra_length_     = 2.  * cm;   // Extra length per side of fibers respect FC length
  fiber_light_tube_gap_   = 2.  * mm;   // Separation gap between fibers & light tube
  fiber_sensor_size_      = 2.  * mm;   // Side length of squared fiber sensors
                                        // (ideally equal to fiber_thickness_)
  fiber_sensor_thickness_ = 2.  * mm;   // (Thickness set to a fix value of 2 mm)
}



NextFlexFieldCage::~NextFlexFieldCage()
{
  delete msg_;

  delete active_gen_;
  delete buffer_gen_;
  delete el_gap_gen_;
  delete light_tube_gen_;
  delete fiber_gen_;
}



void NextFlexFieldCage::DefineConfigurationParameters()
{
  // Verbosity
  msg_->DeclareProperty("fc_verbosity", verbosity_, "Verbosity");

  // Visibilities
  msg_->DeclareProperty("fc_visibility", visibility_, "FIELD_CAGE visibility");

  msg_->DeclareProperty("fiber_sensor_visibility", fiber_sensor_visibility_,
                        "Fiber sensors visibility");

  // FIELD_CAGE configuration
  msg_->DeclareProperty("fc_with_fibers", fc_with_fibers_, "FIELD_CAGE with fibers");

  // ACTIVE dimensions
  G4GenericMessenger::Command& active_length_cmd =
    msg_->DeclareProperty("active_length", active_length_,
                          "Length of the ACTIVE volume.");
  active_length_cmd.SetParameterName("active_length", false);
  active_length_cmd.SetUnitCategory("Length");
  active_length_cmd.SetRange("active_length>0.");

  G4GenericMessenger::Command& active_diam_cmd =
    msg_->DeclareProperty("active_diam", active_diam_,
                          "Diameter of the ACTIVE volume.");
  active_diam_cmd.SetParameterName("active_diam", false);
  active_diam_cmd.SetUnitCategory("Length");
  active_diam_cmd.SetRange("active_diam>0.");

  G4GenericMessenger::Command& drift_transv_diff_cmd =
    msg_->DeclareProperty("drift_transv_diff", drift_transv_diff_,
                          "Tranvsersal diffusion in the drift region");
  drift_transv_diff_cmd.SetParameterName("drift_transv_diff", false);
  drift_transv_diff_cmd.SetUnitCategory("Diffusion");

  G4GenericMessenger::Command& drift_long_diff_cmd =
    msg_->DeclareProperty("drift_long_diff", drift_long_diff_,
                          "Longitudinal diffusion in the drift region");
  drift_long_diff_cmd.SetParameterName("drift_long_diff", false);
  drift_long_diff_cmd.SetUnitCategory("Diffusion");


  // FIELD_CAGE dimensions
  G4GenericMessenger::Command& buffer_length_cmd =
    msg_->DeclareProperty("buffer_length", buffer_length_,
                          "Length of the BUFFER.");
  buffer_length_cmd.SetParameterName("buffer_length", false);
  buffer_length_cmd.SetUnitCategory("Length");
  buffer_length_cmd.SetRange("buffer_length>=0.");

  G4GenericMessenger::Command& el_gap_length_cmd =
    msg_->DeclareProperty("el_gap_length", el_gap_length_,
                          "Length of the EL gap.");
  el_gap_length_cmd.SetParameterName("el_gap_length", false);
  el_gap_length_cmd.SetUnitCategory("Length");
  el_gap_length_cmd.SetRange("el_gap_length>=0.");

  msg_->DeclareProperty("el_field_on", el_field_on_,
                        "true for full simulation), false if fast or parametrized.");

  G4GenericMessenger::Command& el_field_int_cmd =
    msg_->DeclareProperty("el_field_int", el_field_int_,
                          "EL electric field intensity");
  el_field_int_cmd.SetParameterName("el_field_int", false);
  el_field_int_cmd.SetUnitCategory("Electric field");

  G4GenericMessenger::Command& el_transv_diff_cmd =
    msg_->DeclareProperty("el_transv_diff", el_transv_diff_,
                          "Tranvsersal diffusion in the EL region");
  el_transv_diff_cmd.SetParameterName("el_transv_diff", false);
  el_transv_diff_cmd.SetUnitCategory("Diffusion");

  G4GenericMessenger::Command& el_long_diff_cmd =
    msg_->DeclareProperty("el_long_diff", el_long_diff_,
                          "Longitudinal diffusion in the EL region");
  el_long_diff_cmd.SetParameterName("el_long_diff", false);
  el_long_diff_cmd.SetUnitCategory("Diffusion");

  // TRANSPARENCIES
  msg_->DeclareProperty("cathode_transparency", cathode_transparency_,
                        "Cathode transparency");

  msg_->DeclareProperty("anode_transparency", anode_transparency_,
                        "Anode transparency");

  msg_->DeclareProperty("gate_transparency", gate_transparency_,
                        "Gate transparency");

  // PHOTOELECTRIC PROBABILITY
  msg_->DeclareProperty("photoe_prob", photoe_prob_,
                        "Probability of optical photon to ie- conversion");

  // LIGHT TUBE
  msg_->DeclareProperty("fc_wls_mat", wls_mat_name_,
                        "FIELD_CAGE UV wavelength shifting material name");

  // FIBERS
  msg_->DeclareProperty("fiber_mat", fiber_mat_name_, "Fiber core material.");

  G4GenericMessenger::Command& fiber_claddings_cmd =
    msg_->DeclareProperty("fiber_claddings", fiber_claddings_,
                          "Number of fiber claddings.");
  fiber_claddings_cmd.SetParameterName("fiber_claddings", false);
  fiber_claddings_cmd.SetRange("fiber_claddings>=0 && fiber_claddings<=2");

  G4GenericMessenger::Command& fiber_sensor_binning_cmd =
    msg_->DeclareProperty("fiber_sensor_time_binning", fiber_sensor_binning_,
                          "Time bin size of fiber sensors.");
  fiber_sensor_binning_cmd.SetParameterName("fiber_sensor_time_binning", false);
  fiber_sensor_binning_cmd.SetUnitCategory("Time");
  fiber_sensor_binning_cmd.SetRange("fiber_sensor_time_binning>=0.");

  // EL_GAP GENERATOR
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
  el_gap_gen_disk_zmax_cmd.SetRange("el_gap_gen_disk_zmax>=0. && el_gap_gen_disk_zmax<=1.0");
}



void NextFlexFieldCage::ComputeDimensions()
{
  // The field cage goes along (ACTIVE + CATHODE + BUFFER)
  fc_length_ = active_length_ + cathode_thickness_ + buffer_length_;

  fiber_inner_rad_      = active_diam_/2.;
  light_tube_inner_rad_ = active_diam_/2.;

  // Shifting placements in case of extra volumes
  if (fc_with_fibers_) {
    light_tube_inner_rad_ += fiber_thickness_;
    light_tube_inner_rad_ += fiber_light_tube_gap_;
  }
}



void NextFlexFieldCage::DefineMaterials()
{
  // Xenon
  // Read gas properties from mother volume
  xenon_gas_       = mother_logic_->GetMaterial();
  gas_pressure_    = xenon_gas_->GetPressure();
  gas_temperature_ = xenon_gas_->GetTemperature();
  gas_e_lifetime_  = xenon_gas_->GetMaterialPropertiesTable()
                               ->GetConstProperty("ATTACHMENT");
  gas_sc_yield_    = xenon_gas_->GetMaterialPropertiesTable()
                               ->GetConstProperty("SCINTILLATIONYIELD");

  // Teflon
  teflon_mat_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

  // UV shifting material
  if (wls_mat_name_ == "NONE") {
    wls_mat_ = mother_logic_->GetMaterial();
  }
  else if (wls_mat_name_ == "TPB") {
    wls_mat_ = MaterialsList::TPB();
    wls_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
  }
  else if (wls_mat_name_ == "TPH") {
    wls_mat_ = MaterialsList::TPH();
    wls_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::TPH());
  }
  else {
    G4Exception("[NextFlexFieldCage]", "DefineMaterials()", FatalException,
                "Unknown UV shifting material. Valid options are NONE, TPB or TPH.");
  }

  // Meshes materials
  cathode_mat_ = MaterialsList::FakeDielectric(xenon_gas_, "cathode_mat");
  cathode_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(gas_pressure_,
                gas_temperature_, cathode_transparency_, cathode_thickness_,
                gas_sc_yield_, gas_e_lifetime_, photoe_prob_));

  gate_mat_ = MaterialsList::FakeDielectric(xenon_gas_, "gate_mat");
  gate_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(gas_pressure_,
             gas_temperature_, gate_transparency_, gate_thickness_,
             gas_sc_yield_, gas_e_lifetime_, photoe_prob_));

  anode_mat_ = MaterialsList::FakeDielectric(xenon_gas_, "anode_mat");
  anode_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(gas_pressure_,
              gas_temperature_, anode_transparency_, anode_thickness_,
              gas_sc_yield_, gas_e_lifetime_, photoe_prob_));


  // Fiber core material
  if (fiber_mat_name_ == "EJ280") {
    fiber_mat_ = MaterialsList::EJ280();
    fiber_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::EJ280());
  }
  else if (fiber_mat_name_ == "EJ286") {
    fiber_mat_ = MaterialsList::EJ280();   // Same base material than EJ280
    fiber_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::EJ286());
  }
  else if (fiber_mat_name_ == "Y11") {
    fiber_mat_ = MaterialsList::Y11();
    fiber_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::Y11());
  }
  else {
    G4Exception("[NextFlexFieldCage]", "DefineMaterials()", FatalException,
    "Unknown inner WLS material. Valid options are EJ280, EJ286 or Y11.");
  }

  // Fiber cladding materials (inner: PMMA  outer: FPethylene)
  oClad_mat_ = MaterialsList::FPethylene();
  oClad_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::FPethylene());

  iClad_mat_ = MaterialsList::PMMA();
  iClad_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::PMMA());
}



void NextFlexFieldCage::Construct()
{
  // Make sure that the pointer to the mother volume is actually defined
  if (!mother_logic_)
    G4Exception("[NextFlexFieldCage]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  // Verbosity
  if(verbosity_) G4cout << G4endl << "*** NEXT-Flex Field Cage ..." << G4endl;

  // Getting volumes dimensions based on parameters.
  ComputeDimensions();

  // Define materials.
  DefineMaterials();

  // Build the sensors 
  left_sensor_  = new GenericPhotosensor("F_SENSOR_L", fiber_sensor_size_,
                                         fiber_sensor_size_, fiber_sensor_thickness_);
  right_sensor_ = new GenericPhotosensor("F_SENSOR_R", fiber_sensor_size_,
                                         fiber_sensor_size_, fiber_sensor_thickness_);

  // Build components
  BuildActive();
  
  BuildCathode();
  
  BuildBuffer();

  BuildELgap();

  BuildLightTube();

  // If with fibers, build fibers and corresponding detectors
  if (fc_with_fibers_) {
    BuildFibers();
    BuildFiberSensors();
  }
}



void NextFlexFieldCage::BuildActive()
{
  G4String active_name = "ACTIVE";

  G4Tubs* active_solid =
    new G4Tubs(active_name, 0., active_diam_/2., active_length_/2., 0., 360.*deg);

  G4LogicalVolume* active_logic =
    new G4LogicalVolume(active_solid, xenon_gas_, active_name);

  active_phys_ =
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,active_length_/2.),
                      active_logic, active_name, mother_logic_,
                      false, 0, verbosity_);

  // Define the drift field
  UniformElectricDriftField* field = new UniformElectricDriftField();
  field->SetCathodePosition(active_length_);
  field->SetAnodePosition(0.);
  field->SetDriftVelocity(1. * mm/microsecond);
  field->SetTransverseDiffusion(drift_transv_diff_);
  field->SetLongitudinalDiffusion(drift_long_diff_);
  G4Region* drift_region = new G4Region("DRIFT");
  drift_region->SetUserInformation(field);
  drift_region->AddRootLogicalVolume(active_logic);

  // Visibility
  active_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  active_gen_ = new CylinderPointSampler2020(active_phys_);

  // Limit the step size in this volume for better tracking precision
  active_logic->SetUserLimits(new G4UserLimits(1.*mm));
  
  // Set the ACTIVE volume as an ionization sensitive detector
  IonizationSD* active_sd = new IonizationSD("/NEXT_FLEX/ACTIVE");
  active_logic->SetSensitiveDetector(active_sd);
  G4SDManager::GetSDMpointer()->AddNewDetector(active_sd);

  /// Verbosity ///
  if (verbosity_) {
    G4cout << "* ACTIVE Rad: " << active_diam_/2. << G4endl;
    G4cout << "* ACTIVE Z positions: 0 to " << active_length_ << G4endl;
  }
}



void NextFlexFieldCage::BuildCathode()
{
  G4String cathode_name = "CATHODE";

  G4double cathode_diam = active_diam_;
  G4double cathode_posZ = active_length_ + cathode_thickness_/2.;

  G4Tubs* cathode_solid =
    new G4Tubs(cathode_name, 0., cathode_diam/2., cathode_thickness_/2., 0, twopi);

  G4LogicalVolume* cathode_logic =
    new G4LogicalVolume(cathode_solid, cathode_mat_, cathode_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., cathode_posZ), cathode_logic,
                    cathode_name, mother_logic_, false, 0, verbosity_);

  // Visibility
  if (visibility_) cathode_logic->SetVisAttributes(nexus::LightBlue());
  else             cathode_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Verbosity
  if (verbosity_) 
    G4cout << "* CATHODE Z positions: " << cathode_posZ - cathode_thickness_/2. <<
              " to " << cathode_posZ + cathode_thickness_/2. << G4endl;
}



void NextFlexFieldCage::BuildBuffer()
{
  G4String buffer_name = "BUFFER";

  G4double buffer_diam = active_diam_;
  G4double buffer_posZ = active_length_ + cathode_thickness_ + buffer_length_/2.;
  buffer_finalZ_ = buffer_posZ + buffer_length_/2.;

  G4Tubs* buffer_solid =
    new G4Tubs(buffer_name, 0., buffer_diam/2., buffer_length_/2., 0, twopi);

  G4LogicalVolume* buffer_logic =
    new G4LogicalVolume(buffer_solid, xenon_gas_, buffer_name);

  buffer_phys_ =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., buffer_posZ), buffer_logic,
                      buffer_name, mother_logic_, false, 0, verbosity_);

  // Visibility
  buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  buffer_gen_ = new CylinderPointSampler2020(buffer_phys_);

  // Set the BUFFER volume as an ionization sensitive detector
  IonizationSD* buffer_sd = new IonizationSD("/NEXT_FLEX/BUFFER");
  buffer_sd->IncludeInTotalEnergyDeposit(false);
  buffer_logic->SetSensitiveDetector(buffer_sd);
  G4SDManager::GetSDMpointer()->AddNewDetector(buffer_sd);  

  // Verbosity
  if (verbosity_)
    G4cout << "* BUFFER Z positions: " << buffer_posZ - buffer_length_/2. <<
              " to " << buffer_posZ + buffer_length_/2. << G4endl;
}



void NextFlexFieldCage::BuildELgap()
{
  /// EL_GAP ///
  G4String el_gap_name = "EL_GAP";

  G4double el_gap_diam = active_diam_;
  G4double el_gap_posZ = - el_gap_length_/2.;

  G4Tubs* el_gap_solid =
    new G4Tubs(el_gap_name, 0., el_gap_diam/2., el_gap_length_/2., 0, twopi);

  G4LogicalVolume* el_gap_logic =
    new G4LogicalVolume(el_gap_solid, xenon_gas_, el_gap_name);

  G4VPhysicalVolume* el_gap_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., el_gap_posZ), el_gap_logic,
                      el_gap_name, mother_logic_, false, 0, verbosity_);

  // Define EL electric field
  XenonGasProperties xgp(gas_pressure_, gas_temperature_);
  G4double yield = xgp.ELLightYield(el_field_int_);
  if (el_field_on_) {
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    el_field->SetCathodePosition(el_gap_posZ + el_gap_length_/2.);
    el_field->SetAnodePosition  (el_gap_posZ - el_gap_length_/2.);
    el_field->SetDriftVelocity  (2.5 * mm/microsecond);
    el_field->SetTransverseDiffusion(el_transv_diff_);
    el_field->SetLongitudinalDiffusion(el_long_diff_);
    el_field->SetLightYield(yield);
    G4Region* el_region = new G4Region("EL_REGION");
    el_region->SetUserInformation(el_field);
    el_region->AddRootLogicalVolume(el_gap_logic);
  }

  // Visibility
  if (visibility_) el_gap_logic->SetVisAttributes(nexus::LightBlue());
  else             el_gap_logic->SetVisAttributes(G4VisAttributes::Invisible);




  // Vertex generator
  //el_gap_gen_ = new CylinderPointSampler2020(el_gap_phys);

  G4double el_gap_gen_disk_thickn =
    el_gap_length_ * (el_gap_gen_disk_zmax_ - el_gap_gen_disk_zmin_);

  G4double el_gap_gen_disk_z = el_gap_posZ + el_gap_length_/2.
    - el_gap_length_ * el_gap_gen_disk_zmin_ - el_gap_gen_disk_thickn/2.;

  G4ThreeVector el_gap_gen_pos(el_gap_gen_disk_x_,
                               el_gap_gen_disk_y_,
                               el_gap_gen_disk_z);

  el_gap_gen_ = new CylinderPointSampler2020(0., el_gap_gen_disk_diam_/2.,
                                             el_gap_gen_disk_thickn/2., 0., twopi,
                                             nullptr, el_gap_gen_pos);







  // Verbosity
  if (verbosity_) {
    G4cout << "* EL_GAP Z positions: " << el_gap_posZ - el_gap_length_/2. <<
              " to " << el_gap_posZ + el_gap_length_/2. << G4endl;

    if (el_field_on_) {
      G4cout << "* EL field intensity (kV/cm): " << el_field_int_ / (kilovolt/cm)
             << "  ->  EL Light yield (photons/ie-/cm): " << yield / (1/cm) << G4endl;
      G4cout << "* EL Light yield (photons/ie-): " << yield * el_gap_length_ << G4endl;
    }
    else
      G4cout << "* EL field OFF" << G4endl;

    G4cout << "* Photoelectric Probability: " << photoe_prob_ << G4endl;    
  }


  /// GATE ///
  G4String gate_name = "GATE";

  G4double gate_diam = active_diam_;
  G4double gate_posZ = el_gap_length_/2. - gate_thickness_/2.;

  G4Tubs* gate_solid =
    new G4Tubs(gate_name, 0., gate_diam/2., gate_thickness_/2., 0, twopi);

  G4LogicalVolume* gate_logic =
    new G4LogicalVolume(gate_solid, gate_mat_, gate_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., gate_posZ), gate_logic,
                    gate_name, el_gap_logic, false, 0, verbosity_);

  // Visibility
  gate_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Verbosity
  if (verbosity_) G4cout << "* GATE Z position: " << el_gap_posZ + gate_posZ << G4endl;


  /// ANODE ///
  G4String anode_name = "ANODE";

  G4double anode_diam = active_diam_;
  G4double anode_posZ = -el_gap_length_/2. + anode_thickness_/2.;

  G4Tubs* anode_solid =
    new G4Tubs(anode_name, 0., anode_diam/2., anode_thickness_/2., 0, twopi);

  G4LogicalVolume* anode_logic =
    new G4LogicalVolume(anode_solid, anode_mat_, anode_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., anode_posZ), anode_logic,
                    anode_name, el_gap_logic, false, 0, verbosity_);

  // Visibility
  anode_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Verbosity
  if (verbosity_) G4cout << "* ANODE Z position: " << el_gap_posZ + anode_posZ << G4endl;
}



void NextFlexFieldCage::BuildLightTube()
{
  /// LIGHT_TUBE ///
  G4String light_tube_name = "LIGHT_TUBE";

  light_tube_outer_rad_ = light_tube_inner_rad_ + light_tube_thickness_;
  
//  G4double light_tube_iniZ = - el_gap_length_;
  G4double light_tube_iniZ = 0.;

  G4double light_tube_posZ = light_tube_iniZ + fc_length_/2.;

  G4Tubs* light_tube_solid =
    new G4Tubs(light_tube_name, light_tube_inner_rad_, light_tube_outer_rad_,
               fc_length_/2., 0, twopi);

  G4LogicalVolume* light_tube_logic =
    new G4LogicalVolume(light_tube_solid, teflon_mat_, light_tube_name);

  G4VPhysicalVolume* light_tube_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., light_tube_posZ), light_tube_logic,
                      light_tube_name, mother_logic_, false, 0, verbosity_);

  // Adding the optical surface
  G4OpticalSurface* light_tube_optSurf = 
    new G4OpticalSurface(light_tube_name, unified, ground, dielectric_metal);
  light_tube_optSurf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  new G4LogicalSkinSurface(light_tube_name, light_tube_logic, light_tube_optSurf);

  // Visibility
  if (visibility_) {
    G4VisAttributes light_blue_col = nexus::LightBlue();
    //light_blue_col.SetForceSolid(true);
    light_tube_logic->SetVisAttributes(light_blue_col);
  }
  else light_tube_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  light_tube_gen_ = new CylinderPointSampler2020(light_tube_phys);


  /// The UV wavelength Shifter in LIGHT_TUBE ///
  // Only if there are no fibers barrel
  if (not fc_with_fibers_) {
    G4String light_tube_wls_name = "LIGHT_TUBE_WLS";
  
    G4double light_tube_wls_length    = fc_length_;
    G4double light_tube_wls_inner_rad = light_tube_inner_rad_;
    G4double light_tube_wls_outer_rad = light_tube_wls_inner_rad + wls_thickness_;
  
    G4Tubs* light_tube_wls_solid =
      new G4Tubs(light_tube_wls_name, light_tube_wls_inner_rad, light_tube_wls_outer_rad,
                 light_tube_wls_length/2., 0, twopi);
  
    G4LogicalVolume* light_tube_wls_logic =
      new G4LogicalVolume(light_tube_wls_solid, wls_mat_, light_tube_wls_name);
  
    G4VPhysicalVolume* light_tube_wls_phys =
      new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), light_tube_wls_logic,
                        light_tube_wls_name, light_tube_logic, false, 0, verbosity_);
  
    // Visibility
    light_tube_wls_logic->SetVisAttributes(G4VisAttributes::Invisible);
  
    // Optical surface
    G4OpticalSurface* light_tube_wls_optSurf =
      new G4OpticalSurface("LIGHT_TUBE_WLS_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);

    // Border Surface with ACTIVE
    new G4LogicalBorderSurface("LIGHT_TUBE_WLS_ACTIVE_OPSURF", light_tube_wls_phys,
                               active_phys_, light_tube_wls_optSurf);
    new G4LogicalBorderSurface("ACTIVE_LIGHT_TUBE_WLS_OPSURF", active_phys_,
                               light_tube_wls_phys, light_tube_wls_optSurf);

    // Border Surface with BUFFER
    new G4LogicalBorderSurface("LIGHT_TUBE_WLS_BUFFER_OPSURF", light_tube_wls_phys,
                               buffer_phys_, light_tube_wls_optSurf);
    new G4LogicalBorderSurface("BUFFER_LIGHT_TUBE_WLS_OPSURF", buffer_phys_,
                               light_tube_wls_phys, light_tube_wls_optSurf);
  }

  /// Verbosity ///
  if (verbosity_) {
    G4cout << "* Light Tube.  Inner Rad: " << light_tube_inner_rad_ << 
              "   Outer Rad: " << light_tube_outer_rad_ << G4endl;
    G4cout << "* Light Tube Z positions: " << light_tube_iniZ <<
              " to " << light_tube_iniZ + fc_length_ << G4endl;
  } 
}



void NextFlexFieldCage::BuildFibers()
{
  // Number of fibers
  num_fibers_   = (G4int) (twopi * fiber_inner_rad_ / fiber_thickness_);

  // Radius of the different volumes being built
  G4double inner_rad    = fiber_inner_rad_;
  G4double outer_rad    = fiber_inner_rad_ + fiber_thickness_;

  // Lengths & positions
  G4double fiber_length = fc_length_ +  2. * fiber_extra_length_;
  fiber_iniZ_           = - el_gap_length_ - fiber_extra_length_;
  fiber_finZ_           = fiber_iniZ_ + fiber_length;
  G4double fiber_posZ   = fiber_iniZ_ + fiber_length/2.;

  // Most out / inn volumes
  G4LogicalVolume* out_logic_volume = mother_logic_;
  G4LogicalVolume* inn_logic_volume = mother_logic_;

  /// Outer Cladding ///
  if (fiber_claddings_ >= 2) {

    G4String oClad_name = "FIBER_OCLAD";

    G4Tubs* oClad_solid =
      new G4Tubs(oClad_name, inner_rad, outer_rad, fiber_length/2.,
                 0., 360.*deg);

    G4LogicalVolume* oClad_logic =
      new G4LogicalVolume(oClad_solid, oClad_mat_, oClad_name);

    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,fiber_posZ),
                      oClad_logic, oClad_name, inn_logic_volume,
                      false, 0, verbosity_);

    // Visibility
    oClad_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Updating info
    out_logic_volume = oClad_logic;
    inn_logic_volume = oClad_logic;
    fiber_posZ       = 0.;
    inner_rad       += fiber_thickness_ * cladding_perc_;
    outer_rad       -= fiber_thickness_ * cladding_perc_;
  }


  /// Inner Cladding ///
  if (fiber_claddings_ >= 1) {

    G4String iClad_name = "FIBER_ICLAD";
    if (fiber_claddings_ == 1) iClad_name = "FIBER_CLAD";

    G4Tubs* iClad_solid =
      new G4Tubs(iClad_name, inner_rad, outer_rad, fiber_length/2.,
                 0., 360.*deg);

    G4LogicalVolume* iClad_logic =
      new G4LogicalVolume(iClad_solid, iClad_mat_, iClad_name);

    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,fiber_posZ),
                      iClad_logic, iClad_name, inn_logic_volume,
                      false, 0, verbosity_);

    // Visibility
    iClad_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Updating info
    if (fiber_claddings_ == 1) out_logic_volume = iClad_logic;
    inn_logic_volume = iClad_logic;
    fiber_posZ       = 0.;
    inner_rad       += fiber_thickness_ * cladding_perc_;
    outer_rad       -= fiber_thickness_ * cladding_perc_;
  }


  /// Fiber core ///
  G4String core_name = "FIBER_CORE";

  G4Tubs* core_solid =
    new G4Tubs(core_name, inner_rad, outer_rad, fiber_length/2.,
               0., 360.*deg);

  G4LogicalVolume* core_logic =
    new G4LogicalVolume(core_solid, fiber_mat_, core_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,fiber_posZ),
                    core_logic, core_name, inn_logic_volume,
                    false, 0, verbosity_);

  // Visibility
  if (visibility_) core_logic->SetVisAttributes(nexus::LightGreen());
  else             core_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Updating info
  if (fiber_claddings_ == 0) out_logic_volume = core_logic;

  // Vertex generator
  fiber_gen_ = new CylinderPointSampler2020(inner_rad, outer_rad, fiber_length/2., 0., twopi, nullptr,
                                            G4ThreeVector(0., 0., fiber_iniZ_ + fiber_length/2.));


  /// The UV wavelength Shifter in FIBERS ///
  // The inner WLS
  G4String fiber_iWls_name = "FIBER_IWLS";

  G4double fiber_iWls_inner_rad = fiber_inner_rad_;
  G4double fiber_iWls_outer_rad = fiber_iWls_inner_rad + wls_thickness_;

  G4Tubs* fiber_iWls_solid =
    new G4Tubs(fiber_iWls_name, fiber_iWls_inner_rad, fiber_iWls_outer_rad,
               fiber_length/2., 0, twopi);

  G4LogicalVolume* fiber_iWls_logic =
    new G4LogicalVolume(fiber_iWls_solid, wls_mat_, fiber_iWls_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), fiber_iWls_logic,
                    fiber_iWls_name, out_logic_volume, false, 0, verbosity_);

  // The outer WLS
  G4String fiber_oWls_name = "FIBER_OWLS";

  G4double fiber_oWls_outer_rad = fiber_inner_rad_ + fiber_thickness_;;
  G4double fiber_oWls_inner_rad = fiber_oWls_outer_rad - wls_thickness_;

  G4Tubs* fiber_oWls_solid =
    new G4Tubs(fiber_oWls_name, fiber_oWls_inner_rad, fiber_oWls_outer_rad,
               fiber_length/2., 0, twopi);

  G4LogicalVolume* fiber_oWls_logic =
    new G4LogicalVolume(fiber_oWls_solid, wls_mat_, fiber_oWls_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), fiber_oWls_logic,
                    fiber_oWls_name, out_logic_volume, false, 0, verbosity_);

  // Visibilities
  fiber_iWls_logic->SetVisAttributes(G4VisAttributes::Invisible);
  fiber_oWls_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Optical surfaces
  G4OpticalSurface* fiber_wls_optSurf =
    new G4OpticalSurface("FIBER_WLS_OPSURF", glisur, ground,
                         dielectric_dielectric, .01);

  new G4LogicalSkinSurface(fiber_iWls_name, fiber_iWls_logic,
                           fiber_wls_optSurf);

  new G4LogicalSkinSurface(fiber_oWls_name, fiber_oWls_logic,
                           fiber_wls_optSurf);

  /// Verbosity ///
  if (verbosity_) {
    G4cout << "* Number of fibers: " << num_fibers_ << G4endl;
    G4cout << "* Fiber.  Inner Rad: " << fiber_inner_rad_ << 
              "   Outer Rad: " << fiber_inner_rad_ + fiber_thickness_ << G4endl;
    G4cout << "* Fiber Z positions: " << fiber_iniZ_ <<
              " to " << fiber_finZ_ << G4endl;
  } 
}



void NextFlexFieldCage::BuildFiberSensors()
{
  // Num fiber sesnors per plane
  num_fiber_sensors_ = (G4int) (twopi * fiber_inner_rad_ / fiber_sensor_size_);
  G4double fiber_sensor_phi = twopi / num_fiber_sensors_;

  if (num_fibers_ != num_fiber_sensors_)
    G4cout << "* WARNING::BuildFiberSensors - Different sizes of fibers and their sensors"
           << G4endl;

  // Positions to place the sensors
  G4double sensor_rad        = fiber_inner_rad_ + fiber_sensor_size_/2.;
  G4double sensor_left_posZ  = fiber_iniZ_ - fiber_sensor_thickness_/2.;
  G4double sensor_right_posZ = fiber_finZ_ + fiber_sensor_thickness_/2.;

  /// Constructing the sensors
  // Optical Properties of the sensor
  G4MaterialPropertiesTable* photosensor_mpt = new G4MaterialPropertiesTable();
  G4double energy[]       = {0.2 * eV, 3.5 * eV, 3.6 * eV, 11.5 * eV};
  G4double reflectivity[] = {0.0     , 0.0     , 0.0     ,  0.0     };
  G4double efficiency[]   = {1.0     , 1.0     , 0.0     ,  0.0     };
  photosensor_mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 4);
  photosensor_mpt->AddProperty("EFFICIENCY",   energy, efficiency,   4);
  left_sensor_ ->SetOpticalProperties(photosensor_mpt);
  right_sensor_->SetOpticalProperties(photosensor_mpt);

  // Adding to sensors encasing, the Refractive Index of fibers to avoid reflections
  G4MaterialPropertyVector* fibers_rindex = 
    fiber_mat_->GetMaterialPropertiesTable()->GetProperty("RINDEX");
  left_sensor_ ->SetWindowRefractiveIndex(fibers_rindex);
  right_sensor_->SetWindowRefractiveIndex(fibers_rindex);

  // Setting the time binning
  left_sensor_ ->SetTimeBinning(fiber_sensor_binning_);
  right_sensor_->SetTimeBinning(fiber_sensor_binning_);

  // Set mother depth & naming order
  left_sensor_ ->SetSensorDepth(1);
  left_sensor_ ->SetMotherDepth(2);
  left_sensor_ ->SetNamingOrder(1);
  right_sensor_->SetSensorDepth(1);
  right_sensor_->SetMotherDepth(2);
  right_sensor_->SetNamingOrder(1);

  // Set visibilities
  left_sensor_ ->SetVisibility(fiber_sensor_visibility_);
  right_sensor_->SetVisibility(fiber_sensor_visibility_);

  // Construct
  left_sensor_ ->Construct();
  right_sensor_->Construct();

  G4LogicalVolume* left_sensor_logic  = left_sensor_ ->GetLogicalVolume();
  G4LogicalVolume* right_sensor_logic = right_sensor_->GetLogicalVolume();

  /// Placing the sensors
  G4RotationMatrix sensor_left_rot;
  G4RotationMatrix sensor_right_rot;
  sensor_right_rot.rotateY(pi);

  for (G4int sensor_id=0; sensor_id < num_fiber_sensors_; sensor_id++) {

    G4double phi = sensor_id * fiber_sensor_phi;

    // Left Sensors
    if (sensor_id > 0) sensor_left_rot.rotateZ(-fiber_sensor_phi);

    G4ThreeVector case_left_pos = G4ThreeVector(sensor_rad * sin(phi),
                                                sensor_rad * cos(phi),
                                                sensor_left_posZ);

    if (verbosity_) G4cout << "* Left  fiber sensor " << first_left_sensor_id_ + sensor_id
                           << " position: " << case_left_pos << G4endl;

    new G4PVPlacement(G4Transform3D(sensor_left_rot, case_left_pos), left_sensor_logic,
                      left_sensor_logic->GetName(), mother_logic_, true,
                      first_left_sensor_id_ + sensor_id, false);
  
    // Right Sensors
    if (sensor_id > 0) sensor_right_rot.rotateZ(-fiber_sensor_phi);

    G4ThreeVector case_right_pos = G4ThreeVector(sensor_rad * sin(phi),
                                                 sensor_rad * cos(phi),
                                                 sensor_right_posZ);

    if (verbosity_) G4cout << "* Right fiber sensor " << first_right_sensor_id_ + sensor_id
                           << " position: " << case_right_pos << G4endl;

    new G4PVPlacement(G4Transform3D(sensor_right_rot, case_right_pos), right_sensor_logic,
                                    right_sensor_logic->GetName(), mother_logic_, true,
                                    first_right_sensor_id_ + sensor_id, false);
  }

  /// Verbosity
  if (verbosity_) {
    G4cout << "* Num fiber sensors   : " << num_fiber_sensors_ << " * 2" << G4endl;
    G4cout << "* Num fibers / sensor : "
           << 1. * num_fibers_ / num_fiber_sensors_ << G4endl;
    G4cout << "* Fiber sensor phi (deg): " << fiber_sensor_phi / deg << G4endl;
    G4cout << "* LEFT  sensors Z positions: "
           << sensor_left_posZ - fiber_sensor_thickness_/2. << " to " 
           << sensor_left_posZ + fiber_sensor_thickness_/2. << G4endl;
    G4cout << "* RIGHT sensors Z positions: "
           << sensor_right_posZ - fiber_sensor_thickness_/2. << " to " 
           << sensor_right_posZ + fiber_sensor_thickness_/2. << G4endl;
  }
}



G4ThreeVector NextFlexFieldCage::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "ACTIVE") {
    vertex = active_gen_->GenerateVertex("VOLUME");
  }
  else if (region == "BUFFER") {
    vertex = buffer_gen_->GenerateVertex("VOLUME");
  }
  else if (region == "EL_GAP") {
    vertex = el_gap_gen_->GenerateVertex("VOLUME");
  }
  else if (region == "LIGHT_TUBE") {
    vertex = light_tube_gen_->GenerateVertex("VOLUME");
  }
  else if (region == "FIBER_CORE") {
    if (fc_with_fibers_) vertex = fiber_gen_->GenerateVertex("VOLUME");
    else
      G4Exception("[NextFlexFieldCage]", "GenerateVertex()", FatalException,
              "Trying to generate Vertices in NON-existing fibers");
  }
  else {
    G4Exception("[NextFlexFieldCage]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }

  return vertex;
}
