// ----------------------------------------------------------------------------
// nexus | Next100FieldCage.cc
//
// Geometry of the NEXT-100 field cage.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100FieldCage.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"
#include "UniformElectricDriftField.h"
#include "XenonGasProperties.h"
#include "CylinderPointSampler2020.h"

#include <G4Navigator.hh>
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>
#include <G4GenericMessenger.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Tubs.hh>
#include <G4Polyhedra.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4NistManager.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4UnitsTable.hh>
#include <G4TransportationManager.hh>

using namespace nexus;


Next100FieldCage::Next100FieldCage():
  BaseGeometry(),
  // Dimensions
  active_diam_ (984 * mm), // distance between the centers of two opposite panels
  gate_cathode_centre_dist_ (1205. * mm), // distance between gate and the centre of cathode grid
  gate_sapphire_wdw_dist_ (1460.5 * mm), // distance between gate and the surface of sapphire windows
  cathode_diam_ (980. * mm), // internal diameter of cathode mesh
  grid_thickn_ (.1 * mm),
  cathode_gap_ (27. * mm),
  teflon_total_length_ (1432. * mm),
  teflon_thickn_ (5 * mm),
  gate_teflon_dist_ (16. * mm),
  n_panels_ (18),
  tpb_thickn_ (1 * micrometer),
  el_gap_diam_ (1009. * mm), // internal diameter of EL meshes
  el_gap_length_ (1. * cm),
  // Diffusion constants
  drift_transv_diff_ (1. * mm/sqrt(cm)),
  drift_long_diff_ (.3 * mm/sqrt(cm)),
  ELtransv_diff_ (0. * mm/sqrt(cm)),
  ELlong_diff_ (0. * mm/sqrt(cm)),
  // EL electric field
  elfield_ (0),
  ELelectric_field_ (34.5*kilovolt/cm),
  cath_grid_transparency_ (.98), // to check
  el_grid_transparency_ (.88), // to check
  max_step_size_ (1. * mm),
  // EL table generation
  el_table_binning_(1.*mm),
  el_table_point_id_(-1),
  el_table_index_(0),
  visibility_ (1),
  verbosity_(0),
  // EL gap generation disk parameters
  el_gap_gen_disk_diam_(0.),
  el_gap_gen_disk_x_(0.), el_gap_gen_disk_y_(0.),
  el_gap_gen_disk_zmin_(0.), el_gap_gen_disk_zmax_(1.)
{
  /// Define new categories
  new G4UnitDefinition("kilovolt/cm","kV/cm","Electric field", kilovolt/cm);
  new G4UnitDefinition("mm/sqrt(cm)","mm/sqrt(cm)","Diffusion", mm/sqrt(cm));

  /// Initializing the geometry navigator (used in vertex generation)
  geom_navigator_ =
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  /// Messenger
  msg_ = new G4GenericMessenger(this, "/Geometry/Next100/",
                                "Control commands of geometry Next100.");
  msg_->DeclareProperty("field_cage_vis", visibility_, "Field Cage Visibility");
  msg_->DeclareProperty("field_cage_verbosity", verbosity_, "Field Cage Verbosity");

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
                        "True if the EL field is on (full simulation), false if it's not (parametrized simulation.");

  G4GenericMessenger::Command& El_field_cmd =
  msg_->DeclareProperty("EL_field", ELelectric_field_,
                        "Electric field in the EL region");
  El_field_cmd.SetParameterName("EL_field", true);
  El_field_cmd.SetUnitCategory("Electric field");

  G4GenericMessenger::Command& step_cmd =
    msg_->DeclareProperty("max_step_size", max_step_size_, "Maximum Step Size");
  step_cmd.SetUnitCategory("Length");
  step_cmd.SetParameterName("max_step_size", false);
  step_cmd.SetRange("max_step_size>0.");

  G4GenericMessenger::Command& pitch_cmd =
    msg_->DeclareProperty("el_table_binning", el_table_binning_,
                          "Binning of EL lookup tables.");
  pitch_cmd.SetUnitCategory("Length");
  pitch_cmd.SetParameterName("el_table_binning", false);
  pitch_cmd.SetRange("el_table_binning>0.");

  msg_->DeclareProperty("el_table_point_id", el_table_point_id_, "");

  G4GenericMessenger::Command& el_gap_gen_disk_diam_cmd =
    msg_->DeclareProperty("el_gap_gen_disk_diam", el_gap_gen_disk_diam_,
                          "Diameter of the EL gap vertex generation disk.");
  el_gap_gen_disk_diam_cmd.SetUnitCategory("Length");
  el_gap_gen_disk_diam_cmd.SetParameterName("el_gap_gen_disk_diam", false);
  el_gap_gen_disk_diam_cmd.SetRange("el_gap_gen_disk_diam>0.");

  G4GenericMessenger::Command& el_gap_gen_disk_x_cmd =
    msg_->DeclareProperty("el_gap_gen_disk_x", el_gap_gen_disk_x_,
                          "X position of the center of the EL gap vertex generation disk.");
  el_gap_gen_disk_x_cmd.SetUnitCategory("Length");
  el_gap_gen_disk_x_cmd.SetParameterName("el_gap_gen_disk_x", false);
  el_gap_gen_disk_x_cmd.SetRange("el_gap_gen_disk_x>0.");

  G4GenericMessenger::Command& el_gap_gen_disk_y_cmd =
    msg_->DeclareProperty("el_gap_gen_disk_y", el_gap_gen_disk_y_,
                          "Y position of the center of the EL gap vertex generation disk.");
  el_gap_gen_disk_y_cmd.SetUnitCategory("Length");
  el_gap_gen_disk_y_cmd.SetParameterName("el_gap_gen_disk_y", false);
  el_gap_gen_disk_y_cmd.SetRange("el_gap_gen_disk_y>0.");

  G4GenericMessenger::Command& el_gap_gen_disk_zmin_cmd =
    msg_->DeclareProperty("el_gap_gen_disk_zmin", el_gap_gen_disk_zmin_,
                          "Minimum Z range of the EL gap vertex generation disk.");
  el_gap_gen_disk_zmin_cmd.SetUnitCategory("Length");
  el_gap_gen_disk_zmin_cmd.SetParameterName("el_gap_gen_disk_zmin", false);
  el_gap_gen_disk_zmin_cmd.SetRange("el_gap_gen_disk_zmin>=0. && el_gap_gen_disk_zmin<=1.0");

  G4GenericMessenger::Command& el_gap_gen_disk_zmax_cmd =
    msg_->DeclareProperty("el_gap_gen_disk_zmax", el_gap_gen_disk_zmax_,
                          "Maximum Z range of the EL gap vertex generation disk.");
  el_gap_gen_disk_zmax_cmd.SetUnitCategory("Length");
  el_gap_gen_disk_zmax_cmd.SetParameterName("el_gap_gen_disk_zmax", false);
  el_gap_gen_disk_zmax_cmd.SetRange("el_gap_gen_disk_zmax>=0. && el_gap_gen_disk_zmax<=1.0");
}


