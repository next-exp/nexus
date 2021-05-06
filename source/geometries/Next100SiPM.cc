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
#include "PmtSD.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>

using namespace nexus;


Next100SiPM::Next100SiPM():
  BaseGeometry()
{
}


Next100SiPM::~Next100SiPM()
{
}


void Next100SiPM::Construct()
{
  // ENCASING //////////////////////////////////////////////

  G4String sipm_name = "SIPM_S13372";

  G4double sipm_width  = 3.0 * mm;
  G4double sipm_length = 4.0 * mm;
  G4double sipm_thickn = 1.3 * mm;

  G4Box* sipm_solid_vol =
    new G4Box(sipm_name, sipm_width/2., sipm_length/2., sipm_thickn/2.);

  G4LogicalVolume* sipm_logic_vol =
    new G4LogicalVolume(sipm_solid_vol, MaterialsList::FR4(), sipm_name);

  BaseGeometry::SetLogicalVolume(sipm_logic_vol);


  // WINDOW ////////////////////////////////////////////////

  G4String window_name = sipm_name + "_WINDOW";

  G4double window_width  = sipm_width;
  G4double window_length = sipm_length;
  G4double window_thickn = 0.5 * mm;
  G4double window_zpos   = sipm_thickn/2. - window_thickn/2.;

  G4Material* optical_silicone = MaterialsList::OpticalSilicone();
  optical_silicone->SetMaterialPropertiesTable(OpticalMaterialProperties::GlassEpoxy());

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


  // SENSITIVE DETECTOR ////////////////////////////////////

  PmtSD* sensdet = new PmtSD("/" + sipm_name + "/");
  // sensdet->SetDetectorVolumeDepth(sensor_depth_);
  // sensdet->SetMotherVolumeDepth(mother_depth_);
  // sensdet->SetDetectorNamingOrder(naming_order_);
  // sensdet->SetTimeBinning(time_binning_);

  G4SDManager::GetSDMpointer()->AddNewDetector(sensdet);
  window_logic_vol->SetSensitiveDetector(sensdet);


  // VISIBILITY ////////////////////////////////////////////

  if (true) {
    sipm_logic_vol  ->SetVisAttributes(G4VisAttributes::Invisible);
    window_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes red = Red();
    red.SetForceSolid(true);
    sens_logic_vol->SetVisAttributes(red);
  }
  else {
    sipm_logic_vol  ->SetVisAttributes(G4VisAttributes::Invisible);
    window_logic_vol->SetVisAttributes(G4VisAttributes::Invisible);
    sens_logic_vol  ->SetVisAttributes(G4VisAttributes::Invisible);
  }
}
