// -----------------------------------------------------------------------------
//  nexus | NextFlexFieldCage.cc
//
//  * Info:         : NEXT-Flex Field Cage geometry for performance studies.
//  * Author        : <jmunoz@ific.uv.es>
//  * Creation date : January 2020
// -----------------------------------------------------------------------------

#include "NextFlexFieldCage.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "XenonGasProperties.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "CylinderPointSampler2020.h"
#include "PmtSD.h"
#include "Visibilities.h"

#include <G4UnitsTable.hh>
#include <G4GenericMessenger.hh>
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
  _verbosity         (false),
  _visibility        (false),
  _msg               (nullptr),
  _fc_with_fibers    (true), 
  _active_length     (130. * cm),          // Distance GATE - CATHODE (meshes not included)
  _drift_transv_diff (1. * mm/sqrt(cm)),   // Drift field transversal diffusion
  _drift_long_diff   (.3 * mm/sqrt(cm)),   // Drift field longitudinal diffusion
  _buffer_length     (282. * mm),          // Distance CATHODE - sapphire window surfaces
  _el_gap_length     (10. * mm),           // Distance ANODE - GATE (meshes included)
  _el_field_on       (false),              // EL field ON-OFF
  _el_field_int      (34.5 * kilovolt/cm), // EL field intensity
  _el_transv_diff    (0. * mm/sqrt(cm)),   // EL field transversal diffusion
  _el_long_diff      (0. * mm/sqrt(cm)),   // EL field longitudinal diffusion
  _fiber_claddings   (2),                  // Number of fiber claddings (0, 1 or 2)
  _fiber_sensor_pde  (1.),                 // Photon detection efficiency of fiber sensors
  _fiber_sensor_bin  (100. * ns),          // Size of fiber sensors time bins
  _wls_matName       ("TPB"),              // UV wls material name
  _fiber_matName     ("EJ280")             // Fiber core material name
{

  // Messenger
  _msg = new G4GenericMessenger(this, "/Geometry/NextFlex/",
                                "Control commands of the NextFlex geometry.");

  // Define new categories
  new G4UnitDefinition("kilovolt/cm", "kV/cm", "Electric field", kilovolt/cm);
  new G4UnitDefinition("mm/sqrt(cm)", "mm/sqrt(cm)", "Diffusion", mm/sqrt(cm));


  // Hard-wired dimensions
  _active_diam          = 106.9 * cm;   // Same as NEXT100

  _cathode_thickness    = 0.1   * mm;
  _cathode_transparency = 0.98;         // To be checked
  _anode_thickness      = 0.1   * mm;
  _anode_transparency   = 0.98;         // It was .88 in NEW
  _gate_thickness       = 0.1   * mm;
  _gate_transparency    = 0.98;         // To be checked

  _light_tube_thickness = 5.    * mm;
  _wls_thickness        = 1.    * um;   // XXXXXXXX To be checked
  _fiber_thickness      = 2.    * mm;
  _cladding_perc        = 0.02;         // Fraction of fiber thickness devoted to EACH cladding
  _fiber_extra_length   = 20.   * cm;   // Extra length per side of fibers respect FC length
  _fiber_light_tube_gap =  2.   * mm;   // Separation gap between fibers & light tube
  _fiber_sensor_size    =  2.   * mm;   // Side length of squared fiber sensors
                                        // (ideally equal to _fiber_thickness)
  _num_fiber_sectors    = 20;           // Num of sectors of fiber sensors

  // Parametrized dimensions
  DefineConfigurationParameters();
}


NextFlexFieldCage::~NextFlexFieldCage()
{
  delete _msg;

  delete _active_gen;
  delete _buffer_gen;
  delete _el_gap_gen;
  delete _light_tube_gen;
  delete _fiber_gen;
}


