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
#include <G4NistManager.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

using namespace CLHEP;


namespace nexus {


  NextNewFieldCage::NextNewFieldCage(): 
    BaseGeometry(),
    // Field cage dimensions
    _dist_feedthroughs(514. * mm),
    _cathode_thickness(.1 * mm),
    _cathode_gap (16. * mm),
    _buffer_length (130. * mm), // from center of cathode to surface of sapphire windows
    _tube_in_diam (432. * mm),
    _tube_length_drift (508.*mm),
    //_tube_length_buff (122.*mm),
    _dist_tube_el (15.*mm),
    _tube_thickness (2.0 * cm),  
     _reflector_thickness (.5 * cm),
    _tpb_thickness(1.*micrometer),
    _el_gap_length (5 * mm),
    _grid_thickness (.1 * mm), //it's just fake dielectric
     _el_grid_transparency (.88),
    _gate_transparency (.76),
    _anode_quartz_thickness (3 *mm),
    _anode_quartz_diam (522.*mm),  
    _cathode_grid_transparency (.98),
    _ito_transparency (.90),
    _ito_thickness (_grid_thickness),
    //
    _ELtransv_diff(0. * mm/sqrt(cm)),
    _ELlong_diff(0. * mm/sqrt(cm)),
    _drift_transv_diff(1. * mm/sqrt(cm)),
    _drift_long_diff(.3 * mm/sqrt(cm)),
    //
    _ELelectric_field(34.5*kilovolt/cm), // it corresponds to 2.3 kV/cm/bar at 15 bar
    // Visibility
    _visibility(1),
    // Step limiter
    _max_step_size(1.*mm),
    // EL field ON or OFF
    _elfield(0),
    _el_table_index(0),
    _el_table_binning(5.*mm)
  {
    // Derived dimensions 
    //   _drift_length = _dist_EL_cathode-_el_gap_length/2.-_cathode_thickness/2.;
    //  _tube_length_drift = _dist_EL_cathode + _buffer_length -  _el_gap_length/2.;
    _el_gap_z_pos = -_dist_feedthroughs/2. + _cathode_gap/2. +  _tube_length_drift + _dist_tube_el + _el_gap_length/2.;
    _pos_z_anode =  _el_gap_z_pos + _el_gap_length/2. +  _anode_quartz_thickness/2.+ 0.1*mm; // 0.1 mm is needed because EL is produced only if the PostStepVolume is GAS material.

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

    // // declare colors to be used for visibilities
    // _grey_color = new G4VisAttributes;
    // _grey_color->SetColor(.5, .5, .5);
    // _red_color = new G4VisAttributes;
    // _red_color->SetColor(1., 0., 0.);
    // _light_blue_color = new G4VisAttributes;
    // _light_blue_color->SetColor(.6, .8, .79);
    // _blue_color = new G4VisAttributes;
    // _blue_color->SetColor(0., 0., 1.);
    // _green_color = new G4VisAttributes;
    // _green_color->SetColor(0., 1., 0.);
  }


  NextNewFieldCage::~NextNewFieldCage()
  {
    delete _drift_tube_gen;
    delete _reflector_drift_gen;
    delete _buffer_tube_gen;
    delete _reflector_buffer_gen;
    delete _active_gen;

    // delete _grey_color;
    // delete _red_color;
    // delete _light_blue_color;
    // delete _blue_color;
    // delete _green_color;
  }


  void NextNewFieldCage::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
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
    // EL Region
    BuildELRegion();
    //build the quartz anode
    BuildAnodeGrid();
    // Proper field cage and light tube
    BuildFieldCage();

