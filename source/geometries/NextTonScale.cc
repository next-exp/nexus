// -----------------------------------------------------------------------------
// File   : NextTonScale.cc
// Author : Justo Martin-Albo
// Date   : January 2019
// -----------------------------------------------------------------------------

#include "NextTonScale.h"
#include "MaterialsList.h"
#include "Visibilities.h"

#include "CylinderPointSampler.h"
#include "MuonsPointSampler.h"
#include "IonizationSD.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4NistManager.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>

using namespace nexus;


NextTonScale::NextTonScale():
  BaseGeometry(),
  msg_(nullptr),
  gas_("naturalXe"),
  gas_density_(88.*kg/m3),
  active_diam_(300.*cm), active_length_(300.*cm),
  fcage_thickn_(1.*cm), ics_thickn_(12.*cm), vessel_thickn_(2.*cm),
  endcap_hollow_(20.*cm),
  water_thickn_(3.*m),
  tank_visibility_(0),
  vessel_visibility_(1),
  ics_visibility_(1),
  fcage_visibility_(1)
{
  msg_ = new G4GenericMessenger(this, "/Geometry/NextTonScale/",
                                "Control commands of the NextTonScale geometry.");

  msg_->DeclareProperty("gas", gas_, "Gas being used");

  G4GenericMessenger::Command& gas_density_cmd =
    msg_->DeclareProperty("gas_density", gas_density_,
                          "Density of the xenon gas.");
  gas_density_cmd.SetUnitCategory("Volumic Mass");
  gas_density_cmd.SetParameterName("gas_density", false);
  gas_density_cmd.SetRange("gas_density>=0.");

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

  G4GenericMessenger::Command& vessel_thickn_cmd =
    msg_->DeclareProperty("vessel_thickn", vessel_thickn_,
                          "Thickness of the vessel.");
  vessel_thickn_cmd.SetUnitCategory("Length");
  vessel_thickn_cmd.SetParameterName("vessel_thickn", false);
  vessel_thickn_cmd.SetRange("vessel_thickn>=0.");

  G4GenericMessenger::Command& water_thickn_cmd =
    msg_->DeclareProperty("water_thickn", water_thickn_,
                          "Thickness of the water shield.");
  water_thickn_cmd.SetUnitCategory("Length");
  water_thickn_cmd.SetParameterName("water_thickn", false);
  water_thickn_cmd.SetRange("water_thickn>=0.");

  // Specific vertex in case region to shoot from is AD_HOC
  G4GenericMessenger::Command& specific_vertex_X_cmd =
    msg_->DeclareProperty("specific_vertex_X", specific_vertex_X_,
      "If region is AD_HOC, x coord where particles are generated");
  specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
  specific_vertex_X_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command& specific_vertex_Y_cmd =
    msg_->DeclareProperty("specific_vertex_Y", specific_vertex_Y_,
      "If region is AD_HOC, y coord where particles are generated");
  specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
  specific_vertex_Y_cmd.SetUnitCategory("Length");

  G4GenericMessenger::Command& specific_vertex_Z_cmd =
    msg_->DeclareProperty("specific_vertex_Z", specific_vertex_Z_,
      "If region is AD_HOC, z coord where particles are generated");
  specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
  specific_vertex_Z_cmd.SetUnitCategory("Length");

  // Geometry Visibilities
  G4GenericMessenger::Command& tank_visibility_cmd =
    msg_->DeclareProperty("tank_visibility", tank_visibility_, "Water Tank Visibility");
  tank_visibility_cmd.SetParameterName("tank_visibility", true);

  G4GenericMessenger::Command& vessel_visibility_cmd =
    msg_->DeclareProperty("vessel_visibility", vessel_visibility_, "Vessel Visibility");
  vessel_visibility_cmd.SetParameterName("vessel_visibility", true);

  G4GenericMessenger::Command& ics_visibility_cmd =
    msg_->DeclareProperty("ics_visibility", ics_visibility_, "ICS Visibility");
  ics_visibility_cmd.SetParameterName("ics_visibility", true);

  G4GenericMessenger::Command& fcage_visibility_cmd =
    msg_->DeclareProperty("fcage_visibility", fcage_visibility_, "Field Cage Visibility");
  fcage_visibility_cmd.SetParameterName("fcage_visibility", false);
}


NextTonScale::~NextTonScale()
{
  delete msg_;
  delete active_gen_;
  delete field_cage_gen_;
  delete ics_gen_;
  delete vessel_gen_;
  delete readout_plane_gen_;
  delete outer_plane_gen_;
}


