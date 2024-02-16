// ----------------------------------------------------------------------------
// nexus | NextTonScale.cc
//
// Geometry of a possible tonne-scale NEXT detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NextTonScale.h"

#include "MaterialsList.h"
#include "IonizationSD.h"
#include "Visibilities.h"
#include "CylinderPointSamplerLegacy.h"
#include "BoxPointSamplerLegacy.h"
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>

using namespace nexus;

REGISTER_CLASS(NextTonScale, GeometryBase)


NextTonScale::NextTonScale():
  GeometryBase(),
  msg_(nullptr),
  gas_("naturalXe"), gas_pressure_(15.*bar), gas_temperature_(300.*kelvin),
  detector_diam_(0.), detector_length_(0.),
  tank_size_(0.), tank_thickn_(1.*cm), water_thickn_(3.*m),
  vessel_thickn_(1.5*mm), ics_thickn_(12.*cm), endcap_hollow_(20.*cm),
  fcage_thickn_(1.*cm),
  active_diam_(300.*cm), active_length_(300.*cm),
  cathode_thickn_(.2*mm), anode_thickn_(1.5*cm), readout_gap_(5.*mm),
  xe_perc_(100.), helium_mass_num_(4),
  tank_vis_(true), vessel_vis_(true), ics_vis_(true),
  fcage_vis_(true), cathode_vis_(true), anode_vis_(true), readout_vis_(true),
  specific_vertex_{},
  active_gen_(nullptr), field_cage_gen_(nullptr), cathode_gen_(nullptr),
  ics_gen_(nullptr), vessel_gen_(nullptr), readout_plane_gen_(nullptr),
  outer_plane_gen_(nullptr), external_gen_(nullptr), muon_gen_(nullptr)
{
  DefineConfigurationParameters();
}


NextTonScale::~NextTonScale()
{
  delete msg_;
  delete muon_gen_;
  delete external_gen_;
  delete active_gen_;
  delete field_cage_gen_;
  delete ics_gen_;
  delete vessel_gen_;
  delete readout_plane_gen_;
  delete outer_plane_gen_;
}


void NextTonScale::Construct()
{
  // We calculate in the following the overall dimensions (external)
  // of the detector and the water tank.
  detector_diam_   = active_diam_ + 2.*fcage_thickn_ + 2.*ics_thickn_ + 2.*vessel_thickn_;
  detector_length_ = active_length_ + 2.*anode_thickn_  + 2.*readout_gap_ +
                     2.*ics_thickn_ + 2.*endcap_hollow_ + 2.*vessel_thickn_;
  tank_size_ = std::max(detector_diam_, detector_length_) +
               2.*water_thickn_ + 2.*tank_thickn_;

  // LABORATORY ////////////////////////////////////////////
  // Volume of air that contains all other detector volumes.

  G4String lab_name = "LABORATORY";
  G4double lab_size = tank_size_ + 4.*m; // Clearance of 2 m around water tank

  G4Material* lab_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  lab_material->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4Box* lab_solid_vol = new G4Box(lab_name, lab_size/2., lab_size/2., lab_size/2.);
  G4LogicalVolume* lab_logic_vol = new G4LogicalVolume(lab_solid_vol, lab_material, lab_name);
  lab_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
  GeometryBase::SetLogicalVolume(lab_logic_vol);

  //////////////////////////////////////////////////////////
  // We construct now the tank and detector volumes.
  // These methods take as input a pointer to the logical volume
  // that contains them, returning a pointer to the logical volume
  // that should contain the volumes positioned within them.

  G4LogicalVolume* mother_logic_vol = lab_logic_vol;

  mother_logic_vol = ConstructWaterTank(mother_logic_vol);
  mother_logic_vol = ConstructVesselAndICS(mother_logic_vol);
  mother_logic_vol = ConstructFieldCageAndReadout(mother_logic_vol);
}


