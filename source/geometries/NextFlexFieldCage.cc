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
#include <G4UserLimits.hh>


using namespace nexus;


NextFlexFieldCage::NextFlexFieldCage():
  BaseGeometry(),
  _verbosity         (false),
  _visibility        (false),
  _msg               (nullptr),
  _active_length     (116. * cm),          // Distance GATE - CATHODE (meshes not included)
  _drift_transv_diff (1. * mm/sqrt(cm)),   // Drift field transversal diffusion
  _drift_long_diff   (.3 * mm/sqrt(cm)),   // Drift field longitudinal diffusion
  _buffer_length     (282. * mm),          // Distance CATHODE - sapphire window surfaces
  _el_gap_length     (10. * mm),           // Distance ANODE - GATE (meshes included)
  _el_field_on       (false),              // EL field ON-OFF
  _el_field_int      (34.5 * kilovolt/cm), // EL field intensity
  _el_transv_diff    (0. * mm/sqrt(cm)),   // EL field transversal diffusion
  _el_long_diff      (0. * mm/sqrt(cm))    // EL field longitudinal diffusion
{

  // Messenger
  _msg = new G4GenericMessenger(this, "/Geometry/NextFlex/",
                                "Control commands of the NextParam geometry.");

  // Define new categories
  new G4UnitDefinition("kilovolt/cm", "kV/cm", "Electric field", kilovolt/cm);
  new G4UnitDefinition("mm/sqrt(cm)", "mm/sqrt(cm)", "Diffusion", mm/sqrt(cm));

  // Hard-wired dimensions
  _active_diam          = 106.9 * cm;           // Same as NEXT100

  _cathode_thickn       = 0.1   * mm;
  _cathode_transparency = 0.98;
  _anode_thickn         = 0.1   * mm;
  _anode_transparency   = 0.98;                 // It was .88 in NEW
  _gate_thickn          = 0.1   * mm;
  _gate_transparency    = 0.98;

  // Parametrized dimensions
  DefineConfigurationParameters();
}


NextFlexFieldCage::~NextFlexFieldCage()
{
  delete _msg;

  delete _active_gen;
  delete _buffer_gen;
  delete _el_gap_gen;
}


void NextFlexFieldCage::DefineConfigurationParameters()
{
  // Verbosity
  _msg->DeclareProperty("fc_verbosity", _verbosity, "Verbosity");

  // Visibility
  _msg->DeclareProperty("fc_visibility", _visibility, "FIELD_CAGE Visibility");


  // ACTIVE dimensions
  G4GenericMessenger::Command& active_length_cmd =
    _msg->DeclareProperty("active_length", _active_length,
                          "Length of the ACTIVE volume.");
  active_length_cmd.SetUnitCategory("Length");
  active_length_cmd.SetParameterName("active_length", false);
  active_length_cmd.SetRange("active_length>=0.");

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


  // FIELD_CAGE dimensions
  G4GenericMessenger::Command& buffer_length_cmd =
    _msg->DeclareProperty("buffer_length", _buffer_length,
                          "Length of the BUFFER.");
  buffer_length_cmd.SetUnitCategory("Length");
  buffer_length_cmd.SetParameterName("buffer_length", false);
  buffer_length_cmd.SetRange("buffer_length>=0.");

  G4GenericMessenger::Command& el_gap_length_cmd =
    _msg->DeclareProperty("el_gap_length", _el_gap_length,
                          "Length of the EL gap.");
  el_gap_length_cmd.SetUnitCategory("Length");
  el_gap_length_cmd.SetParameterName("el_gap_length", false);
  el_gap_length_cmd.SetRange("el_gap_length>=0.");

  _msg->DeclareProperty("el_field_on", _el_field_on,
                        "true for full simulation), false if fast or parametrized.");

  G4GenericMessenger::Command& _el_field_int_cmd =
    _msg->DeclareProperty("el_field_int", _el_field_int,
                          "EL electric field intensity");
  _el_field_int_cmd.SetParameterName("el_field_int", true);
  _el_field_int_cmd.SetUnitCategory("Electric field");

  G4GenericMessenger::Command& el_transv_diff_cmd =
    _msg->DeclareProperty("el_transv_diff", _el_transv_diff,
                          "Tranvsersal diffusion in the EL region");
  el_transv_diff_cmd.SetParameterName("el_transv_diff", true);
  el_transv_diff_cmd.SetUnitCategory("Diffusion");

  G4GenericMessenger::Command& el_long_diff_cmd =
    _msg->DeclareProperty("el_long_diff", _el_long_diff,
                          "Longitudinal diffusion in the EL region");
  el_long_diff_cmd.SetParameterName("el_long_diff", true);
  el_long_diff_cmd.SetUnitCategory("Diffusion");

  //// Fibers dimensions
  //G4GenericMessenger::Command& fiber_thickness_cmd =
  //  _msg->DeclareProperty("ep_fiber_thickness", _fiber_thickness,
  //                        "Thickness of the EP square fibers.");
  //fiber_thickness_cmd.SetUnitCategory("Length");
  //fiber_thickness_cmd.SetParameterName("ep_fiber_thickness", false);
  //fiber_thickness_cmd.SetRange("ep_fiber_thickness>=0.");


}


