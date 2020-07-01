// -----------------------------------------------------------------------------
//  nexus | NextFlexTrackingPlane.cc
//
//  NEXT-Flex Tracking Plane geometry for performance studies.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "NextFlexTrackingPlane.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "XenonGasProperties.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "CylinderPointSampler2020.h"
#include "GenericPhotosensor.h"
#include "PmtSD.h"
#include "Visibilities.h"

#include <G4UnitsTable.hh>
#include <G4GenericMessenger.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4TransportationManager.hh>
#include <G4RotationMatrix.hh>

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
#include <Randomize.hh>


using namespace nexus;


NextFlexTrackingPlane::NextFlexTrackingPlane():
  BaseGeometry(),
  mother_logic_      (nullptr),
  verbosity_         (false),
  visibility_        (false),
  msg_               (nullptr),
  wls_mat_name_       ("TPB"),
  SiPM_ANODE_dist_   (10.  * mm),   // Distance from ANODE to SiPM surface
  SiPM_size_x_       ( 1.3 * mm),   // Size X (width) of SiPMs
  SiPM_size_y_       ( 1.3 * mm),   // Size Y (height) of SiPMs
  SiPM_pitch_x_      (15.6 * mm),   // SiPMs pitch X
  SiPM_pitch_y_      (15.6 * mm),   // SiPMs pitch Y
  SiPM_binning_      ( 1.  * us),   // SiPMs time binning size
  copper_thickness_  (12.  * cm),   // Thickness of the copper plate
  teflon_thickness_  ( 5.  * mm),   // Thickness of the teflon mask
  teflon_hole_diam_  ( 7.  * mm)    // Diameter of teflon mask holes
{

  // Messenger
  msg_ = new G4GenericMessenger(this, "/Geometry/NextFlex/",
                                "Control commands of the NextFlex geometry.");

  // Parametrized dimensions
  DefineConfigurationParameters();

  // Hard-wired dimensions & components
  SiPM_case_thickness_ = 2. * mm;
  wls_thickness_       = 1. * um;

  // The SiPM
  SiPM_ = new GenericPhotosensor("TP_SiPM", SiPM_size_x_, SiPM_size_y_, 
                                 SiPM_case_thickness_);

  // Initializing the geometry navigator (used in vertex generation)
  geom_navigator_ =
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
}



NextFlexTrackingPlane::~NextFlexTrackingPlane()
{
  delete msg_;
  delete copper_gen_;
  delete SiPM_;
}



