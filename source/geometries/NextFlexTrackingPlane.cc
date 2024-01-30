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
#include "XenonProperties.h"
#include "IonizationSD.h"
#include "UniformElectricDriftField.h"
#include "CylinderPointSampler.h"
#include "GenericPhotosensor.h"
#include "SensorSD.h"
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
#include <G4MultiUnion.hh>
#include <G4PVPlacement.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4UserLimits.hh>
#include <Randomize.hh>


using namespace nexus;


NextFlexTrackingPlane::NextFlexTrackingPlane():
  GeometryBase(),
  mother_logic_      (nullptr),
  verbosity_         (false),
  sipm_verbosity_    (false),
  visibility_        (false),
  SiPM_visibility_   (false),
  msg_               (nullptr),
  wls_mat_name_      ("TPB"),
  kapton_anode_dist_ (15.  * mm),   // Distance from ANODE to Kapton surface
  SiPM_size_x_       ( 1.3 * mm),   // Size X (width) of SiPMs
  SiPM_size_y_       ( 1.3 * mm),   // Size Y (height) of SiPMs
  SiPM_size_z_       ( 2.0 * mm),   // Size Z (thickness) of SiPMs
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
  wls_thickness_  = 1. * um;

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

  // SiPMs verbosity
  msg_->DeclareProperty("tp_sipm_verbosity", sipm_verbosity_, "SiPMs verbosity");

  // Visibilities
  msg_->DeclareProperty("tp_visibility", visibility_, "TRACKING_PLANE Visibility");

  msg_->DeclareProperty("tp_sipm_visibility", SiPM_visibility_,
                        "TRACKING_PLANE SiPMs Visibility");

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
  G4GenericMessenger::Command& kapton_anode_dist_cmd =
    msg_->DeclareProperty("tp_kapton_anode_dist", kapton_anode_dist_,
                          "Distance from tracking kapton to ANODE.");
  kapton_anode_dist_cmd.SetParameterName("tp_kapton_anode_dist", false);
  kapton_anode_dist_cmd.SetUnitCategory("Length");
  kapton_anode_dist_cmd.SetRange("tp_kapton_anode_dist>=0.");

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

  G4GenericMessenger::Command& sipm_sizeZ_cmd =
    msg_->DeclareProperty("tp_sipm_sizeZ", SiPM_size_z_,
                          "SizeZ of tracking SiPMs.");
  sipm_sizeZ_cmd.SetParameterName("tp_sipm_sizeZ", false);
  sipm_sizeZ_cmd.SetUnitCategory("Length");
  sipm_sizeZ_cmd.SetRange("tp_sipm_sizeZ>0.");

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
  teflon_iniZ_ = origin_z_ - kapton_anode_dist_;
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
  copper_mat_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  // Teflon
  teflon_mat_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  teflon_mat_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  // UV shifting material
  if (wls_mat_name_ == "NONE") {
    wls_mat_ = mother_logic_->GetMaterial();
  }
  else if (wls_mat_name_ == "TPB") {
    wls_mat_ = materials::TPB();
    wls_mat_->SetMaterialPropertiesTable(opticalprops::TPB());
  }
  else if (wls_mat_name_ == "TPH") {
    wls_mat_ = materials::TPH();
    wls_mat_->SetMaterialPropertiesTable(opticalprops::TPH());
  }
  else {
    G4Exception("[NextFlexTrackingPlane]", "DefineMaterials()", FatalException,
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
  if (teflon_thickness_) {
    BuildTeflon();
  }
  // SiPMs
  BuildSiPMs();
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
  else             copper_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

  // Vertex generator
  copper_gen_ = new CylinderPointSampler(copper_phys);

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

  G4Tubs* teflon_nh_solid =
    new G4Tubs(teflon_name + "_NOHOLE", 0., diameter_/2.,
	       teflon_thickness_/2., 0, twopi);

  // Making the Teflon holes (a little bit thicker to prevent subtraction problems)
  G4Tubs* teflon_hole_solid =
    new G4Tubs(teflon_name + "_HOLE", 0., teflon_hole_diam_/2.,
	       teflon_thickness_/2. + 0.5*mm, 0, twopi);

  G4MultiUnion* teflon_holes_solid = new G4MultiUnion(teflon_name + "_HOLES");

  G4RotationMatrix rotm = G4RotationMatrix();

  for (G4int i=0; i<num_SiPMs_; i++) {
    G4Transform3D hole_transform = G4Transform3D(rotm, SiPM_positions_[i]);
    teflon_holes_solid->AddNode(*teflon_hole_solid, hole_transform);
  }
  teflon_holes_solid->Voxelize();

  G4SubtractionSolid* teflon_solid =
    new G4SubtractionSolid(teflon_name, teflon_nh_solid, teflon_holes_solid);

  G4LogicalVolume* teflon_logic =
    new G4LogicalVolume(teflon_solid, teflon_mat_, teflon_name);

  // Adding the teflon optical surface
  G4OpticalSurface* teflon_optSurf =
    new G4OpticalSurface(teflon_name, unified, ground, dielectric_metal);
  teflon_optSurf->SetMaterialPropertiesTable(opticalprops::PTFE());

  new G4LogicalSkinSurface(teflon_name, teflon_logic, teflon_optSurf);


  /// The UV WLS in TEFLON ///
  G4String teflon_wls_name = "TP_TEFLON_WLS";

  G4double teflon_wls_posZ = teflon_thickness_/2. - wls_thickness_/2.;
 
  G4Tubs* teflon_wls_nh_solid =
    new G4Tubs(teflon_wls_name + "_NOHOLE", 0., diameter_/2.,
	       wls_thickness_/2., 0, twopi);

  // Making the TEFLON_WLS holes (a little bit thicker to prevent subtraction problems)
  G4Tubs* wls_hole_solid = 
    new G4Tubs(teflon_wls_name + "_HOLE", 0., teflon_hole_diam_/2.,
	       wls_thickness_/2. + 0.5*mm, 0, twopi);

  G4MultiUnion* wls_holes_solid = new G4MultiUnion(teflon_wls_name + "_HOLES");

  for (G4int i=0; i<num_SiPMs_; i++){
    G4Transform3D wls_hole_transform = G4Transform3D(rotm, SiPM_positions_[i]);
    wls_holes_solid->AddNode(*wls_hole_solid, wls_hole_transform); 
  }
  wls_holes_solid->Voxelize();

  G4SubtractionSolid* teflon_wls_solid = 
    new G4SubtractionSolid(teflon_wls_name, teflon_wls_nh_solid, wls_holes_solid);	  

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

  // Placing the TEFLON
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., teflon_posZ), teflon_logic,
                    teflon_name, mother_logic_, false, 0, verbosity_);

  /// Verbosity ///
  if (verbosity_)
    G4cout << "* Teflon Z positions: " << teflon_iniZ_
           << " to " << teflon_iniZ_ + teflon_thickness_ << G4endl;

  /// Visibilities ///
  if (visibility_) {
    teflon_logic->SetVisAttributes(nexus::LightBlue());
    teflon_wls_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
  else {
    teflon_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    teflon_wls_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
}



void NextFlexTrackingPlane::BuildSiPMs()
{
  /// Constructing the TP SiPM ///
  SiPM_ = new GenericPhotosensor("TP_SiPM", SiPM_size_x_, SiPM_size_y_, SiPM_size_z_);

  // Optical Properties of the sensor
  G4MaterialPropertiesTable* photosensor_mpt = new G4MaterialPropertiesTable();
  G4double energy[]       = {0.2 * eV, 3.5 * eV, 3.6 * eV, 11.5 * eV};
  G4double reflectivity[] = {0.0     , 0.0     , 0.0     ,  0.0     };
  G4double efficiency[]   = {1.0     , 1.0     , 0.0     ,  0.0     };
  photosensor_mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 4);
  photosensor_mpt->AddProperty("EFFICIENCY",   energy, efficiency,   4);
  SiPM_->SetOpticalProperties(photosensor_mpt);

  // Set WLS coating
  SiPM_->SetWithWLSCoating(true);

  // Set time binning
  SiPM_->SetTimeBinning(SiPM_binning_);

  // Set mother depth & naming order
  SiPM_->SetSensorDepth(1);
  SiPM_->SetMotherDepth(2);
  SiPM_->SetNamingOrder(1);

  // Set visibility
  SiPM_->SetVisibility(SiPM_visibility_);

  // Construct
  SiPM_->Construct();
  G4LogicalVolume* SiPM_logic = SiPM_->GetLogicalVolume();

  /// Placing the TP SiPMs ///
  G4double SiPM_pos_z = teflon_iniZ_ + SiPM_size_z_/2.;
  if (verbosity_)
    G4cout << "* SiPM Z positions: " << teflon_iniZ_
	   << " to " << teflon_iniZ_ + SiPM_size_z_ << G4endl;

  for (G4int i=0; i<num_SiPMs_; i++){
    G4int SiPM_id = first_sensor_id_ + i;

    G4ThreeVector sipm_pos = SiPM_positions_[i];
    sipm_pos.setZ(SiPM_pos_z);
    new G4PVPlacement(nullptr, sipm_pos, SiPM_logic, SiPM_logic->GetName(),
		      mother_logic_, true, SiPM_id, sipm_verbosity_);
    if (sipm_verbosity_) 
      G4cout << "* TP_SiPM " << SiPM_id << " position: " 
	     << sipm_pos << G4endl;
  }	  
}



