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

using namespace nexus;


GenericPhotosensor::GenericPhotosensor(G4double width, G4double height):
  BaseGeometry(),
  width_(width), height_(height), thickness_(2.*mm),
  window_mat_(nullptr)
{
}


GenericPhotosensor::GenericPhotosensor(G4double size): GenericPhotosensor(size,size)
{
}


GenericPhotosensor::~GenericPhotosensor()
{
}


void GenericPhotosensor::Construct()
{
  // PHOTOSENSOR ENCASING ////////////////////////////////////////////

  G4String name = "PHOTOSENSOR";

  G4Box* encasing_solid_vol = new G4Box(name, width_/2., height_/2., thickness_/2.);

  G4LogicalVolume* encasing_logic_vol =
    new G4LogicalVolume(encasing_solid_vol, MaterialsList::FR4(), name);

  BaseGeometry::SetLogicalVolume(encasing_logic_vol);

  // OPTICAL WINDOW //////////////////////////////////////////////////

  G4double window_thickness = thickness_/4.;

  G4Box* window_solid_vol = new G4Box(name, width_/2., height_/2., window_thickness/2.);

  // We use a duplicate of 'optical silicone' as material for the window.
  // This way, we make sure that the optical properties of the window (which are
  // attached to its material) do not affect other elements of the geometry or
  // are modified unconsciously by a user.
  window_mat_ = MaterialsList::CopyMaterial(MaterialsList::OpticalSilicone(),
                                            "GENERIC_PHOTOSENSOR_WINDOW_MATERIAL");
  window_mat_->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

  G4LogicalVolume* window_logic_vol =
    new G4LogicalVolume(window_solid_vol, window_mat_, name);

  G4double zpos = height_/2. - window_thickness/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,zpos), window_logic_vol,
                    name, encasing_logic_vol, false, 0, true);

  // PHOTOSENSITIVE AREA /////////////////////////////////////////////

  G4double sensarea_thickness = 0.1*mm;

  G4Box* sensarea_solid_vol = new G4Box(name, width_/2., height_/2., sensarea_thickness/2.);

  G4LogicalVolume* sensarea_logic_vol =
    new G4LogicalVolume(sensarea_solid_vol,
                        G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"),
                        name);

  zpos = height_/2. - window_thickness - sensarea_thickness/2.;

  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,zpos), sensarea_logic_vol,
                    name, encasing_logic_vol, false, 0, true);

  // OPTICAL PROPERTIES //////////////////////////////////////////////

  // -------------------------------------------------------
  // TODO: Review this section once we confirm the
  //       correctness of the new optical model.
  // -------------------------------------------------------

  G4double energy[2]       = {1.0*eV, 4.0*eV};
  G4double reflectivity[2] = {0.0,    0.0};
  G4double efficiency[2]   = {1.0,    1.0};

  G4MaterialPropertiesTable* photosensor_mpt = new G4MaterialPropertiesTable();
  photosensor_mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
  photosensor_mpt->AddProperty("EFFICIENCY",   energy, efficiency,   2);

  G4OpticalSurface* photosensor_opsurf =
    new G4OpticalSurface("PHOTOSENSOR_OPSURF", unified, polished, dielectric_metal);
  photosensor_opsurf->SetMaterialPropertiesTable(photosensor_mpt);

  new G4LogicalSkinSurface("PHOTOSENSOR_OPSURF", sensarea_logic_vol, photosensor_opsurf);

  // SENSITIVE DETECTOR //////////////////////////////////////////////

  G4String sdname = "/GENERIC_PHOTOSENSOR";
  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

  if (!sdmgr->FindSensitiveDetector(sdname, false)) {
    PmtSD* sensdet = new PmtSD(sdname);
    sensdet->SetDetectorVolumeDepth(0);
    sensdet->SetDetectorNamingOrder(1000.);
    sensdet->SetTimeBinning(1.*microsecond);
    sensdet->SetMotherVolumeDepth(1);

    G4SDManager::GetSDMpointer()->AddNewDetector(sensdet);
    encasing_logic_vol->SetSensitiveDetector(sensdet);
  }
}


// void GenericPhotosensor::SetDefaultOpticalProperties()
// {
//   G4MaterialPropertiesTable* window_mpt = new G4MaterialPropertiesTable();
//
//   G4double energy[2] = {1.0*eV, 10.*eV};
//   G4double rindex[2] = {1.0, 1.0};
//
//   window_mpt->AddProperty("RINDEX", energy, rindex, 2);
// }
//
//
// void GenericPhotosensor::SetRefractiveIndex(G4MaterialPropertyVector* mpv)
// {
//   window_mat_->GetMaterialPropertiesTable()->AddProperty("RINDEX", mpv);
// }


G4ThreeVector GenericPhotosensor::GenerateVertex(const G4String&) const
{
  return G4ThreeVector(0., 0., 0.);
}
