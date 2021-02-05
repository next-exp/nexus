// -----------------------------------------------------------------------------
//  nexus | NextFlex.cc
//
//  NEXT-Flex Detector is a very flexible geometry for performance studies.
//  Many of the dimensions of the TrackingPlane, FieldCage and WLS-fibers barrel
//  are settable by paramater.
//  A typical use of this geometry can be the performance comparison between
//  different TrackingPlane configurations(EL, SiPM sizes, pitch ...).  
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
  verbosity_         (false),
  ics_visibility_    (false),
  msg_               (nullptr),
  gas_name_          ("enrichedXe"),
  gas_pressure_      (15.  * bar),
  gas_temperature_   (300. * kelvin),
  sc_yield_          (25510. * 1 / MeV),
  e_lifetime_        (1000. * ms),
  ics_thickness_     (12. * cm),
  adhoc_x_           (0.),              // Vertex-X in case of AD_HOC region
  adhoc_y_           (0.),              // Vertex-Y in case of AD_HOC region
  adhoc_z_           (0.)               // Vertex-Z in case of AD_HOC region
{
  
  // Messenger
  msg_ = new G4GenericMessenger(this, "/Geometry/NextFlex/",
                                "Control commands of the NextFlex geometry.");

  // Hard-wired dimensions
  lightTube_ICS_gap_ = 55. * mm;  // (Set as in NEXT100. To be checked)

  // Parametrized dimensions
  DefineConfigurationParameters();

  // Field Cage
  field_cage_ = new NextFlexFieldCage();

  // Energy Plane
  energy_plane_ = new NextFlexEnergyPlane();

  // Tracking Plane
  tracking_plane_ = new NextFlexTrackingPlane();
}



NextFlex::~NextFlex()
{
  delete msg_;
  delete field_cage_;
  delete tracking_plane_;
  delete energy_plane_;
}



void NextFlex::DefineConfigurationParameters()
{
  // Verbosity
  msg_->DeclareProperty("verbosity", verbosity_, "Verbosity");

  // Gas properties
  msg_->DeclareProperty("gas", gas_name_, "Xenon gas type.");

  G4GenericMessenger::Command& gas_pressure_cmd =
    msg_->DeclareProperty("gas_pressure", gas_pressure_,
                          "Pressure of the xenon gas.");
  gas_pressure_cmd.SetUnitCategory("Pressure");
  gas_pressure_cmd.SetParameterName("gas_pressure", false);
  gas_pressure_cmd.SetRange("gas_pressure>0.");

  G4GenericMessenger::Command& gas_temperature_cmd =
    msg_->DeclareProperty("gas_temperature", gas_temperature_,
                          "Temperature of the xenon gas.");
  gas_temperature_cmd.SetUnitCategory("Temperature");
  gas_temperature_cmd.SetParameterName("gas_temperature", false);
  gas_temperature_cmd.SetRange("gas_temperature>0.");

  new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);
  G4GenericMessenger::Command& sc_yield_cmd =
    msg_->DeclareProperty("sc_yield", sc_yield_,
        "Set scintillation yield for GXe. It is in photons/MeV");
  sc_yield_cmd.SetParameterName("sc_yield", true);
  sc_yield_cmd.SetUnitCategory("1/Energy");

  G4GenericMessenger::Command& e_lifetime_cmd =
    msg_->DeclareProperty("e_lifetime", e_lifetime_,
                          "Electron lifetime in gas.");
  e_lifetime_cmd.SetParameterName("e_lifetime", false);
  e_lifetime_cmd.SetUnitCategory("Time");
  e_lifetime_cmd.SetRange("e_lifetime>0.");

  // Specific vertex in case region to shoot from is AD_HOC
  G4GenericMessenger::Command& adhoc_x_cmd =
    msg_->DeclareProperty("specific_vertex_X", adhoc_x_,
      "If region is AD_HOC, x coord where particles are generated");
  adhoc_x_cmd.SetParameterName("specific_vertex_X", false);
  adhoc_x_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command& adhoc_y_cmd =
    msg_->DeclareProperty("specific_vertex_Y", adhoc_y_,
      "If region is AD_HOC, y coord where particles are generated");
  adhoc_y_cmd.SetParameterName("specific_vertex_Y", false);
  adhoc_y_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command& adhoc_z_cmd =
    msg_->DeclareProperty("specific_vertex_Z", adhoc_z_,
      "If region is AD_HOC, z coord where particles are generated");
  adhoc_z_cmd.SetParameterName("specific_vertex_Z", false);
  adhoc_z_cmd.SetUnitCategory("Length");

  // ICS
  G4GenericMessenger::Command& ics_thickness_cmd =
    msg_->DeclareProperty("ics_thickness", ics_thickness_,
                          "Thickness of the ICS barrel.");
  ics_thickness_cmd.SetParameterName("ics_thickness", false);
  ics_thickness_cmd.SetUnitCategory("Length");
  ics_thickness_cmd.SetRange("ics_thickness>=0.");

  msg_->DeclareProperty("ics_visibility", ics_visibility_, "ICS Visibility");
}



