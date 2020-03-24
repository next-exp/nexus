// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 12 Sept 2013
//
//  Copyright (c) 2013-2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextNewFieldCage.h"

#include "MaterialsList.h"
#include "IonizationSD.h"
#include "PmtSD.h"
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
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

using namespace CLHEP;


namespace nexus {


  NextNewFieldCage::NextNewFieldCage():
    BaseGeometry(),
    _mother_logic(nullptr), _mother_phys(nullptr),
    // Field cage dimensions
    _dist_feedthroughs(514. * mm), // distance between the centres of the feedthroughs
    _cathode_thickness(.1 * mm),
    _cathode_gap (20. * mm),
    _windows_end_z (-386.999 * mm), // position in gas where the sapphire windows end. To be read from NextNewEnergyPlane.cc
    _tube_in_diam (416. * mm),
    _tube_length_drift (507. * mm),
    _tube_thickness (1.0 * cm),
    _dist_tube_el (15. * mm),
    _hdpe_length (632. * mm),
    _hdpe_in_diam (452. * mm),
    _hdpe_out_diam (490. * mm),
    _hdpe_ledge (12. * mm),
    _ring_width (10. * mm),
    _ring_thickness (3. * mm),
    _tpb_thickness(1. * micrometer),
    _el_gap_length (6 * mm),
    _grid_thickness (.1 * mm), //it's just fake dielectric
    _el_grid_transparency (.88),
    _gate_transparency (.84),
    _anode_quartz_thickness (3. * mm),
    _anode_quartz_diam (522. * mm),
    _cathode_grid_transparency (.98),
    _ito_transparency (.90),
    _ito_thickness (_grid_thickness),
    //
    _ELtransv_diff(0. * mm/sqrt(cm)),
    _ELlong_diff(0. * mm/sqrt(cm)),
    _drift_transv_diff(1. * mm/sqrt(cm)),
    _drift_long_diff(.3 * mm/sqrt(cm)),
    //
    _ELelectric_field(34.5 * kilovolt/cm), // it corresponds to 2.3 kV/cm/bar at 15 bar
    // Visibility
    _visibility(1),
    // Step limiter
    _max_step_size(1. * mm),
    // EL field ON or OFF
    _elfield(0),
    _el_table_index(0),
    _el_table_binning(5. * mm),

    _sc_yield(25510. * 1/MeV),
    _attachment(1000 * ms)
  {
    // Derived dimensions
    _buffer_length =
      - _dist_feedthroughs/2. - _cathode_thickness - _windows_end_z;// from the end of the cathode to surface of sapphire windows // It's 129.9 mm

    _el_gap_z_pos =
      -_dist_feedthroughs/2. + _cathode_gap/2. +  _tube_length_drift + _dist_tube_el + _el_gap_length/2.;
    _pos_z_anode =
      _el_gap_z_pos + _el_gap_length/2. +  _anode_quartz_thickness/2.+ 0.1*mm; // 0.1 mm is needed because EL is produced only if the PostStepVolume is GAS material.

     // The centre of the feedthrough is the limit of the active volume.
    _pos_z_cathode = - _dist_feedthroughs/2. - _cathode_thickness/2.;

    _el_table_z = _el_gap_z_pos - _el_gap_length/2.;
    //G4cout << "***** " << _el_table_z + 0.1 * mm << G4endl;

    // 0.1 * mm is added to avoid very small negative numbers in drift lengths
    SetELzCoord(_el_table_z + 0.1 * mm);

    // 0.2 * mm is added because ie- in EL table generation must start inside the volume, not on border
    _el_table_z = _el_table_z + .2*mm;

    // Define a new category
    new G4UnitDefinition("kilovolt/cm","kV/cm","Electric field", kilovolt/cm);
    new G4UnitDefinition("mm/sqrt(cm)","mm/sqrt(cm)","Diffusion", mm/sqrt(cm));

    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/",
				  "Control commands of geometry NextNew.");

