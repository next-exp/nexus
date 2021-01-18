// ----------------------------------------------------------------------------
// nexus | SiPMHamamatsuVUV.cc
//
// 6x6 mm2 VUV Hamamatsu SiPM geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SiPMHamamatsuVUV.h"
#include "ToFSD.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "IonizationSD.h"

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
#include <G4PhysicalConstants.hh>

#include <CLHEP/Units/SystemOfUnits.h>


namespace nexus {

  using namespace CLHEP;

  SiPMHamamatsuVUV::SiPMHamamatsuVUV(): BaseGeometry(),
                                        visibility_(1),
                                        time_binning_(200.*nanosecond),
                                        sensor_depth_(-1),
                                        mother_depth_(0),
                                        naming_order_(0),
                                        box_geom_(0)

  {
  }

  SiPMHamamatsuVUV::~SiPMHamamatsuVUV()
  {
  }

  void SiPMHamamatsuVUV::Construct()
  {

    // PACKAGE ///////////////////////////////////////////////////////
    // Hammamatsu 6x6mm VUV
    G4double sipm_x = 5.95 * mm;
    G4double sipm_y = 5.85 * mm;
    G4double sipm_z = 1.2 * mm;

    SetDimensions(G4ThreeVector(sipm_x, sipm_y, sipm_z));

    G4Box* sipm_solid = new G4Box("SiPMHmtsuVUV", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* plastic = MaterialsList::FR4();
    G4LogicalVolume* sipm_logic =
      new G4LogicalVolume(sipm_solid, plastic, "SiPMHmtsuVUV");

    this->SetLogicalVolume(sipm_logic);


    // Active window
    G4double active_window_x = sipm_x;
    G4double active_window_y = sipm_y;
    G4double active_window_depth = 0.01 * mm;

    G4Box* active_window_solid =
      new G4Box("PHOTODIODES", active_window_x/2., active_window_y/2., active_window_depth/2);

    G4Material* silicon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_window_logic =
      new G4LogicalVolume(active_window_solid, silicon, "PHOTODIODES");

    new G4PVPlacement(0, G4ThreeVector(0., 0., sipm_z/2. - active_window_depth/2.),
      active_window_logic, "PHOTODIODES", sipm_logic, false, 0, false);


    // OPTICAL SURFACES //////////////////////////////////////////////

    // SiPM efficiency set using the official Hamamatsu specs.
    const G4int entries = 44;

    G4double energies[entries] = {8.34119 *eV, 8.25842 *eV, 8.12935 *eV, 8.03517 *eV, 7.88295 *eV, 7.8384  *eV,
                                  7.67929 *eV, 7.55382 *eV, 7.5265  *eV, 7.48588 *eV, 7.45904 *eV, 7.39278 *eV,
                                  7.30198 *eV, 7.25108 *eV, 7.17605 *eV, 7.09046 *eV, 6.96    *eV, 6.85679 *eV,
                                  6.74563 *eV, 6.65927 *eV, 6.42286 *eV, 6.34452 *eV, 6.26807 *eV, 6.18423 *eV,
                                  6.07587 *eV, 5.95416 *eV, 5.79657 *eV, 5.68569 *eV, 5.64711 *eV, 5.45465 *eV,
                                  5.39105 *eV, 5.24826 *eV, 5.20884 *eV, 5.1572  *eV, 5.06307 *eV, 4.97232 *eV,
                                  4.7186  *eV, 4.63451 *eV, 4.56837 *eV, 4.51876 *eV, 4.47022 *eV, 4.40398 *eV,
                                  4.32615 *eV, 4.1328  *eV};

    G4double reflectivity[entries] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                      0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                      0., 0., 0., 0., 0., 0., 0., 0.};

    G4double efficiency[entries]   = {0.        , 0.00915551, 0.02487288, 0.04841303, 0.1115    ,
                                      0.13575243, 0.21963205, 0.28548157, 0.29676079, 0.30988794,
                                      0.31369555, 0.31969894, 0.33375418, 0.33448485, 0.3315    ,
                                      0.32928761, 0.32794411, 0.32557521, 0.31967385, 0.3151805 ,
                                      0.29306512, 0.2873244 , 0.28386418, 0.28419561, 0.28438124,
                                      0.28719061, 0.28719061, 0.28448257, 0.28527587, 0.29010869,
                                      0.29232598, 0.29749992, 0.29911531, 0.29454339, 0.27849492,
                                      0.26290128, 0.22088623, 0.20603676, 0.19791766, 0.19677183,
                                      0.19804775, 0.20050162, 0.21057682, 0.24910032};


    G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
    sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
    sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

    G4OpticalSurface* sipm_opsurf =
      new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
    sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);

    new G4LogicalSkinSurface("SIPM_OPSURF", active_window_logic, sipm_opsurf);


    // SENSITIVE DETECTOR ////////////////////////////////////////////

    G4String sdname = "/SIPM/SiPMHmtsuVUV";
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

    if (!sdmgr->FindSensitiveDetector(sdname, false)) {
        ToFSD* sipmsd = new ToFSD(sdname);
        if (sensor_depth_ == -1)
          G4Exception("[SiPMHamamatsuVUV]", "Construct()", FatalException,
                      "Sensor Depth must be set before constructing");

        sipmsd->SetDetectorVolumeDepth(sensor_depth_);
        sipmsd->SetMotherVolumeDepth(mother_depth_);
        sipmsd->SetDetectorNamingOrder(naming_order_);
        sipmsd->SetTimeBinning(time_binning_);
        sipmsd->SetBoxGeom(box_geom_);

        G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
        active_window_logic->SetSensitiveDetector(sipmsd);
    }

    // Visibilities
    if (visibility_) {
      G4VisAttributes sipm_col = nexus::Yellow();
      sipm_logic->SetVisAttributes(sipm_col);
      G4VisAttributes active_window_col = nexus::Blue();
      active_window_col.SetForceSolid(true);
      active_window_logic->SetVisAttributes(active_window_col);
    }
    else {
      sipm_logic->SetVisAttributes(G4VisAttributes::Invisible);
      active_window_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }


} // end namespace nexus