G4LogicalVolume* NextTonScale::ConstructWaterTank(G4LogicalVolume* mother_logic_vol)
{
  // WATER TANK ////////////////////////////////////////////
  // Cylindrical stainless-steel tank of equal height and diameter.

  G4String tank_name = "TANK";
  G4Material* tank_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  tank_material->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4Tubs* tank_solid_vol =
    new G4Tubs(tank_name, 0., tank_size_/2., tank_size_/2., 0., 360.*deg);

  G4LogicalVolume* tank_logic_vol =
    new G4LogicalVolume(tank_solid_vol, tank_material, tank_name);

  if (tank_vis_) tank_logic_vol->SetVisAttributes(nexus::TitaniumGrey());
  else tank_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  G4RotationMatrix* tank_rotation = new G4RotationMatrix();
  tank_rotation->rotateX(90.*deg);
  new G4PVPlacement(tank_rotation, G4ThreeVector(0.,0.,0.), tank_logic_vol,
                    tank_name, mother_logic_vol, false, 0, true);

  // WATER SHIELDING ///////////////////////////////////////
  // Water shielding volume filling the tank defined above.
  // The user chooses the thickness of this shielding layer.

  G4String water_name = "WATER";
  G4double water_size = tank_size_ - 2.*tank_thickn_;
  G4Material* water_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
  water_material->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4Tubs* water_solid_vol =
    new G4Tubs(water_name, 0., water_size/2., water_size/2., 0., 360.*deg);

  G4LogicalVolume* water_logic_vol =
    new G4LogicalVolume(water_solid_vol, water_material, water_name);
  water_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), water_logic_vol,
                    water_name, tank_logic_vol, false, 0, true);

  //////////////////////////////////////////////////////////

  muon_gen_ =
    new BoxPointSamplerLegacy(tank_size_ + 100. * cm, 0.,
                              tank_size_ + 100. * cm, 0.,
                              G4ThreeVector(0., tank_size_/2. + 1. * cm, 0.));

  // Primarily for neutron generation
  external_gen_ = new CylinderPointSamplerLegacy(tank_size_/2. + 1 * cm,
                                                 tank_size_ + 1 * cm,
                                                 1 * mm, 1 * mm,
                                                 G4ThreeVector(0.,0.,0.),
                                                 tank_rotation);

  //////////////////////////////////////////////////////////

  return water_logic_vol;
}


