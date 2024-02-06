// -----------------------------------------------------------------------------
//  nexus | GenericSquarePhotosensor.cc
//
//  Geometry of a configurable box-shaped photosensor.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "GenericSquarePhotosensor.h"

#include "MaterialsList.h"
#include "SensorSD.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4Tubs.hh>

#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4SDManager.hh>
#include <G4GenericMessenger.hh>

using namespace nexus;


GenericSquarePhotosensor::GenericSquarePhotosensor(G4String name,
                                       G4double width,
                                       G4double height,
                                       G4double thickness):
  GeometryBase        (),
  name_               (name),
  width_              (width),           // Width of the Sensitive Area
  height_             (height),          // Height of the Sensitive Area
  thickness_          (thickness),       // Thickness of the whole sensor
  window_thickness_   (0.3 * mm),        // Window thickness    (similar to Sensl SiPMs)
  sensarea_thickness_ (0.1 * mm),        // Sensitive thickness (similar to Sensl SiPMs)
  wls_thickness_      (1. * micrometer), // WLS thickness = 1 micron by definition)
  with_wls_coating_   (false),
  window_rindex_      (nullptr),
  sensitive_mpt_      (nullptr),
  sensor_depth_       (-1),
  mother_depth_       (0),
  naming_order_       (0),
  time_binning_       (1.0 * us),
  visibility_         (false)
{
}


GenericSquarePhotosensor::GenericSquarePhotosensor(G4String name, G4double size):
  GenericSquarePhotosensor(name, size, size)
{
}


GenericSquarePhotosensor::~GenericSquarePhotosensor()
{
}


void GenericSquarePhotosensor::ComputeDimensions()
{
  // Reduced size for components inside the case except the WLS
  reduced_width_  = width_  - 1. * micrometer;
  reduced_height_ = height_ - 1. * micrometer;

  if (!with_wls_coating_) wls_thickness_ = 0.;

  // Check that components (window + sensitive + wls) fits into the case
  if ((window_thickness_ + sensarea_thickness_ + wls_thickness_) > thickness_) {
    G4Exception("[GenericSquarePhotosensor]", "ComputeDimensions()", FatalException,
                ("Sensor size too small. Required thickness >= " +
                 std::to_string(window_thickness_ + sensarea_thickness_ + wls_thickness_) +
                 " mm").data());
  }
}


void GenericSquarePhotosensor::DefineMaterials()
{
  // Case /////
  case_mat_ = materials::FR4();
  case_mat_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  // Window /////
  // The optical properties of a given material, is common for the whole geometry so,
  // making a copy of the window material restricts its use to this photosensor
  // and prevents interferences with other possible uses.
  window_mat_ =
    materials::CopyMaterial(materials::OpticalSilicone(),
                                name_ + "_WINDOW_MATERIAL");
  G4MaterialPropertiesTable* window_optProp = new G4MaterialPropertiesTable();

  // In the default behavior of this class, the refractive index of WLS and window
  // are matched to avoid reflection losses in the interfaces. If the user sets
  // explicitly a refractive index for the window, it won't be used, raising a warning.
  if (with_wls_coating_) {
    if (window_rindex_)
      G4Exception("[GenericSquarePhotosensor]", "DefineMaterials()", JustWarning,
                  "Window rindex set, but NOT USED. Using TPB rindex.");

    window_optProp->AddProperty("RINDEX",
                                opticalprops::TPB()->GetProperty("RINDEX"));
    window_mat_->SetMaterialPropertiesTable(window_optProp);
  }

  // If the sensor has NOT WLS coating the window must have the rindex
  // set by the user. If it is not set, an exception raises.
  else {
    if (!window_rindex_)
      G4Exception("[GenericSquarePhotosensor]", "DefineMaterials()", FatalException,
                  "Window rindex must be set before constructing");

    window_optProp->AddProperty("RINDEX", window_rindex_);
    window_mat_->SetMaterialPropertiesTable(window_optProp);
  }

  // Sensitive /////
  sensitive_mat_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
  sensitive_mat_->SetMaterialPropertiesTable(new G4MaterialPropertiesTable());

  // WLS coating /////
  wls_mat_ = materials::TPB();
  wls_mat_->SetMaterialPropertiesTable(opticalprops::TPB());
}