void NextFlex::DefineMaterials()
{
  // Copper
  copper_mat_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

  // Air
  air_mat_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  // Defining the gas xenon
  if (gas_name_ == "naturalXe")
    xenon_gas_ = MaterialsList::GXe(gas_pressure_, gas_temperature_);

  else if (gas_name_ == "enrichedXe")
    xenon_gas_ = MaterialsList::GXeEnriched(gas_pressure_, gas_temperature_);

  else if (gas_name_ == "depletedXe")
    xenon_gas_ = MaterialsList::GXeDepleted(gas_pressure_, gas_temperature_);

  else
    G4Exception("[NextFlex]", "DefineMaterials()", FatalException,
    "Unknown xenon gas type. Valid options are naturalXe, enrichedXe or depletedXe.");

  xenon_gas_->
    SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(gas_pressure_,
                                                              gas_temperature_,
                                                              sc_yield_,
                                                              e_lifetime_));
}



void NextFlex::Construct()
{

  // Verbosity
  if(verbosity_) {
    G4cout << G4endl << "***** Verbosing NEXT Parametrized geometry *****" << G4endl;
  }

  // Define materials.
  DefineMaterials();


  // The lab
  G4String lab_name = "LABORATORY";
  G4double lab_size = 20. * m;

  G4Box* lab_solid_vol = new G4Box(lab_name, lab_size/2.,
                                   lab_size/2., lab_size/2.);

  G4LogicalVolume* lab_logic_vol = new G4LogicalVolume(lab_solid_vol,
                                                       air_mat_, lab_name);

  BaseGeometry::SetLogicalVolume(lab_logic_vol);

  lab_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);


  // The Gas
  G4String gas_name = "GAS";
  G4double gas_size = 10. * m;

  G4Box* gas_solid_vol = new G4Box(gas_name, gas_size/2.,
                                   gas_size/2., gas_size/2.);

  G4LogicalVolume* gas_logic_vol = new G4LogicalVolume(gas_solid_vol,
                                                       xenon_gas_, gas_name);

  gas_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  G4VPhysicalVolume* gas_phys_vol =
    new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), gas_logic_vol,
                      gas_name, lab_logic_vol, false, 0, verbosity_);

  // The Field Cage
  field_cage_->SetMotherLogicalVolume(gas_logic_vol);
  field_cage_->SetFirstLeftSensorID(FIRST_LEFT_FIBER_SENSOR_ID);
  field_cage_->SetFirstRightSensorID(FIRST_RIGHT_FIBER_SENSOR_ID);
  field_cage_->Construct();

  // Energy Plane
  energy_plane_->SetMotherLogicalVolume(gas_logic_vol);
  energy_plane_->SetNeighGasPhysicalVolume(field_cage_->Get_BUFFER_phys());
  energy_plane_->SetDiameter(field_cage_->Get_ACTIVE_diam());
  energy_plane_->SetOriginZ(field_cage_->Get_BUFFER_finalZ());
  energy_plane_->SetFirstSensorID(FIRST_ENERGY_SENSOR_ID);
  energy_plane_->Construct();

  // Tracking Plane
  tracking_plane_->SetMotherLogicalVolume(gas_logic_vol);
  tracking_plane_->SetNeighGasPhysicalVolume(gas_phys_vol);
  tracking_plane_->SetDiameter(field_cage_->Get_ACTIVE_diam());
  tracking_plane_->SetOriginZ(field_cage_->Get_EL_GAP_iniZ());
  tracking_plane_->SetFirstSensorID(FIRST_TRACKING_SENSOR_ID);
  tracking_plane_->Construct();

  // The ICS
  BuildICS(gas_logic_vol);

  // Verbosity
  if(verbosity_) G4cout << G4endl;
}