void NextFlexFieldCage::DefineConfigurationParameters()
{
  // Verbosity
  _msg->DeclareProperty("fc_verbosity", _verbosity, "Verbosity");

  // Visibility
  _msg->DeclareProperty("fc_visibility", _visibility, "FIELD_CAGE Visibility");

  // FIELD_CAGE configuration
  _msg->DeclareProperty("fc_with_fibers", _fc_with_fibers, "FIELD_CAGE with fibers");

  // ACTIVE dimensions
  G4GenericMessenger::Command& active_length_cmd =
    _msg->DeclareProperty("active_length", _active_length,
                          "Length of the ACTIVE volume.");
  active_length_cmd.SetParameterName("active_length", false);
  active_length_cmd.SetUnitCategory("Length");
  active_length_cmd.SetRange("active_length>=0.");

  G4GenericMessenger::Command& drift_transv_diff_cmd =
    _msg->DeclareProperty("drift_transv_diff", _drift_transv_diff,
                          "Tranvsersal diffusion in the drift region");
  drift_transv_diff_cmd.SetParameterName("drift_transv_diff", false);
  drift_transv_diff_cmd.SetUnitCategory("Diffusion");

  G4GenericMessenger::Command& drift_long_diff_cmd =
    _msg->DeclareProperty("drift_long_diff", _drift_long_diff,
                          "Longitudinal diffusion in the drift region");
  drift_long_diff_cmd.SetParameterName("drift_long_diff", false);
  drift_long_diff_cmd.SetUnitCategory("Diffusion");


  // FIELD_CAGE dimensions
  G4GenericMessenger::Command& buffer_length_cmd =
    _msg->DeclareProperty("buffer_length", _buffer_length,
                          "Length of the BUFFER.");
  buffer_length_cmd.SetParameterName("buffer_length", false);
  buffer_length_cmd.SetUnitCategory("Length");
  buffer_length_cmd.SetRange("buffer_length>=0.");

  G4GenericMessenger::Command& el_gap_length_cmd =
    _msg->DeclareProperty("el_gap_length", _el_gap_length,
                          "Length of the EL gap.");
  el_gap_length_cmd.SetParameterName("el_gap_length", false);
  el_gap_length_cmd.SetUnitCategory("Length");
  el_gap_length_cmd.SetRange("el_gap_length>=0.");

  _msg->DeclareProperty("el_field_on", _el_field_on,
                        "true for full simulation), false if fast or parametrized.");

  G4GenericMessenger::Command& el_field_int_cmd =
    _msg->DeclareProperty("el_field_int", _el_field_int,
                          "EL electric field intensity");
  el_field_int_cmd.SetParameterName("el_field_int", false);
  el_field_int_cmd.SetUnitCategory("Electric field");

  G4GenericMessenger::Command& el_transv_diff_cmd =
    _msg->DeclareProperty("el_transv_diff", _el_transv_diff,
                          "Tranvsersal diffusion in the EL region");
  el_transv_diff_cmd.SetParameterName("el_transv_diff", false);
  el_transv_diff_cmd.SetUnitCategory("Diffusion");

  G4GenericMessenger::Command& el_long_diff_cmd =
    _msg->DeclareProperty("el_long_diff", _el_long_diff,
                          "Longitudinal diffusion in the EL region");
  el_long_diff_cmd.SetParameterName("el_long_diff", false);
  el_long_diff_cmd.SetUnitCategory("Diffusion");


  // LIGHT TUBE
  _msg->DeclareProperty("fc_wls_mat", _wls_matName,
                        "FIELD_CAGE UV wavelength shifting material name");

  // FIBERS
  _msg->DeclareProperty("fiber_mat", _fiber_matName, "Fiber core material.");

  G4GenericMessenger::Command& fiber_claddings_cmd =
    _msg->DeclareProperty("fiber_claddings", _fiber_claddings,
                          "Number of fiber claddings.");
  fiber_claddings_cmd.SetParameterName("fiber_claddings", false);
  fiber_claddings_cmd.SetRange("fiber_claddings>=0 && fiber_claddings<=2");

  G4GenericMessenger::Command& fiber_sensor_pde_cmd =
    _msg->DeclareProperty("fiber_sensor_pde", _fiber_sensor_pde,
                          "Fiber sensors photon detection efficiency.");
  fiber_sensor_pde_cmd.SetParameterName("fiber_sensor_pde", false);
  fiber_sensor_pde_cmd.SetRange("fiber_sensor_pde>=0. && fiber_sensor_pde<=1.");

  G4GenericMessenger::Command& fiber_sensor_bin_cmd =
    _msg->DeclareProperty("fiber_sensor_time_binning", _fiber_sensor_bin,
                          "Time bin size of fiber sensors.");
  fiber_sensor_bin_cmd.SetParameterName("fiber_sensor_time_binning", false);
  fiber_sensor_bin_cmd.SetUnitCategory("Time");
  fiber_sensor_bin_cmd.SetRange("fiber_sensor_time_binning>=0.");
}


void NextFlexFieldCage::ComputeDimensions()
{
  // The field cage goes along the whole field cage
  // (EL_GAP + ACTIVE + CATHODE + BUFFER)
  _fc_length = _el_gap_length     + _active_length +
               _cathode_thickness + _buffer_length;

  _fiber_inner_rad      = _active_diam/2.;
  _light_tube_inner_rad = _active_diam/2.;

  // Shifting placements in case of extra volumes
  if (_fc_with_fibers) {
    _light_tube_inner_rad += _fiber_thickness;
    _light_tube_inner_rad += _fiber_light_tube_gap;
  }
}