G4LogicalVolume* NextTonScale::ConstructVesselAndICS(G4LogicalVolume* mother_logic_vol)
{
  // PRESSURE VESSEL ///////////////////////////////////////
  // Cylindrical stainless-steel pressure vessel filled with xenon gas.

  G4String vessel_name = "VESSEL";
  G4Material* vessel_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  vessel_material->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4Tubs* vessel_solid_vol =
    new G4Tubs(vessel_name, 0., detector_diam_/2., detector_length_/2., 0., 360.*deg);

  G4LogicalVolume* vessel_logic_vol =
    new G4LogicalVolume(vessel_solid_vol, vessel_material, vessel_name);

  G4RotationMatrix* vessel_rotation = new G4RotationMatrix();
  vessel_rotation->rotateX(-90.*deg);
  new G4PVPlacement(vessel_rotation, G4ThreeVector(0.,0.,0.), vessel_logic_vol,
                    vessel_name, mother_logic_vol, false, 0, true);

  if (vessel_vis_) vessel_logic_vol->SetVisAttributes(nexus::TitaniumGrey());
  else vessel_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  // XENON GAS /////////////////////////////////////////////
  // Xenon gas mixture filling the vessel defined above.
  // The user chooses the mixture and its pressure via configuration parameters.

  G4String gas_name = "GAS";
  G4double gas_diam = detector_diam_ - 2.*vessel_thickn_;
  G4double gas_length = detector_length_ - 2.*vessel_thickn_;

  DefineGas();

  G4Tubs* gas_solid_vol =
    new G4Tubs(gas_name, 0., gas_diam/2., gas_length/2., 0., 360.*deg);

  G4LogicalVolume* gas_logic_vol =
    new G4LogicalVolume(gas_solid_vol, xenon_gas_, gas_name);
  gas_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), gas_logic_vol,
                    gas_name, vessel_logic_vol, false, 0, true);

  vessel_gen_ =
    new CylinderPointSamplerLegacy(gas_diam/2., gas_length, vessel_thickn_, vessel_thickn_);

  // INNER COPPER SHIELDING ////////////////////////////////
  // Copper cylinder that shields the active volume of the detector
  // from external radiation originating in the vessel.
  // Longitudinally, the ICS is not as long as the pressure vessel,
  // leaving empty volumes (of depth 'endcap_hollow_') behind the plates
  // that support the readout planes.

  G4String ics_name = "ICS";
  G4double ics_diam = gas_diam;
  G4double ics_length = gas_length - 2.*endcap_hollow_;
  G4Material* ics_material = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
  ics_material->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4Tubs* ics_full_solid_vol =
    new G4Tubs("ICS_FULL", 0., ics_diam/2., ics_length/2., 0., 360.*deg);
  G4Tubs* ics_void_solid_vol =
    new G4Tubs("ICS_VOID", 0., (ics_diam-2.*ics_thickn_)/2.,
               (ics_length-2.*ics_thickn_)/2., 0., 360.*deg);
  G4SubtractionSolid* ics_solid_vol =
    new G4SubtractionSolid(ics_name, ics_full_solid_vol, ics_void_solid_vol);

  G4LogicalVolume* ics_logic_vol =
    new G4LogicalVolume(ics_solid_vol, ics_material, ics_name);

  // Setting ics visibility
  if (ics_vis_) ics_logic_vol->SetVisAttributes(nexus::CopperBrown());
  else ics_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), ics_logic_vol,
                    ics_name, gas_logic_vol, false, 0, true);


  // Vertex generator for ICS
  ics_gen_ = new CylinderPointSamplerLegacy(ics_diam/2. - ics_thickn_,
                                      ics_length - 2*ics_thickn_,
                                      ics_thickn_, ics_thickn_);

  // The outer_readout vertex generator is a vertex generator from
  // a virtual volume placed at Z next to the outer plane of the ICS endcap.
  // It is used to generate backgound associated with the electronics of readout planes.
  G4double outer_readout_thickness = 0.1 * mm;
  outer_plane_gen_ = new CylinderPointSamplerLegacy(0., outer_readout_thickness,
    ics_diam/2., 0., G4ThreeVector(0., 0., (ics_length/2.+outer_readout_thickness/2.)));

  //////////////////////////////////////////////////////////

  return gas_logic_vol;
}