void Next100FieldCage::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
{
  mother_logic_ = mother_logic;
}


void Next100FieldCage::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
{
  mother_phys_ = mother_phys;
}


void Next100FieldCage::Construct()
{
  /// Calculate lengths of active and buffer regions
  active_length_ = gate_cathode_centre_dist_ - grid_thickn_/2.;
  buffer_length_ = gate_sapphire_wdw_dist_ - active_length_ - grid_thickn_;

  /// Calculate derived positions in mother volume
  active_zpos_       = GetELzCoord() + active_length_/2.;
  cathode_grid_zpos_ = GetELzCoord() + gate_cathode_centre_dist_;
  el_gap_zpos_       = active_zpos_ - active_length_/2. - el_gap_length_/2.;

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

  /// Calculate EL table vertices
  G4double z = el_gap_zpos_ + el_gap_length_/2.;
  /// 0.1*mm is added because ie- in EL table generation must start inside volume, not on border
  z = z + .1*mm;
  G4double max_radius =
  floor(el_gap_diam_/2./el_table_binning_)*el_table_binning_;
  CalculateELTableVertices(max_radius, el_table_binning_, z);
}


void Next100FieldCage::DefineMaterials()
{
  /// Read gas properties from mother volume
  gas_         = mother_logic_->GetMaterial();
  pressure_    = gas_->GetPressure();
  temperature_ = gas_->GetTemperature();

  /// High density polyethylene for the field cage
  hdpe_ = MaterialsList::HDPE();

  /// Copper for field rings
  copper_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

  /// Teflon for the light tube
  teflon_ =
  G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

  /// TPB coating
  tpb_ = MaterialsList::TPB();
  tpb_->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
}