void NextFlexFieldCage::DefineMaterials()
{
  // Xenon
  // Read gas properties from mother volume
  _xenon_gas       = _mother_logic->GetMaterial();
  _gas_pressure    = _xenon_gas->GetPressure();
  _gas_temperature = _xenon_gas->GetTemperature();

  // Teflon
  _teflon_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  //_teflon_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  // UV shifting material
  if (_wls_matName == "NONE") {
    _wls_mat = _mother_logic->GetMaterial();
  }
  else if (_wls_matName == "TPB") {
    _wls_mat = MaterialsList::TPB();
    _wls_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
  }
  else if (_wls_matName == "TPH") {
    _wls_mat = MaterialsList::TPH();
    _wls_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::TPH());
  }
  else {
    G4Exception("[NextFlex]", "FieldCage::DefineMaterials()", FatalException,
                "Unknown UV shifting material. Valid options are NONE, TPB or TPH.");
  }

  // Fiber core material
  if (_fiber_matName == "EJ280") {
    _fiber_mat = MaterialsList::EJ280();
    _fiber_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::EJ280());
  }
  else if (_fiber_matName == "EJ286") {
    _fiber_mat = MaterialsList::EJ280();   // Same base material than EJ280
    _fiber_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::EJ286());
  }
  else {
    G4Exception("[NextParam]", "FieldCage::DefineMaterials()", FatalException,
    "Unknown inner WLS material. Valid options are EJ280 or EJ286.");
  }

  // Fiber cladding materials (inner: PMMA  outer: FPethylene)
  _oClad_mat = MaterialsList::FPethylene();
  _oClad_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FPethylene());

  _iClad_mat = MaterialsList::PMMA();
  _iClad_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::PMMA());

  // Fiber sensor case material
  _fiber_sensor_case_mat = MaterialsList::Epoxy();
  _fiber_sensor_case_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GlassEpoxy());

  // Fiber sensor material
  _fiber_sensor_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
}


void NextFlexFieldCage::Construct()
{
  // Verbosity
  if(_verbosity) G4cout << G4endl << "*** NEXT-Flex Field Cage ..." << G4endl;

  // Getting volumes dimensions based on parameters.
  ComputeDimensions();

  // Define materials.
  DefineMaterials();

  // Build components
  BuildActive();
  
  BuildCathode();
  
  BuildBuffer();

  BuildELgap();

  BuildLightTube();

  // If with fibers, build fibers and corresponding detectors
  if (_fc_with_fibers) {
    BuildFibers();
    BuildFiberSensors();
  }
}


void NextFlexFieldCage::BuildActive()
{
  G4String active_name = "ACTIVE";

  G4Tubs* active_solid =
    new G4Tubs(active_name, 0., _active_diam/2., _active_length/2., 0., 360.*deg);

  G4LogicalVolume* active_logic =
    new G4LogicalVolume(active_solid, _xenon_gas, active_name);

  //G4VPhysicalVolume* active_phys =
  _active_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,_active_length/2.),
                      active_logic, active_name, _mother_logic,
                      false, 0, _verbosity);

  // Define the drift field
  UniformElectricDriftField* field = new UniformElectricDriftField();
  field->SetCathodePosition(_active_length);
  field->SetAnodePosition(0.);
  field->SetDriftVelocity(1. * mm/microsecond);
  field->SetTransverseDiffusion(_drift_transv_diff);
  field->SetLongitudinalDiffusion(_drift_long_diff);
  G4Region* drift_region = new G4Region("DRIFT");
  drift_region->SetUserInformation(field);
  drift_region->AddRootLogicalVolume(active_logic);

  // Visibility
  active_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  _active_gen = new CylinderPointSampler2020(_active_phys);

  // Limit the step size in this volume for better tracking precision
  active_logic->SetUserLimits(new G4UserLimits(1.*mm));
  
  // Set the ACTIVE volume as an ionization sensitive detector
  IonizationSD* active_sd = new IonizationSD("/NEXT_FLEX/ACTIVE");
  active_logic->SetSensitiveDetector(active_sd);
  G4SDManager::GetSDMpointer()->AddNewDetector(active_sd);

  /// Verbosity ///
  if (_verbosity) {
    G4cout << "* ACTIVE Rad: " << _active_diam/2. << G4endl;
    G4cout << "* ACTIVE Z positions: 0 to " << _active_length << G4endl;
  }
}


