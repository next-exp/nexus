// -----------------------------------------------------------------------------
// File   : NextTonScale.cc
// Author : Justo Martin-Albo
// Date   : July 2019
// -----------------------------------------------------------------------------

#include "NextTonScale.h"

#include "CylinderPointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>

using namespace nexus;


NextTonScale::NextTonScale():
  BaseGeometry(),
  msg_(nullptr),
  gas_density_(50.*kg/m3),
  active_diam_(300.*cm), active_length_(300.*cm),
  fcage_thickn_(1.*cm), ics_thickn_(20.*cm), vessel_thickn_(10.*cm),
  endcap_hollow_(20.*cm),
  water_thickn_(3.*m),
  rnd_(nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/NextTonScale/",
                                "Control commands of the NextTonScale geometry.");

  G4GenericMessenger::Command& active_diam_cmd =
    msg_->DeclareProperty("active_diam", active_diam_,
                        "Diameter of the active diameter of the detector.");
  active_diam_cmd.SetUnitCategory("Length");
  active_diam_cmd.SetParameterName("active_diam", false);
  active_diam_cmd.SetRange("active_diam>=0.");

  G4GenericMessenger::Command& active_length_cmd =
    msg_->DeclareProperty("active_length", active_length_,
                        "Length of the active diameter of the detector.");
  active_length_cmd.SetUnitCategory("Length");
  active_length_cmd.SetParameterName("active_length", false);
  active_length_cmd.SetRange("active_length>=0.");

  G4GenericMessenger::Command& fcage_thickn_cmd =
    msg_->DeclareProperty("fcage_thickn", fcage_thickn_,
                        "Thickness of the field cage of the detector.");
  fcage_thickn_cmd.SetUnitCategory("Length");
  fcage_thickn_cmd.SetParameterName("fcage_thickn", false);
  fcage_thickn_cmd.SetRange("fcage_thickn>=0.");

  G4GenericMessenger::Command& ics_thickn_cmd =
    msg_->DeclareProperty("ics_thickn", ics_thickn_,
                        "Thickness of the inner copper shield.");
  ics_thickn_cmd.SetUnitCategory("Length");
  ics_thickn_cmd.SetParameterName("ics_thickn", false);
  ics_thickn_cmd.SetRange("ics_thickn>=0.");

}


NextTonScale::~NextTonScale()
{
  delete msg_;
  delete rnd_;
}


void NextTonScale::Construct()
{
  // LAB ///////////////////////////////////////////////////
  // Volume of air that contains all other detector volumes.

  G4String lab_name = "LAB";

  // Calculate the maximum span of the detector
  G4double lab_size =
    std::max(active_diam_+2.*fcage_thickn_, active_length_+2.*endcap_hollow_) +
    2.*ics_thickn_ + 2.*vessel_thickn_ + 2.*water_thickn_;

  //  Add a couple of meters around as buffer
  lab_size += 4.*m;

  G4Material* lab_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  G4Box* lab_solid_vol =
    new G4Box(lab_name, lab_size/2., lab_size/2., lab_size/2.);

  G4LogicalVolume* lab_logic_vol =
    new G4LogicalVolume(lab_solid_vol, lab_material, lab_name);
  lab_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
  BaseGeometry::SetLogicalVolume(lab_logic_vol);

  //////////////////////////////////////////////////////////

  // Construct now the rest of detector volumes. These methods take
  // as input a pointer to the logical volume that contains them,
  // and return a pointer to the logical volume that should contain the
  // volumes positioned within them.

  G4LogicalVolume* mother_logic_vol = lab_logic_vol;

  mother_logic_vol = ConstructWaterTank(mother_logic_vol);
  mother_logic_vol = ConstructVessel(mother_logic_vol);
  mother_logic_vol = ConstructInnerShield(mother_logic_vol);
  mother_logic_vol = ConstructFieldCage(mother_logic_vol);

  rnd_ = new CylinderPointSampler(active_diam_/2., active_length_/2.,
                                  fcage_thickn_, 0.);
}


G4LogicalVolume* NextTonScale::ConstructWaterTank(G4LogicalVolume* mother_logic_vol)
{
  // TANK //////////////////////////////////////////////////

  G4String tank_name = "WATER_TANK";

  G4double tank_thickn = 10.*cm;
  G4double tank_height = active_diam_ + 2.*fcage_thickn_ + 2.*ics_thickn_ +
                         2.*vessel_thickn_ + 2.*water_thickn_ + 2.*tank_thickn;
  G4double tank_diam   = active_length_ + 2.*ics_thickn_ + 2.*endcap_hollow_ +
                         2.*vessel_thickn_ + 2.*water_thickn_ + 2.*tank_thickn;

  G4Material* tank_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");

  G4Tubs* tank_solid_vol = new G4Tubs(tank_name, 0., tank_diam/2.+tank_thickn,
                                                 tank_height/2.+tank_thickn,
                                                 0., 360.*deg);

  G4LogicalVolume* tank_logic_vol =
    new G4LogicalVolume(tank_solid_vol, tank_material, tank_name);

  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateX(90.*deg);

  new G4PVPlacement(rotation, G4ThreeVector(0.,0.,0.), tank_logic_vol,
                    tank_name, mother_logic_vol, false, 0, true);

  // WATER /////////////////////////////////////////////////

  G4String water_name = "WATER";

  G4double water_diam   = tank_diam - 2.*tank_thickn;
  G4double water_height = tank_height - 2.*tank_thickn;

  G4Material* water_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");

  G4Tubs* water_solid_vol = new G4Tubs(water_name, 0., water_diam/2.,
                                                   water_height/2.,
                                                   0., 360.*deg);

  G4LogicalVolume* water_logic_vol =
    new G4LogicalVolume(water_solid_vol, water_material, water_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), water_logic_vol,
                    water_name, tank_logic_vol, false, 0, true);

  //////////////////////////////////////////////////////////

  return water_logic_vol;
}