void Next100FieldCage::BuildActive()
{
  /// Position of z planes
  G4double zplane[2] = {-active_length_/2., active_length_/2.};
  /// Inner radius
  G4double rinner[2] = {0., 0.};
  /// Outer radius
  G4double router[2] = {active_diam_/2., active_diam_/2.};

  G4Polyhedra* active_solid =
  new G4Polyhedra("ACTIVE", 0., twopi, n_panels_, 2, zplane, rinner, router);

  G4LogicalVolume* active_logic =
  new G4LogicalVolume(active_solid, gas_, "ACTIVE");

  new G4PVPlacement(0, G4ThreeVector(0., 0., active_zpos_), active_logic,
  "ACTIVE", mother_logic_, false, 0, false);


  /// Limit the step size in this volume for better tracking precision
  active_logic->SetUserLimits(new G4UserLimits(max_step_size_));


  /// Set the volume as an ionization sensitive detector
  IonizationSD* ionisd = new IonizationSD("/NEXT100/ACTIVE");
  active_logic->SetSensitiveDetector(ionisd);
  G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

  /// Define a drift field for this volume
  UniformElectricDriftField* field = new UniformElectricDriftField();
  G4double global_active_zpos = active_zpos_ - GetELzCoord();
  field->SetCathodePosition(global_active_zpos + active_length_/2.);
  field->SetAnodePosition(global_active_zpos - active_length_/2.);
  field->SetDriftVelocity(1. * mm/microsecond);
  field->SetTransverseDiffusion(drift_transv_diff_);
  field->SetLongitudinalDiffusion(drift_long_diff_);
  G4Region* drift_region = new G4Region("DRIFT");
  drift_region->SetUserInformation(field);
  drift_region->AddRootLogicalVolume(active_logic);


  /// Vertex generator
  active_gen_ = new CylinderPointSampler2020(0., active_diam_/2., active_length_/2.,
                                             0., twopi, nullptr,
                                             G4ThreeVector(0., 0., active_zpos_));


  /// Visibilities
  active_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // G4VisAttributes active_col = nexus::Red();
  // active_col.SetForceSolid(true);
  // active_logic->SetVisAttributes(active_col);

  /// Verbosity
  if (verbosity_) {
    G4cout << "Active starts in " << (active_zpos_ - active_length_/2.)/mm
           << " mm and ends in "
           << (active_zpos_ + active_length_/2.)/mm << " mm" << G4endl;
  }
}


void Next100FieldCage::BuildCathodeGrid()
{
  G4Material* fgrid_mat = MaterialsList::FakeDielectric(gas_, "cath_grid_mat");
  fgrid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(pressure_, temperature_, cath_grid_transparency_, grid_thickn_));

  G4Tubs* diel_grid_solid =
  new G4Tubs("CATHODE_GRID", 0., cathode_diam_/2., grid_thickn_/2., 0, twopi);

  G4LogicalVolume* diel_grid_logic =
  new G4LogicalVolume(diel_grid_solid, fgrid_mat, "CATHODE_GRID");

  new G4PVPlacement(0, G4ThreeVector(0., 0., cathode_grid_zpos_),
  diel_grid_logic, "CATHODE_GRID", mother_logic_,
  false, 0, false);


  /// Visibilities
  if (visibility_) {
    G4VisAttributes grey = nexus::LightGrey();
    diel_grid_logic->SetVisAttributes(grey);
  } else {
    diel_grid_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }


  /// Verbosity
  if (verbosity_) {
    G4cout << "Cathode grid pos z: " << (cathode_grid_zpos_)/mm << " mm" << G4endl;
  }

}


