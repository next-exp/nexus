// -----------------------------------------------------------------------------
// nexus | NextNewFieldCage.cc
//
// Field cage of the NEXT-WHITE detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextNewFieldCage.h"

#include "MaterialsList.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "OpticalMaterialProperties.h"
#include "IonizationSD.h"
#include "XenonGasProperties.h"
#include "CylinderPointSampler.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4NistManager.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>
#include <G4TransportationManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>

using namespace CLHEP;


namespace nexus {


  NextNewFieldCage::NextNewFieldCage():
    BaseGeometry(),
    mother_logic_(nullptr), mother_phys_(nullptr),
    // Field cage dimensions
    dist_feedthroughs_(514. * mm), // distance between the centres of the feedthroughs
    cathode_thickness_(.1 * mm),
    cathode_gap_ (20. * mm),
    windows_end_z_ (-386.999 * mm), // position in gas where the sapphire windows end. To be read from NextNewEnergyPlane.cc
    tube_in_diam_ (416. * mm),
    tube_length_drift_ (507. * mm),
    tube_thickness_ (1.0 * cm),
    dist_tube_el_ (15. * mm),
    hdpe_length_ (632. * mm),
    hdpe_in_diam_ (452. * mm),
    hdpe_out_diam_ (490. * mm),
    hdpe_ledge_ (12. * mm),
    ring_width_ (10. * mm),
    ring_thickness_ (3. * mm),
    tpb_thickness_(1. * micrometer),
    el_gap_length_ (6 * mm),
    grid_thickness_ (.1 * mm), //it's just fake dielectric
    gate_transparency_ (.84),
    anode_quartz_thickness_ (3. * mm),
    anode_quartz_diam_ (522. * mm),
    cathode_grid_transparency_ (.98),
    pedot_thickness_(200.*nm),
    ito_thickness_  ( 15.*nm),
    //
    ELtransv_diff_(0. * mm/sqrt(cm)),
    ELlong_diff_(0. * mm/sqrt(cm)),
    drift_transv_diff_(1. * mm/sqrt(cm)),
    drift_long_diff_(.3 * mm/sqrt(cm)),
    //
    ELelectric_field_(34.5 * kilovolt/cm), // it corresponds to 2.3 kV/cm/bar at 15 bar
    // Visibility
    visibility_(1),
    // Step limiter
    max_step_size_(1. * mm),
    // EL field ON or OFF
    elfield_(0),
    el_table_index_(0),
    el_table_binning_(5. * mm),
    // EL gap generation disk parameters
    el_gap_gen_disk_diam_(0.),
    el_gap_gen_disk_x_(0.), el_gap_gen_disk_y_(0.),
    el_gap_gen_disk_zmin_(0.), el_gap_gen_disk_zmax_(1.),
    photoe_prob_(0)
  {
    // Derived dimensions
    buffer_length_ =
      - dist_feedthroughs_/2. - cathode_thickness_ - windows_end_z_;// from the end of the cathode to surface of sapphire windows // It's 129.9 mm

    el_gap_z_pos_ =
      -dist_feedthroughs_/2. + cathode_gap_/2. +  tube_length_drift_ + dist_tube_el_ + el_gap_length_/2.;
    pos_z_anode_ =
      el_gap_z_pos_ + el_gap_length_/2. +  anode_quartz_thickness_/2.+ 0.1*mm; // 0.1 mm is needed because EL is produced only if the PostStepVolume is GAS material.

     // The centre of the feedthrough is the limit of the active volume.
    pos_z_cathode_ = - dist_feedthroughs_/2. - cathode_thickness_/2.;

    el_table_z_ = el_gap_z_pos_ - el_gap_length_/2.;
    //G4cout << "***** " << el_table_z_ + 0.1 * mm << G4endl;

    // 0.1 * mm is added to avoid very small negative numbers in drift lengths
    SetELzCoord(el_table_z_ + 0.1 * mm);

    // 0.2 * mm is added because ie- in EL table generation must start inside the volume, not on border
    el_table_z_ = el_table_z_ + .2*mm;

    // Define a new category
    new G4UnitDefinition("kilovolt/cm","kV/cm","Electric field", kilovolt/cm);
    new G4UnitDefinition("mm/sqrt(cm)","mm/sqrt(cm)","Diffusion", mm/sqrt(cm));

    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/",
				  "Control commands of geometry NextNew.");

