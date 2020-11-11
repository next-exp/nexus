// -----------------------------------------------------------------------------
// nexus | NextDemoVessel.cc
//
// Geometry of the pressure vessel of the NEXT-DEMO++ detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextDemoVessel.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"

#include <G4GenericMessenger.hh>
#include <G4UnitsTable.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>

using namespace nexus;


NextDemoVessel::NextDemoVessel():
  BaseGeometry(),
  vessel_diam_  (298.8*mm), // Internal diameter
  vessel_length_(840.0*mm), // Internal length
  vessel_thickn_(  3.0*mm),
  gate_endcap_distance_(-vessel_length_/2. + 92.1*mm),
  gas_pressure_(10.*bar),
  gas_temperature_(300.*kelvin),
  sc_yield_(0.),
  calsrc_position_(0),
  calsrc_angle_(0),
  msg_(nullptr),
  gas_phys_vol_(nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/",
                                "Control commands of the NEXT-DEMO geometry.");

  G4GenericMessenger::Command& gas_pressure_cmd =
    msg_->DeclareProperty("pressure", gas_pressure_, "Xenon gas pressure.");
  gas_pressure_cmd.SetUnitCategory("Pressure");
  gas_pressure_cmd.SetParameterName("pressure", false);
  gas_pressure_cmd.SetRange("pressure>0.");

  new G4UnitDefinition("1/MeV", "1/MeV", "1/Energy", 1./MeV);

  G4GenericMessenger::Command& sc_yield_cmd =
     msg_->DeclareProperty("sc_yield", sc_yield_,
                           "Scintillation yield (photons/MeV) of xenon gas.");
  sc_yield_cmd.SetParameterName("sc_yield", true);
  sc_yield_cmd.SetUnitCategory("1/Energy");

  G4GenericMessenger::Command& calsrc_position_cmd =
    msg_->DeclareProperty("calsrc_position", calsrc_position_,
                          "Position of calibration with respect to anode endcap.");

  G4GenericMessenger::Command& calsrc_angle_cmd =
    msg_->DeclareProperty("calsrc_angle", calsrc_angle_,
                          "Angle of calibration with respect to horizontal plane.");
}


NextDemoVessel::~NextDemoVessel()
{
  delete msg_;
}


void NextDemoVessel::Construct()
{
  ////////////////////////////////////////////////////////////////////

  G4String vessel_name = "VESSEL";

  G4Tubs* vessel_solid_vol =
    new G4Tubs(vessel_name, 0., (vessel_diam_/2.+vessel_thickn_),
               vessel_length_/2., 0, twopi);

  G4LogicalVolume* vessel_logic_vol =
    new G4LogicalVolume(vessel_solid_vol, MaterialsList::Steel(), vessel_name);

  BaseGeometry::SetLogicalVolume(vessel_logic_vol);

  ////////////////////////////////////////////////////////////////////

  G4String gas_name = "GAS";

  G4Material* gas_material = MaterialsList::GXe(gas_pressure_, gas_temperature_);
  gas_material->
    SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(gas_pressure_,
                                                              gas_temperature_,
                                                              sc_yield_));

  G4Tubs* gas_solid_vol =
    new G4Tubs(gas_name, 0., vessel_diam_/2., vessel_length_/2., 0, twopi);

  G4LogicalVolume* gas_logic_vol =
    new G4LogicalVolume(gas_solid_vol, MaterialsList::Steel(), gas_name);

  gas_phys_vol_ = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.),
                                    gas_logic_vol, gas_name, vessel_logic_vol,
                                    false, 0, false);
}


G4ThreeVector NextDemoVessel::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vtx;

  if (region == "CALIBRATION_SOURCE") {
    G4double radius = vessel_diam_/2. + vessel_thickn_;
    vtx.setX(radius * std::cos(calsrc_angle_));
    vtx.setY(radius * std::sin(calsrc_angle_));
    vtx.setZ(-vessel_length_/2. + calsrc_position_);
  }

  return G4ThreeVector();
}