void Next100FieldCage::BuildBuffer()
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
  new G4Polyhedra("BUFFER", 0., twopi, n_panels_, 2, zplane, rinner, router);

  G4LogicalVolume* buffer_logic =
  new G4LogicalVolume(buffer_solid, gas_, "BUFFER");
  new G4PVPlacement(0, G4ThreeVector(0., 0., buffer_zpos), buffer_logic,
  "BUFFER", mother_logic_, false, 0, false);


  /// Set the volume as an ionization sensitive detector
  IonizationSD* buffsd = new IonizationSD("/NEXT100/BUFFER");
  buffsd->IncludeInTotalEnergyDeposit(false);
  buffer_logic->SetSensitiveDetector(buffsd);
  G4SDManager::GetSDMpointer()->AddNewDetector(buffsd);


  /// Vertex generator
  G4double active_ext_radius = active_diam_/2. / cos(pi/n_panels_);
  buffer_gen_ = new CylinderPointSampler2020(0., active_ext_radius, buffer_length_/2.,
                                             0., twopi, nullptr,
                                             G4ThreeVector(0., 0., buffer_zpos));

  /// Vertex generator for all xenon
  G4double xenon_length =
    el_gap_length_ + active_length_ + grid_thickn_ + buffer_length_ ;
  G4double xenon_zpos = (el_gap_length_ * el_gap_zpos_ +
                         active_length_ * active_zpos_ +
                         grid_thickn_ * cathode_grid_zpos_ +
                         buffer_length_ * buffer_zpos) / xenon_length;
  xenon_gen_ = new CylinderPointSampler2020(0., active_ext_radius, xenon_length,
                                            0., twopi, nullptr,
                                            G4ThreeVector(0., 0., xenon_zpos));

  /// Visibilities
  buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // G4VisAttributes active_col = nexus::Yellow();
  // active_col.SetForceSolid(true);
  // buffer_logic->SetVisAttributes(active_col);

  /// Verbosity
  if (verbosity_) {
    G4cout << "Buffer (gas) starts in " << buffer_zpos - buffer_length_/2.
           << " and ends in "
           << buffer_zpos + buffer_length_/2. << G4endl;
  }
}



