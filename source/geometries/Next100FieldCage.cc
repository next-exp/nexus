// ----------------------------------------------------------------------------
// nexus | Next100FieldCage.cc
//
// Geometry of the NEXT-100 field cage.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100FieldCage.h"
#include "Next100Stave.h"
#include "MaterialsList.h"
#include "Visibilities.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"
#include "UniformElectricDriftField.h"
#include "XenonProperties.h"
#include "CylinderPointSampler.h"
#include "HexagonMeshTools.h"

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
#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4Box.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4NistManager.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4UnitsTable.hh>
#include <G4TransportationManager.hh>

#include <cassert>

using namespace nexus;


Next100FieldCage::Next100FieldCage(G4double grid_thickn):
  GeometryBase(),
  // Dimensions
  active_diam_         (983.3 * mm), // distance between the centers of two opposite panels
  n_panels_            (18),
  active_ext_radius_   (active_diam_/2. / cos(pi/n_panels_)),

  grid_thickn_         (grid_thickn),

  cathode_int_diam_    (965. * mm),
  cathode_ext_diam_    (1020.* mm),
  cathode_thickn_      (13.5  * mm),
  cathode_mesh_diam_   (5.0*mm),

  //  teflon_drift_length_ (1178.*mm), //distance from the gate to the beginning of the cathode volume
  teflon_long_length_   (465. * mm),
  teflon_buffer_length_ (241. * mm),
  teflon_thickn_       (5. * mm),

  el_gap_length_ (10. * mm),
  el_mesh_diam_  (2.5 * mm),
  el_mesh_rot_   (15 * deg),

  gate_ext_diam_    (1050. * mm),
  gate_int_diam_    (995. * mm),
  gate_ring_thickn_ (13.5   * mm),

  gate_teflon_dist_         (0.7 * mm + gate_ring_thickn_ - grid_thickn_), //distance from gate-grid to teflon
  gate_cathode_dist_        (1174. * mm + gate_ring_thickn_ - grid_thickn_),
  // cathode_sapphire_wdw_dist_(257.8 * mm),

  // external to teflon (hdpe + rings + holders)
  hdpe_tube_int_diam_ (1081. * mm),
  hdpe_tube_ext_diam_ (1094 * mm),
  hdpe_length_        (1115. * mm),

  ring_ext_diam_ (1038. * mm),
  ring_int_diam_ (1014. * mm),
  ring_thickn_   (10. * mm),
  drift_ring_dist_  (24. * mm),
  buffer_ring_dist_ (48. * mm), // distance between buffer rings
  buffer_first_ring_dist_ (69. * mm), // distance of first ring from end of the stave
  ring_drift_buffer_dist_ (72. * mm), // distance between last ring of buffer and first ring of drift part.
  num_drift_rings_ (48),
  num_buffer_rings_ (4),

  tpb_thickn_ (1 * micrometer),
  overlap_    (0.001*mm), //defined for G4UnionSolids to ensure a common volume within the two joined solids
  // Diffusion constants
  drift_transv_diff_ (1. * mm/sqrt(cm)),
  drift_long_diff_ (.3 * mm/sqrt(cm)),
  ELtransv_diff_ (0. * mm/sqrt(cm)),
  ELlong_diff_ (0. * mm/sqrt(cm)),
  // EL electric field
  elfield_ (0),
  ELelectric_field_ (34.5*kilovolt/cm),
  cath_grid_transparency_(.95),
  el_grid_transparency_  (.90),
  max_step_size_ (1. * mm),
  visibility_ (0),
  grid_visibility_ (0),
  verbosity_(0),
  use_dielectric_grid_(0),
  // EL gap generation disk parameters
  el_gap_gen_disk_diam_(0.),
  el_gap_gen_disk_x_(0.), el_gap_gen_disk_y_(0.),
  el_gap_gen_disk_zmin_(0.), el_gap_gen_disk_zmax_(1.),
  photoe_prob_(0)
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
  msg_->DeclareProperty("grid_vis", grid_visibility_, "Grid Visibility");
  msg_->DeclareProperty("field_cage_verbosity", verbosity_, "Field Cage Verbosity");

  msg_->DeclareProperty("use_dielectric_grid", use_dielectric_grid_, "Switch on Fake Grids");

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
                        "True if the EL field is on (full simulation), "
                        "false if it's not (parametrized simulation.");

  G4GenericMessenger::Command& El_field_cmd =
  msg_->DeclareProperty("EL_field", ELelectric_field_, "Electric field in the EL region");
  El_field_cmd.SetParameterName("EL_field", true);
  El_field_cmd.SetUnitCategory("Electric field");

  G4GenericMessenger::Command& step_cmd =
    msg_->DeclareProperty("max_step_size", max_step_size_, "Maximum Step Size");
  step_cmd.SetUnitCategory("Length");
  step_cmd.SetParameterName("max_step_size", true);
  step_cmd.SetRange("max_step_size>0.");

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

  msg_->DeclareProperty("photoe_prob", photoe_prob_,
                        "Probability of photon to ie- conversion");
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
  /// Calculate drift lengths of teflon
  teflon_drift_length_ = 2 * teflon_long_length_ + teflon_buffer_length_;

  /// Calculate lengths of active and buffer regions
  active_length_ = gate_cathode_dist_;
  buffer_length_ = gate_sapphire_wdw_dist_ - gate_cathode_dist_ - grid_thickn_;

  /// Calculate relative z positions in mother volume.
  /// All of them are calculated using GetCoordOrigin(), which is the position
  /// of the beginning of the ACTIVE volume and the end of the gate grid
  /// w.r.t. the mother volume (vessel gas).
  gate_grid_zpos_  = GetCoordOrigin().z() - grid_thickn_/2.;
  active_zpos_     = GetCoordOrigin().z() + active_length_/2.;
  cathode_zpos_    = GetCoordOrigin().z() + gate_cathode_dist_ + cathode_thickn_/2.;
  gate_zpos_       = GetCoordOrigin().z() - grid_thickn_ + gate_ring_thickn_/2.;
  el_gap_zpos_     = GetCoordOrigin().z() - grid_thickn_ - el_gap_length_/2.;
  anode_grid_zpos_ = el_gap_zpos_ - el_gap_length_/2. - grid_thickn_/2.;

  teflon_drift_zpos_  = GetCoordOrigin().z() + gate_teflon_dist_ + teflon_drift_length_/2.;
  // 15.6 mm = value used to obtain 24.8 mm as EP plate-teflon distance
  teflon_buffer_zpos_ = GetCoordOrigin().z() + gate_sapphire_wdw_dist_ - 15.6*mm - teflon_buffer_length_/2.;

  if (verbosity_) {
    G4cout << "Active length = " << active_length_/mm << " mm" << G4endl;
    G4cout << "Buffer length = " << buffer_length_/mm << " mm" << G4endl;
    G4cout << G4endl;
  }

  /// Define materials to be used
  DefineMaterials();
  /// Build the different parts of the field cage
  BuildActive();
  BuildCathode();
  BuildBuffer();
  BuildELRegion();
  BuildLightTube();
  BuildFieldCage();
}