void NextFlexTrackingPlane::DefineConfigurationParameters()
{
  // Verbosity
  msg_->DeclareProperty("tp_verbosity", verbosity_, "Verbosity");

  // Visibility
  msg_->DeclareProperty("tp_visibility", visibility_, "TRACKING_PLANE Visibility");

  // Copper dimensions
  G4GenericMessenger::Command& copper_thickness_cmd =
    msg_->DeclareProperty("tp_copper_thickness", copper_thickness_,
                          "Thickness of the TP Copper plate.");
  copper_thickness_cmd.SetParameterName("tp_copper_thickness", false);
  copper_thickness_cmd.SetUnitCategory("Length");
  copper_thickness_cmd.SetRange("tp_copper_thickness>=0.");

  // Teflon dimensions
  G4GenericMessenger::Command& teflon_thickness_cmd =
    msg_->DeclareProperty("tp_teflon_thickness", teflon_thickness_,
                          "Thickness of the TP teflon mask.");
  teflon_thickness_cmd.SetParameterName("tp_teflon_thickness", false);
  teflon_thickness_cmd.SetUnitCategory("Length");
  teflon_thickness_cmd.SetRange("tp_teflon_thickness>=0.");

  G4GenericMessenger::Command& teflon_hole_diam_cmd =
    msg_->DeclareProperty("tp_teflon_hole_diam", teflon_hole_diam_,
                          "Diameter of the TP teflon mask holes.");
  teflon_hole_diam_cmd.SetParameterName("tp_teflon_hole_diam", false);
  teflon_hole_diam_cmd.SetUnitCategory("Length");
  teflon_hole_diam_cmd.SetRange("tp_teflon_hole_diam>=0.");

  // UV shifting material
  msg_->DeclareProperty("tp_wls_mat", wls_mat_name_,
                        "TP UV wavelength shifting material name");

  // SiPMs
  G4GenericMessenger::Command& sipm_anode_dist_cmd =
    msg_->DeclareProperty("tp_sipm_anode_dist", SiPM_ANODE_dist_,
                          "Distance from tracking SiPMs to ANODE.");
  sipm_anode_dist_cmd.SetParameterName("tp_sipm_anode_dist", false);
  sipm_anode_dist_cmd.SetUnitCategory("Length");
  sipm_anode_dist_cmd.SetRange("tp_sipm_anode_dist>=0.");

  G4GenericMessenger::Command& sipm_sizeX_cmd =
    msg_->DeclareProperty("tp_sipm_sizeX", SiPM_size_x_,
                          "SizeX of tracking SiPMs.");
  sipm_sizeX_cmd.SetParameterName("tp_sipm_sizeX", false);
  sipm_sizeX_cmd.SetUnitCategory("Length");
  sipm_sizeX_cmd.SetRange("tp_sipm_sizeX>0.");

  G4GenericMessenger::Command& sipm_sizeY_cmd =
    msg_->DeclareProperty("tp_sipm_sizeY", SiPM_size_y_,
                          "SizeY of tracking SiPMs.");
  sipm_sizeY_cmd.SetParameterName("tp_sipm_sizeY", false);
  sipm_sizeY_cmd.SetUnitCategory("Length");
  sipm_sizeY_cmd.SetRange("tp_sipm_sizeY>0.");

  G4GenericMessenger::Command& sipm_pitchX_cmd =
    msg_->DeclareProperty("tp_sipm_pitchX", SiPM_pitch_x_,
                          "PitchX of tracking SiPMs.");
  sipm_pitchX_cmd.SetParameterName("tp_sipm_pitchX", false);
  sipm_pitchX_cmd.SetUnitCategory("Length");
  sipm_pitchX_cmd.SetRange("tp_sipm_pitchX>0.");

  G4GenericMessenger::Command& sipm_pitchY_cmd =
    msg_->DeclareProperty("tp_sipm_pitchY", SiPM_pitch_y_,
                          "PitchY of tracking SiPMs.");
  sipm_pitchY_cmd.SetParameterName("tp_sipm_pitchY", false);
  sipm_pitchY_cmd.SetUnitCategory("Length");
  sipm_pitchY_cmd.SetRange("tp_sipm_pitchY>0.");

  G4GenericMessenger::Command& sipm_binning_cmd =
    msg_->DeclareProperty("tp_sipm_time_binning", SiPM_binning_,
                          "Time bin size of SiPMs.");
  sipm_binning_cmd.SetParameterName("tp_sipm_time_binning", false);
  sipm_binning_cmd.SetUnitCategory("Time");
  sipm_binning_cmd.SetRange("tp_sipm_time_binning>=0.");
}



void NextFlexTrackingPlane::ComputeDimensions()
{
  teflon_iniZ_ = origin_z_ - SiPM_ANODE_dist_ - SiPM_case_thickness_;
  copper_iniZ_ = teflon_iniZ_ - copper_thickness_;

  // Generate SiPM positions
  GenerateSiPMpositions();
}



void NextFlexTrackingPlane::DefineMaterials()
{
  // Xenon
  xenon_gas_  = mother_logic_->GetMaterial();

  // Copper
  copper_mat_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

  // Teflon
  teflon_mat_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");

  // UV shifting material
  if (wls_mat_name_ == "NONE") {
    wls_mat_ = mother_logic_->GetMaterial();
  }
  else if (wls_mat_name_ == "TPB") {
    wls_mat_ = MaterialsList::TPB();
    wls_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
  }
  else if (wls_mat_name_ == "TPH") {
    wls_mat_ = MaterialsList::TPH();
    wls_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::TPH());
  }
  else {
    G4Exception("[NextFlex]", "EnergyPlane::DefineMaterials()", FatalException,
                "Unknown UV shifting material. Valid options are NONE, TPB or TPH.");
  }
}



void NextFlexTrackingPlane::Construct()
{
  // Make sure that the pointer to the mother volume is actually defined
  if (!mother_logic_)
    G4Exception("[NextFlexTrackingPlane]", "Construct()",
                FatalException, "Mother volume is a nullptr.");

  // Verbosity
  if(verbosity_) {
    G4cout << G4endl << "*** NEXT-Flex Tracking Plane ..." << G4endl;
  }

  // Getting volumes dimensions based on parameters.
  ComputeDimensions();

  // Define materials.
  DefineMaterials();

  // Copper
  BuildCopper();

  // Teflon
  BuildTeflon();
}