void Next100FieldCage::BuildELRegion()
{
  /// EL gap
  G4Tubs* el_gap_solid =
    new G4Tubs("EL_GAP", 0., el_gap_diam_/2., el_gap_length_/2., 0, twopi);

  G4LogicalVolume* el_gap_logic =
    new G4LogicalVolume(el_gap_solid, gas_, "EL_GAP");

    new G4PVPlacement(0, G4ThreeVector(0., 0., el_gap_zpos_), el_gap_logic,
                      "EL_GAP", mother_logic_, false, 0);

  if (elfield_) {
    /// Define EL electric field
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    G4double global_el_gap_zpos = el_gap_zpos_ - GetELzCoord();
    el_field->SetCathodePosition(global_el_gap_zpos + el_gap_length_/2.);
    el_field->SetAnodePosition  (global_el_gap_zpos - el_gap_length_/2.);
    el_field->SetDriftVelocity(2.5 * mm/microsecond);
    el_field->SetTransverseDiffusion(ELtransv_diff_);
    el_field->SetLongitudinalDiffusion(ELlong_diff_);
    XenonGasProperties xgp(pressure_, temperature_);
    el_field->SetLightYield(xgp.ELLightYield(ELelectric_field_));
    G4Region* el_region = new G4Region("EL_REGION");
    el_region->SetUserInformation(el_field);
    el_region->AddRootLogicalVolume(el_gap_logic);
  }

  /// EL grids
  G4Material* fgrid_mat = MaterialsList::FakeDielectric(gas_, "el_grid_mat");
  fgrid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(pressure_, temperature_, el_grid_transparency_, grid_thickn_));

  /// Dimensions & position: the grids are simulated inside the EL gap.
  /// Their thickness is symbolic.

  G4double posz1 =  el_gap_length_/2. - grid_thickn_/2.;
  G4double posz2 = -el_gap_length_/2. + grid_thickn_/2.;

  G4Tubs* diel_grid_solid =
    new G4Tubs("EL_GRID", 0., el_gap_diam_/2., grid_thickn_/2., 0, twopi);

  G4LogicalVolume* diel_grid_logic =
    new G4LogicalVolume(diel_grid_solid, fgrid_mat, "EL_GRID");

  new G4PVPlacement(0, G4ThreeVector(0., 0., posz1), diel_grid_logic,
                    "EL_GRID_GATE", el_gap_logic, false, 0, false);
  new G4PVPlacement(0, G4ThreeVector(0., 0., posz2), diel_grid_logic,
                    "EL_GRID_ANODE", el_gap_logic, false, 1, false);

  // Vertex generator
  G4double el_gap_gen_disk_thickn =
    el_gap_length_ * (el_gap_gen_disk_zmax_ - el_gap_gen_disk_zmin_);

  G4double el_gap_gen_disk_z = el_gap_zpos_ + el_gap_length_/2.
    - el_gap_length_ * el_gap_gen_disk_zmin_ + el_gap_gen_disk_thickn/2.;

  G4ThreeVector el_gap_gen_pos(el_gap_gen_disk_x_,
                               el_gap_gen_disk_y_,
                               el_gap_gen_disk_z);

  el_gap_gen_ = new CylinderPointSampler2020(0., el_gap_gen_disk_diam_/2.,
                                             el_gap_gen_disk_thickn/2., 0., twopi,
                                             nullptr, el_gap_gen_pos);

  /// Visibilities
  if (visibility_) {
    G4VisAttributes light_blue = nexus::LightBlue();
    el_gap_logic->SetVisAttributes(light_blue);
    G4VisAttributes grey = nexus::LightGrey();
    diel_grid_logic->SetVisAttributes(grey);
  } else {
    el_gap_logic->SetVisAttributes(G4VisAttributes::Invisible);
    diel_grid_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }

  /// Verbosity
  if (verbosity_) {
    G4cout << "EL gap starts in " << (el_gap_zpos_ - el_gap_length_/2.)/mm
           << " mm and ends in " << (el_gap_zpos_ + el_gap_length_/2.)/mm << G4endl;
    G4cout << G4endl;
  }
}


