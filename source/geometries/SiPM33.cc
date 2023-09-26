// ----------------------------------------------------------------------------
// nexus | SiPM33.cc
//
// Geometry of a Hamamatsu 3x3 mm2 SiPM.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SiPM33.h"
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

  SiPM33::SiPM33(): GeometryBase(),
		    visibility_(1)

  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    msg_->DeclareProperty("SiPM33_vis", visibility_, "SiPM33 Visibility");
  }



  SiPM33::~SiPM33()
  {
  }



  G4ThreeVector SiPM33::GetDimensions() const
  {
    return dimensions_;
  }



  void SiPM33::Construct()
  {
    // PACKAGE ///////////////////////////////////////////////////////

    G4double sipm_x = (6.55 )* mm;
    G4double sipm_y = (5.9 ) * mm;
    G4double sipm_z = 2 * mm;

    dimensions_.setX(sipm_x);
    dimensions_.setY(sipm_y);
    dimensions_.setZ(sipm_z);

    G4Box* sipm_solid = new G4Box("SIPM33", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* silicone = materials::OpticalSilicone();
    silicone->SetMaterialPropertiesTable(opticalprops::OptCoupler());

    G4LogicalVolume* sipm_logic =
      new G4LogicalVolume(sipm_solid, silicone, "SIPM33");

    this->SetLogicalVolume(sipm_logic);


    // PCB ///////////////////////////////////////////////////////

    G4double pcb_z = 2 * mm;

    G4Material* plastic = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYCARBONATE");



    G4double epoxy_z = 0.45 * 2 * mm;
    G4Box* plastic_solid = new G4Box("PLASTIC", sipm_x/2., sipm_y/2., pcb_z/2 - epoxy_z / 4);

    G4LogicalVolume* plastic_logic =
    new G4LogicalVolume(plastic_solid, silicone, "PLASTIC");
    new G4PVPlacement(0, G4ThreeVector(0, 0., epoxy_z/4), plastic_logic,
		      "PLASTIC", sipm_logic, false, 0, false);

    // ACTIVE WINDOW /////////////////////////////////////////////////

    G4double active_side     = 3.0   * mm;
    G4double active_depth    = 0.01   * mm;
    G4double active_offset_x = 1.75 * mm;

    G4Box* active_solid =
      new G4Box("PHOTODIODES", active_side/2., active_side/2., active_depth/2);

    G4Material* silicon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    G4double pos_x = - sipm_x/2. + active_offset_x + active_side/2.;
    G4double pos_z = epoxy_z/2. - active_depth/2. - pcb_z/2;

    new G4PVPlacement(0, G4ThreeVector(pos_x, 0., pos_z), active_logic,
		      "PHOTODIODES", sipm_logic, false, 0, false);


    // OPTICAL SURFACES //////////////////////////////////////////////

    // SiPM efficiency set using the official Hamamatsu specs.

    const G4int entries = 10;

    G4double energies[entries]     = {
          h_Planck * c_light / (895.04 * nm), h_Planck * c_light / (714.05 * nm),
          h_Planck * c_light / (573.55 * nm), h_Planck * c_light / (467.77 * nm),
          h_Planck * c_light / (396.69 * nm), h_Planck * c_light / (357.02 * nm),
          h_Planck * c_light / (342.15 * nm), h_Planck * c_light / (321.49 * nm),
          h_Planck * c_light / (290.08 * nm), h_Planck * c_light / (277.69 * nm),
		};
    G4double reflectivity[entries] = {
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0
    };
    G4double efficiency[entries]   = {
      0.0237, 0.081,
      0.1881, 0.2521,
      0.2284, 0.1814,
      0.1809, 0.1649,
      0.1149, 0.0247

      // 1., 1.,
      // 1., 1.,
      // 1., 1.,
      // 1., 1.,
      // 1., 1.
    };

    G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
    sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
    sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

    G4OpticalSurface* sipm_opsurf =
      new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
    sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);

    new G4LogicalSkinSurface("SIPM_OPSURF", active_logic, sipm_opsurf);


    // SENSITIVE DETECTOR ////////////////////////////////////////////

    G4String sdname = "/SIPM33/SiPM";
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

    if (!sdmgr->FindSensitiveDetector(sdname, false)) {
      SensorSD* sipmsd = new SensorSD(sdname);
      sipmsd->SetDetectorVolumeDepth(1);
      sipmsd->SetDetectorNamingOrder(1000.);
      sipmsd->SetTimeBinning(10.*ns);
      sipmsd->SetMotherVolumeDepth(2);

      G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
      active_logic->SetSensitiveDetector(sipmsd);
    }

    // Visibilities
    if (visibility_) {
       G4VisAttributes sipm_col = nexus::WhiteAlpha();
      sipm_logic->SetVisAttributes(sipm_col);
      G4VisAttributes blue_col = nexus::DarkGreen();
      blue_col.SetForceSolid(true);
      active_logic->SetVisAttributes(blue_col);
      G4VisAttributes plastic_col = nexus::RedAlpha();
      plastic_col.SetForceSolid(true);
      plastic_logic->SetVisAttributes(plastic_col);
    }
    else {
      sipm_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      active_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
      plastic_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    }
  }


} // end namespace nexus