void NextTonScale::Construct()
{
  // Materials definition /////////////////////////////////
  //xenon_gas_ = MaterialsList::GXe(15. * bar, 300. * kelvin);
  if (gas_ == "naturalXe"){
    xenon_gas_ = MaterialsList::GXe_bydensity(gas_density_);
  // xenon_gas_ = new G4Material("denseXe", gas_density_, 1, kStateGas);
  // G4Element* Xe = G4NistManager::Instance()->FindOrBuildElement("Xe");
  // xenon_gas_->AddElement(Xe,1);
  } else if (gas_ == "enrichedXe"){
    xenon_gas_ = MaterialsList::GXeEnriched_bydensity(gas_density_);
  } else if (gas_ == "depletedXe"){
    xenon_gas_ = MaterialsList::GXeDepleted_bydensity(gas_density_);
  } else {
    G4Exception("[NextTonScale]", "Construct()", FatalException,
		"Unknown kind of gas, valid options are:naturalXe, enrichedXe, depletedXe");
  }

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

  G4Tubs* tank_solid_vol = new G4Tubs(tank_name, 0., tank_diam/2.,
                                      tank_height/2., 0., 360.*deg);

  G4LogicalVolume* tank_logic_vol =
    new G4LogicalVolume(tank_solid_vol, tank_material, tank_name);

  // Setting tank visibility
  if (tank_visibility_)
    tank_logic_vol->SetVisAttributes(nexus::DarkGrey());
  else
    tank_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateX(90.*deg);

  new G4PVPlacement(rotation, G4ThreeVector(0.,0.,0.), tank_logic_vol,
                    tank_name, mother_logic_vol, false, 0, true);

  muon_gen_ = new MuonsPointSampler(tank_diam/2. + 50. * cm,
				    tank_height/2. + 1. * cm,
				    tank_diam/2. + 50. * cm);


  // WATER /////////////////////////////////////////////////
  G4String water_name = "WATER";

  G4double water_diam   = tank_diam   - 2.*tank_thickn;
  G4double water_height = tank_height - 2.*tank_thickn;

  G4Material* water_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");

  G4Tubs* water_solid_vol = new G4Tubs(water_name, 0., water_diam/2.,
                                       water_height/2., 0., 360.*deg);

  G4LogicalVolume* water_logic_vol =
    new G4LogicalVolume(water_solid_vol, water_material, water_name);
  water_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

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

  G4Tubs* vessel_solid_vol = new G4Tubs(vessel_name, 0., vessel_diam/2.,
                                        vessel_length/2., 0., 360.*deg);

  G4LogicalVolume* vessel_logic_vol =
    new G4LogicalVolume(vessel_solid_vol, vessel_material, vessel_name);

  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateX(90.*deg);

  // Setting vessel visibility
  if (vessel_visibility_)
    vessel_logic_vol->SetVisAttributes(nexus::TitaniumGrey());
  else
    vessel_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  new G4PVPlacement(rotation, G4ThreeVector(0.,0.,0.), vessel_logic_vol,
                    vessel_name, mother_logic_vol, false, 0, true);


  // GAS ///////////////////////////////////////////////////
  G4String gas_name = "GAS";

  G4double gas_diam   = vessel_diam   - 2.*vessel_thickn_;
  G4double gas_length = vessel_length - 2.*vessel_thickn_;

  G4Tubs* gas_solid_vol =  new G4Tubs(gas_name, 0., gas_diam/2.,
                                      gas_length/2., 0., 360.*deg);

  G4LogicalVolume* gas_logic_vol =
    new G4LogicalVolume(gas_solid_vol, xenon_gas_, gas_name);

  gas_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), gas_logic_vol,
                    gas_name, vessel_logic_vol, false, 0, true);


  // VERTEX GENERATORS /////////////////////////////////////
  vessel_gen_ = new CylinderPointSampler(gas_diam/2., gas_length,
                                         vessel_thickn_, vessel_thickn_);

  //////////////////////////////////////////////////////////
  return gas_logic_vol;
}