void Next100FieldCage::BuildFieldCage()
{
  /// DRIFT PART ///
  G4double teflon_drift_length = gate_cathode_centre_dist_ - gate_teflon_dist_ - cathode_gap_/2.;
  G4double teflon_drift_zpos = GetELzCoord() + gate_teflon_dist_ + teflon_drift_length/2.;

  /// Position of z planes
  G4double zplane[2] = {-teflon_drift_length/2., teflon_drift_length/2.};
  /// Inner radius
  G4double rinner[2] = {active_diam_/2., active_diam_/2.};
  /// Outer radius
  G4double router[2] =
    {(active_diam_ + 2.*teflon_thickn_)/2., (active_diam_ + 2.*teflon_thickn_)/2.};

  G4Polyhedra* teflon_drift_solid =
    new G4Polyhedra("LIGHT_TUBE_DRIFT", 0., twopi, n_panels_, 2,
                    zplane, rinner, router);

  G4LogicalVolume* teflon_drift_logic =
    new G4LogicalVolume(teflon_drift_solid, teflon_, "LIGHT_TUBE_DRIFT");

  new G4PVPlacement(0, G4ThreeVector(0., 0., teflon_drift_zpos),
                    teflon_drift_logic, "LIGHT_TUBE_DRIFT", mother_logic_,
                    false, 0, false);


  /// TPB on teflon surface
  G4double router_tpb[2] =
    {(active_diam_ + 2.*tpb_thickn_)/2., (active_diam_ + 2.*tpb_thickn_)/2.};

  G4Polyhedra* tpb_drift_solid =
    new  G4Polyhedra("DRIFT_TPB", 0., twopi, n_panels_, 2,
                     zplane, rinner, router_tpb);
  G4LogicalVolume* tpb_drift_logic =
    new G4LogicalVolume(tpb_drift_solid, tpb_, "DRIFT_TPB");
  G4VPhysicalVolume* tpb_drift_phys =
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), tpb_drift_logic,
                      "DRIFT_TPB", teflon_drift_logic, false, 0, false);

  /// BUFFER PART ///
  G4double teflon_buffer_length = teflon_total_length_ - cathode_gap_ - teflon_drift_length;
  G4double teflon_buffer_zpos = teflon_drift_zpos + teflon_drift_length/2.
                              + cathode_gap_ + teflon_buffer_length/2;

  G4double zplane_buff[2] = {-teflon_buffer_length/2., teflon_buffer_length/2.};
  G4double router_buff[2] =
    {(active_diam_ + 2.*teflon_thickn_)/2., (active_diam_ + 2.*teflon_thickn_)/2.};

  G4Polyhedra* teflon_buffer_solid =
    new G4Polyhedra("LIGHT_TUBE_BUFFER", 0., twopi, n_panels_, 2,
                    zplane_buff, rinner, router_buff);

  G4LogicalVolume* teflon_buffer_logic =
    new G4LogicalVolume(teflon_buffer_solid, teflon_, "LIGHT_TUBE_BUFFER");

  new G4PVPlacement(0, G4ThreeVector(0., 0., teflon_buffer_zpos),
                    teflon_buffer_logic, "LIGHT_TUBE_BUFFER", mother_logic_,
                    false, 0, false);

  /// TPB on teflon surface
  G4double router_tpb_buff[2] =
    {(active_diam_ + 2.*tpb_thickn_)/2., (active_diam_ + 2.*tpb_thickn_)/2.};

  G4Polyhedra* tpb_buffer_solid =
    new  G4Polyhedra("BUFFER_TPB", 0., twopi, n_panels_, 2,
                     zplane_buff, rinner, router_tpb_buff);
  G4LogicalVolume* tpb_buffer_logic =
    new G4LogicalVolume(tpb_buffer_solid, tpb_, "BUFFER_TPB");
  G4VPhysicalVolume* tpb_buffer_phys =
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), tpb_buffer_logic,
                      "BUFFER_TPB", teflon_buffer_logic, false, 0, false);

  /// Optical surface on teflon ///
  G4OpticalSurface* refl_Surf =
    new G4OpticalSurface("refl_Surf", unified, ground, dielectric_metal, .01);
  refl_Surf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
  new G4LogicalSkinSurface("refl_teflon_surf", teflon_drift_logic, refl_Surf);
  new G4LogicalSkinSurface("refl_teflon_surf", teflon_buffer_logic, refl_Surf);

  /// Optical surface between xenon and TPB to model roughness ///
  G4OpticalSurface* gas_tpb_teflon_surf =
    new G4OpticalSurface("gas_tpb_teflon_surf", glisur, ground,
                         dielectric_dielectric, .01);

  new G4LogicalBorderSurface("gas_tpb_teflon_surf", tpb_drift_phys, mother_phys_,
                             gas_tpb_teflon_surf);
  new G4LogicalBorderSurface("gas_tpb_teflon_surf", mother_phys_, tpb_drift_phys,
                             gas_tpb_teflon_surf);
  new G4LogicalBorderSurface("gas_tpb_teflon_surf", tpb_buffer_phys, mother_phys_,
                             gas_tpb_teflon_surf);
  new G4LogicalBorderSurface("gas_tpb_teflon_surf", mother_phys_, tpb_buffer_phys,
                             gas_tpb_teflon_surf);

  // Vertex generator
  G4double teflon_ext_radius =
    (active_diam_ + 2.*teflon_thickn_)/2. / cos(pi/n_panels_);
  G4double cathode_gap_zpos =
    teflon_drift_zpos + teflon_drift_length/2. + cathode_gap_/2.;
  G4double teflon_zpos =
    (teflon_drift_length * teflon_drift_zpos + cathode_gap_ * cathode_gap_zpos +
     teflon_buffer_length * teflon_buffer_zpos) / teflon_total_length_;

  teflon_gen_ = new CylinderPointSampler2020(active_diam_/2., teflon_ext_radius,
                                             teflon_total_length_/2., 0., twopi,
                                             nullptr, G4ThreeVector (0., 0., teflon_zpos));

  // Visibilities
  if (visibility_) {
    G4VisAttributes light_green = nexus::LightGreen();
    G4VisAttributes green = nexus::DarkGreen();
    //light_green.SetForceSolid(true);
    teflon_drift_logic->SetVisAttributes(light_green);
    teflon_buffer_logic->SetVisAttributes(green);
    G4VisAttributes red = nexus::Red();
    tpb_drift_logic->SetVisAttributes(red);
    tpb_buffer_logic->SetVisAttributes(red);
  }
  else {
    teflon_drift_logic->SetVisAttributes(G4VisAttributes::Invisible);
    teflon_buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);
    tpb_drift_logic->SetVisAttributes(G4VisAttributes::Invisible);
    tpb_buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }
}