void Next100FieldCage::DefineMaterials()
{
  /// Read gas properties from mother volume
  gas_         = mother_logic_->GetMaterial();
  pressure_    = gas_->GetPressure();
  temperature_ = gas_->GetTemperature();
  sc_yield_    = gas_->GetMaterialPropertiesTable()->GetConstProperty("SCINTILLATIONYIELD");
  e_lifetime_  = gas_->GetMaterialPropertiesTable()->GetConstProperty("ATTACHMENT");

  /// High density polyethylene for the field cage
  hdpe_ = materials::HDPE();

  /// Copper for field rings
  copper_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
  copper_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  /// Teflon for the light tube
  teflon_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  // teflon is the material used in the light-tube  and is covered by
  // a G4LogicalSkinSurface. In Geant4 11.0.0, a bug in treating the
  // OpBoundaryProcess produced in the surface makes the code fail. This is
  // avoided by setting an empty G4MaterialPropertiesTable of the G4Material.
  teflon_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  /// TPB coating
  tpb_ = materials::TPB();
  tpb_->SetMaterialPropertiesTable(opticalprops::TPB());

  /// Steel
  steel_ = materials::Steel316Ti();
  // In Geant4 11.0.0, a bug in treating the OpBoundaryProcess produced in the
  // surface makes the code fail. This is avoided by setting
  // an empty G4MaterialPropertiesTable of the G4Material.
  steel_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());
}


void Next100FieldCage::BuildActive()
{
  /// Position of z planes
  G4double zplane[2] = {-active_length_/2. + gate_teflon_dist_ - overlap_, active_length_/2.};
  /// Inner radius
  G4double rinner[2] = {0., 0.};
  /// Outer radius
  G4double router[2] = {active_diam_/2., active_diam_/2.};

  G4Polyhedra* active_solid =
    new G4Polyhedra("ACTIVE_TEFLON", 0., twopi, n_panels_, 2, zplane, rinner, router);

  // This volume is added as an extension of the active volume that reaches the gate grid.
  G4Tubs* active_gate_solid =
    new G4Tubs("ACT_GATE_GAS", 0, gate_int_diam_/2., gate_teflon_dist_/2., 0, twopi);

  G4ThreeVector act_gate_pos =
    G4ThreeVector(-GetCoordOrigin().x(), -GetCoordOrigin().y(),
                  -active_length_/2. + gate_teflon_dist_/2.);

  G4UnionSolid* union_active =
    new G4UnionSolid ("ACTIVE", active_solid, active_gate_solid, 0, act_gate_pos);

  G4LogicalVolume* active_logic =
    new G4LogicalVolume(union_active, gas_, "ACTIVE");

  active_phys_ =
    new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(),
                                       GetCoordOrigin().y(), active_zpos_),
                      active_logic, "ACTIVE", mother_logic_, false, 0, false);

  /// Limit the step size in this volume for better tracking precision
  active_logic->SetUserLimits(new G4UserLimits(max_step_size_));


  /// Set the volume as an ionization sensitive detector
  IonizationSD* ionisd = new IonizationSD("/NEXT100/ACTIVE");
  active_logic->SetSensitiveDetector(ionisd);
  G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

  /// Define a drift field for this volume
  UniformElectricDriftField* field = new UniformElectricDriftField();
  G4double global_active_zpos = active_zpos_ - GetCoordOrigin().z();
  field->SetCathodePosition(global_active_zpos + active_length_/2.);
  field->SetAnodePosition(global_active_zpos - active_length_/2.);
  field->SetDriftVelocity(1. * mm/microsecond);
  field->SetTransverseDiffusion(drift_transv_diff_);
  field->SetLongitudinalDiffusion(drift_long_diff_);
  field->SetLifetime(e_lifetime_);
  G4Region* drift_region = new G4Region("DRIFT");
  drift_region->SetUserInformation(field);
  drift_region->AddRootLogicalVolume(active_logic);

  /// Vertex generator
  active_gen_radius_ =
    std::max(gate_int_diam_/2. - GetCoordOrigin().x(),
             gate_int_diam_/2. - GetCoordOrigin().y());

  active_gen_radius_ = std::max(active_gen_radius_, active_ext_radius_);
  active_gen_ = new CylinderPointSampler(0., active_gen_radius_,
                                         active_length_/2., 0., twopi, nullptr,
                                         G4ThreeVector(GetCoordOrigin().x(),
                                                       GetCoordOrigin().y(),
                                                       active_zpos_));

  /// Visibilities
  active_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  /// Verbosity
  if (verbosity_) {
    G4cout << "Active starts in " << (active_zpos_ - active_length_/2.)/mm
           << " mm and ends in "  << (active_zpos_ + active_length_/2.)/mm
           << " mm" << G4endl;
  }
}


