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
#include <G4VisAttributes.hh>

using namespace nexus;


NextDemoVessel::NextDemoVessel():
  GeometryBase(),
  vessel_vis_(true),
  vessel_diam_  (298.8*mm), // Internal diameter
  vessel_length_(840.0*mm), // Internal length
  vessel_thickn_(  3.0*mm),
  gate_endcap_distance_(-vessel_length_/2. + 92.1*mm),
  gas_pressure_(10.*bar),
  gas_temperature_(300.*kelvin),
  sc_yield_(0.),
  e_lifetime_(1.*s),
  calsrc_zpos_(0.),
  calsrc_angle_(0.),
  msg_(nullptr),
  gas_phys_vol_(nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/NextDemo/",
                                "Control commands of the NEXT-DEMO geometry.");

  msg_->DeclareProperty("vessel_vis", vessel_vis_,
                        "Visibility of pressure vessel.");

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

  G4GenericMessenger::Command& e_lifetime_cmd =
    msg_->DeclareProperty("e_lifetime", e_lifetime_, "Electron lifetime in gas.");
  e_lifetime_cmd.SetParameterName("e_lifetime", false);
  e_lifetime_cmd.SetUnitCategory("Time");
  e_lifetime_cmd.SetRange("e_lifetime>0.");

  G4GenericMessenger::Command& calsrc_zpos_cmd =
    msg_->DeclarePropertyWithUnit("calsrc_zpos", "mm", calsrc_zpos_,
                                  "Longitudinal position of calibration source wrt to anode endcap.");
  calsrc_zpos_cmd.SetParameterName("calsrc_zpos", false);
  calsrc_zpos_cmd.SetUnitCategory("Length");

  msg_->DeclarePropertyWithUnit("calsrc_angle", "deg", calsrc_angle_,
                                "Angular position of calibration source wrt to horizontal plane");

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
    new G4LogicalVolume(vessel_solid_vol, materials::Steel(), vessel_name);

  if (!vessel_vis_) vessel_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  GeometryBase::SetLogicalVolume(vessel_logic_vol);

  ////////////////////////////////////////////////////////////////////

  G4String gas_name = "GAS";

  G4Material* gas_material = materials::GXe(gas_pressure_, gas_temperature_);
  gas_material->SetMaterialPropertiesTable(opticalprops::GXe(gas_pressure_,
                                                             gas_temperature_,
                                                             sc_yield_,
                                                             e_lifetime_));

  G4Tubs* gas_solid_vol =
    new G4Tubs(gas_name, 0., vessel_diam_/2., vessel_length_/2., 0, twopi);

  G4LogicalVolume* gas_logic_vol =
    new G4LogicalVolume(gas_solid_vol, gas_material, gas_name);

  gas_phys_vol_ = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.),
                                    gas_logic_vol, gas_name, vessel_logic_vol,
                                    false, 0, false);

  gas_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
}


G4ThreeVector NextDemoVessel::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vtx;

  if (region == "CALIBRATION_SOURCE") {
    G4double radius = vessel_diam_/2. + vessel_thickn_;
    vtx.setX(radius * std::cos(calsrc_angle_));
    vtx.setY(radius * std::sin(calsrc_angle_));
    vtx.setZ(-vessel_length_/2. + calsrc_zpos_);
  }

  return vtx;
}