void NextFlexFieldCage::DefineMaterials()
{
  // Read gas properties from mother volume
  _xenon_gas       = _mother_logic->GetMaterial();
  _gas_pressure    = _xenon_gas->GetPressure();
  _gas_temperature = _xenon_gas->GetTemperature();
}


void NextFlexFieldCage::Construct()
{
  // Verbosity
  if(_verbosity) G4cout << G4endl << "*** NEXT-Flex Field Cage ..." << G4endl;

  // Getting volumes dimensions based on parameters.
  //ComputeDimensions();

  // Define materials.
  DefineMaterials();

  // ACTIVE
  BuildActive();

  // CATHODE
  BuildCathode();

  // BUFFER
  BuildBuffer();

  // EL GAP
  BuildELgap();
}


void NextFlexFieldCage::BuildActive()
{
  G4String active_name = "ACTIVE";

  G4Tubs* active_solid =
    new G4Tubs(active_name, 0., _active_diam/2., _active_length/2., 0., 360.*deg);

  G4LogicalVolume* active_logic =
    new G4LogicalVolume(active_solid, _xenon_gas, active_name);

  G4VPhysicalVolume* active_phys =
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
  _active_gen = new CylinderPointSampler2020(active_phys);

  // Limit the step size in this volume for better tracking precision
  active_logic->SetUserLimits(new G4UserLimits(1.*mm));
  
  // Set the ACTIVE volume as an ionization sensitive detector
  IonizationSD* active_sd = new IonizationSD("/NEXT_FLEX/ACTIVE");
  active_logic->SetSensitiveDetector(active_sd);
  G4SDManager::GetSDMpointer()->AddNewDetector(active_sd);  
}


void NextFlexFieldCage::BuildCathode()
{
  G4String cathode_name = "CATHODE";

  G4Material* cathode_mat = MaterialsList::FakeDielectric(_xenon_gas, "cathode_mat");
  cathode_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_gas_pressure,
    _gas_temperature, _cathode_transparency, _cathode_thickn));

  G4double cathode_diam = _active_diam;
  G4double cathode_posZ = _active_length + _cathode_thickn/2.;

  G4Tubs* cathode_solid =
    new G4Tubs(cathode_name, 0., cathode_diam/2., _cathode_thickn/2., 0, twopi);

  G4LogicalVolume* cathode_logic =
    new G4LogicalVolume(cathode_solid, cathode_mat, cathode_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., cathode_posZ), cathode_logic,
                    cathode_name, _mother_logic, false, 0, _verbosity);

  // Visibility
  if (_visibility) cathode_logic->SetVisAttributes(nexus::LightBlue());
  else             cathode_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Verbosity
  if (_verbosity) G4cout << "* CATHODE Z position: " << cathode_posZ << G4endl;
}