void Next100FieldCage::BuildCathode()
{

  // Cathode Ring
  G4Tubs* cathode_solid;
  G4LogicalVolume* cathode_logic;

  G4Material* fgrid_mat = materials::FakeDielectric(gas_, "cath_grid_mat");
  fgrid_mat->SetMaterialPropertiesTable(opticalprops::FakeGrid(pressure_,
                                                               temperature_,
                                                               cath_grid_transparency_,
                                                               grid_thickn_));

  // Cathode Mesh
  G4LogicalVolume *cathode_grid_logic;
  G4LogicalVolume *cathode_hex_logic;
  G4double cathode_grid_zpos = cathode_zpos_ - cathode_thickn_/2. + grid_thickn_/2.;

  if (use_dielectric_grid_){

    // Cathode Ring
    cathode_solid =
    new G4Tubs("CATHODE_RING", cathode_int_diam_/2.,cathode_ext_diam_/2.,
               cathode_thickn_/2., 0, twopi);

    cathode_logic =
    new G4LogicalVolume(cathode_solid, steel_, "CATHODE_RING");

    new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(), GetCoordOrigin().y(),
                                     cathode_zpos_),
                    cathode_logic, "CATHODE_RING", mother_logic_, false, 0, false);

    G4Tubs* diel_grid_solid =
      new G4Tubs("CATHODE_GRID", 0., cathode_int_diam_/2., grid_thickn_/2., 0, twopi);

    cathode_grid_logic = new G4LogicalVolume(diel_grid_solid, fgrid_mat, "CATHODE_GRID");

    new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(),
                                       GetCoordOrigin().y(), cathode_grid_zpos),
                                       cathode_grid_logic, "CATHODE_GRID", mother_logic_,
                                       false, 0, false);

  }
  // Use SS hexagonal mesh
  else {

      // Cathode Ring 
      // Shift in the +z direction by half-mesh thickness and reduce thickness
      // by the grid thickness. The grid thickness makes up the remaining ring thickness
      cathode_solid =
      new G4Tubs("CATHODE_RING", cathode_int_diam_/2.,cathode_ext_diam_/2.,
                cathode_thickn_/2. - grid_thickn_/2., 0, twopi);

      cathode_logic =
      new G4LogicalVolume(cathode_solid, steel_, "CATHODE_RING");

      new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(), GetCoordOrigin().y(),
                                      cathode_zpos_ + grid_thickn_/2.0),
                      cathode_logic, "CATHODE_RING", mother_logic_, false, 0, false);
      
      // Dist from centre of hex to hex vertex, excluding the land width (circumradius)
      G4double hex_circumradius = cathode_mesh_diam_/std::sqrt(3);  

      // Total number of hexagons that would fit side-by-side along the diameter
      G4int n_hex = (G4int) ((cathode_int_diam_/2.0) / hex_circumradius);
    
      // Define the disk to punch hexagon holes through for the mesh
      G4Tubs* grid_solid = new G4Tubs("CATHODE_GRID", 0., cathode_ext_diam_/2.0 , grid_thickn_/2., 0., twopi);
      cathode_grid_logic = new G4LogicalVolume(grid_solid, steel_, "CATHODE_MESH_LOGIC");

      // Define a hexagonal prism
      G4ExtrudedSolid* hex_prism = CreateHexagon(grid_thickn_/2.0, hex_circumradius);
      cathode_hex_logic  = new G4LogicalVolume(hex_prism, gas_, "MESH_HEX_GAS");

      PlaceHexagons(n_hex, cathode_mesh_diam_, grid_thickn_, cathode_grid_logic, cathode_hex_logic, cathode_int_diam_);

      new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(),
                                         GetCoordOrigin().y(), cathode_grid_zpos),
                                         cathode_grid_logic,  "CATHODE_GRID", mother_logic_,
                                         false, 0, false);

      // Add optical surface
      G4OpticalSurface* gas_mesh_opsur = new G4OpticalSurface("GAS_CATHODE_MESH_OPSURF");
      gas_mesh_opsur->SetType(dielectric_metal);
      gas_mesh_opsur->SetModel(unified);
      gas_mesh_opsur->SetFinish(ground);
      gas_mesh_opsur->SetSigmaAlpha(0.0);
      gas_mesh_opsur->SetMaterialPropertiesTable(opticalprops::Steel());
      new G4LogicalSkinSurface("GAS_CATHODE_MESH_OPSURF",
                              cathode_grid_logic, gas_mesh_opsur);

  }

  // Cathode ring vertex generator
  cathode_gen_ =
    new CylinderPointSampler(cathode_int_diam_/2.,cathode_ext_diam_/2.,
                             cathode_thickn_/2.,0., twopi, nullptr,
                             G4ThreeVector(GetCoordOrigin().x(),
                                           GetCoordOrigin().y(),
                                           cathode_zpos_));


  /// Visibilities
  G4VisAttributes cathode_col = nexus::DarkGrey();
  cathode_col.SetForceSolid(true);
  cathode_logic->SetVisAttributes(cathode_col);
  cathode_grid_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  if (visibility_) {
    cathode_grid_logic->SetVisAttributes(nexus::LightGrey());
  }

  if (!grid_visibility_ && !use_dielectric_grid_)
    cathode_hex_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  /// Verbosity
  if (verbosity_) {
    G4cout << "Cathode grid pos z: " << (cathode_zpos_)/mm << " mm" << G4endl;
  }

}