void NextFlexFieldCage::BuildCathode()
{
  G4String cathode_name = "CATHODE";

  G4Material* cathode_mat = MaterialsList::FakeDielectric(_xenon_gas, "cathode_mat");
  cathode_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_gas_pressure,
    _gas_temperature, _cathode_transparency, _cathode_thickness));

  G4double cathode_diam = _active_diam;
  G4double cathode_posZ = _active_length + _cathode_thickness/2.;

  G4Tubs* cathode_solid =
    new G4Tubs(cathode_name, 0., cathode_diam/2., _cathode_thickness/2., 0, twopi);

  G4LogicalVolume* cathode_logic =
    new G4LogicalVolume(cathode_solid, cathode_mat, cathode_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., cathode_posZ), cathode_logic,
                    cathode_name, _mother_logic, false, 0, _verbosity);

  // Visibility
  if (_visibility) cathode_logic->SetVisAttributes(nexus::LightBlue());
  else             cathode_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Verbosity
  if (_verbosity) 
    G4cout << "* CATHODE Z positions: " << cathode_posZ - _cathode_thickness/2. <<
              " to " << cathode_posZ + _cathode_thickness/2. << G4endl;
}


void NextFlexFieldCage::BuildBuffer()
{
  G4String buffer_name = "BUFFER";

  G4double buffer_diam = _active_diam;
  G4double buffer_posZ = _active_length + _cathode_thickness + _buffer_length/2.;
  _buffer_finalZ = buffer_posZ + _buffer_length/2.;

  G4Tubs* buffer_solid =
    new G4Tubs(buffer_name, 0., buffer_diam/2., _buffer_length/2., 0, twopi);

  G4LogicalVolume* buffer_logic =
    new G4LogicalVolume(buffer_solid, _xenon_gas, buffer_name);

  _buffer_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., buffer_posZ), buffer_logic,
                      buffer_name, _mother_logic, false, 0, _verbosity);

  // Visibility
  buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  _buffer_gen = new CylinderPointSampler2020(_buffer_phys);

  // Set the BUFFER volume as an ionization sensitive detector
  IonizationSD* buffer_sd = new IonizationSD("/NEXT_FLEX/BUFFER");
  buffer_sd->IncludeInTotalEnergyDeposit(false);
  buffer_logic->SetSensitiveDetector(buffer_sd);
  G4SDManager::GetSDMpointer()->AddNewDetector(buffer_sd);  

  // Verbosity
  if (_verbosity)
    G4cout << "* BUFFER Z positions: " << buffer_posZ - _buffer_length/2. <<
              " to " << buffer_posZ + _buffer_length/2. << G4endl;
}


void NextFlexFieldCage::BuildELgap()
{
  G4String el_gap_name = "EL_GAP";

  G4double el_gap_diam = _active_diam;
  G4double el_gap_posZ = - _el_gap_length/2.;

  G4Tubs* el_gap_solid =
    new G4Tubs(el_gap_name, 0., el_gap_diam/2., _el_gap_length/2., 0, twopi);

  G4LogicalVolume* el_gap_logic =
    new G4LogicalVolume(el_gap_solid, _xenon_gas, el_gap_name);

  G4VPhysicalVolume* el_gap_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., el_gap_posZ), el_gap_logic,
                      el_gap_name, _mother_logic, false, 0, _verbosity);

  // Define EL electric field
  XenonGasProperties xgp(_gas_pressure, _gas_temperature);
  if (_el_field_on) {
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    el_field->SetCathodePosition(el_gap_posZ + _el_gap_length/2.);
    el_field->SetAnodePosition  (el_gap_posZ - _el_gap_length/2.);
    el_field->SetDriftVelocity  (2.5 * mm/microsecond);
    el_field->SetTransverseDiffusion(_el_transv_diff);
    el_field->SetLongitudinalDiffusion(_el_long_diff);
    el_field->SetLightYield(xgp.ELLightYield(_el_field_int));
    G4Region* el_region = new G4Region("EL_REGION");
    el_region->SetUserInformation(el_field);
    el_region->AddRootLogicalVolume(el_gap_logic);
  }

  // Visibility
  if (_visibility) el_gap_logic->SetVisAttributes(nexus::LightBlue());
  else             el_gap_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  _el_gap_gen = new CylinderPointSampler2020(el_gap_phys);

  // Verbosity
  if (_verbosity) {
    G4cout << "* EL_GAP Z positions: " << el_gap_posZ - _el_gap_length/2. <<
              " to " << el_gap_posZ + _el_gap_length/2. << G4endl;

    if (_el_field_on)
      G4cout << "* EL field intensity: " << _el_field_int <<
                "  ->  EL Light yield: " << xgp.ELLightYield(_el_field_int) << G4endl;
    else
      G4cout << "* EL field OFF" << G4endl;
  }


  // GATE //
  G4String gate_name = "GATE";

  G4Material* gate_mat = MaterialsList::FakeDielectric(_xenon_gas, "gate_mat");
  gate_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_gas_pressure,
    _gas_temperature, _gate_transparency, _gate_thickness));

  G4double gate_diam = _active_diam;
  G4double gate_posZ = _el_gap_length/2. - _gate_thickness/2.;

  G4Tubs* gate_solid =
    new G4Tubs(gate_name, 0., gate_diam/2., _gate_thickness/2., 0, twopi);

  G4LogicalVolume* gate_logic =
    new G4LogicalVolume(gate_solid, gate_mat, gate_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., gate_posZ), gate_logic,
                    gate_name, el_gap_logic, false, 0, _verbosity);

  // Visibility
  gate_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Verbosity
  if (_verbosity) G4cout << "* GATE Z position: " << el_gap_posZ + gate_posZ << G4endl;


  // ANODE //
  G4String anode_name = "ANODE";

  G4Material* anode_mat = MaterialsList::FakeDielectric(_xenon_gas, "anode_mat");
  anode_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_gas_pressure,
    _gas_temperature, _anode_transparency, _anode_thickness));

  G4double anode_diam = _active_diam;
  G4double anode_posZ = -_el_gap_length/2. + _anode_thickness/2.;

  G4Tubs* anode_solid =
    new G4Tubs(anode_name, 0., anode_diam/2., _anode_thickness/2., 0, twopi);

  G4LogicalVolume* anode_logic =
    new G4LogicalVolume(anode_solid, anode_mat, anode_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., anode_posZ), anode_logic,
                    anode_name, el_gap_logic, false, 0, _verbosity);

  // Visibility
  anode_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Verbosity
  if (_verbosity) G4cout << "* ANODE Z position: " << el_gap_posZ + anode_posZ << G4endl;
}