    _msg->DeclareProperty("field_cage_vis", _visibility,
			  "Field Cage Visibility");
    _msg->DeclareProperty("elfield", _elfield,
			  "True if the EL field is on (full simulation), false if it's not (parametrized simulation.");

    G4GenericMessenger::Command& step_cmd =
      _msg->DeclareProperty("max_step_size", _max_step_size,
			    "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");

    G4GenericMessenger::Command&  specific_vertex_X_cmd =
      _msg->DeclareProperty("specific_vertex_X", _specific_vertex_X,
			    "If region is AD_HOC, x coord where particles are generated");
    specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
    specific_vertex_X_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Y_cmd =
      _msg->DeclareProperty("specific_vertex_Y", _specific_vertex_Y,
			    "If region is AD_HOC, y coord where particles are generated");
    specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
    specific_vertex_Y_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Z_cmd =
      _msg->DeclareProperty("specific_vertex_Z", _specific_vertex_Z,
			    "If region is AD_HOC, z coord where particles are generated");
    specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
    specific_vertex_Z_cmd.SetUnitCategory("Length");

    G4GenericMessenger::Command&  ELtransv_diff_cmd =
      _msg->DeclareProperty("ELtransv_diff", _ELtransv_diff,
			    "Tranvsersal diffusion in the EL region");
    ELtransv_diff_cmd.SetParameterName("ELtransv_diff", true);
    ELtransv_diff_cmd.SetUnitCategory("Diffusion");

    G4GenericMessenger::Command&  ELlong_diff_cmd =
      _msg->DeclareProperty("ELlong_diff", _ELlong_diff,
			    "Longitudinal diffusion in the EL region");
    ELlong_diff_cmd.SetParameterName("ELlong_diff", true);
    ELlong_diff_cmd.SetUnitCategory("Diffusion");

    G4GenericMessenger::Command& drift_transv_diff_cmd =
      _msg->DeclareProperty("drift_transv_diff", _drift_transv_diff,
			    "Tranvsersal diffusion in the drift region");
    drift_transv_diff_cmd.SetParameterName("drift_transv_diff", true);
    drift_transv_diff_cmd.SetUnitCategory("Diffusion");

    G4GenericMessenger::Command& drift_long_diff_cmd =
      _msg->DeclareProperty("drift_long_diff", _drift_long_diff,
			    "Longitudinal diffusion in the drift region");
    drift_long_diff_cmd.SetParameterName("drift_long_diff", true);
    drift_long_diff_cmd.SetUnitCategory("Diffusion");

    G4GenericMessenger::Command& El_field_cmd =
      _msg->DeclareProperty("EL_field", _ELelectric_field,
			    "Electric field in the EL region");
    El_field_cmd.SetParameterName("EL_field", true);
    El_field_cmd.SetUnitCategory("Electric field");

    // Calculate vertices for EL table generation
    G4GenericMessenger::Command& pitch_cmd =
      _msg->DeclareProperty("el_table_binning", _el_table_binning,
			    "Pitch for EL generation");
    pitch_cmd.SetUnitCategory("Length");
    pitch_cmd.SetParameterName("el_table_binning", false);
    pitch_cmd.SetRange("el_table_binning>0.");

    _msg->DeclareProperty("el_table_point_id", _el_table_point_id, "");