    msg_->DeclareProperty("field_cage_vis", visibility_,
			  "Field Cage Visibility");
    msg_->DeclareProperty("elfield", elfield_,
			  "True if the EL field is on (full simulation), false if it's not (parametrized simulation.");

    G4GenericMessenger::Command& step_cmd =
      msg_->DeclareProperty("max_step_size", max_step_size_,
			    "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");

    G4GenericMessenger::Command&  specific_vertex_X_cmd =
      msg_->DeclareProperty("specific_vertex_X", specific_vertex_X_,
			    "If region is AD_HOC, x coord where particles are generated");
    specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
    specific_vertex_X_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Y_cmd =
      msg_->DeclareProperty("specific_vertex_Y", specific_vertex_Y_,
			    "If region is AD_HOC, y coord where particles are generated");
    specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
    specific_vertex_Y_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Z_cmd =
      msg_->DeclareProperty("specific_vertex_Z", specific_vertex_Z_,
			    "If region is AD_HOC, z coord where particles are generated");
    specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
    specific_vertex_Z_cmd.SetUnitCategory("Length");

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

    G4GenericMessenger::Command& El_field_cmd =
      msg_->DeclareProperty("EL_field", ELelectric_field_,
			    "Electric field in the EL region");
    El_field_cmd.SetParameterName("EL_field", true);
    El_field_cmd.SetUnitCategory("Electric field");

    // Calculate vertices for EL table generation
    G4GenericMessenger::Command& pitch_cmd =
      msg_->DeclareProperty("el_table_binning", el_table_binning_,
			    "Pitch for EL generation");
    pitch_cmd.SetUnitCategory("Length");
    pitch_cmd.SetParameterName("el_table_binning", false);
    pitch_cmd.SetRange("el_table_binning>0.");

    msg_->DeclareProperty("el_table_point_id", el_table_point_id_, "");

     G4GenericMessenger::Command& eltable_z_cmd =
      msg_->DeclareProperty("el_table_z", el_table_z_,
			    "Z coordinate for EL generation");
    eltable_z_cmd.SetUnitCategory("Length");
    eltable_z_cmd.SetParameterName("el_table_z", false);

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