G4LogicalVolume* NextTonScale::ConstructFieldCageAndReadout(G4LogicalVolume* mother_logic_vol)
{
  // FIELD CAGE ////////////////////////////////////////////
  // Simplified representation of the field cage as a polyethylene
  // cylindrical shell.

  G4String fcage_name = "FIELD_CAGE";
  G4double fcage_diam  = active_diam_ + 2.*fcage_thickn_;
  G4double fcage_length = active_length_;
  G4Material* fcage_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE");
  fcage_material->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4Tubs* fcage_solid_vol = new G4Tubs(fcage_name, active_diam_/2., fcage_diam/2.,
                                       fcage_length/2., 0., 360.*deg);

  G4LogicalVolume* fcage_logic_vol =
    new G4LogicalVolume(fcage_solid_vol, fcage_material, fcage_name);

  if (fcage_vis_) fcage_logic_vol->SetVisAttributes(nexus::LightBlue());
  else fcage_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), fcage_logic_vol,
                    fcage_name, mother_logic_vol, false, 0, true);

  // ACTIVE ////////////////////////////////////////////////
  // Active volume of the detector.

  G4String active_name = "ACTIVE";

  G4Tubs* active_solid_vol =
    new G4Tubs(active_name, 0., active_diam_/2., active_length_/2., 0., 360.*deg);

  G4LogicalVolume* active_logic_vol =
    new G4LogicalVolume(active_solid_vol, xenon_gas_, active_name);

  active_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  // Limit the step size in this volume for better tracking precision
  active_logic_vol->SetUserLimits(new G4UserLimits(1.*mm));
  // Set the volume as an ionization sensitive detector
  IonizationSD* active_sd = new IonizationSD("/TON_SCALE/ACTIVE");
  active_logic_vol->SetSensitiveDetector(active_sd);
  G4SDManager::GetSDMpointer()->AddNewDetector(active_sd);

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), active_logic_vol,
                    active_name, mother_logic_vol, false, 0, true);

  // CATHODE ///////////////////////////////////////////////

  G4String cathode_name = "CATHODE";
  G4double cathode_diam = active_diam_;

  G4Material* cathode_mat =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  cathode_mat->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4Tubs* cathode_solid_vol =
    new G4Tubs(cathode_name, 0., cathode_diam/2., cathode_thickn_/2., 0., 360.*deg);

  G4LogicalVolume* cathode_logic_vol =
    new G4LogicalVolume(cathode_solid_vol, cathode_mat, cathode_name);

  if (cathode_vis_) cathode_logic_vol->SetVisAttributes(nexus::CopperBrown());
  else cathode_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), cathode_logic_vol,
                    cathode_name, active_logic_vol, false, 0, true);

  // ANODES ////////////////////////////////////////////////

  G4String anode_name = "ANODE";
  G4double anode_diam = active_diam_;
  G4double anode_posZ = active_length_/2. + anode_thickn_/2.;

  G4Tubs* anode_solid_vol =
    new G4Tubs(anode_name, 0., anode_diam/2., anode_thickn_/2., 0., 360.*deg);

  G4LogicalVolume* anode_logic_vol =
    new G4LogicalVolume(anode_solid_vol, xenon_gas_, anode_name);

  if (anode_vis_) anode_logic_vol->SetVisAttributes(nexus::CopperBrown());
  else anode_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,anode_posZ), anode_logic_vol,
                    anode_name, mother_logic_vol, false, 0, true);
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,-anode_posZ), anode_logic_vol,
                    anode_name, mother_logic_vol, false, 1, true);

  // READOUT PLANES ////////////////////////////////////////
  // The readout planes are two thin (250 microns) volumes made of kapton
  // placed one 'readout_gap_' length away from the anode

  G4String readout_name = "READOUT_PLANE";
  G4double readout_diam = active_diam_;
  G4double readout_thickn = 0.25*mm;
  G4double readout_posZ =
    active_length_/2. + anode_thickn_ + readout_gap_ - readout_thickn/2.;

  G4Material* readout_material =
    G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
  readout_material->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  G4Tubs* readout_solid_vol =
    new G4Tubs(readout_name, 0., readout_diam/2., readout_thickn/2., 0., 360.*deg);

  G4LogicalVolume* readout_logic_vol =
    new G4LogicalVolume(readout_solid_vol, readout_material, readout_name);

  if (readout_vis_) readout_logic_vol->SetVisAttributes(nexus::CopperBrown());
  else readout_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,readout_posZ), readout_logic_vol,
                    readout_name, mother_logic_vol, false, 0, true);
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,-readout_posZ), readout_logic_vol,
                    readout_name, mother_logic_vol, false, 1, true);

  // VERTEX GENERATORS /////////////////////////////////////

  field_cage_gen_ =
    new CylinderPointSamplerLegacy(active_diam_/2., fcage_length, fcage_thickn_, 0.);

  active_gen_ =
    new CylinderPointSamplerLegacy(0., active_length_, active_diam_/2., 0.);

  readout_plane_gen_ =
    new CylinderPointSamplerLegacy(0., readout_thickn, readout_diam/2., 0.,
                                   G4ThreeVector(0.,0.,readout_posZ));

  cathode_gen_ =
    new CylinderPointSamplerLegacy(0., cathode_thickn_, cathode_diam/2., cathode_thickn_/2.);

  //////////////////////////////////////////////////////////

  return nullptr;
}


G4ThreeVector NextTonScale::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "AD_HOC") {
    vertex = specific_vertex_;
  }
  else if (region == "ACTIVE") {
    vertex = active_gen_->GenerateVertex("BODY_VOL");
  }
  else if (region == "FIELD_CAGE") {
    vertex = field_cage_gen_->GenerateVertex("BODY_VOL");
  }
  else if (region == "CATHODE") {
    vertex = cathode_gen_->GenerateVertex("ENDCAP_VOL");
  }
  else if (region == "READOUT_PLANE") {
    vertex = readout_plane_gen_->GenerateVertex("BODY_VOL");
  }
  else if (region == "INNER_SHIELDING") {
    vertex = ics_gen_->GenerateVertex("WHOLE_VOL");
  }
  else if (region == "OUTER_PLANE") {
    vertex = outer_plane_gen_->GenerateVertex("BODY_VOL");
  }
  else if (region == "VESSEL") {
    vertex = vessel_gen_->GenerateVertex("WHOLE_VOL");
  }
  else if (region == "MUONS") {
    vertex = muon_gen_->GenerateVertex("INSIDE");
  }
  else if (region == "EXTERNAL") {
    vertex = external_gen_->GenerateVertex("WHOLE_VOL");
  }
  else {
    G4cerr << "Unknown detector region " << region << "."
           << "Event generated by default at origin of coordinates." << G4endl;
  }

  return vertex;
}