     G4GenericMessenger::Command& eltable_z_cmd =
      _msg->DeclareProperty("el_table_z", _el_table_z,
			    "Z coordinate for EL generation");
    eltable_z_cmd.SetUnitCategory("Length");
    eltable_z_cmd.SetParameterName("el_table_z", false);

  }


  NextNewFieldCage::~NextNewFieldCage()
  {
    delete _drift_tube_gen;
    delete _hdpe_tube_gen;
    delete _active_gen;
    delete _buffer_gen;
    delete _xenon_gen;
    delete _anode_quartz_gen;
    delete _cathode_gen;
    delete _tracking_frames_gen;
  }


  void NextNewFieldCage::SetMotherLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }


  void NextNewFieldCage::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
  {
    _mother_phys = mother_phys;
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

    G4double max_radius = floor(_tube_in_diam/2./_el_table_binning)*_el_table_binning;

    CalculateELTableVertices(max_radius, _el_table_binning, _el_table_z);

    // for (G4int i=0; i<_el_table_vertices.size(); ++i) {
    //   std::cout << i << ": "<< _el_table_vertices[i] << std::endl;
    // }
  }


  void  NextNewFieldCage::DefineMaterials()
  {
    _gas = _mother_logic->GetMaterial();
    _pressure =    _gas->GetPressure();
    _temperature = _gas->GetTemperature();

    // High density polyethylene for the field cage
    _hdpe = MaterialsList::HDPE();

    // Copper for field rings
    _copper = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    // Teflon for the light tube
    _teflon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
    // TPB coating
    _tpb = MaterialsList::TPB();
    _tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
    //ITO coating
    _ito = MaterialsList::ITO();
    _ito->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeFusedSilica(_ito_transparency, _ito_thickness));

    // Quartz
    _quartz =  MaterialsList::FusedSilica();
    _quartz->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
  }


  void NextNewFieldCage::BuildCathodeGrid()
  {
    ///// CATHODE //////
    G4Material* fgrid_mat =
      MaterialsList::FakeDielectric(_gas, "cath_grid_mat");
    fgrid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, _temperature, _cathode_grid_transparency, _cathode_thickness,
                                                                              _sc_yield, _attachment));
    // Dimensions & position
    G4double grid_diam = _tube_in_diam;

    // Building the grid
    G4Tubs* diel_grid_solid =
      new G4Tubs("CATHODE_GRID", 0., grid_diam/2., _cathode_thickness/2., 0, twopi);
    G4LogicalVolume* diel_grid_logic =
      new G4LogicalVolume(diel_grid_solid, fgrid_mat, "CATHODE_GRID");
    new G4PVPlacement(0, G4ThreeVector(0., 0., _pos_z_cathode), diel_grid_logic,
		      "CATHODE_GRID", _mother_logic, false, 0, false);

    if (_visibility) {
      G4VisAttributes cathode_col = nexus::Red();
      diel_grid_logic->SetVisAttributes(cathode_col);
    }
  }


  void NextNewFieldCage::BuildActive()
  {
    _active_length = _cathode_gap/2. + _tube_length_drift + _dist_tube_el;
    _active_posz   = -_dist_feedthroughs/2.  +  _active_length/2.;

    G4Tubs* active_solid =
      new G4Tubs("ACTIVE",  0., _tube_in_diam/2., _active_length/2., 0, twopi);
    // G4cout << "Active starts in " <<  active_posz  -  active_length/2. << " and ends in "
    // 	   << active_posz +  active_length/2. << G4endl;

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, _gas, "ACTIVE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., _active_posz), active_logic,
                      "ACTIVE", _mother_logic, false, 0, false);

    // Limit the step size in this volume for better tracking precision
    active_logic->SetUserLimits(new G4UserLimits(_max_step_size));

    // Set the volume as an ionization sensitive detector
    IonizationSD* ionisd = new IonizationSD("/NEXTNEW/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Define a drift field for this volume
    UniformElectricDriftField* field = new UniformElectricDriftField();
    // electrodes are at the end of active region
    field->SetCathodePosition(-(_active_posz - _active_length/2.) + GetELzCoord());
    field->SetAnodePosition(-(_active_posz + _active_length/2.) + GetELzCoord());
    field->SetDriftVelocity(1. * mm/microsecond);
    field->SetTransverseDiffusion(_drift_transv_diff);
    field->SetLongitudinalDiffusion(_drift_long_diff);
    G4Region* drift_region = new G4Region("DRIFT");
    drift_region->SetUserInformation(field);
    drift_region->AddRootLogicalVolume(active_logic);

    /// Visibilities
    active_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // VERTEX GENERATOR
    _active_gen =
      new CylinderPointSampler(0., _active_length, _tube_in_diam/2.,
                               0., G4ThreeVector (0., 0., _active_posz));
  }