    msg_->DeclareProperty("photoe_prob", photoe_prob_,
          "Probability of photon to ie- conversion");
  }


  NextNewFieldCage::~NextNewFieldCage()
  {
    delete drift_tube_gen_;
    delete hdpe_tube_gen_;
    delete active_gen_;
    delete el_gap_gen_;
    delete buffer_gen_;
    delete xenon_gen_;
    delete anode_quartz_gen_;
    delete cathode_gen_;
    delete tracking_frames_gen_;
  }


  void NextNewFieldCage::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
  {
    mother_logic_ = mother_logic;
  }


  void NextNewFieldCage::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
  {
    mother_phys_ = mother_phys;
  }


  void NextNewFieldCage::Construct()
  {
    // Define materials to be used
    DefineMaterials();
     // Cathode Grid
    BuildCathodeGrid();
    // Build the different parts
     // ACTIVE region
    BuildActive();
    // Buffer region
    BuildBuffer();
    // EL Region
    BuildELRegion();
    //build the quartz anode
    BuildAnodeGrid();
    // Proper field cage and light tube
    BuildFieldCage();
    // Tracking Frames
    BuildTrackingFrames();

    G4double max_radius = floor(tube_in_diam_/2./el_table_binning_)*el_table_binning_;

    CalculateELTableVertices(max_radius, el_table_binning_, el_table_z_);

    // for (G4int i=0; i<el_table_vertices_.size(); ++i) {
    //   std::cout << i << ": "<< el_table_vertices_[i] << std::endl;
    // }
  }


  void  NextNewFieldCage::DefineMaterials()
  {
    gas_         = mother_logic_->GetMaterial();
    pressure_    = gas_->GetPressure();
    temperature_ = gas_->GetTemperature();
    sc_yield_    = gas_->GetMaterialPropertiesTable()->GetConstProperty("SCINTILLATIONYIELD");
    e_lifetime_  = gas_->GetMaterialPropertiesTable()->GetConstProperty("ATTACHMENT");

    // High density polyethylene for the field cage
    hdpe_ = MaterialsList::HDPE();

    // Copper for field rings
    copper_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    // Teflon for the light tube
    teflon_ =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
    // TPB coating
    tpb_ = MaterialsList::TPB();
    tpb_->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
    //ITO coating
    ito_ = MaterialsList::ITO();
    ito_->SetMaterialPropertiesTable(OpticalMaterialProperties::ITO());
    // PEDOT coating
    pedot_ = MaterialsList::PEDOT();
    pedot_->SetMaterialPropertiesTable(OpticalMaterialProperties::PEDOT());
    // Quartz
    quartz_ =  MaterialsList::FusedSilica();
    quartz_->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
  }


  void NextNewFieldCage::BuildCathodeGrid()
  {
    ///// CATHODE //////
    G4Material* fgrid_mat =
      MaterialsList::FakeDielectric(gas_, "cath_grid_mat");
    fgrid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(pressure_,
                                                                              temperature_,
                                                                              cathode_grid_transparency_,
                                                                              cathode_thickness_));
    // Dimensions & position
    G4double grid_diam = tube_in_diam_;

    // Building the grid
    G4Tubs* diel_grid_solid =
      new G4Tubs("CATHODE_GRID", 0., grid_diam/2., cathode_thickness_/2., 0, twopi);
    G4LogicalVolume* diel_grid_logic =
      new G4LogicalVolume(diel_grid_solid, fgrid_mat, "CATHODE_GRID");
    new G4PVPlacement(0, G4ThreeVector(0., 0., pos_z_cathode_), diel_grid_logic,
		      "CATHODE_GRID", mother_logic_, false, 0, false);

    if (visibility_) {
      G4VisAttributes cathode_col = nexus::Red();
      diel_grid_logic->SetVisAttributes(cathode_col);
    }
  }


  void NextNewFieldCage::BuildActive()
  {
    active_length_ = cathode_gap_/2. + tube_length_drift_ + dist_tube_el_;
    active_posz_   = -dist_feedthroughs_/2.  +  active_length_/2.;

    G4Tubs* active_solid =
      new G4Tubs("ACTIVE",  0., tube_in_diam_/2., active_length_/2., 0, twopi);
    // G4cout << "Active starts in " <<  active_posz  -  active_length/2. << " and ends in "
    // 	   << active_posz +  active_length/2. << G4endl;

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, gas_, "ACTIVE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., active_posz_), active_logic,
                      "ACTIVE", mother_logic_, false, 0, false);

    // Limit the step size in this volume for better tracking precision
    active_logic->SetUserLimits(new G4UserLimits(max_step_size_));

    // Set the volume as an ionization sensitive detector
    IonizationSD* ionisd = new IonizationSD("/NEXTNEW/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Define a drift field for this volume
    UniformElectricDriftField* field = new UniformElectricDriftField();
    // electrodes are at the end of active region
    field->SetCathodePosition(-(active_posz_ - active_length_/2.) + GetELzCoord());
    field->SetAnodePosition(-(active_posz_ + active_length_/2.) + GetELzCoord());
    field->SetDriftVelocity(1. * mm/microsecond);
    field->SetTransverseDiffusion(drift_transv_diff_);
    field->SetLongitudinalDiffusion(drift_long_diff_);
    G4Region* drift_region = new G4Region("DRIFT");
    drift_region->SetUserInformation(field);
    drift_region->AddRootLogicalVolume(active_logic);

    /// Visibilities
    active_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // VERTEX GENERATOR
    active_gen_ =
      new CylinderPointSampler(0., active_length_, tube_in_diam_/2.,
                               0., G4ThreeVector (0., 0., active_posz_));
  }

