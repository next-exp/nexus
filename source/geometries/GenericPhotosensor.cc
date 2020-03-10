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


GenericPhotosensor::GenericPhotosensor(G4double width,
                                       G4double height,
                                       G4double thickness):
  BaseGeometry(),
  width_(width), height_(height), thickness_(thickness),
  time_binning_(1.0*microsecond),
  window_mat_(nullptr),
  msg_(nullptr)
{
  // We use a duplicate of 'optical silicone' as material for the window.
  // This way, we make sure that the optical properties of the window (which are
  // attached to its material) do not affect other elements of the geometry or
  // are modified unconsciously by a user.
  window_mat_ = MaterialsList::CopyMaterial(MaterialsList::OpticalSilicone(),
                                            "GENERIC_PHOTOSENSOR_WINDOW_MATERIAL");
  window_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

  // User control commands for generic photosensor
  msg_ = new G4GenericMessenger(this, "/Geometry/GenericPhotosensor/",
                                "Control commands of the generic photosensor geometry");

  G4GenericMessenger::Command& time_binning_cmd =
    msg_->DeclarePropertyWithUnit("time_binning", "ns", time_binning_,
                                  "Time binning of the recorded waveform.");
  time_binning_cmd.SetUnitCategory("Time");
  time_binning_cmd.SetParameterName("time_binning", false);
  time_binning_cmd.SetRange("time_binning>0.");
}


GenericPhotosensor::GenericPhotosensor(G4double size): GenericPhotosensor(size,size)
{
}


GenericPhotosensor::~GenericPhotosensor()
{
}


void GenericPhotosensor::Construct()
{
  // PHOTOSENSOR ENCASING //////////////////////////////////

  G4String name = "PHOTOSENSOR";

  G4Box* encasing_solid_vol =
    new G4Box(name, width_/2., height_/2., thickness_/2.);

  G4LogicalVolume* encasing_logic_vol =
    new G4LogicalVolume(encasing_solid_vol, MaterialsList::FR4(), name);

  BaseGeometry::SetLogicalVolume(encasing_logic_vol);

  // OPTICAL WINDOW ////////////////////////////////////////

  name = "PHOTOSENSOR_WINDOW";

  G4double window_thickness = thickness_/4.;

  G4Box* window_solid_vol =
    new G4Box(name, width_/2., height_/2., window_thickness/2.);

  G4LogicalVolume* window_logic_vol =
    new G4LogicalVolume(window_solid_vol, window_mat_, name);

  G4double zpos = height_/2. - window_thickness/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., zpos),
                    window_logic_vol, name, encasing_logic_vol,
                    false, 0, false);

  // PHOTOSENSITIVE AREA /////////////////////////////////////////////

  name = "PHOTOSENSOR_SENSAREA";

  G4double sensarea_thickness = 0.1*mm;

  G4Box* sensarea_solid_vol =
    new G4Box(name, width_/2., height_/2., sensarea_thickness/2.);

  G4LogicalVolume* sensarea_logic_vol =
    new G4LogicalVolume(sensarea_solid_vol,
                        G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"),
                        name);

  zpos = height_/2. - window_thickness - sensarea_thickness/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., zpos),
                    sensarea_logic_vol, name, encasing_logic_vol,
                    false, 0, false);

  // OPTICAL PROPERTIES //////////////////////////////////////////////

  name = "PHOTOSENSOR_OPSURF";

  G4double energy[]       = {0.2*eV, 11.5*eV};
  G4double reflectivity[] = {0.0   ,  0.0};
  G4double efficiency[]   = {1.0   ,  1.0};

  G4MaterialPropertiesTable* photosensor_mpt = new G4MaterialPropertiesTable();
  photosensor_mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
  photosensor_mpt->AddProperty("EFFICIENCY",   energy, efficiency,   2);

  G4OpticalSurface* photosensor_opsurf =
    new G4OpticalSurface(name, unified, polished, dielectric_metal);
  photosensor_opsurf->SetMaterialPropertiesTable(photosensor_mpt);
  new G4LogicalSkinSurface(name, sensarea_logic_vol, photosensor_opsurf);

  // SENSITIVE DETECTOR //////////////////////////////////////////////

  G4String sdname = "/GENERIC_PHOTOSENSOR";
  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

  if (!sdmgr->FindSensitiveDetector(sdname, false)) {
    PmtSD* sensdet = new PmtSD(sdname);
    sensdet->SetDetectorVolumeDepth(0);
    sensdet->SetDetectorNamingOrder(1000.);
    sensdet->SetTimeBinning(time_binning_);
    sensdet->SetMotherVolumeDepth(1);

    G4SDManager::GetSDMpointer()->AddNewDetector(sensdet);
    encasing_logic_vol->SetSensitiveDetector(sensdet);
  }
}


void GenericPhotosensor::SetRefractiveIndex(G4MaterialPropertyVector* mpv)
{
  window_mat_->GetMaterialPropertiesTable()->AddProperty("RINDEX", mpv);
}


G4ThreeVector GenericPhotosensor::GenerateVertex(const G4String&) const
{
  return G4ThreeVector();
}