void NextFlexFieldCage::BuildLightTube()
{
  /// LIGHT_TUBE ///
  G4String light_tube_name = "LIGHT_TUBE";

  _light_tube_outer_rad = _light_tube_inner_rad + _light_tube_thickness;
  
  G4double light_tube_iniZ = - _el_gap_length;

  G4double light_tube_posZ = light_tube_iniZ + _fc_length/2.;

  G4Tubs* light_tube_solid =
    new G4Tubs(light_tube_name, _light_tube_inner_rad, _light_tube_outer_rad,
               _fc_length/2., 0, twopi);

  G4LogicalVolume* light_tube_logic =
    new G4LogicalVolume(light_tube_solid, _teflon_mat, light_tube_name);

  G4VPhysicalVolume* light_tube_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., light_tube_posZ), light_tube_logic,
                      light_tube_name, _mother_logic, false, 0, _verbosity);

  // Adding the optical surface
  G4OpticalSurface* light_tube_optSurf = 
    new G4OpticalSurface(light_tube_name, unified, ground, dielectric_metal);
  light_tube_optSurf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  new G4LogicalSkinSurface(light_tube_name, light_tube_logic, light_tube_optSurf);

  // Visibility
  if (_visibility) {
    G4VisAttributes light_blue_col = nexus::LightBlue();
    //light_blue_col.SetForceSolid(true);
    light_tube_logic->SetVisAttributes(light_blue_col);
  }
  else light_tube_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  _light_tube_gen = new CylinderPointSampler2020(light_tube_phys);


  /// The UV wavelength Shifter in LIGHT_TUBE ///
  // Only if there are no fibers barrel
  if (not _fc_with_fibers) {
    G4String light_tube_wls_name = "LIGHT_TUBE_WLS";
  
    G4double light_tube_wls_length    = _fc_length;
    G4double light_tube_wls_inner_rad = _light_tube_inner_rad;
    G4double light_tube_wls_outer_rad = light_tube_wls_inner_rad + _wls_thickness;
  
    G4Tubs* light_tube_wls_solid =
      new G4Tubs(light_tube_wls_name, light_tube_wls_inner_rad, light_tube_wls_outer_rad,
                 light_tube_wls_length/2., 0, twopi);
  
    G4LogicalVolume* light_tube_wls_logic =
      new G4LogicalVolume(light_tube_wls_solid, _wls_mat, light_tube_wls_name);
  
    G4VPhysicalVolume* light_tube_wls_phys =
      new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), light_tube_wls_logic,
                        light_tube_wls_name, light_tube_logic, false, 0, _verbosity);
  
    // Visibility
    light_tube_wls_logic->SetVisAttributes(G4VisAttributes::Invisible);
  
    // Optical surface
    G4OpticalSurface* light_tube_wls_optSurf =
      new G4OpticalSurface("light_tube_wls_optSurf", glisur, ground,
                           dielectric_dielectric, .01);

    // Border Surface with ACTIVE
    new G4LogicalBorderSurface("LIGHT_TUBE_WLS_ACTIVE_surf", light_tube_wls_phys,
                               _active_phys, light_tube_wls_optSurf);
    new G4LogicalBorderSurface("ACTIVE_LIGHT_TUBE_WLS_surf", _active_phys,
                               light_tube_wls_phys, light_tube_wls_optSurf);

    // Border Surface with BUFFER
    new G4LogicalBorderSurface("LIGHT_TUBE_WLS_BUFFER_surf", light_tube_wls_phys,
                               _buffer_phys, light_tube_wls_optSurf);
    new G4LogicalBorderSurface("BUFFER_LIGHT_TUBE_WLS_surf", _buffer_phys,
                               light_tube_wls_phys, light_tube_wls_optSurf);
  }

  /// Verbosity ///
  if (_verbosity) {
    G4cout << "* Light Tube.  Inner Rad: " << _light_tube_inner_rad << 
              "   Outer Rad: " << _light_tube_outer_rad << G4endl;
    G4cout << "* Light Tube Z positions: " << light_tube_iniZ <<
              " to " << light_tube_iniZ + _fc_length << G4endl;
  } 
}