// Function that computes and stores the XY positions of SiPMs in the copper plate
void NextFlexTrackingPlane::GenerateSiPMpositions()
{
  // Maximum radius to place the SiPMs
  // It must be lower than diameter to prevent SiPMs being partially out.
  G4double safety_dist = 4. * mm;
  G4double max_radius  = diameter_/2. - safety_dist;
  if (safety_dist < teflon_hole_diam_/2.)
    G4Exception("[NextFlexTrackingPlane]", "GenerateSiPMpositions()", FatalException,
                "Safety distance lower than teflon hole radius.");

  G4int num_rows    = (G4int) (max_radius * 2 / SiPM_pitch_y_) + 1;
  G4int num_columns = (G4int) (max_radius * 2 / SiPM_pitch_x_) + 1;

  G4double ini_pos_x = - (num_columns - 1) * SiPM_pitch_x_/2.;
  G4double ini_pos_y = - (num_rows    - 1) * SiPM_pitch_y_/2.;

  for (G4int num_row=0; num_row<num_rows; num_row++) {
    G4double pos_y = ini_pos_y + num_row * SiPM_pitch_y_;

    for (G4int num_column=0; num_column<num_columns; num_column++) {
      G4double pos_x = ini_pos_x + num_column * SiPM_pitch_x_;

      G4double radius = pow (pow(pos_x, 2.) + pow(pos_y, 2.), 0.5);

      if (radius <= max_radius)
        SiPM_positions_.push_back(G4ThreeVector(pos_x, pos_y, 0.));
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
      vertex       = copper_gen_->GenerateVertex(VOLUME);
      VertexVolume = geom_navigator_->LocateGlobalPointAndSetup(vertex, 0, false);
    } while (VertexVolume->GetName() != region);
  }

  else {
    G4Exception("[NextFlexTrackingPlane]", "GenerateVertex()", FatalException,
                "Unknown vertex generation region!");
  }

  return vertex;
}
