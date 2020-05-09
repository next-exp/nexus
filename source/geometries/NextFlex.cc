// -----------------------------------------------------------------------------
//  nexus | NextFlex.cc
//
//  NEXT-Flex Detector geometry for performance studies.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextFlex.h"
#include "NextFlexFieldCage.h"
#include "NextFlexEnergyPlane.h"
#include "NextFlexTrackingPlane.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "XenonGasProperties.h"
#include "CylinderPointSampler2020.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>


using namespace nexus;


NextFlex::NextFlex():
  BaseGeometry(),
  _verbosity         (false),
  _ics_visibility    (false),
  _msg               (nullptr),
  _gas_name          ("enrichedXe"),
  _gas_pressure      (15.  * bar),
  _gas_temperature   (300. * kelvin),
  _e_lifetime        (1000. * ms),
  _adhoc_x           (0.),              // Vertex-X in case of AD_HOC region
  _adhoc_y           (0.),              // Vertex-Y in case of AD_HOC region
  _adhoc_z           (0.)               // Vertex-Z in case of AD_HOC region
{
  
  // Messenger
  _msg = new G4GenericMessenger(this, "/Geometry/NextFlex/",
                                "Control commands of the NextFlex geometry.");

  // Hard-wired dimensions
  _sc_yield = 25510. * 1 / MeV;   // Scintillation yield

  _lightTube_ICS_gap = 55. * mm;  // (Set as in NEXT100. To be checked)

  // Parametrized dimensions
  DefineConfigurationParameters();

  // Field Cage
  _field_cage = new NextFlexFieldCage();

  // Energy Plane
  _energy_plane = new NextFlexEnergyPlane();

  // Tracking Plane
  _tracking_plane = new NextFlexTrackingPlane();
}



NextFlex::~NextFlex()
{
  delete _msg;
  delete _field_cage;
  delete _tracking_plane;
  delete _energy_plane;
}



void NextFlex::DefineConfigurationParameters()
{
  // Verbosity
  _msg->DeclareProperty("verbosity", _verbosity, "Verbosity");

  // Gas properties
  _msg->DeclareProperty("gas", _gas_name, "Xenon gas type.");

  G4GenericMessenger::Command& gas_pressure_cmd =
    _msg->DeclareProperty("gas_pressure", _gas_pressure,
                          "Pressure of the xenon gas.");
  gas_pressure_cmd.SetUnitCategory("Pressure");
  gas_pressure_cmd.SetParameterName("gas_pressure", false);
  gas_pressure_cmd.SetRange("gas_pressure>0.");

  G4GenericMessenger::Command& gas_temperature_cmd =
    _msg->DeclareProperty("gas_temperature", _gas_temperature,
                          "Temperature of the xenon gas.");
  gas_temperature_cmd.SetUnitCategory("Temperature");
  gas_temperature_cmd.SetParameterName("gas_temperature", false);
  gas_temperature_cmd.SetRange("gas_temperature>0.");

  G4GenericMessenger::Command& e_lifetime_cmd =
    _msg->DeclareProperty("e_lifetime", _e_lifetime,
                          "Electron lifetime in gas.");
  e_lifetime_cmd.SetParameterName("e_lifetime", false);
  e_lifetime_cmd.SetUnitCategory("Time");
  e_lifetime_cmd.SetRange("e_lifetime>0.");

  // Specific vertex in case region to shoot from is AD_HOC
  G4GenericMessenger::Command& _adhoc_x_cmd =
    _msg->DeclareProperty("specific_vertex_X", _adhoc_x,
      "If region is AD_HOC, x coord where particles are generated");
  _adhoc_x_cmd.SetParameterName("specific_vertex_X", false);
  _adhoc_x_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command& _adhoc_y_cmd =
    _msg->DeclareProperty("specific_vertex_Y", _adhoc_y,
      "If region is AD_HOC, y coord where particles are generated");
  _adhoc_y_cmd.SetParameterName("specific_vertex_Y", false);
  _adhoc_y_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command& _adhoc_z_cmd =
    _msg->DeclareProperty("specific_vertex_Z", _adhoc_z,
      "If region is AD_HOC, z coord where particles are generated");
  _adhoc_z_cmd.SetParameterName("specific_vertex_Z", false);
  _adhoc_z_cmd.SetUnitCategory("Length");

  // ICS
  G4GenericMessenger::Command& ics_thickness_cmd =
    _msg->DeclareProperty("ics_thickness", _ics_thickness,
                          "Thickness of the ICS barrel.");
  ics_thickness_cmd.SetParameterName("ics_thickness", false);
  ics_thickness_cmd.SetUnitCategory("Length");
  ics_thickness_cmd.SetRange("ics_thickness>=0.");

  _msg->DeclareProperty("ics_visibility", _ics_visibility, "ICS Visibility");
}



void NextFlex::DefineMaterials()
{
  // Copper
  _copper_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

  // Defining the gas xenon
  if (_gas_name == "naturalXe")
    _xenon_gas = MaterialsList::GXe(_gas_pressure, _gas_temperature);

  else if (_gas_name == "enrichedXe")
    _xenon_gas = MaterialsList::GXeEnriched(_gas_pressure, _gas_temperature);

  else if (_gas_name == "depletedXe")
    _xenon_gas = MaterialsList::GXeDepleted(_gas_pressure, _gas_temperature);

  else
    G4Exception("[NextParam]", "DefineMaterials()", FatalException,
    "Unknown xenon gas type. Valid options are naturalXe, enrichedXe or depletedXe.");

  _xenon_gas->
    SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_gas_pressure,
                                                              _gas_temperature,
                                                              _sc_yield,
                                                              _e_lifetime));
}