void NextTonScale::DefineGas()
{
  if (gas_ == "naturalXe")
    xenon_gas_ = materials::GXe(gas_pressure_, gas_temperature_);
  else if (gas_ == "enrichedXe")
    xenon_gas_ = materials::GXeEnriched(gas_pressure_, gas_temperature_);
  else if (gas_ == "depletedXe")
    xenon_gas_ = materials::GXeDepleted(gas_pressure_, gas_temperature_);
  else if (gas_ == "XeHe")
    xenon_gas_ = materials::GXeHe(gas_pressure_, gas_temperature_,
				      xe_perc_, helium_mass_num_);
  else
    G4Exception("[NextTonScale]", "DefineGas()", FatalException,
    "Unknown xenon gas type. Valid options are naturalXe, enrichedXe or depletedXe.");
}


void NextTonScale::DefineConfigurationParameters()
{
  msg_ = new G4GenericMessenger(this, "/Geometry/NextTonScale/",
                                "Control commands of the NextTonScale geometry.");

  msg_->DeclareProperty("gas", gas_, "Xenon gas type.");

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
  msg_->DeclarePropertyWithUnit("specific_vertex", "mm",  specific_vertex_,
    "Set generation vertex.");

  // Percentage xenon for mixtures /////////////////////////
  G4GenericMessenger::Command& Xe_percent_cmd =
    msg_->DeclareProperty("XePercentage", xe_perc_,
                          "Percent of the gas that is Xenon.");
  Xe_percent_cmd.SetParameterName("xe_perc", false);
  Xe_percent_cmd.SetRange("xe_perc<=100.");

  G4GenericMessenger::Command& type_helium_cmd =
    msg_->DeclareProperty("helium_A", helium_mass_num_,
			  "Mass number for helium used, 3 or 4");
  type_helium_cmd.SetParameterName("helium_A", false);


  // Geometry visibilities /////////////////////////////////

  G4GenericMessenger::Command& tank_vis_cmd =
    msg_->DeclareProperty("tank_visibility", tank_vis_, "Water Tank Visibility");
  tank_vis_cmd.SetParameterName("tank_visibility", true);

  G4GenericMessenger::Command& vessel_vis_cmd =
    msg_->DeclareProperty("vessel_visibility", vessel_vis_, "Vessel Visibility");
  vessel_vis_cmd.SetParameterName("vessel_visibility", true);

  G4GenericMessenger::Command& ics_vis_cmd =
    msg_->DeclareProperty("ics_visibility", ics_vis_, "ICS Visibility");
  ics_vis_cmd.SetParameterName("ics_visibility", true);

  G4GenericMessenger::Command& fcage_vis_cmd =
    msg_->DeclareProperty("fcage_visibility", fcage_vis_, "Field Cage Visibility");
  fcage_vis_cmd.SetParameterName("fcage_visibility", false);

  G4GenericMessenger::Command& cathode_vis_cmd =
    msg_->DeclareProperty("cathode_visibility", cathode_vis_, "Cathode Visibility");
  cathode_vis_cmd.SetParameterName("cathode_visibility", false);

  G4GenericMessenger::Command& anode_vis_cmd =
    msg_->DeclareProperty("anode_visibility", anode_vis_, "Anode Visibility");
  anode_vis_cmd.SetParameterName("anode_visibility", false);

  G4GenericMessenger::Command& readout_vis_cmd =
    msg_->DeclareProperty("readout_visibility", readout_vis_, "Readout Visibility");
  readout_vis_cmd.SetParameterName("readout_visibility", false);
}