G4LogicalVolume* NextTonScale::ConstructInnerShield(G4LogicalVolume* mother_logic_vol)
{
  // INNER SHIELD //////////////////////////////////////////
  G4String ics_name = "INNER_SHIELD";

  G4double ics_diam   = active_diam_   + 2.*fcage_thickn_ + 2.*ics_thickn_;
  G4double ics_length = active_length_ + 2.*ics_thickn_;

  G4Material* ics_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

  G4Tubs* ics_solid_vol =  new G4Tubs(ics_name, 0., ics_diam/2.,
                                      ics_length/2., 0., 360.*deg);

  G4LogicalVolume* ics_logic_vol =
    new G4LogicalVolume(ics_solid_vol, ics_material, ics_name);

  // Setting ics visibility
  if (ics_visibility_)
    ics_logic_vol->SetVisAttributes(nexus::CopperBrown());
  else
    ics_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), ics_logic_vol,
                    ics_name, mother_logic_vol, false, 0, true);

  // VERTEX GENERATORS //////////////////////////////////////
  ics_gen_ = new CylinderPointSampler(ics_diam/2. - ics_thickn_,
                                      ics_length - 2*ics_thickn_,
                                      ics_thickn_, ics_thickn_);

  // The outer_readout vertex generator is a vertex generator from
  // a virtual volume placed at -Z next to the outer plane of the ICS endcap
  // It is used to generate backgound associated with the electronics of readout planes.
  G4double outer_readout_thickness = 0.1 * mm;
  outer_plane_gen_ = new CylinderPointSampler(0., outer_readout_thickness,
     ics_diam/2., 0., G4ThreeVector(0., 0., -(ics_length/2.+outer_readout_thickness/2.)));

  //////////////////////////////////////////////////////////
  //return mother_logic_vol;
  return ics_logic_vol;
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
    new G4Tubs(fcage_name, active_diam_/2., fcage_diam/2., fcage_length/2.,
               0., 360.*deg);

  G4LogicalVolume* fcage_logic_vol =
    new G4LogicalVolume(fcage_solid_vol, fcage_material, fcage_name);

  // Setting field cage visibility
  if (fcage_visibility_)
    fcage_logic_vol->SetVisAttributes(nexus::LightBlue());
  else
    fcage_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), fcage_logic_vol,
                    fcage_name, mother_logic_vol, false, 0, true);


  // ACTIVE ////////////////////////////////////////////////
  G4String active_name = "ACTIVE";

  G4Tubs* active_solid_vol =
    new G4Tubs(active_name, 0., active_diam_/2., active_length_/2., 0., 360.*deg);

  G4LogicalVolume* active_logic_vol =
    new G4LogicalVolume(active_solid_vol, xenon_gas_, active_name);

  active_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);

  // Limit the step size in this volume for better tracking precision
  active_logic_vol->SetUserLimits(new G4UserLimits(1.*mm));
  // Set the volume as an ionization sensitive detector
  IonizationSD* ionisd = new IonizationSD("/NEXTNEW/ACTIVE");
  active_logic_vol->SetSensitiveDetector(ionisd);
  G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), active_logic_vol,
                    active_name, mother_logic_vol, false, 0, true);


  // VERTEX GENERATORS //////////////////////////////////////
  field_cage_gen_ = new CylinderPointSampler(active_diam_/2., active_length_,
                                             fcage_thickn_, 0.);

  active_gen_ = new CylinderPointSampler(0., active_length_,
                                         active_diam_/2., 0.);

  // The inner_readout vertex generator is a vertex generator from
  // a virtual volume placed at -Z next to the inner plane of the ICS endcap
  // It is used to generate backgound associated with the readout planes.
  G4double inner_readout_thickness = 0.1 * mm;
  readout_plane_gen_ =
    new CylinderPointSampler(0., inner_readout_thickness, active_diam_/2., 0.,
      G4ThreeVector(0., 0., -(active_length_/2.-inner_readout_thickness/2.)));

  //////////////////////////////////////////////////////////
  return nullptr;
}


G4ThreeVector NextTonScale::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "AD_HOC") {
    vertex = G4ThreeVector(specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_);
  }
  else if (region == "ACTIVE") {
    vertex = active_gen_->GenerateVertex("BODY_VOL");
  }
  else if (region == "FIELD_CAGE") {
    vertex = field_cage_gen_->GenerateVertex("BODY_VOL");
  }
  else if (region == "READOUT_PLANES") {
    vertex = readout_plane_gen_->GenerateVertex("BODY_VOL");
  }
  else if (region == "INNER_SHIELDING") {
    vertex = ics_gen_->GenerateVertex("WHOLE_VOL");
  }
  else if (region == "OUTER_PLANES") {
    vertex = outer_plane_gen_->GenerateVertex("BODY_VOL");
  }
  else if (region == "VESSEL") {
    vertex = vessel_gen_->GenerateVertex("WHOLE_VOL");
  }
  else if (region == "MUONS") {
    muon_gen_->GenerateVertex();
  }
  else {
    G4cerr << "Unknown detector region " << region << "."
           << "Event generated by default at origin of coordinates." << G4endl;
  }

  return vertex;
}