void GenericSquarePhotosensor::Construct()
{
  ComputeDimensions();

  DefineMaterials();

  // PHOTOSENSOR CASE //////////////////////////////////
  G4String name = name_ + "_CASE";

  // G4Tubs* case_solid_vol = new G4Tubs(name, 0, width_ / 2., thickness_ / 2., 0, twopi);
  G4Box* case_solid_vol =
    new G4Box(name, width_/2. , height_/2. , thickness_/2.);

  G4LogicalVolume* case_logic_vol =
    new G4LogicalVolume(case_solid_vol, case_mat_, name);

  GeometryBase::SetLogicalVolume(case_logic_vol);


  // OPTICAL WINDOW ////////////////////////////////////////
  name = name_ + "_WINDOW";

  // G4Tubs* window_solid_vol = new G4Tubs(name, 0, reduced_width_ / 2., window_thickness_ / 2., 0, twopi);

  G4Box* window_solid_vol =
    new G4Box(name, reduced_width_/2., reduced_height_/2., window_thickness_/2.);

  G4LogicalVolume* window_logic_vol =
    new G4LogicalVolume(window_solid_vol, window_mat_, name);

  G4double window_zpos = thickness_/2. - wls_thickness_ - window_thickness_/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., window_zpos), window_logic_vol,
                    name, case_logic_vol, false, 0, false);


  // PHOTOSENSITIVE AREA /////////////////////////////////////////////
  name = name_ + "_SENSAREA";

  // G4Tubs* sensarea_solid_vol = new G4Tubs(name, 0, reduced_width_ / 2., sensarea_thickness_ / 2., 0, twopi);

  G4Box* sensarea_solid_vol =
    new G4Box(name, reduced_width_/2., reduced_height_/2., sensarea_thickness_/2.);

  G4LogicalVolume* sensarea_logic_vol =
    new G4LogicalVolume(sensarea_solid_vol, sensitive_mat_, name);

  G4double sensarea_zpos = thickness_/2. - wls_thickness_ - window_thickness_ -
                           sensarea_thickness_/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., sensarea_zpos), sensarea_logic_vol,
                    name, case_logic_vol, false, 0, false);


  // WLS_COATING /////////////////////////////////////////////
  G4LogicalVolume* wls_logic_vol;
  if (with_wls_coating_) {
    name = name_ + "_WLS";

    G4Box* wls_solid_vol =
      new G4Box(name, width_/2., height_/2., wls_thickness_/2.);

    wls_logic_vol = new G4LogicalVolume(wls_solid_vol, wls_mat_, name);

    G4double wls_zpos = thickness_/2. - wls_thickness_/2.;

    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., wls_zpos), wls_logic_vol,
                      name, case_logic_vol, false, 0, false);

    G4OpticalSurface* wls_optSurf = new G4OpticalSurface(name + "_OPSURF",
                                                         glisur, ground,
                                                         dielectric_dielectric, .01);

    new G4LogicalSkinSurface(name + "_OPSURF", wls_logic_vol, wls_optSurf);

    wls_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
  }


  // VISIBILITIES /////////////////////////////////////////////
  if (visibility_) {
    window_logic_vol->SetVisAttributes(nexus::LightBlueAlpha());
    sensarea_logic_vol->SetVisAttributes(nexus::DarkGrey());
    case_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
  else {
    window_logic_vol  ->SetVisAttributes(G4VisAttributes::GetInvisible());
    sensarea_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
    case_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
  }


  // SENSOR OPTICAL PROPERTIES ////////////////////////////////////////
  if (!sensitive_mpt_)
    G4Exception("[GenericSquarePhotosensor]", "Construct()", FatalException,
                "Sensor Optical Properties must be set before constructing");
  G4OpticalSurface* sensitive_opsurf =
    new G4OpticalSurface(name + "_optSurf", unified, polished, dielectric_metal);
  sensitive_opsurf->SetMaterialPropertiesTable(sensitive_mpt_);
  new G4LogicalSkinSurface(name + "_optSurf", sensarea_logic_vol, sensitive_opsurf);


  // SENSITIVE DETECTOR //////////////////////////////////////////////
  G4String sdname = "/GENERIC_PHOTOSENSOR/" + name_;
  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

  if (!sdmgr->FindSensitiveDetector(sdname, false)) {
    SensorSD* sensdet = new SensorSD(sdname);
    if (sensor_depth_ == -1)
      G4Exception("[GenericSquarePhotosensor]", "Construct()", FatalException,
                  "Sensor Depth must be set before constructing");

    if ((naming_order_ > 0) & (mother_depth_ == 0))
      G4Exception("[GenericSquarePhotosensor]", "Construct()", FatalException,
                  "naming_order set without setting mother_depth");

    sensdet->SetDetectorVolumeDepth(sensor_depth_);
    sensdet->SetMotherVolumeDepth  (mother_depth_);
    sensdet->SetDetectorNamingOrder(naming_order_);
    sensdet->SetTimeBinning        (time_binning_);

    G4SDManager::GetSDMpointer()->AddNewDetector(sensdet);
    sensarea_logic_vol->SetSensitiveDetector(sensdet);
  }
}