void NextFlexTrackingPlane::BuildCopper()
{
  G4String copper_name = "TP_COPPER";

  G4double copper_posZ = copper_iniZ_ + copper_thickness_/2.;

  G4Tubs* copper_solid =
    new G4Tubs(copper_name, 0., diameter_/2., copper_thickness_/2., 0, twopi);

  G4LogicalVolume* copper_logic =
    new G4LogicalVolume(copper_solid, copper_mat_, copper_name);

  G4VPhysicalVolume* copper_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., copper_posZ), copper_logic,
                      copper_name, mother_logic_, false, 0, verbosity_);

  // Visibility
  if (visibility_) copper_logic->SetVisAttributes(nexus::CopperBrown());
  else             copper_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Vertex generator
  copper_gen_ = new CylinderPointSampler2020(copper_phys);

  // Verbosity
  if (verbosity_) {
    G4cout << "* TP Copper Z positions: " << copper_iniZ_
           << " to " << copper_iniZ_ + copper_thickness_ << G4endl;
  }
}



void NextFlexTrackingPlane::BuildTeflon()
{
  /// The TEFLON ///
  G4String teflon_name = "TP_TEFLON";

  G4double teflon_posZ = teflon_iniZ_ + teflon_thickness_/2.;

  G4Tubs* teflon_solid =
    new G4Tubs(teflon_name, 0., diameter_/2., teflon_thickness_/2., 0, twopi);

  G4LogicalVolume* teflon_logic =
    new G4LogicalVolume(teflon_solid, teflon_mat_, teflon_name);

  // Adding the teflon optical surface
  G4OpticalSurface* teflon_optSurf = 
    new G4OpticalSurface(teflon_name, unified, ground, dielectric_metal);
  teflon_optSurf->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());

  new G4LogicalSkinSurface(teflon_name, teflon_logic, teflon_optSurf);


  /// The UV WLS in TEFLON ///
  G4String teflon_wls_name = "TP_TEFLON_WLS";

  G4double teflon_wls_posZ = teflon_thickness_/2. - wls_thickness_/2.;

  G4Tubs* teflon_wls_solid =
    new G4Tubs(teflon_wls_name, 0., diameter_/2., wls_thickness_/2., 0, twopi);

  G4LogicalVolume* teflon_wls_logic =
    new G4LogicalVolume(teflon_wls_solid, wls_mat_, teflon_wls_name);

  G4VPhysicalVolume* teflon_wls_phys =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., teflon_wls_posZ), teflon_wls_logic,
                      teflon_wls_name, teflon_logic, false, 0, verbosity_);

  // Adding the WLS optical surface
  G4OpticalSurface* teflon_wls_optSurf =
    new G4OpticalSurface("TEFLON_WLS_OPSURF", glisur, ground,
                         dielectric_dielectric, .01);

  new G4LogicalBorderSurface("TEFLON_WLS_GAS_OPSURF", teflon_wls_phys,
                             neigh_gas_phys_, teflon_wls_optSurf);
  new G4LogicalBorderSurface("GAS_TEFLON_WLS_OPSURF", neigh_gas_phys_,
                             teflon_wls_phys, teflon_wls_optSurf);

  /// Adding the SiPMs ///

  // The SiPM
  G4LogicalVolume* SiPM_logic = BuildSiPM();

  // teflon wls hole
  G4String wls_hole_name   = "TP_TEFLON_WLS_HOLE";
  G4double wls_hole_diam   = teflon_hole_diam_;
  G4double wls_hole_length = wls_thickness_;

  G4Tubs* wls_hole_solid =
    new G4Tubs(wls_hole_name, 0., wls_hole_diam/2., wls_hole_length/2., 0, twopi);

  G4LogicalVolume* wls_hole_logic = 
    new G4LogicalVolume(wls_hole_solid, xenon_gas_, wls_hole_name);


  // teflon hole
  G4String hole_name   = "TP_TEFLON_HOLE";
  G4double hole_diam   = teflon_hole_diam_;
  G4double hole_length = teflon_thickness_ - wls_thickness_;
  G4double hole_posz   = -teflon_thickness_/2. + hole_length/2.;

  G4Tubs* hole_solid =
    new G4Tubs(hole_name, 0., hole_diam/2., hole_length/2., 0, twopi);

  G4LogicalVolume* hole_logic = 
    new G4LogicalVolume(hole_solid, xenon_gas_, hole_name);

  // Placing the SiPM into the teflon hole
  G4double SiPM_pos_z = - hole_length/2. + SiPM_case_thickness_ / 2.;

  new G4PVPlacement(0, G4ThreeVector(0., 0., SiPM_pos_z), SiPM_logic,
                    SiPM_logic->GetName(), hole_logic, false, 0, true);

  // Replicating the teflon & wls-teflon holes
  for (G4int i=0; i<num_SiPMs_; i++) {
    G4int SiPM_id = first_sensor_id_ + i;

    G4ThreeVector hole_pos = SiPM_positions_[i];
    hole_pos.setZ(hole_posz);
    new G4PVPlacement(nullptr, hole_pos, hole_logic, hole_name,
                      teflon_logic, true, SiPM_id, false);

    G4ThreeVector wls_hole_pos = SiPM_positions_[i];
    new G4PVPlacement(nullptr, wls_hole_pos, wls_hole_logic, wls_hole_name,
                      teflon_wls_logic, true, SiPM_id, false);

    //if (verbosity_) G4cout << "* TP_SiPM " << SiPM_id << " position: " 
    //                       << hole_pos << G4endl;
  }

  // Placing the overall teflon sub-system
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., teflon_posZ), teflon_logic,
                    teflon_name, mother_logic_, false, 0, verbosity_);

  /// Verbosity ///
  if (verbosity_) {
    G4cout << "* Teflon Z positions: " << teflon_iniZ_
           << " to " << teflon_iniZ_ + teflon_thickness_ << G4endl;
    G4cout << "* SiPM Z positions: " << teflon_iniZ_ 
           << " to " << teflon_iniZ_ + SiPM_case_thickness_ << G4endl;
  } 


  /// Visibilities ///
  if (visibility_) {
    G4VisAttributes light_blue_col = nexus::LightBlue();
    teflon_logic    ->SetVisAttributes(light_blue_col);
    teflon_wls_logic->SetVisAttributes(G4VisAttributes::Invisible);
    hole_logic      ->SetVisAttributes(G4VisAttributes::Invisible);
    wls_hole_logic ->SetVisAttributes(G4VisAttributes::Invisible);
  }
  else {
    teflon_logic    ->SetVisAttributes(G4VisAttributes::Invisible);
    teflon_wls_logic->SetVisAttributes(G4VisAttributes::Invisible);
    hole_logic      ->SetVisAttributes(G4VisAttributes::Invisible);
    wls_hole_logic  ->SetVisAttributes(G4VisAttributes::Invisible);
  }

}