void Next100FieldCage::BuildBuffer()
{
  G4double buffer_zpos = active_zpos_ + active_length_/2. + grid_thickn_ + buffer_length_/2.;

  /// Position of z planes
  G4double zplane[2] = {-buffer_length_/2. - grid_thickn_ + cathode_thickn_, buffer_length_/2.};
  /// Inner radius
  G4double rinner[2] = {0., 0.};
  /// Outer radius
  G4double router[2] = {active_diam_/2., active_diam_/2.};

  G4Polyhedra* buffer_solid =
    new G4Polyhedra("BUFFER_POLY", 0., twopi, n_panels_, 2, zplane, rinner, router);

  G4Tubs* buffer_cathode_solid =
    new G4Tubs("BUFF_CATHODE_RING", 0, cathode_int_diam_/2.,
               (cathode_thickn_ - grid_thickn_)/2. +  overlap_/2., 0, twopi);


  G4ThreeVector buff_cathode_pos =
    G4ThreeVector(0., 0., -buffer_length_/2.+(cathode_thickn_-grid_thickn_)/2.+overlap_/2.);

  G4UnionSolid* union_buffer =
    new G4UnionSolid("BUFFER", buffer_solid, buffer_cathode_solid, 0, buff_cathode_pos);

  G4LogicalVolume* buffer_logic =
    new G4LogicalVolume(union_buffer, gas_, "BUFFER");

  buffer_phys_ =
    new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(),
                                       GetCoordOrigin().y(), buffer_zpos),
                      buffer_logic, "BUFFER", mother_logic_, false, 0, false);

  /// Set the volume as an ionization sensitive detector
  IonizationSD* buffsd = new IonizationSD("/NEXT100/BUFFER");
  buffsd->IncludeInTotalEnergyDeposit(false);
  buffer_logic->SetSensitiveDetector(buffsd);
  G4SDManager::GetSDMpointer()->AddNewDetector(buffsd);


  /// Vertex generator
  buffer_gen_ =
    new CylinderPointSampler(0., active_ext_radius_, buffer_length_/2.,
                             0., twopi, nullptr,
                             G4ThreeVector(GetCoordOrigin().x(),
                                           GetCoordOrigin().y(),
                                           buffer_zpos));

  /// Vertex generator for all xenon
  G4double xenon_length = el_gap_length_ + active_length_ + grid_thickn_ + buffer_length_ ;
  G4double xenon_zpos   = (el_gap_length_ * el_gap_zpos_ +
                          active_length_ * active_zpos_ +
                          grid_thickn_ * cathode_zpos_ +
                          buffer_length_ * buffer_zpos) / xenon_length;
  xenon_gen_ =
    new CylinderPointSampler(0., active_gen_radius_, xenon_length,
                             0., twopi, nullptr,
                             G4ThreeVector(GetCoordOrigin().x(),
                                           GetCoordOrigin().y(),
                                           xenon_zpos));

  /// Visibilities
  buffer_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  /// Verbosity
  if (verbosity_) {
    G4cout << "Buffer (gas) starts in " << buffer_zpos - buffer_length_/2.
           << " and ends in " << buffer_zpos + buffer_length_/2. << G4endl;
  }
}