void NextNewFieldCage::BuildBuffer()
  {
    //G4double length = buffer_length_ - cathode_gap_/2.;
    G4double buffer_posz =
      -dist_feedthroughs_/2.  - cathode_thickness_ -  buffer_length_/2.;
    G4Tubs* buffer_solid =
      new G4Tubs("BUFFER",  0., hdpe_in_diam_/2.,
		 buffer_length_ /2., 0, twopi);

    // G4cout << "Buffer (gas) starts in " << buffer_posz - buffer_length_/2. << " and ends in "
    // 	   << buffer_posz + buffer_length_/2. << G4endl;

    G4LogicalVolume* buffer_logic =
      new G4LogicalVolume(buffer_solid, gas_, "BUFFER");

    new G4PVPlacement(0, G4ThreeVector(0., 0., buffer_posz), buffer_logic,
                      "BUFFER", mother_logic_, false, 0, false);

     // Set the volume as an ionization sensitive detector
    IonizationSD* buffsd = new IonizationSD("/NEXTNEW/BUFFER");
    buffsd->IncludeInTotalEnergyDeposit(false);
    buffer_logic->SetSensitiveDetector(buffsd);
    G4SDManager::GetSDMpointer()->AddNewDetector(buffsd);

    // VERTEX GENERATOR
    buffer_gen_ =
      new CylinderPointSampler(0., buffer_length_, hdpe_in_diam_/2.,
                               0., G4ThreeVector (0., 0., buffer_posz));

    // VERTEX GENERATOR FOR ALL XENON
    G4double xenon_posz = (buffer_length_ * buffer_posz +
			   active_length_ * active_posz_ +
			   cathode_thickness_ * pos_z_cathode_ +
			   el_gap_length_ * el_gap_z_pos_) / (buffer_length_ +
							active_length_ +
							cathode_thickness_ +
							el_gap_length_);
    G4double xenon_len = buffer_length_ + active_length_
      + cathode_thickness_ + el_gap_length_;
    xenon_gen_ =
      new CylinderPointSampler(0., xenon_len, tube_in_diam_/2.,
                               0., G4ThreeVector (0., 0., xenon_posz));
  }

  void NextNewFieldCage::BuildELRegion()
  {
    ///// EL GAP /////
    G4double el_gap_diam = tube_in_diam_ + 5. * mm;
    G4Tubs* el_gap_solid =
      new G4Tubs("EL_GAP", 0., el_gap_diam/2., el_gap_length_/2., 0, twopi);
    G4LogicalVolume* el_gap_logic =
      new G4LogicalVolume(el_gap_solid, gas_, "EL_GAP");
    new G4PVPlacement(0, G4ThreeVector(0., 0., el_gap_z_pos_), el_gap_logic,
		      "EL_GAP", mother_logic_, false, 0, false);
    // G4cout << "EL gap region starts in " << el_gap_z_pos_ - el_gap_length_/2. << " and ends in "
    //        << el_gap_z_pos_ + el_gap_length_/2. << G4endl;

    if (elfield_) {
      // Define EL electric field
      UniformElectricDriftField* el_field = new UniformElectricDriftField();
      el_field->SetCathodePosition(-(el_gap_z_pos_ - el_gap_length_/2.) + GetELzCoord());
      el_field->SetAnodePosition(-(el_gap_z_pos_ + el_gap_length_/2.) + GetELzCoord());
      el_field->SetDriftVelocity(2.5 * mm/microsecond);
      el_field->SetTransverseDiffusion(ELtransv_diff_);
      el_field->SetLongitudinalDiffusion(ELlong_diff_);
      XenonGasProperties xgp(pressure_, temperature_);
      el_field->SetLightYield(xgp.ELLightYield(ELelectric_field_));
      G4Region* el_region = new G4Region("EL_REGION");
      el_region->SetUserInformation(el_field);
      el_region->AddRootLogicalVolume(el_gap_logic);
    }

    ///// EL GRIDS /////

    G4Material* fgate_mat =
      MaterialsList::FakeDielectric(gas_, "el_grid_gate_mat");
    // We have to set the defaults explicitely because C++ doesn't support
    // named arguments
    fgate_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(pressure_,
                                                                              temperature_,
                                                                              gate_transparency_,
                                                                              grid_thickness_,
                                                                              sc_yield_,
                                                                              e_lifetime_,
                                                                              photoe_prob_));

    // Dimensions & position: the grids are simulated inside the EL gap.
    // Their thickness is symbolic.
    G4double grid_diam = tube_in_diam_; // active_diam_;
    G4double poszInner =  - el_gap_length_/2. + grid_thickness_/2.;

    G4Tubs* diel_grid_solid =
      new G4Tubs("EL_GRID", 0., grid_diam/2., grid_thickness_/2., 0, twopi);

    G4LogicalVolume* gate_logic =
      new G4LogicalVolume(diel_grid_solid, fgate_mat, "EL_GRID_GATE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., poszInner), gate_logic,
		      "EL_GRID_GATE", el_gap_logic, false, 0, false);

    // Vertex generator
    G4double el_gap_gen_disk_thickn =
      el_gap_length_ * (el_gap_gen_disk_zmax_ - el_gap_gen_disk_zmin_);

    G4double el_gap_gen_disk_z = el_gap_z_pos_ - el_gap_length_/2.
      + el_gap_length_ * el_gap_gen_disk_zmin_ + el_gap_gen_disk_thickn/2.;

    // (We change below the sign of x to correct the effect that a rotation
    //  will introduce later.)
    G4ThreeVector el_gap_gen_pos(-el_gap_gen_disk_x_,
                                 el_gap_gen_disk_y_,
                                 el_gap_gen_disk_z);

    el_gap_gen_ = new CylinderPointSampler(0., el_gap_gen_disk_thickn, el_gap_gen_disk_diam_/2.,
                                           0., el_gap_gen_pos);

    /// Visibilities
    if (visibility_) {
      G4VisAttributes gap_col = nexus::LightGrey();
      gap_col.SetForceSolid(true);
      el_gap_logic->SetVisAttributes(gap_col);
      G4VisAttributes gate_col = nexus::LightBlue();
      // gate_col.SetForceSolid(true);
      gate_logic->SetVisAttributes(gate_col);
    }
    else {
      gate_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  void NextNewFieldCage::BuildAnodeGrid()
  {
    G4double anode_diam = anode_quartz_diam_;

    ///// ANODE //////

    G4Tubs* anode_quartz_solid =
      new G4Tubs("QUARTZ_ANODE", 0., anode_diam/2. , anode_quartz_thickness_/2., 0, twopi);
    G4LogicalVolume* anode_logic =
      new G4LogicalVolume(anode_quartz_solid, quartz_, "EL_QUARTZ_ANODE");
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., pos_z_anode_),
                      anode_logic, "EL_QUARTZ_ANODE", mother_logic_, false, 0, false);

    G4Tubs* tpb_anode_solid =
      new G4Tubs("TPB_ANODE", 0., anode_diam/2. , tpb_thickness_/2., 0, twopi);
    G4LogicalVolume* tpb_anode_logic =
      new G4LogicalVolume(tpb_anode_solid, tpb_, "TPB_ANODE");
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -anode_quartz_thickness_/2.+tpb_thickness_/2.),
                      tpb_anode_logic, "TPB_ANODE", anode_logic, false, 0, false);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0.,  anode_quartz_thickness_/2.-tpb_thickness_/2.),
                      tpb_anode_logic, "TPB_ANODE", anode_logic, false, 1, false);

    // Optical surface between gas and TPB to model the latter's roughness
    G4OpticalSurface* tpb_anode_surf =
      new G4OpticalSurface("TPB_ANODE_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);
    new G4LogicalSkinSurface("TPB_ANODE_OPSURF", tpb_anode_logic, tpb_anode_surf);

    G4Tubs* pedot_anode_solid =
      new G4Tubs("PEDOT_ANODE", 0., anode_diam/2. , pedot_thickness_/2., 0, 360.*deg);
    G4LogicalVolume* pedot_anode_logic =
      new G4LogicalVolume(pedot_anode_solid, pedot_, "PEDOT_ANODE");
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -anode_quartz_thickness_/2.+tpb_thickness_+pedot_thickness_/2.),
                      pedot_anode_logic, "PEDOT_ANODE", anode_logic, false, 0, false);

    G4Tubs* ito_anode_solid =
      new G4Tubs("ITO_ANODE", 0., anode_diam/2. , ito_thickness_/2., 0, twopi);
    G4LogicalVolume* ito_anode_logic =
      new G4LogicalVolume(ito_anode_solid, ito_, "ITO_ANODE");
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., anode_quartz_thickness_/2.-tpb_thickness_-ito_thickness_/2.),
                      ito_anode_logic, "ITO_ANODE", anode_logic, false, 0, false);

    if (visibility_) {
      G4VisAttributes anode_col = nexus::Red();
      anode_col.SetForceSolid(true);
      anode_logic->SetVisAttributes(anode_col);
      G4VisAttributes tpb_col = nexus::DarkGreen();
      //tpb_col.SetForceSolid(true);
      tpb_anode_logic->SetVisAttributes(tpb_col);
    } else {
      anode_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_anode_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

  }


  void NextNewFieldCage::BuildFieldCage()
  {

    // High density polyethylene tube to support copper rings
    G4double hdpe_tube_z_pos =
      el_gap_z_pos_ -el_gap_length_/2. - dist_tube_el_ - hdpe_ledge_ - hdpe_length_/2.;

    G4Tubs* hdpe_tube_solid =
      new G4Tubs("HDPE_TUBE", hdpe_in_diam_/2.,
                 hdpe_out_diam_/2., hdpe_length_/2., 0, twopi);

    G4LogicalVolume* hdpe_tube_logic = new G4LogicalVolume(hdpe_tube_solid,
                                                           hdpe_, "HDPE_TUBE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., hdpe_tube_z_pos), hdpe_tube_logic,
                                       "HDPE_TUBE", mother_logic_, false, 0, false);
    // G4cout << "Hdpe tube starts in " << hdpe_tube_z_pos - hdpe_length_/2.  <<
    //   " and ends in " << hdpe_tube_z_pos + hdpe_length_/2. << G4endl;

    // Copper rings
    G4double ring_in_diam = hdpe_in_diam_ - 2. * ring_thickness_;
    G4double ring_out_diam =hdpe_in_diam_;
    G4Tubs* ring_solid = new G4Tubs("FIELD_RING", ring_in_diam/2., ring_out_diam/2.,
                                    ring_width_/2., 0, twopi);

    G4LogicalVolume* ring_logic = new G4LogicalVolume(ring_solid, copper_, "FIELD_RING");

    G4int num_rings = 42;
    G4double separation = 1.80 * mm;
    G4double pitch = ring_width_ + separation;
    G4double posz = hdpe_tube_z_pos + hdpe_length_/2. - ring_width_/2. + separation;

    for (G4int i=0; i<num_rings; i++) {
      new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
                        "FIELD_RING", mother_logic_, false, i, false);
      posz = posz - pitch;
    }

    // Light  tube
    G4double drift_tube_z_pos =
      el_gap_z_pos_ -el_gap_length_/2. - dist_tube_el_ - tube_length_drift_/2.;

    G4Tubs* drift_tube_solid =
      new G4Tubs("DRIFT_TUBE", tube_in_diam_/2., tube_in_diam_/2. + tube_thickness_,
                 tube_length_drift_/2., 0, twopi);
    // G4double fieldcagevol= drift_tube_solid->GetCubicVolume();
    // std::cout<<"FIELD CAGE VOLUME:\t"<<fieldcagevol<<std::endl;

    G4LogicalVolume* drift_tube_logic = new G4LogicalVolume(drift_tube_solid,
                                                            teflon_, "DRIFT_TUBE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., drift_tube_z_pos),
                      drift_tube_logic, "DRIFT_TUBE", mother_logic_,
                      false, 0, false);
    // G4cout << "Light tube drift starts in " << drift_tube_z_pos - tube_length_drift_/2.  <<
    //   " and ends in " << drift_tube_z_pos + tube_length_drift_/2. << G4endl;

    G4Tubs* tpb_drift_solid =
      new G4Tubs("DRIFT_TPB", tube_in_diam_/2., tube_in_diam_/2. + tpb_thickness_,
                 tube_length_drift_/2., 0, twopi);

    G4LogicalVolume* tpb_drift_logic =
      new G4LogicalVolume(tpb_drift_solid, tpb_, "DRIFT_TPB");

    G4VPhysicalVolume* tpb_drift_phys =
      new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), tpb_drift_logic,
                        "DRIFT_TPB", drift_tube_logic, false, 0, false);


    /// OPTICAL SURFACE PROPERTIES    ////////
    G4OpticalSurface* reflector_opt_surf = new G4OpticalSurface("REFLECTOR");
    reflector_opt_surf->SetType(dielectric_metal);
    reflector_opt_surf->SetModel(unified);
    reflector_opt_surf->SetFinish(ground);
    reflector_opt_surf->SetSigmaAlpha(0.01);
    reflector_opt_surf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
    new G4LogicalSkinSurface("DRIFT_TUBE", drift_tube_logic,
    			     reflector_opt_surf);

    // Optical surface between gas and TPB to model the latter's roughness
    G4OpticalSurface* gas_tpb_teflon_surf =
      new G4OpticalSurface("GAS_TPB_TEFLON_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);

    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", tpb_drift_phys, mother_phys_,
                               gas_tpb_teflon_surf);
    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", mother_phys_, tpb_drift_phys,
                               gas_tpb_teflon_surf);

    // We set the reflectivity of HDPE to 50% for the moment
    G4OpticalSurface* abs_opt_surf = new G4OpticalSurface("ABSORBER");
    abs_opt_surf->SetType(dielectric_metal);
    abs_opt_surf->SetModel(unified);
    abs_opt_surf->SetFinish(ground);
    abs_opt_surf->SetSigmaAlpha(0.1);
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
    const G4int REFL_NUMENTRIES = 6;
    G4double ENERGIES[REFL_NUMENTRIES] =
      {1.0*eV, 2.8*eV, 4.*eV, 6.*eV, 7.2*eV, 30.*eV};
    G4double REFLECTIVITY[REFL_NUMENTRIES] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, REFL_NUMENTRIES);

    abs_opt_surf->SetMaterialPropertiesTable(mpt);
    new G4LogicalSkinSurface("HDPE_TUBE", hdpe_tube_logic,
                             abs_opt_surf);

    /// SETTING VISIBILITIES   //////////
    if (visibility_) {
      G4VisAttributes tube_col = nexus::LightBlue();
      tube_col.SetForceSolid(true);
      drift_tube_logic->SetVisAttributes(tube_col);
      G4VisAttributes hdpe_col = nexus::DarkGreen();
      //     hdpe_col.SetForceSolid(true);
      hdpe_tube_logic->SetVisAttributes(hdpe_col);
      G4VisAttributes tpb_col = nexus::DarkGreen();
      tpb_drift_logic->SetVisAttributes(tpb_col);
      G4VisAttributes ring_col = nexus::White();
      ring_col.SetForceSolid(true);
      ring_logic->SetVisAttributes(ring_col);
    } else {
      drift_tube_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_drift_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    /// VERTEX GENERATORS   //////////
    hdpe_tube_gen_  =
      new CylinderPointSampler(hdpe_in_diam_/2., hdpe_length_,
                               hdpe_out_diam_/2. - hdpe_in_diam_/2.,
                               0., G4ThreeVector (0., 0., hdpe_tube_z_pos));

    // G4double posz = hdpe_tube_z_pos + hdpe_length_/2. - ring_width_/2. + separation;
    // ring_gen_ =
    //   new CylinderPointSampler(ring_in_diam,ring_width_, ring_out_diam/2.,
    // 			                      0., G4ThreeVector (0., 0., pos_z_anode_));

    drift_tube_gen_  =
      new CylinderPointSampler(tube_in_diam_/2., tube_length_drift_, tube_thickness_,
                               0., G4ThreeVector (0., 0., drift_tube_z_pos));

    anode_quartz_gen_ =
      new CylinderPointSampler(0., anode_quartz_thickness_, anode_quartz_diam_/2.,
                               0., G4ThreeVector (0., 0., pos_z_anode_));

    cathode_gen_ =
      new CylinderPointSampler(0., cathode_thickness_, tube_in_diam_/2.,
                               0., G4ThreeVector (0., 0., pos_z_cathode_));
  }


  void NextNewFieldCage::BuildTrackingFrames()
  {
    // Tracking Frames are the responsible of fixing the ANODE and GATE to the copper structure
    // They are 2 different rings made of stainless steel, and very similar in size and placement.
    // In the area were there rings live, there is another HDPE structure that somehow shields
    // different parts of the detector.
    // In current implementation the HDPE is not implemented, and only one of the 2 S-Steel rings
    // is simulated with a very close size to both of them.

    // Dimensions
    G4double frame_length =  10. * mm;
    G4double frame_thickn =  16. * mm;
    G4double frame_in_rad = 250. * mm;
    G4double frame_posz   = pos_z_anode_ - 8. * mm;

    // Construction
    G4Tubs* tracking_frame_solid = new G4Tubs("TRACKING_FRAMES", frame_in_rad,
                                              frame_in_rad + frame_thickn,
                                              frame_length/2., 0, twopi);

    G4LogicalVolume* tracking_frame_logic = new G4LogicalVolume(tracking_frame_solid,
                                                                MaterialsList::Steel316Ti(),
                                                                "TRACKING_FRAMES");

    new G4PVPlacement(0, G4ThreeVector(0., 0., frame_posz), tracking_frame_logic,
                      "TRACKING_FRAMES", mother_logic_, false, 0, false);

    // Visibility
    if (visibility_) {
      G4VisAttributes frame_col = nexus::DarkGrey();
      frame_col.SetForceSolid(true);
      tracking_frame_logic->SetVisAttributes(frame_col);
    } else {
      tracking_frame_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    // Vertex generator
    tracking_frames_gen_ = new CylinderPointSampler(frame_in_rad, frame_length, frame_thickn,
                                                    0., G4ThreeVector (0., 0., frame_posz));
  }


  // Vertex Generator
  G4ThreeVector NextNewFieldCage::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "CENTER") {
      vertex = G4ThreeVector(0., 0., active_posz_);
    }
    else if (region == "DRIFT_TUBE") {
      vertex = drift_tube_gen_->GenerateVertex("BODY_VOL");
    }
    else if (region == "HDPE_TUBE") {
      vertex = hdpe_tube_gen_->GenerateVertex("BODY_VOL");
    }
    else if (region == "XENON") {
      G4Navigator *geom_navigator =
        G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
      G4String volume_name;
      do {
        vertex = xenon_gen_->GenerateVertex("BODY_VOL");
        G4ThreeVector glob_vtx(vertex);
        CalculateGlobalPos(glob_vtx);
        volume_name =
          geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false)->GetName();
      } while (volume_name == "CATHODE_GRID" || volume_name == "EL_GRID_GATE");
    }
    else if (region == "BUFFER") {
      vertex = buffer_gen_->GenerateVertex("BODY_VOL");
    }
    else if (region == "ACTIVE") {
      vertex = active_gen_->GenerateVertex("BODY_VOL");
    }
    else if (region == "EL_GAP") {
      vertex = el_gap_gen_->GenerateVertex("BODY_VOL");
    }
    else if (region == "ANODE_QUARTZ") {
      vertex = anode_quartz_gen_->GenerateVertex("BODY_VOL");
    }
    else if (region == "CATHODE") {
      vertex = cathode_gen_->GenerateVertex("BODY_VOL");
    }
    else if (region == "TRACKING_FRAMES") {
      vertex = tracking_frames_gen_->GenerateVertex("BODY_VOL");
    }
    else if (region == "AD_HOC") {
      vertex = G4ThreeVector(specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_);
    }
    else if (region == "EL_TABLE") {
      unsigned int i = el_table_point_id_ + el_table_index_;
      if (i == (el_table_vertices_.size()-1)) {
        G4Exception("[NextNewFieldcage]", "GenerateVertex()",
		    RunMustBeAborted, "Reached last event in EL lookup table.");
      }
      try {
        vertex = el_table_vertices_.at(i);
        el_table_index_++;
      }
      catch (const std::out_of_range& oor) {
        G4Exception("[NextNewFieldCage]", "GenerateVertex()", FatalErrorInArgument,
                    "EL lookup table point out of range.");
      }
    }

    else {
      G4Exception("[NextNewFieldCage]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    return vertex;
  }


  void NextNewFieldCage::CalculateELTableVertices(G4double radius, G4double binning, G4double z)
  {
    // Calculate the xyz positions of the points of an EL lookup table
    // (arranged as a square grid) given a certain binning

    G4ThreeVector xyz(0.,0.,z);

    G4int imax = floor(2.*radius/binning); // max bin number (minus 1)

    for (G4int i=0; i<imax+1; ++i) { // Loop through the x bins

      xyz.setX(-radius + i*binning); // x position

      for (G4int j=0; j<imax+1; ++j) { // Loop through the y bins

        xyz.setY(-radius + j*binning); // y position

        // Store the point if it's inside the active volume defined by the
        // field cage (of circular cross section). Discard it otherwise.
        if (sqrt(xyz.x()*xyz.x()+xyz.y()*xyz.y()) <= radius)
          el_table_vertices_.push_back(xyz);
      }
    }
  }


  G4ThreeVector NextNewFieldCage::GetPosition() const
  {
    return G4ThreeVector(0., 0., tube_z_pos_);
  }

}//end namespace nexus