void NextFlexFieldCage::BuildBuffer()
{
  G4String buffer_name = "BUFFER";

  G4double buffer_diam = _active_diam;
  G4double buffer_posZ = _active_length + _cathode_thickn + _buffer_length/2.;
  _buffer_finalZ = buffer_posZ + _buffer_length/2.;

  G4Tubs* buffer_solid =
    new G4Tubs(buffer_name, 0., buffer_diam/2., _buffer_length/2., 0, twopi);

  G4LogicalVolume* buffer_logic =
    new G4LogicalVolume(buffer_solid, _xenon_gas, buffer_name);

  G4VPhysicalVolume* buffer_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., buffer_posZ), buffer_logic,
                      buffer_name, _mother_logic, false, 0, _verbosity);

  // Visibility
  buffer_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  _buffer_gen = new CylinderPointSampler2020(buffer_phys);

  // Set the BUFFER volume as an ionization sensitive detector
  IonizationSD* buffer_sd = new IonizationSD("/NEXT_FLEX/BUFFER");
  buffer_sd->IncludeInTotalEnergyDeposit(false);
  buffer_logic->SetSensitiveDetector(buffer_sd);
  G4SDManager::GetSDMpointer()->AddNewDetector(buffer_sd);  

  // Verbosity
  if (_verbosity) {
    G4cout << "* BUFFER Z position: " << buffer_posZ    << G4endl;
    G4cout << "* BUFFER length:     " << _buffer_length << G4endl;
  } 
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
  if (_el_field_on) {
    UniformElectricDriftField* el_field = new UniformElectricDriftField();
    el_field->SetCathodePosition(el_gap_posZ + _el_gap_length/2.);
    el_field->SetAnodePosition  (el_gap_posZ - _el_gap_length/2.);
    el_field->SetDriftVelocity  (2.5 * mm/microsecond);
    el_field->SetTransverseDiffusion(_el_transv_diff);
    el_field->SetLongitudinalDiffusion(_el_long_diff);
    XenonGasProperties xgp(_gas_pressure, _gas_temperature);
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
    G4cout << "* EL_GAP Z position: " << el_gap_posZ    << G4endl;
    G4cout << "* EL_GAP length:     " << _el_gap_length << G4endl;
  }


  // GATE //
  G4String gate_name = "GATE";

  G4Material* gate_mat = MaterialsList::FakeDielectric(_xenon_gas, "gate_mat");
  gate_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGrid(_gas_pressure,
    _gas_temperature, _gate_transparency, _gate_thickn));

  G4double gate_diam = _active_diam;
  G4double gate_posZ = _el_gap_length/2. - _gate_thickn/2.;

  G4Tubs* gate_solid =
    new G4Tubs(gate_name, 0., gate_diam/2., _gate_thickn/2., 0, twopi);

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
    _gas_temperature, _anode_transparency, _anode_thickn));

  G4double anode_diam = _active_diam;
  G4double anode_posZ = -_el_gap_length/2. + _anode_thickn/2.;

  G4Tubs* anode_solid =
    new G4Tubs(anode_name, 0., anode_diam/2., _anode_thickn/2., 0, twopi);

  G4LogicalVolume* anode_logic =
    new G4LogicalVolume(anode_solid, anode_mat, anode_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., anode_posZ), anode_logic,
                    anode_name, el_gap_logic, false, 0, _verbosity);

  // Visibility
  anode_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Verbosity
  if (_verbosity) G4cout << "* ANODE Z position: " << el_gap_posZ + anode_posZ << G4endl;
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

  else {
    G4Exception("[NextNew]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }

  return vertex;
}