void Next100FieldCage::BuildELRegion()
{
  /// GATE ring.
  G4Tubs* gate_solid;
  G4LogicalVolume* gate_logic;

  /// ANODE ring.
  // We simulate a thinner ring to avoid cutting the teflon masks,
  // which would overlap with it. 4.7 mm is the difference between
  // the thickness of sipm board+mask (6.2 mm) and the real distance
  // between the anode ring and the TP, measured by Jordi (1.5 mm).
  G4double anode_ring_thickn = gate_ring_thickn_ - 4.7 * mm;
  G4Tubs* anode_solid;
  G4LogicalVolume* anode_logic;
  G4double anode_sim_zpos = el_gap_zpos_ - el_gap_length_/2. - anode_ring_thickn/2.;

  /// EL gap.
  G4Tubs* el_gap_solid;
  G4LogicalVolume* el_gap_logic;

  /// EL Grid Logical
  G4LogicalVolume *el_grid_logic;
  G4LogicalVolume *el_hex_logic;

  // EL Grid -- use Fake Dielectric
  if (use_dielectric_grid_){

    // GATE ring.
    gate_solid =
      new G4Tubs("GATE_RING", gate_int_diam_/2., gate_ext_diam_/2.,
                gate_ring_thickn_/2., 0, twopi);

    gate_logic = new G4LogicalVolume(gate_solid, steel_, "GATE_RING");

    new G4PVPlacement(0, G4ThreeVector(0., 0., gate_zpos_),
                      gate_logic, "GATE_RING", mother_logic_, false, 0, false);


    // Anode ring.
    anode_solid =
    new G4Tubs("ANODE_RING", gate_int_diam_/2., gate_ext_diam_/2.,
               anode_ring_thickn/2., 0, twopi);

    anode_logic =
      new G4LogicalVolume(anode_solid, steel_, "ANODE_RING");

    new G4PVPlacement(0, G4ThreeVector(0., 0., anode_sim_zpos),
                      anode_logic, "ANODE_RING", mother_logic_, false, 0, false);


    /// EL gap.
    el_gap_solid =
      new G4Tubs("EL_GAP", 0., gate_int_diam_/2.,
                (el_gap_length_ + 2*grid_thickn_)/2., 0, twopi);

    /// EL grids
    G4Material* fgrid_mat = materials::FakeDielectric(gas_, "el_grid_mat");
    fgrid_mat->SetMaterialPropertiesTable(opticalprops::FakeGrid(pressure_,
                                                               temperature_,
                                                               el_grid_transparency_,
                                                               grid_thickn_,
                                                               sc_yield_,
                                                               1000.*ms,
                                                               photoe_prob_));

    /// Dimensions & position: the grids are simulated inside the EL gap.
    /// Their thickness is symbolic.
    G4Tubs* diel_grid_solid =
      new G4Tubs("EL_GRID", 0., gate_int_diam_/2., grid_thickn_/2., 0, twopi);

    el_grid_logic = new G4LogicalVolume(diel_grid_solid, fgrid_mat, "EL_GRID");

  }
  // EL Grids -- use SS hexagonal mesh
  else {

    // GATE ring.
    // Shift in the +z direction by half-mesh thickness and reduce thickness
    // by the grid thickness. The grid thickness makes up the remaining ring thickness
    gate_solid =
      new G4Tubs("GATE_RING", gate_int_diam_/2., gate_ext_diam_/2.,
                gate_ring_thickn_/2. - grid_thickn_/2., 0, twopi);

    gate_logic = new G4LogicalVolume(gate_solid, steel_, "GATE_RING");

    new G4PVPlacement(0, G4ThreeVector(0., 0., gate_zpos_ + grid_thickn_/2.),
                      gate_logic, "GATE_RING", mother_logic_, false, 0, false);


    // Anode ring.
    // Shift in the -z direction by half-mesh thickness and reduce thickness
    // by the grid thickness. The grid thickness makes up the remaining ring thickness
    anode_solid =
    new G4Tubs("ANODE_RING", gate_int_diam_/2., gate_ext_diam_/2.,
               anode_ring_thickn/2. - grid_thickn_/2., 0, twopi);

    anode_logic =
      new G4LogicalVolume(anode_solid, steel_, "ANODE_RING");

    new G4PVPlacement(0, G4ThreeVector(0., 0., anode_sim_zpos - grid_thickn_/2.),
                      anode_logic, "ANODE_RING", mother_logic_, false, 0, false);

    /// EL gap.
    el_gap_solid =
      new G4Tubs("EL_GAP", 0., gate_ext_diam_/2.,
                (el_gap_length_ + 2*grid_thickn_)/2., 0, twopi);
    
    // Meshes

    // Dist from centre of hex to hex vertex, excluding the land width (circumradius)
    G4double hex_circumradius = el_mesh_diam_/std::sqrt(3)*mm;  

    // Total number of hexagons that would fit side-by-side along the diameter
    G4int n_hex = (G4int) ((gate_int_diam_/2.0) / hex_circumradius);
  
    // Define the disk to punch hexagon holes through for the mesh
    G4Tubs* grid_solid = new G4Tubs("EL_GRID", 0., gate_ext_diam_/2.0 , grid_thickn_/2., 0., twopi);
    el_grid_logic = new G4LogicalVolume(grid_solid, steel_, "EL_GRID");

    // Define a hexagonal prism
    G4ExtrudedSolid* hex_prism = CreateHexagon(grid_thickn_/2.0, hex_circumradius);
    el_hex_logic  = new G4LogicalVolume(hex_prism, gas_, "MESH_HEX_GAS");
   
    // Place GXe hexagons in the disk to make the mesh
    PlaceHexagons(n_hex, el_mesh_diam_, grid_thickn_, el_grid_logic, el_hex_logic, gate_int_diam_);

    // Add optical surface
    G4OpticalSurface* gas_mesh_opsur = new G4OpticalSurface("GAS_EL_MESH_OPSURF");
    gas_mesh_opsur->SetType(dielectric_metal);
    gas_mesh_opsur->SetModel(unified);
    gas_mesh_opsur->SetFinish(ground);
    gas_mesh_opsur->SetSigmaAlpha(0.0);
    gas_mesh_opsur->SetMaterialPropertiesTable(opticalprops::Steel());
    new G4LogicalSkinSurface("GAS_EL_MESH_OPSURF",
                             el_grid_logic, gas_mesh_opsur);

  }

  /// Placing the EL grids

  // Place the EL gap
  el_gap_logic = new G4LogicalVolume(el_gap_solid, gas_, "EL_GAP");

  new G4PVPlacement(0, G4ThreeVector(0., 0., el_gap_zpos_),
                    el_gap_logic, "EL_GAP", mother_logic_, false, 0, false);
  
  // Create a rotation vector to change the orientation of the EL mesh
  CLHEP::HepRotationZ Roty(el_mesh_rot_);
  G4RotationMatrix* pRot = new G4RotationMatrix();
  pRot->set(Roty);

  new G4PVPlacement(0, G4ThreeVector(0., 0., el_gap_length_/2. + grid_thickn_/2.), el_grid_logic,
                    "EL_GRID_GATE", el_gap_logic, false, 0, false);
  
  new G4PVPlacement(pRot, G4ThreeVector(0., 0., -el_gap_length_/2. - grid_thickn_/2.), el_grid_logic,
                    "EL_GRID_ANODE", el_gap_logic, false, 1, false);

  

  /// Define EL electric field
  if (elfield_) {
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    G4double global_el_gap_zpos = el_gap_zpos_ - GetCoordOrigin().z();
    el_field->SetCathodePosition(global_el_gap_zpos + el_gap_length_/2. + grid_thickn_);
    el_field->SetAnodePosition  (global_el_gap_zpos - el_gap_length_/2. - grid_thickn_);
    el_field->SetDriftVelocity(2.5 * mm/microsecond);
    el_field->SetTransverseDiffusion(ELtransv_diff_);
    el_field->SetLongitudinalDiffusion(ELlong_diff_);
    el_field->SetLightYield(XenonELLightYield(ELelectric_field_, pressure_));
    G4Region* el_region = new G4Region("EL_REGION");
    el_region->SetUserInformation(el_field);
    el_region->AddRootLogicalVolume(el_gap_logic);
  }

  // Vertex generator
  if (el_gap_gen_disk_zmin_ > el_gap_gen_disk_zmax_)
    G4Exception("[Next100FieldCage]", "Next100FieldCage()",
                FatalErrorInArgument, "Error in configuration of EL gap generator: zmax < zmin");

  G4double el_gap_gen_disk_thickn =
    el_gap_length_ * (el_gap_gen_disk_zmax_ - el_gap_gen_disk_zmin_);

  G4double el_gap_gen_disk_z = el_gap_zpos_ + el_gap_length_/2.-
                               el_gap_length_ * el_gap_gen_disk_zmin_ -
                               el_gap_gen_disk_thickn/2.;

  G4ThreeVector el_gap_gen_pos(el_gap_gen_disk_x_, el_gap_gen_disk_y_, el_gap_gen_disk_z);

  el_gap_gen_ =
    new CylinderPointSampler(0., el_gap_gen_disk_diam_/2.,
                             el_gap_gen_disk_thickn/2., 0., twopi,
                             nullptr, el_gap_gen_pos);
  
  // Gate ring vertex generator
  gate_gen_ =
    new CylinderPointSampler(gate_int_diam_/2., gate_ext_diam_/2.,
                             gate_ring_thickn_/2., 0., twopi, nullptr,
                             G4ThreeVector(0., 0., gate_zpos_));
  // Anode ring vertex generator
  anode_gen_ =
    new CylinderPointSampler(gate_int_diam_/2., gate_ext_diam_/2.,
                             anode_ring_thickn/2., 0., twopi, nullptr,
                             G4ThreeVector(0., 0., anode_sim_zpos));


  /// Visibilities
  if (visibility_) {
    G4VisAttributes light_blue = nexus::LightBlue();
    el_gap_logic->SetVisAttributes(light_blue);
  } else {
    el_gap_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }

  if (!grid_visibility_ && !use_dielectric_grid_)
    el_hex_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  
  G4VisAttributes grey = nexus::DarkGrey();
  grey.SetForceSolid(true);
  gate_logic->SetVisAttributes(grey);
  anode_logic->SetVisAttributes(grey);
  el_grid_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  /// Verbosity
  if (verbosity_) {
    G4cout << "EL gap starts in " << (el_gap_zpos_ - el_gap_length_/2.)/mm
           << " mm and ends in " << (el_gap_zpos_ + el_gap_length_/2.)/mm
           << G4endl;
    G4cout << G4endl;
  }
}