Next100FieldCage::~Next100FieldCage()
{
  delete active_gen_;
  delete buffer_gen_;
  delete xenon_gen_;
  delete teflon_gen_;
  delete el_gap_gen_;
}


G4ThreeVector Next100FieldCage::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex(0., 0., 0.);

  if (region == "CENTER") {
    vertex = G4ThreeVector(0., 0., active_zpos_);
  }

  else if (region == "ACTIVE") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = active_gen_->GenerateVertex("VOLUME");
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx + G4ThreeVector(0, 0, -GetELzCoord());
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else if (region == "BUFFER") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = buffer_gen_->GenerateVertex("VOLUME");
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx + G4ThreeVector(0, 0, -GetELzCoord());
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else if (region == "XENON") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = xenon_gen_->GenerateVertex("VOLUME");
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx + G4ThreeVector(0, 0, -GetELzCoord());
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (
    VertexVolume->GetName() != "ACTIVE" &&
    VertexVolume->GetName() != "BUFFER" &&
    VertexVolume->GetName() != "EL_GAP");
  }

  else if (region == "LIGHT_TUBE") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = teflon_gen_->GenerateVertex("VOLUME");
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx + G4ThreeVector(0, 0, -GetELzCoord());
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (
    VertexVolume->GetName() != "LIGHT_TUBE_DRIFT" &&
    VertexVolume->GetName() != "LIGHT_TUBE_BUFFER" );
  }
  else if (region == "EL_TABLE") {
    unsigned int i = el_table_point_id_ + el_table_index_;
    if (i == (table_vertices_.size()-1)) {
      G4Exception("[Next100FieldCage]", "GenerateVertex()",
      RunMustBeAborted, "Reached last event in EL lookup table.");
    }
    try {
      vertex = table_vertices_.at(i);
      el_table_index_++;
    }
    catch (const std::out_of_range& oor) {
      G4Exception("[Next100FieldCage]", "GenerateVertex()", FatalErrorInArgument,
      "EL lookup table point out of range.");
    }
  }

  else if (region == "EL_GAP") {
    vertex = el_gap_gen_->GenerateVertex("VOLUME");
  }

  else {
    G4Exception("[Next100FieldCage]", "GenerateVertex()", FatalException,
    "Unknown vertex generation region!");
  }

  return vertex;
}


void Next100FieldCage::CalculateELTableVertices(G4double radius, G4double binning, G4double z)
{
  /// Calculate the xyz positions of the points of an EL lookup table
  /// (arranged as a square grid) given a certain binning

  G4ThreeVector xyz(0., 0., z);

  G4int imax = floor(2*radius/binning); // maximum bin number (minus 1)

  for (int i=0; i<imax+1; i++) { // Loop through the x bins

    xyz.setX(-radius + i * binning); // x position

    for (int j=0; j<imax+1; j++) { // Loop through the y bins

      xyz.setY(-radius + j * binning); // y position

      // Store the point if it is inside the active volume defined by the
      // field cage (of circular cross section). Discard it otherwise.
      if (sqrt(xyz.x()*xyz.x()+xyz.y()*xyz.y()) <= radius)
      table_vertices_.push_back(xyz);
    }

  }
}


G4ThreeVector Next100FieldCage::GetActivePosition() const
{
  return G4ThreeVector (0., 0., active_zpos_);
}


G4double Next100FieldCage::GetDistanceGateSapphireWindows() const
{
  return active_length_ + grid_thickn_ +  buffer_length_;
}
