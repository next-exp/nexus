// ----------------------------------------------------------------------------
// nexus | SiPMSensl.cc
//
// Geometry of the SensL SiPM used in the NEXT-WHITE detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SiPMSensl.h"
#include "SensorSD.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4Box.hh>
#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>

#include <CLHEP/Units/SystemOfUnits.h>


namespace nexus {

  using namespace CLHEP;

  SiPMSensl::SiPMSensl():
    GeometryBase   (),
    time_binning_  (1.0 * us),
    sensor_depth_  (-1),
    mother_depth_  (0),
    naming_order_  (0),
    visibility_    (1)
  {
  }



  SiPMSensl::~SiPMSensl()
  {
  }



  G4ThreeVector SiPMSensl::GetDimensions() const
  {
    return dimensions_;
  }



  void SiPMSensl::Construct()
  {
    // PACKAGE ///////////////////////////////////////////////////////

    G4double sipm_x = 1.50 * mm;
    G4double sipm_y = 1.80 * mm;
    G4double sipm_z = 0.65 * mm;

    dimensions_.setX(sipm_x);
    dimensions_.setY(sipm_y);
    dimensions_.setZ(sipm_z);

    G4Box* sipm_solid = new G4Box("SIPMSensl", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* epoxy = materials::Epoxy();
    epoxy->SetMaterialPropertiesTable(opticalprops::GlassEpoxy());

    G4LogicalVolume* sipm_logic =
      new G4LogicalVolume(sipm_solid, epoxy, "SIPMSensl");

    this->SetLogicalVolume(sipm_logic);


    // SUPPORT ///////////////////////////////////////////////////////

    G4double support_x = 1.17 * mm;
    G4double support_y = 1.17 * mm;
    G4double support_z = 0.20 * mm;

    G4double support_pos_z = - sipm_z/2. + support_z/2. + 0.21 * mm;


    G4Material* plastic = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYCARBONATE");

    G4Box* plastic_solid = new G4Box("PLASTIC", support_x/2., support_y/2., support_z/2);

    G4LogicalVolume* plastic_logic =
    new G4LogicalVolume(plastic_solid, plastic, "PLASTIC");

    new G4PVPlacement(0, G4ThreeVector(0, 0., support_pos_z), plastic_logic,
		      "PLASTIC", sipm_logic, false, 0, false);

    // ACTIVE WINDOW /////////////////////////////////////////////////

    G4double active_side  = 1.0   * mm;
    G4double active_depth = 0.01  * mm;

    G4Box* active_solid =
      new G4Box("PHOTODIODES", active_side/2., active_side/2., active_depth/2);

    G4Material* silicon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    G4double pos_z = -support_z/2. + active_depth/2.;

    new G4PVPlacement(0, G4ThreeVector(0., 0., pos_z), active_logic,
		      "PHOTODIODES", plastic_logic, false, 0, false);


    // OPTICAL SURFACES //////////////////////////////////////////////

    const G4int entries = 21;

    G4double energies[entries] =
      {1.54980241262 * eV, 1.59979603883 * eV,
       1.65312257346 * eV, 1.71012680013 * eV, 1.77120275727 * eV,
       1.83680285940 * eV, 1.90744912322 * eV, 1.98374708815 * eV,
       2.06640321682 * eV, 2.15624683494 * eV, 2.25425805471 * eV,
       2.36160367637 * eV, 2.47968386018 * eV, 2.61019353704 * eV,
       2.75520428909 * eV, 2.91727512963 * eV, 3.09960482523 * eV,
       3.30624514691 * eV, 3.54240551455 * eV, 3.81489824644 * eV,
       3.96749000000 * eV };

    G4double reflectivity[entries] =
      {0.      ,0.      ,0.      ,0.      ,0.      ,
			 0.      ,0.      ,0.      ,0.      ,0.      ,
			 0.      ,0.      ,0.      ,0.      ,0.      ,
			 0.      ,0.      ,0.      ,0.      ,0.,
			 0. };

    G4double efficiency[entries] =
      {0.036, 0.048,
			 0.060, 0.070, 0.090,
			 0.105, 0.120, 0.145,
			 0.170, 0.200, 0.235,
			 0.275, 0.320, 0.370,
			 0.420, 0.425, 0.415,
			 0.350, 0.315, 0.185,
			 0.060 };

    G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
    sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
    sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

    G4OpticalSurface* sipm_opsurf =
      new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
    sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);

    new G4LogicalSkinSurface("SIPM_OPSURF", active_logic, sipm_opsurf);


    // SENSITIVE DETECTOR ////////////////////////////////////////////

    G4String sdname = "/SIPMSensl/SiPM";
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

    if (!sdmgr->FindSensitiveDetector(sdname, false)) {
      SensorSD* sipmsd = new SensorSD(sdname);

      if (sensor_depth_ == -1)
        G4Exception("[SiPMSensl]", "Construct()", FatalException,
                    "Sensor Depth must be set before constructing");

      if ((naming_order_ > 0) && (mother_depth_ == 0))
        G4Exception("[SiPMSensl]", "Construct()", FatalException,
                    "Naming Order set without setting Mother Depth");

      sipmsd->SetDetectorVolumeDepth(sensor_depth_);
      sipmsd->SetMotherVolumeDepth(mother_depth_);
      sipmsd->SetDetectorNamingOrder(naming_order_);
      sipmsd->SetTimeBinning(time_binning_);

      G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
      active_logic->SetSensitiveDetector(sipmsd);
    }

      // Visibilities
    if (visibility_) {
      G4VisAttributes sipm_col = nexus::DirtyWhite();
      sipm_logic->SetVisAttributes(sipm_col);
      G4VisAttributes blue_col = nexus::Blue();
      blue_col.SetForceSolid(true);
      active_logic->SetVisAttributes(blue_col);
      G4VisAttributes plastic_col = nexus::Lilla();
      plastic_col.SetForceSolid(true);
      plastic_logic->SetVisAttributes(plastic_col);
    } else {
      sipm_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      active_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      plastic_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }
  }

} // end namespace nexus