void Next100FieldCage::BuildLightTube()
{
  /// DRIFT PART ///
  /// Position of z planes
  G4double zplane[2] = {-teflon_drift_length_/2., teflon_drift_length_/2.};
  /// Inner radius
  G4double rinner[2] = {active_diam_/2., active_diam_/2.};
  /// Outer radius
  G4double router[2] = {(active_diam_ + 2.*teflon_thickn_)/2.,
    (active_diam_ + 2.*teflon_thickn_)/2.};

  G4Polyhedra* teflon_drift_solid =
    new G4Polyhedra("LIGHT_TUBE_DRIFT", 0., twopi, n_panels_, 2, zplane, rinner, router);

  G4LogicalVolume* teflon_drift_logic =
    new G4LogicalVolume(teflon_drift_solid, teflon_, "LIGHT_TUBE_DRIFT");

  new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(),
                                     GetCoordOrigin().y(), teflon_drift_zpos_),
                    teflon_drift_logic, "LIGHT_TUBE_DRIFT", mother_logic_, false, 0, false);


  /// TPB on teflon surface
  G4double router_tpb[2] =
    {(active_diam_ + 2.*tpb_thickn_)/2., (active_diam_ + 2.*tpb_thickn_)/2.};

  G4Polyhedra* tpb_drift_solid =
    new G4Polyhedra("DRIFT_TPB", 0., twopi, n_panels_, 2, zplane, rinner, router_tpb);
  G4LogicalVolume* tpb_drift_logic =
    new G4LogicalVolume(tpb_drift_solid, tpb_, "DRIFT_TPB");
  G4VPhysicalVolume* tpb_drift_phys =
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.),
                      tpb_drift_logic, "DRIFT_TPB", teflon_drift_logic, false, 0, false);

  /// BUFFER PART ///
  G4double zplane_buff[2] = {-teflon_buffer_length_/2., teflon_buffer_length_/2.};
  G4double router_buff[2] =
    {(active_diam_ + 2.*teflon_thickn_)/2., (active_diam_ + 2.*teflon_thickn_)/2.};

  G4Polyhedra* teflon_buffer_solid =
   new G4Polyhedra("LIGHT_TUBE_BUFFER", 0., twopi, n_panels_, 2, zplane_buff, rinner, router_buff);

  G4LogicalVolume* teflon_buffer_logic =
    new G4LogicalVolume(teflon_buffer_solid, teflon_, "LIGHT_TUBE_BUFFER");

  new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(),
                                     GetCoordOrigin().y(), teflon_buffer_zpos_),
                    teflon_buffer_logic, "LIGHT_TUBE_BUFFER", mother_logic_, false, 0, false);

  /// TPB on teflon surface
  G4double router_tpb_buff[2] =
    {(active_diam_ + 2.*tpb_thickn_)/2., (active_diam_ + 2.*tpb_thickn_)/2.};

  G4Polyhedra* tpb_buffer_solid =
    new  G4Polyhedra("BUFFER_TPB", 0., twopi, n_panels_, 2, zplane_buff,
                     rinner, router_tpb_buff);

  G4LogicalVolume* tpb_buffer_logic =
    new G4LogicalVolume(tpb_buffer_solid, tpb_, "BUFFER_TPB");

  G4VPhysicalVolume* tpb_buffer_phys =
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.),
                      tpb_buffer_logic, "BUFFER_TPB", teflon_buffer_logic, false, 0, false);

  /// Optical surface on teflon ///
  G4OpticalSurface* refl_Surf =
    new G4OpticalSurface("refl_Surf", unified, ground, dielectric_metal, .01);
  refl_Surf->SetMaterialPropertiesTable(opticalprops::PTFE());
  new G4LogicalSkinSurface("refl_teflon_surf", teflon_drift_logic, refl_Surf);
  new G4LogicalSkinSurface("refl_teflon_surf", teflon_buffer_logic, refl_Surf);

  /// Optical surface between xenon and TPB to model roughness ///
  G4OpticalSurface* gas_tpb_teflon_surf =
    new G4OpticalSurface("gas_tpb_teflon_surf", glisur, ground,
                         dielectric_dielectric, .01);

  new G4LogicalBorderSurface("gas_tpb_teflon_surf", tpb_drift_phys,
                             active_phys_, gas_tpb_teflon_surf);
  new G4LogicalBorderSurface("gas_tpb_teflon_surf", active_phys_,
                             tpb_drift_phys, gas_tpb_teflon_surf);
  new G4LogicalBorderSurface("gas_tpb_teflon_surf", tpb_buffer_phys,
                             buffer_phys_, gas_tpb_teflon_surf);
  new G4LogicalBorderSurface("gas_tpb_teflon_surf", buffer_phys_,
                             tpb_buffer_phys, gas_tpb_teflon_surf);

  // Vertex generator
  G4double teflon_ext_radius = (active_diam_ + 2.*teflon_thickn_)/2. / cos(pi/n_panels_);
  G4double teflon_zpos = GetCoordOrigin().z() + gate_sapphire_wdw_dist_/2.;
  teflon_gen_ =
    new CylinderPointSampler(active_diam_/2., teflon_ext_radius,
                             gate_sapphire_wdw_dist_/2., 0., twopi,
                             nullptr, G4ThreeVector (GetCoordOrigin().x(),
                                                     GetCoordOrigin().y(),
                                                     teflon_zpos));

  // Visibilities
  if (visibility_) {
    G4VisAttributes light_yellow = nexus::YellowAlpha();
    light_yellow.SetForceSolid(true);
    teflon_drift_logic->SetVisAttributes(light_yellow);
    teflon_buffer_logic->SetVisAttributes(light_yellow);
    tpb_drift_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    tpb_buffer_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
  else {
    teflon_drift_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    teflon_buffer_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    tpb_drift_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    tpb_buffer_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
}


void Next100FieldCage::BuildFieldCage()
{
  // HDPE cylinder.
  // It is placed in such a way that it ends at the same z
  // as the teflon buffer reflector, in the side of the EP.
  G4double hdpe_tube_z_pos = teflon_buffer_zpos_ - (hdpe_length_ - teflon_buffer_length_)/2.;

  G4Tubs* hdpe_tube_solid =
    new G4Tubs("HDPE_TUBE", hdpe_tube_int_diam_/2., hdpe_tube_ext_diam_/2.,
               hdpe_length_/2., 0, twopi);

  G4LogicalVolume* hdpe_tube_logic = new G4LogicalVolume(hdpe_tube_solid, hdpe_, "HDPE_TUBE");

  new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(),
                                     GetCoordOrigin().y(), hdpe_tube_z_pos),
                    hdpe_tube_logic, "HDPE_TUBE", mother_logic_, false, 0, false);

  hdpe_gen_ =
    new CylinderPointSampler(hdpe_tube_int_diam_/2., hdpe_tube_ext_diam_/2.,
                             hdpe_length_/2.,0., twopi, nullptr,
                             G4ThreeVector(GetCoordOrigin().x(),
                                           GetCoordOrigin().y(),
                                           hdpe_tube_z_pos));


  // Buffer field rings
  G4Tubs* ring_solid =
    new G4Tubs("FIELD_RING", ring_int_diam_/2., ring_ext_diam_/2., ring_thickn_/2., 0, twopi);

  G4LogicalVolume* ring_logic =
    new G4LogicalVolume(ring_solid, copper_, "FIELD_RING");


  // Placement of the buffer rings, in such a way that the holder ends
  // at the same z position as the teflon buffer reflector.
  // The placements go from higher to lower z.
  G4double first_ring_buff_z_pos =
    teflon_buffer_zpos_ + teflon_buffer_length_/2. - buffer_first_ring_dist_;

  G4double posz;
  for (G4int i=0; i<num_buffer_rings_; i++) {
    posz = first_ring_buff_z_pos - i*buffer_ring_dist_;
    new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(),
                                       GetCoordOrigin().y(), posz),
                      ring_logic, "FIELD_RING", mother_logic_, false, i, false);
  }

  // Placement of the drift rings.
  G4double first_ring_drift_z_pos = posz - ring_drift_buffer_dist_;
  for (G4int i=0; i<num_drift_rings_; i++) {
    posz = first_ring_drift_z_pos - i*drift_ring_dist_;
    new G4PVPlacement(0, G4ThreeVector(GetCoordOrigin().x(),
                                       GetCoordOrigin().y(), posz),
                      ring_logic, "FIELD_RING", mother_logic_, false, i, false);
  }

  // Ring vertex generator
  G4double ring_gen_length = first_ring_buff_z_pos + ring_thickn_/2. - (posz - ring_thickn_/2.);
  G4double ring_gen_zpos = first_ring_buff_z_pos + ring_thickn_/2. - ring_gen_length/2.;
  ring_gen_ =
    new CylinderPointSampler(ring_int_diam_/2., ring_ext_diam_/2.,
                             ring_gen_length/2., 0., twopi, nullptr,
                             G4ThreeVector(GetCoordOrigin().x(),
                                           GetCoordOrigin().y(),
                                           ring_gen_zpos));

  // Ring holders (a.k.a. staves)

  // They are placed in such a way that they end at the same z position
  // as the teflon buffer reflector, in the side of the EP.
  Next100Stave stave = Next100Stave(ring_drift_buffer_dist_, num_drift_rings_, num_buffer_rings_);
  stave.Construct();
  G4LogicalVolume* stave_logic = stave.GetLogicalVolume();

  G4double holder_long_y      = stave.GetHolderLongY();
  G4double holder_short_y     = stave.GetHolderShortY();
  G4double cathode_long_y     = stave.GetCathodeLongY();
  G4double buffer_long_length = stave.GetBufferLongLength();
  G4double cathode_opening    = stave.GetCathodeOpening();

  G4double stave_r = active_diam_/2. + teflon_thickn_ + holder_long_y +
    holder_short_y - cathode_long_y/2.;
  G4double stave_zpos = teflon_buffer_zpos_ + teflon_buffer_length_/2. -
    buffer_long_length - cathode_opening/2.;

  G4int numbering = 0;
  for (G4int i=10; i<360; i +=20){
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot -> rotateZ((90-i) * deg);
    new G4PVPlacement(rot,
                      G4ThreeVector(stave_r*cos(i*deg) + GetCoordOrigin().x(),
                                    stave_r*sin(i*deg) + GetCoordOrigin().y(),
                                    stave_zpos), stave_logic, "STAVE",
                      mother_logic_, false, numbering, false);
    numbering +=1;
  }

  // Stave vertex generator
  // Length is approximated to avoid complicated calculations.
  // The correct positioning of vertices is checked at run time anyway.
  G4double stave_gen_length =
    teflon_buffer_zpos_ - GetCoordOrigin().z() + teflon_buffer_length_/2;
  holder_gen_ =
    new CylinderPointSampler(active_diam_/2. + teflon_thickn_ ,
                             active_diam_/2. + teflon_thickn_ +
                             holder_long_y + holder_short_y,
                             stave_gen_length/2., 0., twopi, nullptr,
                             G4ThreeVector(GetCoordOrigin().x(),
                                           GetCoordOrigin().y(),
                                           GetCoordOrigin().z() +
                                           stave_gen_length/2.));

   /// Visibilities
  if (visibility_) {
    G4VisAttributes ring_col = nexus::CopperBrown();
    ring_col.SetForceSolid(true);
    ring_logic->SetVisAttributes(ring_col);
    G4VisAttributes hdpe_col =nexus::WhiteAlpha();
    hdpe_col.SetForceSolid(true);
    hdpe_tube_logic->SetVisAttributes(hdpe_col);
    G4VisAttributes stave_col = nexus::LightGrey();
    stave_col.SetForceSolid(true);
    stave_logic->SetVisAttributes(stave_col);
  } else {
    ring_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    hdpe_tube_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    stave_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
}