void NextFlex::BuildICS(G4LogicalVolume* mother_logic) {

  // Verbosity
  if(verbosity_) {
    G4cout << G4endl << "*** NEXT-Flex ICS ..." << G4endl;
  }

  G4String ics_name = "ICS";

  G4double ics_inner_rad = field_cage_->Get_FC_outer_rad() + lightTube_ICS_gap_;
  G4double ics_outer_rad = ics_inner_rad + ics_thickness_;

  G4double ics_iniZ      = tracking_plane_->Get_TP_iniZ();
  G4double ics_finZ      = energy_plane_  ->Get_EP_finZ();
  G4double ics_length    = ics_finZ - ics_iniZ;
  G4double ics_posZ      = ics_iniZ + ics_length/2.;

  G4Tubs* ics_solid =
    new G4Tubs(ics_name, ics_inner_rad, ics_outer_rad, ics_length/2., 0, twopi);

  G4LogicalVolume* ics_logic =
    new G4LogicalVolume(ics_solid, copper_mat_, ics_name);

  G4VPhysicalVolume* ics_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., ics_posZ), ics_logic,
                      ics_name, mother_logic, false, 0, verbosity_);

  // Visibility
  if (ics_visibility_) ics_logic->SetVisAttributes(nexus::CopperBrown());
  else                 ics_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  copper_gen_ = new CylinderPointSampler2020(ics_phys);

  // Verbosity
  if (verbosity_) {
    G4cout << "* ICS.  Inner Rad: " << ics_inner_rad << 
              "   Outer Rad: " << ics_outer_rad << G4endl;
    G4cout << "* ICS Z positions: " << ics_iniZ << " to " << ics_finZ << G4endl;
  }
}



G4ThreeVector NextFlex::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "AD_HOC") {
    vertex = G4ThreeVector(adhoc_x_, adhoc_y_, adhoc_z_);
  }

  // ICS region
  else if (region == "ICS") {
    vertex = copper_gen_->GenerateVertex("VOLUME");
  }

  // Field Cage regions
  else if (
    (region == "ACTIVE") ||
    (region == "BUFFER") ||
    (region == "EL_GAP") ||
    (region == "LIGHT_TUBE") ||
    (region == "FIBER_CORE")) {
    vertex = field_cage_->GenerateVertex(region);
  }

  // Energy Plane regions
  else if (
    (region == "EP_COPPER") ||
    (region == "EP_WINDOWS")) {
    vertex = energy_plane_->GenerateVertex(region);
  }

  // Tracking Plane regions
  else if (
    (region == "TP_COPPER")) {
    vertex = tracking_plane_->GenerateVertex(region);
  }

  else {
    G4Exception("[NextFlex]", "GenerateVertex()", FatalException,
      "Unknown vertex generation region!");
  }

  return vertex;
}