void NextNewFieldCage::BuildBuffer()
  {
    //G4double length = _buffer_length - _cathode_gap/2.;
    G4double buffer_posz =
      -_dist_feedthroughs/2.  - _cathode_thickness -  _buffer_length/2.;
    G4Tubs* buffer_solid =
      new G4Tubs("BUFFER",  0., _hdpe_in_diam/2.,
		 _buffer_length /2., 0, twopi);

    // G4cout << "Buffer (gas) starts in " << buffer_posz - _buffer_length/2. << " and ends in "
    // 	   << buffer_posz + _buffer_length/2. << G4endl;

    G4LogicalVolume* buffer_logic =
      new G4LogicalVolume(buffer_solid, _gas, "BUFFER");

    new G4PVPlacement(0, G4ThreeVector(0., 0., buffer_posz), buffer_logic,
                      "BUFFER", _mother_logic, false, 0, false);

     // Set the volume as an ionization sensitive detector
    IonizationSD* buffsd = new IonizationSD("/NEXTNEW/BUFFER");
    buffsd->IncludeInTotalEnergyDeposit(false);
    buffer_logic->SetSensitiveDetector(buffsd);
    G4SDManager::GetSDMpointer()->AddNewDetector(buffsd);

    // VERTEX GENERATOR
    _buffer_gen =
      new CylinderPointSampler(0., _buffer_length,
			       _hdpe_in_diam/2.,
			       0., G4ThreeVector (0., 0., buffer_posz));

    // VERTEX GENERATOR FOR ALL XENON
    G4double pos_el = _el_gap_z_pos / 2. + _grid_thickness / 2.;
    G4double xenon_posz = (_buffer_length * buffer_posz +
			   _active_length * _active_posz +
			   _cathode_thickness * _pos_z_cathode +
			   _el_gap_length * _el_gap_z_pos +
			   _grid_thickness * pos_el) / (_buffer_length +
							_active_length +
							_cathode_thickness +
							_el_gap_length +
							_grid_thickness);
    G4double xenon_len = _buffer_length + _active_length
      + _cathode_thickness + _grid_thickness + _el_gap_length;
    _xenon_gen =
      new CylinderPointSampler(0., xenon_len, _tube_in_diam/2.,
                               0., G4ThreeVector (0., 0., xenon_posz));
  }

  void NextNewFieldCage::BuildELRegion()
  {
    ///// EL GAP /////
    G4double el_gap_diam = _tube_in_diam + 5. * mm;
    G4Tubs* el_gap_solid =
      new G4Tubs("EL_GAP", 0., el_gap_diam/2., _el_gap_length/2., 0, twopi);
    G4LogicalVolume* el_gap_logic =
      new G4LogicalVolume(el_gap_solid, _gas, "EL_GAP");
    new G4PVPlacement(0, G4ThreeVector(0., 0., _el_gap_z_pos), el_gap_logic,
		      "EL_GAP", _mother_logic, false, 0, false);
    // G4cout << "EL gap region starts in " << _el_gap_z_pos - _el_gap_length/2. << " and ends in "
    //        << _el_gap_z_pos + _el_gap_length/2. << G4endl;

    if (_elfield) {
      // Define EL electric field
      UniformElectricDriftField* el_field = new UniformElectricDriftField();
      el_field->SetCathodePosition(-(_el_gap_z_pos - _el_gap_length/2.) + GetELzCoord());
      el_field->SetAnodePosition(-(_el_gap_z_pos + _el_gap_length/2.) + GetELzCoord());
      el_field->SetDriftVelocity(2.5 * mm/microsecond);
      el_field->SetTransverseDiffusion(_ELtransv_diff);
      el_field->SetLongitudinalDiffusion(_ELlong_diff);
      XenonGasProperties xgp(_pressure, _temperature);
      el_field->SetLightYield(xgp.ELLightYield(_ELelectric_field));
      G4Region* el_region = new G4Region("EL_REGION");
      el_region->SetUserInformation(el_field);
      el_region->AddRootLogicalVolume(el_gap_logic);
    }

    ///// EL GRIDS /////

    G4Material* fgrid_mat =
      MaterialsList::FakeDielectric(_gas, "el_grid_anode_mat");
    fgrid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, _temperature, _el_grid_transparency, _grid_thickness,
                                                                              _sc_yield, _attachment));

    G4Material* fgate_mat =
      MaterialsList::FakeDielectric(_gas, "el_grid_gate_mat");
    fgate_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, _temperature, _gate_transparency, _grid_thickness,
                                                                              _sc_yield, _attachment));

    // Dimensions & position: the grids are simulated inside the EL gap.
    // Their thickness is symbolic.
    G4double grid_diam = _tube_in_diam; // _active_diam;
    G4double poszInner =  - _el_gap_length/2. + _grid_thickness/2.;

    G4Tubs* diel_grid_solid =
      new G4Tubs("EL_GRID", 0., grid_diam/2., _grid_thickness/2., 0, twopi);

    G4LogicalVolume* gate_logic =
      new G4LogicalVolume(diel_grid_solid, fgate_mat, "EL_GRID_GATE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., poszInner), gate_logic,
		      "EL_GRID_GATE", el_gap_logic, false, 0, false);

    /// Visibilities
    if (_visibility) {
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
    G4double anode_diam = _anode_quartz_diam;

    ///// ANODE //////

    G4Tubs* anode_quartz_solid =
      new G4Tubs("QUARTZ_ANODE", 0., anode_diam/2. , _anode_quartz_thickness/2., 0, twopi);
    G4LogicalVolume* anode_logic =
      new G4LogicalVolume(anode_quartz_solid, _quartz, "EL_QUARTZ_ANODE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., _pos_z_anode), anode_logic,
                      "EL_QUARTZ_ANODE", _mother_logic, false, 0, false);
    //G4cout << "Anode plate starts in " << _pos_z_anode - _anode_quartz_thickness/2. << " and ends in " <<
    //  _pos_z_anode + _anode_quartz_thickness/2. << G4endl;

    G4Tubs* tpb_anode_solid =
      new G4Tubs("TPB_ANODE", 0., anode_diam/2. , _tpb_thickness/2., 0, twopi);
    G4LogicalVolume* tpb_anode_logic =
      new G4LogicalVolume(tpb_anode_solid, _tpb, "TPB_ANODE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., -_anode_quartz_thickness/2.+_tpb_thickness/2.), tpb_anode_logic,
                        "TPB_ANODE", anode_logic, false, 0, false);

    // Optical surface between gas and TPB to model the latter's roughness
    G4OpticalSurface* tpb_anode_surf =
      new G4OpticalSurface("TPB_ANODE_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);
    new G4LogicalSkinSurface("TPB_ANODE_OPSURF", tpb_anode_logic, tpb_anode_surf);


    G4Tubs* ito_anode_solid =
      new G4Tubs("ITO_ANODE", 0., anode_diam/2. , _ito_thickness/2., 0, twopi);
    G4LogicalVolume* ito_anode_logic =
      new G4LogicalVolume(ito_anode_solid, _ito, "ITO_ANODE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., +_anode_quartz_thickness/2.- _ito_thickness/2.), ito_anode_logic,
  			"ITO_ANODE", anode_logic, false, 0, false);

    if (_visibility) {
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
      _el_gap_z_pos -_el_gap_length/2. - _dist_tube_el - _hdpe_ledge - _hdpe_length/2.;

    G4Tubs* hdpe_tube_solid =
      new G4Tubs("HDPE_TUBE", _hdpe_in_diam/2.,
                 _hdpe_out_diam/2., _hdpe_length/2., 0, twopi);

    G4LogicalVolume* hdpe_tube_logic = new G4LogicalVolume(hdpe_tube_solid,
                                                           _hdpe, "HDPE_TUBE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., hdpe_tube_z_pos), hdpe_tube_logic,
                                       "HDPE_TUBE", _mother_logic, false, 0, false);
    // G4cout << "Hdpe tube starts in " << hdpe_tube_z_pos - _hdpe_length/2.  <<
    //   " and ends in " << hdpe_tube_z_pos + _hdpe_length/2. << G4endl;

    // Copper rings
    G4double ring_in_diam = _hdpe_in_diam - 2. * _ring_thickness;
    G4double ring_out_diam =_hdpe_in_diam;
    G4Tubs* ring_solid = new G4Tubs("FIELD_RING", ring_in_diam/2., ring_out_diam/2.,
                                    _ring_width/2., 0, twopi);

    G4LogicalVolume* ring_logic = new G4LogicalVolume(ring_solid, _copper, "FIELD_RING");

    G4int num_rings = 42;
    G4double separation = 1.80 * mm;
    G4double pitch = _ring_width + separation;
    G4double posz = hdpe_tube_z_pos + _hdpe_length/2. - _ring_width/2. + separation;

    for (G4int i=0; i<num_rings; i++) {
      new G4PVPlacement(0, G4ThreeVector(0., 0., posz), ring_logic,
                        "FIELD_RING", _mother_logic, false, i, false);
      posz = posz - pitch;
    }

    // Light  tube
    G4double drift_tube_z_pos =
      _el_gap_z_pos -_el_gap_length/2. - _dist_tube_el - _tube_length_drift/2.;

    G4Tubs* drift_tube_solid =
      new G4Tubs("DRIFT_TUBE", _tube_in_diam/2., _tube_in_diam/2. + _tube_thickness,
                 _tube_length_drift/2., 0, twopi);
    // G4double fieldcagevol= drift_tube_solid->GetCubicVolume();
    // std::cout<<"FIELD CAGE VOLUME:\t"<<fieldcagevol<<std::endl;

    G4LogicalVolume* drift_tube_logic = new G4LogicalVolume(drift_tube_solid,
                                                            _teflon, "DRIFT_TUBE");

    new G4PVPlacement(0, G4ThreeVector(0., 0., drift_tube_z_pos),
                      drift_tube_logic, "DRIFT_TUBE", _mother_logic,
                      false, 0, false);
    // G4cout << "Light tube drift starts in " << drift_tube_z_pos - _tube_length_drift/2.  <<
    //   " and ends in " << drift_tube_z_pos + _tube_length_drift/2. << G4endl;

    G4Tubs* tpb_drift_solid =
      new G4Tubs("DRIFT_TPB", _tube_in_diam/2., _tube_in_diam/2. + _tpb_thickness,
                 _tube_length_drift/2., 0, twopi);

    G4LogicalVolume* tpb_drift_logic =
      new G4LogicalVolume(tpb_drift_solid, _tpb, "DRIFT_TPB");

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

    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", tpb_drift_phys, _mother_phys,
                               gas_tpb_teflon_surf);
    new G4LogicalBorderSurface("GAS_TPB_TEFLON_OPSURF", _mother_phys, tpb_drift_phys,
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
    if (_visibility) {
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
    _hdpe_tube_gen  =
      new CylinderPointSampler(_hdpe_in_diam/2., _hdpe_length,
                               _hdpe_out_diam/2. - _hdpe_in_diam/2.,
                               0., G4ThreeVector (0., 0., hdpe_tube_z_pos));

    // G4double posz = hdpe_tube_z_pos + _hdpe_length/2. - _ring_width/2. + separation;
    // _ring_gen =
    //   new CylinderPointSampler(ring_in_diam,_ring_width, ring_out_diam/2.,
    // 			                      0., G4ThreeVector (0., 0., _pos_z_anode));

    _drift_tube_gen  =
      new CylinderPointSampler(_tube_in_diam/2., _tube_length_drift, _tube_thickness,
                               0., G4ThreeVector (0., 0., drift_tube_z_pos));

    _anode_quartz_gen =
      new CylinderPointSampler(0., _anode_quartz_thickness, _anode_quartz_diam/2.,
                               0., G4ThreeVector (0., 0., _pos_z_anode));

    _cathode_gen =
      new CylinderPointSampler(0., _cathode_thickness, _tube_in_diam/2.,
                               0., G4ThreeVector (0., 0., _pos_z_cathode));
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
    G4double frame_posz   = _pos_z_anode - 8. * mm;

    // Construction
    G4Tubs* tracking_frame_solid = new G4Tubs("TRACKING_FRAMES", frame_in_rad,
                                              frame_in_rad + frame_thickn,
                                              frame_length/2., 0, twopi);

    G4LogicalVolume* tracking_frame_logic = new G4LogicalVolume(tracking_frame_solid,
                                                                MaterialsList::Steel316Ti(),
                                                                "TRACKING_FRAMES");

    new G4PVPlacement(0, G4ThreeVector(0., 0., frame_posz), tracking_frame_logic,
                      "TRACKING_FRAMES", _mother_logic, false, 0, false);

    // Visibility
    if (_visibility) {
      G4VisAttributes frame_col = nexus::DarkGrey();
      frame_col.SetForceSolid(true);
      tracking_frame_logic->SetVisAttributes(frame_col);
    } else {
      tracking_frame_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    // Vertex generator
    _tracking_frames_gen = new CylinderPointSampler(frame_in_rad, frame_length, frame_thickn,
                                                    0., G4ThreeVector (0., 0., frame_posz));
  }


  // Vertex Generator
  G4ThreeVector NextNewFieldCage::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "CENTER") {
      vertex = G4ThreeVector(0., 0., _active_posz);
    }
    else if (region == "DRIFT_TUBE") {
      vertex = _drift_tube_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "HDPE_TUBE") {
      vertex = _hdpe_tube_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "XENON") {
      G4Navigator *geom_navigator =
        G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
      G4String volume_name;
      do {
	vertex = _xenon_gen->GenerateVertex("BODY_VOL");
	G4ThreeVector glob_vtx(vertex);
	CalculateGlobalPos(glob_vtx);
	volume_name =
	  geom_navigator->LocateGlobalPointAndSetup(glob_vtx, 0, false)->GetName();
      } while (volume_name == "CATHODE_GRID" || volume_name == "EL_GRID_GATE");
    }
    else if (region == "BUFFER") {
      vertex = _buffer_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "ACTIVE") {
      vertex = _active_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "ANODE_QUARTZ") {
      vertex = _anode_quartz_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "CATHODE") {
      vertex = _cathode_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "TRACKING_FRAMES") {
      vertex = _tracking_frames_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "AD_HOC") {
      vertex = G4ThreeVector(_specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z);
    }
    else if (region == "EL_TABLE") {
      unsigned int i = _el_table_point_id + _el_table_index;
      if (i == (_el_table_vertices.size()-1)) {
        G4Exception("[NextNewFieldcage]", "GenerateVertex()",
		    RunMustBeAborted, "Reached last event in EL lookup table.");
      }
      try {
        vertex = _el_table_vertices.at(i);
        _el_table_index++;
      }
      catch (const std::out_of_range& oor) {
        G4Exception("[NextNewFieldCage]", "GenerateVertex()", FatalErrorInArgument, "EL lookup table point out of range.");
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
          _el_table_vertices.push_back(xyz);
      }
    }
  }


  G4ThreeVector NextNewFieldCage::GetPosition() const
  {
    return G4ThreeVector(0., 0., _tube_z_pos);
  }

}//end namespace nexus
