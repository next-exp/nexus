// -----------------------------------------------------------------------------
//  nexus | GenericPhotosensor.cc
//
//  * Author: <justo.martin-albo@ific.uv.es>
//  * Creation date: 22 January 2020
// -----------------------------------------------------------------------------

#include "GenericPhotosensor.h"

#include "MaterialsList.h"
#include "PmtSD.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4SDManager.hh>
#include <G4GenericMessenger.hh>

using namespace nexus;


GenericPhotosensor::GenericPhotosensor(G4String name,
                                       G4double width,
                                       G4double height,
                                       G4double thickness):
  BaseGeometry     (),
  msg_             (nullptr),
  name_            (name),
  width_           (width),      // Width of the Sensitive Area
  height_          (height),     // Height of the Sensitive Area
  thickness_       (thickness),  // Thickness of the whole sensor
  with_WLScoating_ (false),
  window_rIndex_   (nullptr),
  sensitive_mpt_   (nullptr),
  sensor_depth_    (1),          // Be sure to set it properly
  mother_depth_    (1),          // Be sure to set it properly
  naming_order_    (1),          // Be sure to set it properly
  time_binning_    (1.0 * us),
  visibility_      (false)
{
  // User control commands for generic photosensor
  msg_ = new G4GenericMessenger(this, "/Geometry/" + name_ + '/',
                                "Control commands of the generic photosensor geometry");

  G4GenericMessenger::Command& time_binning_cmd =
    msg_->DeclarePropertyWithUnit("time_binning", "ns", time_binning_,
                                  "Time binning of the recorded waveform.");
  time_binning_cmd.SetUnitCategory("Time");
  time_binning_cmd.SetParameterName("time_binning", false);
  time_binning_cmd.SetRange("time_binning>0.");

  msg_->DeclareProperty("visibility", visibility_,
                        "Visibility of the GenericPhotosensor volumes.");
}


GenericPhotosensor::GenericPhotosensor(G4String name, G4double size): 
  GenericPhotosensor(name, size, size)
{
}


GenericPhotosensor::~GenericPhotosensor()
{
  delete msg_;
}


void GenericPhotosensor::ComputeDimensions()
{
  // Encasing 0.1mm bigger than the sensitive area
  encasing_x_ = width_  + 0.1 * mm;
  encasing_y_ = height_ + 0.1 * mm;
  encasing_z_ = thickness_;

  // Window thickness of 0.2mm (Similar to Sensl SiPMs)
  window_x_ = encasing_x_;
  window_y_ = encasing_y_;
  window_z_ = 0.2 * mm;

  // Sensitive area thickness of 0.2mm (Similar to Sensl SiPMs)
  sensitive_z_ = 0.2 * mm;

  // Check that window + sensitive fits into the encasing
  if ((window_z_ + sensitive_z_) > encasing_z_)
    G4Exception("[GenericPhotosensor]", "ComputeDimensions()", FatalException,
                "Sensor thickness too small.");

  // WLS coating dimensions (thickness = 1 micron by definition)
  wls_x_ = encasing_x_;
  wls_y_ = encasing_y_;
  wls_z_ = 1. * micrometer;
}


void GenericPhotosensor::DefineMaterials()
{
  // Encasing /////
  encasing_mat_ = MaterialsList::FR4();

  // Window /////
  // Duplicate of 'optical silicone' to avoid interferences with other uses.
  window_mat_ =
    MaterialsList::CopyMaterial(MaterialsList::OpticalSilicone(),
                                "PHOTOSENSOR_WINDOW_MATERIAL");
    G4MaterialPropertiesTable* window_optProp = new G4MaterialPropertiesTable();

  // If the sensor has WLS coating the window must have the rIndex from WLS
  // If the user set an specific rIndex for the window, a WARNING is raised
  // as the set values are not used.   
  if (with_WLScoating_) {
    if (window_rIndex_)
      G4Exception("[GenericPhotosensor]", "DefineMaterials()", JustWarning,
                  "Window rIndex set, but NOT USED. Using TPB rIndex.");

    window_optProp->AddProperty("RINDEX",
                                OpticalMaterialProperties::TPB()->GetProperty("RINDEX"));
    window_mat_->SetMaterialPropertiesTable(window_optProp);
  }

  // If the sensor has NOT WLS coating the window must have the rIndex
  // set by the user. If it is not set, an exception raises.
  else {
    if (!window_rIndex_)
      G4Exception("[GenericPhotosensor]", "DefineMaterials()", FatalException,
                  "Window rIndex must be set before constructing");

    window_optProp->AddProperty("RINDEX", window_rIndex_);
    window_mat_->SetMaterialPropertiesTable(window_optProp);    
  }

  // Sensitive /////
  sensitive_mat_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

  // WLS coating /////
  wls_mat_ = MaterialsList::TPB();
  wls_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB());
}