    G4double z = _el_gap_z_pos - _el_gap_length/2. + .5*mm;
    G4double max_radius = floor(_tube_in_diam/2./_el_table_binning)*_el_table_binning;
    CalculateELTableVertices(max_radius, _el_table_binning, z);
  }


  void  NextNewFieldCage::DefineMaterials()
  {
    _gas = _mother_logic->GetMaterial();
    _pressure =    _gas->GetPressure();
    _temperature = _gas->GetTemperature();
   
    // High density polyethylene for the field cage
    _hdpe = MaterialsList::HDPE();
    // Teflon for the light tube
    _teflon = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
    // TPB coating
    _tpb = MaterialsList::TPB();
    _tpb->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB(_pressure, _temperature));
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
    fgrid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, _temperature, _cathode_grid_transparency, _cathode_thickness));
    // Dimensions & position
    G4double grid_diam = _tube_in_diam - 2*_reflector_thickness;
    G4double posz = - _dist_feedthroughs/2. - _cathode_thickness/2.; 
    // Building the grid
    G4Tubs* diel_grid_solid = 
      new G4Tubs("CATH_GRID", 0., grid_diam/2., _cathode_thickness/2., 0, twopi);
    G4LogicalVolume* diel_grid_logic = 
      new G4LogicalVolume(diel_grid_solid, fgrid_mat, "CATHODE_GRID");
    new G4PVPlacement(0, G4ThreeVector(0.,0.,posz), diel_grid_logic, 
		      "CATHODE_GRID", _mother_logic, false, 0, false);
    
    if (_visibility) {
      G4VisAttributes cathode_col = nexus::Red();
      diel_grid_logic->SetVisAttributes(cathode_col);
    }
  }


  void NextNewFieldCage::BuildActive()
  {
    G4double active_length = 
      _cathode_gap/2. + _tube_length_drift + _dist_tube_el;
    G4double active_posz = 
      -_dist_feedthroughs/2.  +  active_length/2.;

    G4Tubs* active_solid = 
      new G4Tubs("ACTIVE",  0., _tube_in_diam/2.-_reflector_thickness, 
		 active_length/2., 0, twopi);

    
    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, _gas, "ACTIVE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., active_posz), active_logic, 
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
    field->SetCathodePosition(active_posz - active_length/2.); 
    field->SetAnodePosition(active_posz + active_length/2.);
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
      new CylinderPointSampler(0., active_length,
			       _tube_in_diam/2.-_reflector_thickness,
			       0., G4ThreeVector (0., 0., active_posz));
  }

  void NextNewFieldCage::BuildELRegion()
  {
    ///// EL GAP /////
    G4double el_gap_diam = _tube_in_diam + 5.*mm;  
    G4Tubs* el_gap_solid = 
      new G4Tubs("EL_GAP", 0., el_gap_diam/2., _el_gap_length/2., 0, twopi);   
    G4LogicalVolume* el_gap_logic = 
      new G4LogicalVolume(el_gap_solid, _gas, "EL_GAP");  
    new G4PVPlacement(0, G4ThreeVector(0., 0., _el_gap_z_pos), el_gap_logic,
		      "EL_GAP", _mother_logic, false, 0, false);
   
    if (_elfield) {
      // Define EL electric field
      UniformElectricDriftField* el_field = new UniformElectricDriftField();
      el_field->SetCathodePosition(_el_gap_z_pos - _el_gap_length/2.);
      el_field->SetAnodePosition  (_el_gap_z_pos + _el_gap_length/2.);
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
    fgrid_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, _temperature, _el_grid_transparency, _grid_thickness));

    G4Material* fgate_mat = 
      MaterialsList::FakeDielectric(_gas, "el_grid_gate_mat");
    fgate_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_pressure, _temperature, _gate_transparency, _grid_thickness));
    
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
      //   gap_col.SetForceSolid(true);
      el_gap_logic->SetVisAttributes(gap_col);
      G4VisAttributes gate_col = nexus::LightBlue();
      //    gate_col.SetForceSolid(true);
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
   
    G4Tubs* tpb_anode_solid =
      new G4Tubs("TPB_ANODE", 0., anode_diam/2. , _tpb_thickness/2., 0, twopi);
    G4LogicalVolume* tpb_anode_logic = 
      new G4LogicalVolume(tpb_anode_solid, _tpb, "TPB_ANODE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., -_anode_quartz_thickness/2.+_tpb_thickness/2.), tpb_anode_logic, 
		      "TPB_ANODE", anode_logic, false, 0, false);  

    G4Tubs* ito_anode_solid =
      new G4Tubs("ITO_ANODE", 0., anode_diam/2. , _ito_thickness/2., 0, twopi);
    G4LogicalVolume* ito_anode_logic = 
      new G4LogicalVolume(ito_anode_solid, _ito, "ITO_ANODE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., -_anode_quartz_thickness/2.+_tpb_thickness + _ito_thickness/2.), ito_anode_logic, 
  			"ITO_ANODE", anode_logic, false, 0, false);
     
    if (_visibility) {
      G4VisAttributes anode_col = nexus::Red();
      // anode_col.SetForceSolid(true);
      anode_logic->SetVisAttributes(anode_col);
      G4VisAttributes tpb_col = nexus::DarkGreen();
      //  tpb_col.SetForceSolid(true);
      tpb_anode_logic->SetVisAttributes(tpb_col);
    } else {
      anode_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_anode_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
   
  }


   void NextNewFieldCage::BuildFieldCage()
   {
     G4double drift_tube_z_pos =  
       _el_gap_z_pos -_el_gap_length/2. - _dist_tube_el - _tube_length_drift/2.;

    G4Tubs* drift_tube_solid =
      new G4Tubs("DRIFT_TUBE", _tube_in_diam/2.,
    		 _tube_in_diam/2. + _tube_thickness, _tube_length_drift/2., 0, twopi);
    //  G4double fieldcagevol= drift_tube_solid->GetCubicVolume();
    // std::cout<<"FIELD CAGE VOLUME:\t"<<fieldcagevol<<std::endl;
    G4LogicalVolume* drift_tube_logic = 
      new G4LogicalVolume(drift_tube_solid, _hdpe, "DRIFT_TUBE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., drift_tube_z_pos), 
		      drift_tube_logic, "DRIFT_TUBE", _mother_logic, 
		      false, 0, false);
    
    //Internal reflector drift region
    G4Tubs* reflector_drift_solid = 
      new G4Tubs("FC_REFLECTOR_DRIFT", 
    		 _tube_in_diam/2. -  _reflector_thickness, _tube_in_diam/2., 
    		 _tube_length_drift/2., 0, twopi);
    G4LogicalVolume* reflector_drift_logic = 
      new G4LogicalVolume(reflector_drift_solid, _teflon, "FC_REFLECTOR_DRIFT");
    new G4PVPlacement(0, G4ThreeVector(0., 0., drift_tube_z_pos), 
		      reflector_drift_logic, "FC_REFLECTOR_DRIFT", 
		      _mother_logic, false, 0, false);
    
    G4double tube_length_buffer = _buffer_length - _cathode_gap/2.;
    G4double buffer_tube_z_pos = 
      - _dist_feedthroughs/2. - _cathode_gap/2. - tube_length_buffer/2.;
    G4Tubs* buffer_tube_solid =
      new G4Tubs("BUFFER_TUBE", _tube_in_diam/2.,
    		 _tube_in_diam/2. + _tube_thickness, tube_length_buffer/2., 0, twopi);   
    G4LogicalVolume* buffer_tube_logic = 
      new G4LogicalVolume(buffer_tube_solid, _hdpe, "BUFFER_TUBE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., buffer_tube_z_pos), 
		      buffer_tube_logic, "BUFFER_TUBE", _mother_logic, 
		      false, 0, false);

    // //Internal reflector buffer region
    
    G4Tubs* reflector_buffer_solid = 
      new G4Tubs("FC_REFLECTOR_BUFFER", _tube_in_diam/2.-_reflector_thickness, 
    		 _tube_in_diam/2., tube_length_buffer/2., 0, twopi);
    G4LogicalVolume* reflector_buffer_logic = 
      new G4LogicalVolume(reflector_buffer_solid, _teflon, 
    			  "FC_REFLECTOR_BUFFER");
    new G4PVPlacement(0, G4ThreeVector(0., 0., buffer_tube_z_pos), 
		      reflector_buffer_logic, "FC_REFLECTOR_BUFFER", 
		      _mother_logic, false, 0, false);


    G4Tubs* tpb_drift_solid =
      new G4Tubs("DRIFT_TPB", _tube_in_diam/2.-_reflector_thickness,
    		 _tube_in_diam/2.-_reflector_thickness + _tpb_thickness,
    		 _tube_length_drift/2., 0, twopi);   
    G4LogicalVolume* tpb_drift_logic = 
      new G4LogicalVolume(tpb_drift_solid, _tpb, "DRIFT_TPB");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), 
		      tpb_drift_logic, "DRIFT_TPB", reflector_drift_logic, 
		      false, 0, false);
    
    G4Tubs* tpb_buffer_solid =
      new G4Tubs("BUFFER_TPB", _tube_in_diam/2.-_reflector_thickness,
    		 _tube_in_diam/2.-_reflector_thickness + _tpb_thickness,
    		 tube_length_buffer/2., 0, twopi);    
    G4LogicalVolume* tpb_buffer_logic = 
      new G4LogicalVolume(tpb_buffer_solid, _tpb, "BUFFER_TPB"); 
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), 
		      tpb_buffer_logic, "BUFFER_TPB", reflector_buffer_logic, 
		      false, 0, false);

    /// OPTICAL SURFACE PROPERTIES    ////////
    G4OpticalSurface* reflector_opt_surf = new G4OpticalSurface("FC_REFLECTOR");
    reflector_opt_surf->SetType(dielectric_metal);
    reflector_opt_surf->SetModel(unified);
    reflector_opt_surf->SetFinish(ground);
    reflector_opt_surf->SetSigmaAlpha(0.1);
    reflector_opt_surf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE_with_TPB());
    new G4LogicalSkinSurface("FC_REFLECTOR_DRIFT", reflector_drift_logic, 
    			     reflector_opt_surf);
    new G4LogicalSkinSurface("FC_REFLECTOR_BUFFER", reflector_buffer_logic, 
    			     reflector_opt_surf);

    /// SETTING VISIBILITIES   //////////
    if (_visibility) {
      G4VisAttributes tube_col = nexus::LightBlue();    
      drift_tube_logic->SetVisAttributes(tube_col);
      buffer_tube_logic->SetVisAttributes(tube_col);
      G4VisAttributes reflector_col = nexus::White();
      reflector_drift_logic->SetVisAttributes(reflector_col);
      reflector_buffer_logic->SetVisAttributes(reflector_col);
      G4VisAttributes tpb_col = nexus::DarkGreen();
      tpb_drift_logic->SetVisAttributes(tpb_col);
      tpb_buffer_logic->SetVisAttributes(tpb_col);
    } else {
      drift_tube_logic->SetVisAttributes(G4VisAttributes::Invisible);
      buffer_tube_logic->SetVisAttributes(G4VisAttributes::Invisible);
      reflector_drift_logic->SetVisAttributes(G4VisAttributes::Invisible);
      reflector_buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_drift_logic->SetVisAttributes(G4VisAttributes::Invisible);
      tpb_buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }

    /// VERTEX GENERATORS   //////////
    _drift_tube_gen  = 
      new CylinderPointSampler(_tube_in_diam/2., _tube_length_drift, _tube_thickness,
    			       0., G4ThreeVector (0., 0., drift_tube_z_pos));

    _reflector_drift_gen  = 
      new CylinderPointSampler(_tube_in_diam/2.- _reflector_thickness, 
    			       _tube_length_drift,  _reflector_thickness,
    			       0., G4ThreeVector (0., 0., drift_tube_z_pos));

     _buffer_tube_gen  = 
      new CylinderPointSampler(_tube_in_diam/2., tube_length_buffer, _tube_thickness,
    			       0., G4ThreeVector (0., 0., buffer_tube_z_pos));

    _reflector_buffer_gen  = 
      new CylinderPointSampler(_tube_in_diam/2.- _reflector_thickness, 
    			       tube_length_buffer,  _reflector_thickness,
    			       0., G4ThreeVector (0., 0., buffer_tube_z_pos));

     _anode_quartz_gen = 
      new CylinderPointSampler(0.,_anode_quartz_thickness,_anode_quartz_diam/2., 
			       0., G4ThreeVector (0., 0., _pos_z_anode));
  }

  G4ThreeVector NextNewFieldCage::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "DRIFT_TUBE") {
      vertex = _drift_tube_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "REFLECTOR_DRIFT") {
      vertex = _reflector_drift_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "BUFFER_TUBE") {
      vertex = _buffer_tube_gen->GenerateVertex("BODY_VOL");
    }
     else if (region == "REFLECTOR_BUFFER") {
      vertex = _reflector_buffer_gen->GenerateVertex("BODY_VOL");
    }
    else if (region == "ACTIVE") {
      vertex = _active_gen->GenerateVertex("BODY_VOL");
    } 
    else if (region == "ANODE_QUARTZ") {
      vertex = _anode_quartz_gen->GenerateVertex("BODY_VOL");
    } 
    else if (region == "AD_HOC") {
      vertex = G4ThreeVector(_specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z);
    }
    else if (region == "EL_TABLE") {

      unsigned int i = _el_table_point_id + _el_table_index;

      if (i == (_el_table_vertices.size()-1)) {
        G4Exception("[Next100InnerElements]", "GenerateVertex()", 
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

