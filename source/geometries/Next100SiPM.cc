// -----------------------------------------------------------------------------
//  nexus | Next100SiPM.cc
//
//  Geometry of the Hamamatsu MPPC S13372-1350TE, the model of
//  silicon photomultiplier (SiPM) used in the NEXT-100 detector.
//
//  The NEXT Collaboration
// -----------------------------------------------------------------------------

#include "Next100SiPM.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "SensorSD.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

using namespace nexus;


Next100SiPM::Next100SiPM():
  GeometryBase(),
  sensor_depth_       (-1),
  mother_depth_       (0),
  naming_order_       (0),
  time_binning_       (1.0 * us),
  coating_thickn_     (2. * micrometer),
  visibility_         (true)
{
}


Next100SiPM::~Next100SiPM()
{
}


G4ThreeVector Next100SiPM::GetDimensions() const
{
  return dimensions_;
}


void Next100SiPM::Construct()
{
  // ENCASING //////////////////////////////////////////////
  //dimensions are increased in case of SiPM coating
  G4String sipm_name = "SIPM_S13372";

  G4double sipm_width  = 3.0 * mm;
  G4double sipm_length = 4.0 * mm;
  G4double sipm_thickn = 1.3 * mm;

  sipm_thickn = sipm_thickn + coating_thickn_;

  dimensions_.setX(sipm_width);
  dimensions_.setY(sipm_length);
  dimensions_.setZ(sipm_thickn);

  G4Box* sipm_solid_vol =
    new G4Box(sipm_name, sipm_width/2., sipm_length/2., sipm_thickn/2.);

  G4LogicalVolume* sipm_logic_vol =
    new G4LogicalVolume(sipm_solid_vol, materials::FR4(), sipm_name);

  GeometryBase::SetLogicalVolume(sipm_logic_vol);

  // COATING /////////////////////////////////////////////
  if (coating_thickn_ > 0.) {
    G4String coating_name = sipm_name + "_WLS";

    G4Box* coating_solid_vol =
      new G4Box(coating_name, sipm_width/2., sipm_length/2., coating_thickn_/2.);

    G4Material* coating_mt = materials::TPB();
    coating_mt->SetMaterialPropertiesTable(opticalprops::TPB());

    G4LogicalVolume* coating_logic_vol =
      new G4LogicalVolume(coating_solid_vol, coating_mt, coating_name);

    G4double coating_zpos = sipm_thickn/2. - coating_thickn_/2;

    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., coating_zpos),
                      coating_logic_vol,
                      coating_name, sipm_logic_vol, false, 0, false);

    G4OpticalSurface* coating_optSurf =
      new G4OpticalSurface(coating_name + "_OPSURF", glisur, ground,
                           dielectric_dielectric, .01);

    new G4LogicalSkinSurface(coating_name + "_OPSURF", coating_logic_vol, coating_optSurf);

    coating_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
  }

  // WINDOW ////////////////////////////////////////////////

  G4String window_name = sipm_name + "_WINDOW";

  G4double window_width  = sipm_width;
  G4double window_length = sipm_length;
  G4double window_thickn = 0.5 * mm;
  G4double window_zpos   = sipm_thickn/2. - coating_thickn_ - window_thickn/2.;

  G4Material* optical_silicone = materials::OpticalSilicone();
  optical_silicone->SetMaterialPropertiesTable(opticalprops::Epoxy());

  G4Box* window_solid_vol =
    new G4Box(window_name, window_width/2., window_length/2., window_thickn/2.);

  G4LogicalVolume* window_logic_vol =
    new G4LogicalVolume(window_solid_vol, optical_silicone, window_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., window_zpos),
                    window_logic_vol, window_name, sipm_logic_vol, false, 0, false);


  // SENSITIVE AREA ////////////////////////////////////////

  G4String sens_name = sipm_name + "_SENSAREA";

  G4double sens_width  = 1.3 * mm;
  G4double sens_length = 1.3 * mm;
  G4double sens_thickn = 0.2 * mm;
  G4double sens_zpos   = window_zpos - window_thickn/2. - sens_thickn/2.;

  G4Box* sens_solid_vol =
    new G4Box(sens_name, sens_width/2., sens_length/2., sens_thickn/2.);

  G4LogicalVolume* sens_logic_vol =
    new G4LogicalVolume(sens_solid_vol,
                        G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"),
                        sens_name);

  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., sens_zpos),
                    sens_logic_vol, sens_name, sipm_logic_vol, false, 0, false);

  // OPTICAL SURFACES //////////////////////////////////////
  const G4int entries = 42;
  G4double energies[entries] =
    {0.2 * eV, 1.0 * eV, 1.3 * eV,
     1.3776022  * eV, 1.4472283  * eV, 1.52041305 * eV, 1.59290956 * eV,
     1.66341179 * eV, 1.72546158 * eV, 1.78169885 * eV, 1.8473836  * eV,
     1.90593775 * eV, 1.94918431 * eV, 1.99443901 * eV, 2.05580636 * eV,
     2.12107005 * eV, 2.17476803 * eV, 2.23125551 * eV, 2.29951572 * eV,
     2.37208426 * eV, 2.46950045 * eV, 2.75730000 * eV, 3.04467524 * eV,
     3.14006977 * eV, 3.20705792 * eV, 3.2592052  * eV, 3.31307637 * eV,
     3.38773724 * eV, 3.44597914 * eV, 3.50625866 * eV, 3.54763044 * eV,
     3.58999021 * eV, 3.61155171 * eV, 3.65546116 * eV, 3.67781872 * eV,
     3.72336446 * eV, 3.74656299 * eV, 3.79383824 * eV, 3.86703126 * eV,
     4.0 * eV, 4.2 * eV, 11.5 * eV};

  G4double reflectivity[entries] =
    {0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0};

  G4double efficiency[entries] =
    {0.0, 0.0, 0.0,
     0.03506431, 0.0560223 , 0.07944598, 0.10291004,
     0.12607905, 0.14954693, 0.17061431, 0.193974  , 0.21740781,
     0.23843982, 0.26178898, 0.28501443, 0.30860135, 0.33010702,
     0.35361856, 0.37751412, 0.40109223, 0.42743901, 0.43779628,
     0.41021884, 0.38111716, 0.36057765, 0.33517116, 0.31491576,
     0.28931617, 0.26968918, 0.24455707, 0.22893024, 0.20156831,
     0.18538152, 0.15444223, 0.12549548, 0.1040183 , 0.0902669 ,
     0.05498552, 0.02944706,
     0.0, 0.0, 0.0};

  G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
  sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
  sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);
  G4OpticalSurface* sipm_opsurf =
    new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
  sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);
  new G4LogicalSkinSurface("SIPM_OPSURF", sens_logic_vol, sipm_opsurf);

  // SENSITIVE DETECTOR ////////////////////////////////////

  G4String sdname = "/" + sipm_name + "/SiPM";
  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

  if (!sdmgr->FindSensitiveDetector(sdname, false)) {
    SensorSD* sensdet = new SensorSD(sdname);

    if (sensor_depth_ == -1)
      G4Exception("[Next100SiPM]", "Construct()", FatalException,
                  "Sensor Depth must be set before constructing");

    if ((naming_order_ > 0) && (mother_depth_ == 0))
      G4Exception("[Next100SiPM]", "Construct()", FatalException,
                  "Naming Order set without setting Mother Depth");

    sensdet->SetDetectorVolumeDepth(sensor_depth_);
    sensdet->SetMotherVolumeDepth(mother_depth_);
    sensdet->SetDetectorNamingOrder(naming_order_);
    sensdet->SetTimeBinning(time_binning_);

    G4SDManager::GetSDMpointer()->AddNewDetector(sensdet);
    sens_logic_vol->SetSensitiveDetector(sensdet);
  }

  // VISIBILITY ////////////////////////////////////////////

  if (visibility_) {
    sipm_logic_vol  ->SetVisAttributes(G4VisAttributes::GetInvisible());
    window_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
    G4VisAttributes red = Red();
    red.SetForceSolid(true);
    sens_logic_vol->SetVisAttributes(red);
  }
  else {
    sipm_logic_vol  ->SetVisAttributes(G4VisAttributes::GetInvisible());
    window_logic_vol->SetVisAttributes(G4VisAttributes::GetInvisible());
    sens_logic_vol  ->SetVisAttributes(G4VisAttributes::GetInvisible());
  }
}