void GenericPhotosensor::Construct()
{
  ComputeDimensions();

  DefineMaterials();

  // PHOTOSENSOR ENCASING //////////////////////////////////
  G4String name = name_ + "_ENCASING";

  G4Box* encasing_solid_vol =
    new G4Box(name, encasing_x_/2., encasing_y_/2., encasing_z_/2.);

  G4LogicalVolume* encasing_logic_vol =
    new G4LogicalVolume(encasing_solid_vol, encasing_mat_, name);

  BaseGeometry::SetLogicalVolume(encasing_logic_vol);


  // OPTICAL WINDOW ////////////////////////////////////////
  name = name_ + "_WINDOW";

  G4Box* window_solid_vol =
    new G4Box(name, window_x_/2., window_y_/2., window_z_/2.);

  G4LogicalVolume* window_logic_vol =
    new G4LogicalVolume(window_solid_vol, window_mat_, name);

  G4double zpos = encasing_z_/2. - window_z_/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., zpos), window_logic_vol,
                    name, encasing_logic_vol, false, 0, false);


  // PHOTOSENSITIVE AREA /////////////////////////////////////////////
  name = name_;

  G4Box* sensarea_solid_vol =
    new G4Box(name, width_/2., height_/2., sensitive_z_/2.);

  G4LogicalVolume* sensarea_logic_vol =
    new G4LogicalVolume(sensarea_solid_vol, sensitive_mat_, name);

  zpos = encasing_z_/2. - window_z_ - sensitive_z_/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., zpos), sensarea_logic_vol,
                    name, encasing_logic_vol, false, 0, false);


  // WLS_COATING /////////////////////////////////////////////
  G4LogicalVolume* wls_logic_vol;
  if (with_WLScoating_) {
    name = name_ + "_WLS";

    G4Box* wls_solid_vol =
      new G4Box(name, wls_x_/2., wls_y_/2., wls_z_/2.);

    wls_logic_vol = new G4LogicalVolume(wls_solid_vol, wls_mat_, name);

    zpos = window_z_/2. - wls_z_/2.;

    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., zpos), wls_logic_vol,
                      name, window_logic_vol, false, 0, false);

    G4OpticalSurface* wls_optSurf = new G4OpticalSurface(name + "_optSurf",
                                                         glisur, ground,
                                                         dielectric_dielectric, .01);
    
    new G4LogicalSkinSurface(name + "_optSurf", wls_logic_vol, wls_optSurf);
  }


  // VISIBILITIES /////////////////////////////////////////////
  if (visibility_) {
    window_logic_vol  ->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes blood_red = BloodRed();
    blood_red.SetForceSolid(true);
    sensarea_logic_vol->SetVisAttributes(blood_red);
    encasing_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
    if (with_WLScoating_) wls_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
  }
  else {
    window_logic_vol  ->SetVisAttributes(G4VisAttributes::Invisible);
    sensarea_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
    encasing_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
    if (with_WLScoating_) wls_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
  }


  // SENSOR OPTICAL PROPERTIES ////////////////////////////////////////
  if (!sensitive_mpt_)
    G4Exception("[GenericPhotosensor]", "Construct()", FatalException,
                "Sensor Optical Properties must be set before constructing");
  G4OpticalSurface* sensitive_opsurf =
    new G4OpticalSurface(name + "_optSurf", unified, polished, dielectric_metal);
  sensitive_opsurf->SetMaterialPropertiesTable(sensitive_mpt_);
  new G4LogicalSkinSurface(name + "_optSurf", sensarea_logic_vol, sensitive_opsurf);


  // SENSITIVE DETECTOR //////////////////////////////////////////////
  G4String sdname = "/GENERIC_PHOTOSENSOR/" + name_;
  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

  if (!sdmgr->FindSensitiveDetector(sdname, false)) {
    PmtSD* sensdet = new PmtSD(sdname);
    sensdet->SetDetectorVolumeDepth(sensor_depth_);
    sensdet->SetDetectorNamingOrder(naming_order_);
    sensdet->SetTimeBinning(time_binning_);
    sensdet->SetMotherVolumeDepth(mother_depth_);

    G4SDManager::GetSDMpointer()->AddNewDetector(sensdet);
    window_logic_vol->SetSensitiveDetector(sensdet);
  }
}
