// ----------------------------------------------------------------------------
// nexus | SiPMFBKVUV.cc
//
// 3x3 mm2 FBK SiPM geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SiPMFBKVUV.h"
#include "PmtSD.h"
#include "ToFSD.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

namespace nexus {

  using namespace CLHEP;

  SiPMFBKVUV::SiPMFBKVUV(): BaseGeometry(),
                            visibility_(1),
                            eff_(1.),
                            time_binning_(200.*nanosecond)

  {
  }

  SiPMFBKVUV::~SiPMFBKVUV()
  {
  }

  void SiPMFBKVUV::Construct()
  {

    // PACKAGE ///////////////////////////////////////////////////////
    G4double sipm_x = 3. * mm;
    G4double sipm_y = 3.4 * mm;
    G4double sipm_z = 0.6 * mm;

    SetDimensions(G4ThreeVector(sipm_x, sipm_y, sipm_z));

    G4Box* sipm_solid = new G4Box("SiPMFBKVUV", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* plastic = MaterialsList::FR4();
    G4LogicalVolume* sipm_logic =
      new G4LogicalVolume(sipm_solid, plastic, "SiPMFBKVUV");

    this->SetLogicalVolume(sipm_logic);


    // ACTIVE WINDOW /////////////////////////////////////////////////

    G4double active_x = sipm_x;
    G4double active_y = sipm_y;
    G4double active_depth = 0.1 * mm;

    G4Box* active_solid =
      new G4Box("PHOTODIODES", active_x/2., active_y/2., active_depth/2);

    G4Material* silicon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    new G4PVPlacement(0, G4ThreeVector(0., 0., sipm_z/2. - active_depth/2.),
                      active_logic, "PHOTODIODES", sipm_logic, false, 0, false);


    // OPTICAL SURFACES /////////////////////////////////////////////

    const G4int entries = 12;

    G4double energies[entries] = {1.5*eV, 6.19919*eV, 6.35814*eV,
                                  6.52546*eV, 6.70182*eV, 6.88799*eV,
                                  7.08479*eV, 7.29316*eV, 7.51417*eV,
                                  7.74898*eV, 7.99895*eV, 8.26558*eV};
    G4double reflectivity[entries] = {0., 0., 0.,
                                      0., 0., 0.,
                                      0., 0., 0.,
                                      0., 0., 0.};
    G4double efficiency[entries]   = {eff_, eff_, eff_,
                                      eff_, eff_, eff_,
                                      eff_, eff_, eff_,
                                      eff_, eff_, eff_};


    G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
    sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
    sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

    G4OpticalSurface* sipm_opsurf =
      new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
    sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);

    new G4LogicalSkinSurface("SIPM_OPSURF", active_logic, sipm_opsurf);


    // SENSITIVE DETECTOR ////////////////////////////////////////////

    G4String sdname = "/SIPM/SiPMFBKVUV";
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

    if (!sdmgr->FindSensitiveDetector(sdname, false)) {
      ToFSD* sipmsd = new ToFSD(sdname);
      sipmsd->SetDetectorVolumeDepth(sensor_depth_);
      sipmsd->SetMotherVolumeDepth(mother_depth_);
      sipmsd->SetDetectorNamingOrder(naming_order_);
      sipmsd->SetTimeBinning(time_binning_);
      sipmsd->SetBoxGeom(box_geom_);

      G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
      active_logic->SetSensitiveDetector(sipmsd);

    }

    // Visibilities
    if (visibility_) {
      G4VisAttributes sipm_col = nexus::Yellow();
      sipm_logic->SetVisAttributes(sipm_col);
      G4VisAttributes active_col = nexus::Blue();
      active_col.SetForceSolid(true);
      active_logic->SetVisAttributes(active_col);
    }
    else {
      sipm_logic->SetVisAttributes(G4VisAttributes::Invisible);
      active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }


} // end namespace nexus