G4LogicalVolume* NextTonScale::ConstructVessel(G4LogicalVolume* mother_logic_vol)
{
  // PRESSURE VESSEL ///////////////////////////////////////

  G4String vessel_name = "VESSEL";

  G4double vessel_diam   = active_diam_ + 2.*fcage_thickn_ +
                           2.*ics_thickn_ + 2.*vessel_thickn_;
  G4double vessel_length = active_length_ + 2.*ics_thickn_ +
                           2.*endcap_hollow_ + 2.*vessel_thickn_;

  G4Material* vessel_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");

  G4Tubs* vessel_solid_vol =
    new G4Tubs(vessel_name, 0., vessel_diam/2., vessel_length/2., 0., 360.*deg);

  G4LogicalVolume* vessel_logic_vol =
    new G4LogicalVolume(vessel_solid_vol, vessel_material, vessel_name);

  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateX(90.*deg);

  new G4PVPlacement(rotation, G4ThreeVector(0.,0.,0.), vessel_logic_vol,
                    vessel_name, mother_logic_vol, false, 0, true);

  // GAS ///////////////////////////////////////////////////

  G4String gas_name = "GAS";

  G4double gas_diam   = vessel_diam - 2.*vessel_thickn_;
  G4double gas_length = vessel_length - 2.*vessel_thickn_;

  G4Material* gas_material =
    new G4Material("GXe", gas_density_,
                   G4NistManager::Instance()->FindOrBuildMaterial("G4_Xe"),
                   kStateGas);

  G4Tubs* gas_solid_vol =
    new G4Tubs(gas_name, 0., gas_diam/2., gas_length/2., 0., 360.*deg);

  G4LogicalVolume* gas_logic_vol =
    new G4LogicalVolume(gas_solid_vol, gas_material, gas_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), gas_logic_vol,
                    gas_name, vessel_logic_vol, false, 0, true);



  //////////////////////////////////////////////////////////

  return gas_logic_vol;
}


G4LogicalVolume* NextTonScale::ConstructInnerShield(G4LogicalVolume* mother_logic_vol)
{
  // INNER SHIELD //////////////////////////////////////////

  G4String ics_name = "INNER_SHIELD";

  G4double ics_diam   = active_diam_ + 2.*fcage_thickn_ + 2.*ics_thickn_;
  G4double ics_length = active_length_ + 2.*ics_thickn_;

  G4Material* ics_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

  G4Tubs* ics_cyl =
    new G4Tubs("INNER_SHIELD_CYL", 0., ics_diam/2., ics_length/2., 0., 360.*deg);
  G4Tubs* ics_void =
    new G4Tubs("INNER_SHIELD_VOID", 0., ics_diam/2. - ics_thickn_,
                                        ics_length/2. - ics_thickn_,
                                        0., 360.*deg);
  G4SubtractionSolid* ics_solid_vol =
    new G4SubtractionSolid(ics_name, ics_cyl, ics_void);

  G4LogicalVolume* ics_logic_vol =
    new G4LogicalVolume(ics_solid_vol, ics_material, ics_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), ics_logic_vol,
                      ics_name, mother_logic_vol, false, 0, true);

  //////////////////////////////////////////////////////////

  return mother_logic_vol;
}


G4LogicalVolume* NextTonScale::ConstructFieldCage(G4LogicalVolume* mother_logic_vol)
{
  // FIELD CAGE ////////////////////////////////////////////

  G4String fcage_name = "FIELD_CAGE";

  G4double fcage_diam   = active_diam_ + 2.*fcage_thickn_;
  G4double fcage_length = active_length_;

  G4Material* fcage_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE");

  G4Tubs* fcage_solid_vol =
    new G4Tubs(fcage_name, 0., fcage_diam/2., fcage_length/2., 0., 360.*deg);

  G4LogicalVolume* fcage_logic_vol =
    new G4LogicalVolume(fcage_solid_vol, fcage_material, fcage_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), fcage_logic_vol,
                    fcage_name, mother_logic_vol, false, 0, true);

  return nullptr;
}


G4ThreeVector NextTonScale::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "ACTIVE") {
    vertex = rnd_->GenerateVertex("INSIDE");
  }
  else if (region == "READOUT_PLANES") {
    vertex = rnd_->GenerateVertex("ENDCAP_SURF");
  }
  else {
    G4cerr << "Unknown detector region " << region << "."
           << "Event generated by default at origin of coordinates." << G4endl;
  }

  return vertex;
}
