// ----------------------------------------------------------------------------
// nexus | SiPMSensl.cc
//
// Geometry of the SensL SiPM used in the NEXT-WHITE detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SiPMSensl.h"
#include "PmtSD.h"
#include "MaterialsList.h"
#include <G4GenericMessenger.hh>
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

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

  SiPMSensl::SiPMSensl(): BaseGeometry(),
                          visibility_(1),
                          binning_(1.*microsecond)

  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/SiPMSensl/",
                                  "Control commands of SiPMSensl geometry.");
    msg_->DeclareProperty("visibility", visibility_, "SiPMSensl visibility");

    G4GenericMessenger::Command& bin_cmd =
      msg_->DeclareProperty("time_binning", binning_,
			    "Time binning of SensL SiPM");
    bin_cmd.SetUnitCategory("Time");
    bin_cmd.SetParameterName("time_binning", false);
    bin_cmd.SetRange("time_binning>0.");
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

    G4double sipm_x = 1.5 * mm;
    G4double sipm_y = 1.8 * mm;
    G4double sipm_z = 0.65 * mm;

    dimensions_.setX(sipm_x);
    dimensions_.setY(sipm_y);
    dimensions_.setZ(sipm_z);

    G4Box* sipm_solid = new G4Box("SIPMSensl", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* epoxy = MaterialsList::Epoxy();
    epoxy->SetMaterialPropertiesTable(OpticalMaterialProperties::GlassEpoxy());

    G4LogicalVolume* sipm_logic =
      new G4LogicalVolume(sipm_solid, epoxy, "SIPMSensl");

    this->SetLogicalVolume(sipm_logic);


    // SUPPORT ///////////////////////////////////////////////////////

    G4double support_x = 1.17 * mm;
    G4double support_y = 1.17 * mm;
    G4double support_z = 0.2 * mm;

    G4Material* plastic = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYCARBONATE");

    G4Box* plastic_solid = new G4Box("PLASTIC", support_x/2., support_y/2., support_z/2);

    G4LogicalVolume* plastic_logic =
    new G4LogicalVolume(plastic_solid, plastic, "PLASTIC");

    new G4PVPlacement(0, G4ThreeVector(0, 0., 0.), plastic_logic,
		      "PLASTIC", sipm_logic, false, 0, false);

    // ACTIVE WINDOW /////////////////////////////////////////////////

    G4double active_side     = 1.0   * mm;
    G4double active_depth    = 0.01   * mm;

    G4Box* active_solid =
      new G4Box("PHOTODIODES", active_side/2., active_side/2., active_depth/2);

    G4Material* silicon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    G4double pos_z = -support_z/2. - active_depth/2.;

    new G4PVPlacement(0, G4ThreeVector(0., 0., pos_z), active_logic,
		      "PHOTODIODES", sipm_logic, false, 0, false);


    // OPTICAL SURFACES //////////////////////////////////////////////

    // SiPM efficiency set using the official Hamamatsu specs.

    const G4int entries = 21;

    // G4double energies[entries] = {1.5498*eV, 1.5998*eV, 1.6531*eV, 1.763*eV,
    // 				  1.836*eV, 1.915*eV, 2.007*eV, 2.105*eV,
    // 				  2.156*eV, 2.25426*eV,
    // 				  2.47968*eV,
    // 				  2.563*eV,
    // 				      2.718*eV, 2.838*eV, 2.977*eV, 3.099*eV,
    // 				  3.30625*eV,
    // 				      3.5424*eV, 3.8149*eV,
    // 				      3.93601*eV };

   G4double energies[entries] = {1.54980241262*eV,1.59979603883*eV,
				 1.65312257346*eV,1.71012680013*eV,1.77120275727*eV,
				 1.8368028594*eV,1.90744912322*eV,1.98374708815*eV,
				 2.06640321682*eV,2.15624683494*eV,2.25425805471*eV,
				 2.36160367637*eV,2.47968386018*eV,2.61019353704*eV,
				 2.75520428909*eV,2.91727512963*eV,3.09960482523*eV,
				 3.30624514691*eV,3.54240551455*eV,3.81489824644*eV,
				 3.96749*eV};
    G4double reflectivity[entries] = {0.      ,0.      ,0.      ,0.      ,0.      ,
				      0.      ,0.      ,0.      ,0.      ,0.      ,
				      0.      ,0.      ,0.      ,0.      ,0.      ,
				      0.      ,0.      ,0.      ,0.      ,0.,
				       0. };
    G4double efficiency[entries]   = {0.036, 0.048,
				      0.06, 0.07, 0.09,
				      0.105, 0.12, 0.145,
				      0.17, 0.2, 0.235,
				      0.275, 0.32, 0.37,
				      0.42,  0.425, 0.415,
				      0.35,  0.315, 0.185,
				      0.06
    };
    // G4double efficiency[entries]   = {0.06, 0.08, 0.10, 0.14, 0.18, 0.20, 0.24, 0.32,  0.34,
    // 				      0.38, 0.54, 0.59,
    // 				      0.68, 0.71, 0.72, 0.69,
    // 				      0.59 ,
    // 				      0.52, 0.30,
    // 				      0.09};



    // G4double efficiency_red[entries];
    // for (G4int i=0; i<entries; ++i) {
    //   efficiency_red[i] = efficiency[i]*.6;
    // }

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
      PmtSD* sipmsd = new PmtSD(sdname);
      sipmsd->SetDetectorVolumeDepth(0);
      sipmsd->SetDetectorNamingOrder(1000.);
      sipmsd->SetTimeBinning(binning_);
      sipmsd->SetMotherVolumeDepth(1);

      G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
      sipm_logic->SetSensitiveDetector(sipmsd);
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
      sipm_logic->SetVisAttributes(G4VisAttributes::Invisible);
      active_logic->SetVisAttributes(G4VisAttributes::Invisible);
      plastic_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }


} // end namespace nexus