G4LogicalVolume* NextFlexTrackingPlane::BuildSiPM()
{
  /// Constructing the TP SiPM ///
  // Optical Properties of the sensor
  G4MaterialPropertiesTable* photosensor_mpt = new G4MaterialPropertiesTable();
  G4double energy[]       = {0.2 * eV, 11.5 * eV};
  G4double reflectivity[] = {0.0     ,  0.0};
  G4double efficiency[]   = {1.0     ,  1.0};
  photosensor_mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
  photosensor_mpt->AddProperty("EFFICIENCY",   energy, efficiency,   2);
  SiPM_->SetOpticalProperties(photosensor_mpt);

  // Set WLS coating
  SiPM_->SetWithWLSCoating(true);

  // Set time binning
  SiPM_->SetTimeBinning(SiPM_binning_);

  // Set mother depth & naming order
  SiPM_->SetSensorDepth(1);
  SiPM_->SetMotherDepth(2);
  SiPM_->SetNamingOrder(1);

  // Construct
  SiPM_->Construct();

  return SiPM_->GetLogicalVolume();
}



// Function that computes and stores the XY positions of SiPMs in the copper plate
void NextFlexTrackingPlane::GenerateSiPMpositions()
{
  // Maximum radius to place SiPMs
  // Lower than diameter to prevent SiPMs being partially out.
  G4double max_radius = diameter_/2. - teflon_hole_diam_/2.;
  G4double ini_pos_XY = - diameter_/2.;

  G4int num_rows    = (G4int) (max_radius * 2 / SiPM_pitch_y_);
  G4int num_columns = (G4int) (max_radius * 2 / SiPM_pitch_x_);

  for (G4int num_row=0; num_row<num_rows; num_row++) {
    G4double posY = ini_pos_XY + num_row * SiPM_pitch_y_;

    for (G4int num_column=0; num_column<num_columns; num_column++) {
      G4double posX = ini_pos_XY + num_column * SiPM_pitch_x_;

      G4double radius = pow (pow(posX, 2.) + pow(posY, 2.), 0.5);

      if (radius <= max_radius)
        SiPM_positions_.push_back(G4ThreeVector(posX, posY, 0.));
    }
  }

  num_SiPMs_ = SiPM_positions_.size();

  if (verbosity_) {
    G4cout << "* TP num SiPM rows   : " << num_rows    << G4endl;
    G4cout << "* TP num SiPM columns: " << num_columns << G4endl;
    G4cout << "* Total num SiPMs    : " << num_SiPMs_  << G4endl;
  }
}



G4ThreeVector NextFlexTrackingPlane::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex;

  if (region == "TP_COPPER") {
    G4VPhysicalVolume *VertexVolume;
    do {
      vertex       = copper_gen_->GenerateVertex("VOLUME");
      VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(vertex, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else {
    G4Exception("[NextNew]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }

  return vertex;
}