void NextFlexFieldCage::BuildFibers()
{
  // Radius of the different volumes being built
  G4double inner_rad    = _fiber_inner_rad;
  G4double outer_rad    = _fiber_inner_rad + _fiber_thickness;

  // Lengths & positions
  G4double fiber_length = _fc_length +  2. * _fiber_extra_length;
  _fiber_iniZ           = - _el_gap_length - _fiber_extra_length;
  _fiber_finZ           = _fiber_iniZ  + fiber_length;
  G4double fiber_posZ   = _fiber_iniZ  + fiber_length/2.;

  // Most out / inn volumes
  G4LogicalVolume* out_logic_volume = _mother_logic;
  G4LogicalVolume* inn_logic_volume = _mother_logic;

  /// Outer Cladding ///
  if (_fiber_claddings >= 2) {

    G4String oClad_name = "FIBER_oCLAD";

    G4Tubs* oClad_solid =
      new G4Tubs(oClad_name, inner_rad, outer_rad, fiber_length/2.,
                 0., 360.*deg);

    G4LogicalVolume* oClad_logic =
      new G4LogicalVolume(oClad_solid, _oClad_mat, oClad_name);

    //G4VPhysicalVolume* oClad_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,fiber_posZ),
                      oClad_logic, oClad_name, inn_logic_volume,
                      false, 0, _verbosity);

    // Visibility
    oClad_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Updating info
    out_logic_volume = oClad_logic;
    inn_logic_volume = oClad_logic;
    fiber_posZ       = 0.;
    inner_rad       += _fiber_thickness * _cladding_perc;
    outer_rad       -= _fiber_thickness * _cladding_perc;
  }


  /// Inner Cladding ///
  if (_fiber_claddings >= 1) {

    G4String iClad_name = "FIBER_iCLAD";
    if (_fiber_claddings == 1) iClad_name = "FIBER_CLAD";

    G4Tubs* iClad_solid =
      new G4Tubs(iClad_name, inner_rad, outer_rad, fiber_length/2.,
                 0., 360.*deg);

    G4LogicalVolume* iClad_logic =
      new G4LogicalVolume(iClad_solid, _iClad_mat, iClad_name);

    //G4VPhysicalVolume* iClad_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,fiber_posZ),
                      iClad_logic, iClad_name, inn_logic_volume,
                      false, 0, _verbosity);

    // Visibility
    iClad_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Updating info
    if (_fiber_claddings == 1) out_logic_volume = iClad_logic;
    inn_logic_volume = iClad_logic;
    fiber_posZ       = 0.;
    inner_rad       += _fiber_thickness * _cladding_perc;
    outer_rad       -= _fiber_thickness * _cladding_perc;
  }


  /// Fiber core ///
  G4String core_name = "FIBER_CORE";

  G4Tubs* core_solid =
    new G4Tubs(core_name, inner_rad, outer_rad, fiber_length/2.,
               0., 360.*deg);

  G4LogicalVolume* core_logic =
    new G4LogicalVolume(core_solid, _fiber_mat, core_name);

  G4VPhysicalVolume* core_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,fiber_posZ),
                      core_logic, core_name, inn_logic_volume,
                      false, 0, _verbosity);

  // Visibility
  if (_visibility) core_logic->SetVisAttributes(nexus::LightGreen());
  else             core_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Updating info
  if (_fiber_claddings == 0) out_logic_volume = core_logic;

  // Vertex generator
  _fiber_gen = new CylinderPointSampler2020(core_phys);


  /// The UV wavelength Shifter in FIBERS ///
  // The inner WLS
  G4String fiber_iWls_name = "FIBER_iWLS";

  G4double fiber_iWls_inner_rad = _fiber_inner_rad;
  G4double fiber_iWls_outer_rad = fiber_iWls_inner_rad + _wls_thickness;

  G4Tubs* fiber_iWls_solid =
    new G4Tubs(fiber_iWls_name, fiber_iWls_inner_rad, fiber_iWls_outer_rad,
               fiber_length/2., 0, twopi);

  G4LogicalVolume* fiber_iWls_logic =
    new G4LogicalVolume(fiber_iWls_solid, _wls_mat, fiber_iWls_name);

  //G4VPhysicalVolume* fiber_iWls_phys =
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), fiber_iWls_logic,
                    fiber_iWls_name, out_logic_volume, false, 0, _verbosity);

  // The outer WLS
  G4String fiber_oWls_name = "FIBER_oWLS";

  G4double fiber_oWls_outer_rad = _fiber_inner_rad + _fiber_thickness;;
  G4double fiber_oWls_inner_rad = fiber_oWls_outer_rad - _wls_thickness;

  G4Tubs* fiber_oWls_solid =
    new G4Tubs(fiber_oWls_name, fiber_oWls_inner_rad, fiber_oWls_outer_rad,
               fiber_length/2., 0, twopi);

  G4LogicalVolume* fiber_oWls_logic =
    new G4LogicalVolume(fiber_oWls_solid, _wls_mat, fiber_oWls_name);

  //G4VPhysicalVolume* fiber_oWls_phys =
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), fiber_oWls_logic,
                    fiber_oWls_name, out_logic_volume, false, 0, _verbosity);

  // Visibilities
  fiber_iWls_logic->SetVisAttributes(G4VisAttributes::Invisible);
  fiber_oWls_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Optical surfaces
  G4OpticalSurface* fiber_wls_optSurf =
    new G4OpticalSurface("fiber_wls_optSurf", glisur, ground,
                         dielectric_dielectric, .01);

  new G4LogicalSkinSurface(fiber_iWls_name, fiber_iWls_logic,
                           fiber_wls_optSurf);

  new G4LogicalSkinSurface(fiber_oWls_name, fiber_oWls_logic,
                           fiber_wls_optSurf);

  /// Verbosity ///
  if (_verbosity) {
    G4cout << "* Fiber.  Inner Rad: " << _fiber_inner_rad << 
              "   Outer Rad: " << _fiber_inner_rad + _fiber_thickness << G4endl;
    G4cout << "* Fiber Z positions: " << _fiber_iniZ <<
              " to " << _fiber_finZ << G4endl;
  } 
}