Next100FieldCage::~Next100FieldCage()
{
  delete active_gen_;
  delete buffer_gen_;
  delete xenon_gen_;
  delete teflon_gen_;
  delete el_gap_gen_;
  delete hdpe_gen_;
  delete ring_gen_;
  delete cathode_gen_;
  delete gate_gen_;
  delete anode_gen_;
  delete holder_gen_;
}


G4ThreeVector Next100FieldCage::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex(0., 0., 0.);

  if (region == "CENTER") {
    vertex = G4ThreeVector(GetCoordOrigin().x(), GetCoordOrigin().y(),
                           active_zpos_);
  }

  else if (region == "ACTIVE") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = active_gen_->GenerateVertex(VOLUME);
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx - GetCoordOrigin();
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else if (region == "CATHODE_RING") {
    vertex = cathode_gen_->GenerateVertex(VOLUME);
  }

  else if (region == "BUFFER") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = buffer_gen_->GenerateVertex(VOLUME);
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx - GetCoordOrigin();
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else if (region == "XENON") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = xenon_gen_->GenerateVertex(VOLUME);
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx - GetCoordOrigin();
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
      vertex = teflon_gen_->GenerateVertex(VOLUME);
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx - GetCoordOrigin();
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (
    VertexVolume->GetName() != "LIGHT_TUBE_DRIFT" &&
    VertexVolume->GetName() != "LIGHT_TUBE_BUFFER" );
  }

  else if (region == "HDPE_TUBE") {
    vertex = hdpe_gen_->GenerateVertex(VOLUME);
  }

  else if (region == "EL_GAP") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = el_gap_gen_->GenerateVertex(VOLUME);
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx - GetCoordOrigin();
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else if (region == "FIELD_RING") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = ring_gen_->GenerateVertex(VOLUME);
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx - GetCoordOrigin();
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else if (region == "GATE_RING") {
    vertex = gate_gen_->GenerateVertex(VOLUME);
  }

  else if (region == "ANODE_RING") {
    vertex = anode_gen_->GenerateVertex(VOLUME);
  }

  else if (region == "RING_HOLDER"){
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex = holder_gen_->GenerateVertex(VOLUME);
      G4ThreeVector glob_vtx(vertex);
      glob_vtx = glob_vtx - GetCoordOrigin();
      VertexVolume =
        geom_navigator_->LocateGlobalPointAndSetup(glob_vtx, 0, false);
    } while (VertexVolume->GetName() != "STAVE");
 }

  else {
    G4Exception("[Next100FieldCage]", "GenerateVertex()", FatalException,
    "Unknown vertex generation region!");
  }

  return vertex;
}


G4ThreeVector Next100FieldCage::GetActivePosition() const
{
  return G4ThreeVector (0., 0., active_zpos_);
}