void NextFlex::Construct()
{

  // Verbosity
  if(_verbosity) {
    G4cout << G4endl << "***** Verbosing NEXT Parametrized geometry *****" << G4endl;
  }

  // Define materials.
  DefineMaterials();


  // The lab
  G4String lab_name = "LABORATORY";
  G4double lab_size = 50. * m;

  G4Box* lab_solid_vol = new G4Box(lab_name, lab_size/2.,
                                   lab_size/2., lab_size/2.);

  G4LogicalVolume* lab_logic_vol = new G4LogicalVolume(lab_solid_vol,
                                                       _xenon_gas, lab_name);

  BaseGeometry::SetLogicalVolume(lab_logic_vol);

  lab_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);


  // The Gas
  G4String gas_name = "GAS";
  G4double gas_size = 20. * m;

  G4Box* gas_solid_vol = new G4Box(gas_name, gas_size/2.,
                                   gas_size/2., gas_size/2.);

  G4LogicalVolume* gas_logic_vol = new G4LogicalVolume(gas_solid_vol,
                                                       _xenon_gas, gas_name);

  gas_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  G4VPhysicalVolume* gas_phys_vol =
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), gas_logic_vol,
                      gas_name, lab_logic_vol, false, 0, true);

  // The Field Cage
  _field_cage->SetMotherLogicalVolume(gas_logic_vol);
  _field_cage->SetFirstLeftSensorID(FIRST_LEFT_FIBER_SENSOR_ID);
  _field_cage->SetFirstRightSensorID(FIRST_RIGHT_FIBER_SENSOR_ID);
  _field_cage->Construct();

  // Energy Plane
  _energy_plane->SetMotherLogicalVolume(gas_logic_vol);
  _energy_plane->SetNeighGasPhysicalVolume(_field_cage->Get_BUFFER_phys());
  _energy_plane->SetDiameter(_field_cage->Get_ACTIVE_diam());
  _energy_plane->SetOriginZ(_field_cage->Get_BUFFER_finalZ());
  _energy_plane->SetFirstSensorID(FIRST_ENERGY_SENSOR_ID);
  _energy_plane->Construct();

  // Tracking Plane
  _tracking_plane->SetMotherLogicalVolume(gas_logic_vol);
  _tracking_plane->SetNeighGasPhysicalVolume(gas_phys_vol);
  _tracking_plane->SetDiameter(_field_cage->Get_ACTIVE_diam());
  _tracking_plane->SetOriginZ(_field_cage->Get_EL_GAP_iniZ());
  _tracking_plane->SetFirstSensorID(FIRST_TRACKING_SENSOR_ID);
  _tracking_plane->Construct();

  // The ICS
  BuildICS(gas_logic_vol);

  // Verbosity
  if(_verbosity) G4cout << G4endl;
}



void NextFlex::BuildICS(G4LogicalVolume* mother_logic) {

  // Verbosity
  if(_verbosity) {
    G4cout << G4endl << "*** NEXT-Flex ICS ..." << G4endl;
  }

  G4String ics_name = "ICS";

  G4double ics_inner_rad = _field_cage->Get_FC_outer_rad() + _lightTube_ICS_gap;
  G4double ics_outer_rad = ics_inner_rad + _ics_thickness;

  G4double ics_iniZ      = _tracking_plane->Get_TP_iniZ();
  G4double ics_finZ      = _energy_plane  ->Get_EP_finZ();
  G4double ics_length    = ics_finZ - ics_iniZ;
  G4double ics_posZ      = ics_iniZ + ics_length/2.;

  G4Tubs* ics_solid =
    new G4Tubs(ics_name, ics_inner_rad, ics_outer_rad, ics_length/2., 0, twopi);

  G4LogicalVolume* ics_logic =
    new G4LogicalVolume(ics_solid, _copper_mat, ics_name);

  G4VPhysicalVolume* ics_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., ics_posZ), ics_logic,
                      ics_name, mother_logic, false, 0, _verbosity);

  // Visibility
  if (_ics_visibility) ics_logic->SetVisAttributes(nexus::CopperBrown());
  else                 ics_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  _copper_gen = new CylinderPointSampler2020(ics_phys);

  // Verbosity
  if (_verbosity)
    G4cout << "* ICS.  Inner Rad: " << ics_inner_rad << 
              "   Outer Rad: " << ics_outer_rad << G4endl;
    G4cout << "* ICS Z positions: " << ics_iniZ << " to " << ics_finZ << G4endl;
}



G4ThreeVector NextFlex::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "AD_HOC") {
    vertex = G4ThreeVector(_adhoc_x, _adhoc_y, _adhoc_z);
  }

  // ICS region
  else if (region == "ICS") {
    vertex = _copper_gen->GenerateVertex("VOLUME");
  }

  // Field Cage regions
  else if (
    (region == "ACTIVE") ||
    (region == "BUFFER") ||
    (region == "EL_GAP") ||
    (region == "LIGHT_TUBE") ||
    (region == "FIBER_CORE")) {
    vertex = _field_cage->GenerateVertex(region);
  }

  // Energy Plane regions
  else if (
    (region == "EP_COPPER") ||
    (region == "EP_WINDOWS")) {
    vertex = _energy_plane->GenerateVertex(region);
  }

  // Tracking Plane regions
  else if (
    (region == "TP_COPPER")) {
    vertex = _tracking_plane->GenerateVertex(region);
  }

  else {
    G4Exception("[NextFlex]", "GenerateVertex()", FatalException,
      "Unknown vertex generation region!");
  }

  return vertex;
}