void NextFlexFieldCage::BuildFiberSensors()
{
  G4double fiber_sensor_inner_rad  = _fiber_inner_rad;
  G4double fiber_sensor_outer_rad  = fiber_sensor_inner_rad + _fiber_sensor_size;

  G4int num_fiber_sensors          = (G4int) (twopi * fiber_sensor_inner_rad /
                                              _fiber_sensor_size);

  G4double fiber_sector_phi        = twopi / _num_fiber_sectors;


  /// Fiber Sensor Case ///
  G4String case_name = "FIBER_SENSOR_CASE";

  // Setting a higher thickness for a better visualization
  G4double case_thickness  = 20 * mm;

  G4double case_left_posZ  = _fiber_iniZ - case_thickness/2.;
  G4double case_right_posZ = _fiber_finZ + case_thickness/2.;

  G4Tubs* case_solid =
    new G4Tubs(case_name, fiber_sensor_inner_rad, fiber_sensor_outer_rad,
               case_thickness/2., 0., fiber_sector_phi);

  G4LogicalVolume* left_case_logic =
    new G4LogicalVolume(case_solid, _fiber_sensor_case_mat, case_name);

  G4LogicalVolume* right_case_logic =
    new G4LogicalVolume(case_solid, _fiber_sensor_case_mat, case_name);

  // Visibilities
  if (_visibility) {
    G4VisAttributes red_col = nexus::BloodRed();
    red_col.SetForceSolid(true);
    left_case_logic ->SetVisAttributes(red_col);
    right_case_logic->SetVisAttributes(red_col);
  }
  else {
    left_case_logic ->SetVisAttributes(G4VisAttributes::Invisible);
    right_case_logic->SetVisAttributes(G4VisAttributes::Invisible);
  }


  /// Fiber Sensor ///
  G4String fiber_sensor_name = "FIBER_SENSOR";

  G4double fiber_sensor_thickness  = 10 * um;
  G4double gap_surf_phot           =  5 * um;

  G4double fiber_sensor_left_posZ  = case_thickness/2. - gap_surf_phot
                                     - fiber_sensor_thickness/2.;
  G4double fiber_sensor_right_posZ = - case_thickness/2. + gap_surf_phot
                                     + fiber_sensor_thickness/2.;

  G4Tubs* fiber_sensor_solid =
    new G4Tubs(fiber_sensor_name, fiber_sensor_inner_rad, fiber_sensor_outer_rad,
               fiber_sensor_thickness/2., 0., fiber_sector_phi);

  G4LogicalVolume* fiber_sensor_logic =
    new G4LogicalVolume(fiber_sensor_solid, _fiber_sensor_mat, fiber_sensor_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., fiber_sensor_left_posZ),
                    fiber_sensor_logic, fiber_sensor_name + "_L", left_case_logic,
                    false, 0, _verbosity);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., fiber_sensor_right_posZ),
                    fiber_sensor_logic, fiber_sensor_name + "_R", right_case_logic,
                    false, 0, _verbosity);

  // Visibilities
  fiber_sensor_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // READOUT optical properties
  G4MaterialPropertiesTable* fiber_sensor_optProp = new G4MaterialPropertiesTable();

  const G4int entries = 4;
  G4double energies[entries] = {1.00 * eV, 3.26 * eV, 3.27 * eV, 100. * eV};

  // It does not reflect anything
  G4double sensor_reflectivity[entries] = {0., 0., 0., 0.};

  fiber_sensor_optProp->AddProperty("REFLECTIVITY", energies,
                                    sensor_reflectivity, entries);

  // It detects 0% for UV and "pde" for visible
  G4double sensor_efficiency[entries] = {_fiber_sensor_pde, _fiber_sensor_pde, 0., 0.};

  fiber_sensor_optProp->AddProperty("EFFICIENCY", energies,
                                    sensor_efficiency, entries);

  G4OpticalSurface* fiber_sensor_optSurf = 
    new G4OpticalSurface(fiber_sensor_name, unified, polished, dielectric_metal);

  fiber_sensor_optSurf->SetMaterialPropertiesTable(fiber_sensor_optProp);

  new G4LogicalSkinSurface(fiber_sensor_name, fiber_sensor_logic, fiber_sensor_optSurf);

  // fiber sensor sensitive detector 
  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
  if (!sdmgr->FindSensitiveDetector(fiber_sensor_name, false)) {
    PmtSD* fiber_sensor_sd = new PmtSD(fiber_sensor_name);
    fiber_sensor_sd->SetDetectorVolumeDepth(0);
    fiber_sensor_sd->SetMotherVolumeDepth(0);
    fiber_sensor_sd->SetTimeBinning(_fiber_sensor_bin);

    G4SDManager::GetSDMpointer()->AddNewDetector(fiber_sensor_sd);
    left_case_logic ->SetSensitiveDetector(fiber_sensor_sd);
    right_case_logic->SetSensitiveDetector(fiber_sensor_sd);
  }


  /// Placing the sensor cases (one per sector) ///
  G4RotationMatrix fiber_sensor_rot;
  for (G4int sector_id=0; sector_id < _num_fiber_sectors; sector_id++) {

    if (sector_id > 0) fiber_sensor_rot.rotateZ(fiber_sector_phi);

    new G4PVPlacement(G4Transform3D(fiber_sensor_rot,
                      G4ThreeVector(0., 0., case_left_posZ)),
                      left_case_logic, fiber_sensor_name + "_L", _mother_logic,
                      true, _first_left_sensor_id + sector_id, false);
  
    new G4PVPlacement(G4Transform3D(fiber_sensor_rot,
                      G4ThreeVector(0., 0., case_right_posZ)),
                      right_case_logic, fiber_sensor_name + "_R", _mother_logic,
                      true, _first_right_sensor_id + sector_id, false);
  }

  /// Verbosity
  if (_verbosity)
    G4cout << "* Num fiber sensors     : " << num_fiber_sensors  << G4endl;
    G4cout << "* Num fiber sectors     : " << _num_fiber_sectors << G4endl;
    G4cout << "* Num fibers / sector   : ";
    G4cout << 1. * num_fiber_sensors / _num_fiber_sectors << G4endl;
    G4cout << "* Fiber sector phi (deg): " << fiber_sector_phi / deg << G4endl;  
}


G4ThreeVector NextFlexFieldCage::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "ACTIVE") {
    vertex = _active_gen->GenerateVertex("VOLUME");
  }
  else if (region == "BUFFER") {
    vertex = _buffer_gen->GenerateVertex("VOLUME");
  }
  else if (region == "EL_GAP") {
    vertex = _el_gap_gen->GenerateVertex("VOLUME");
  }
  else if (region == "LIGHT_TUBE") {
    vertex = _light_tube_gen->GenerateVertex("VOLUME");
  }
  else if (region == "FIBER_CORE") {
    vertex = _fiber_gen->GenerateVertex("VOLUME");
  }
  else {
    G4Exception("[NextNew]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }

  return vertex;
}
