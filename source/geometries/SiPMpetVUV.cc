// ----------------------------------------------------------------------------
// nexus | SiPMpetVUV.cc
//
// Basic 3x3 mm2 SiPM geometry without TPB coating.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SiPMpetVUV.h"
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
#include <G4PhysicalConstants.hh>

#include <CLHEP/Units/SystemOfUnits.h>


namespace nexus {

  using namespace CLHEP;

  SiPMpetVUV::SiPMpetVUV(): BaseGeometry(),
			    visibility_(0),
			    refr_index_(1),
			    eff_(1.),
                            time_binning_(1.*microsecond)
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/SiPMpet/", "Control commands of geometry.");
    msg_->DeclareProperty("visibility", visibility_, "SiPMpet Visibility");
    msg_->DeclareProperty("refr_index", refr_index_, "Refraction index for epoxy");
    msg_->DeclareProperty("efficiency", eff_, "Efficiency of SiPM");

    G4GenericMessenger::Command& time_cmd =
      msg_->DeclareProperty("time_binning", time_binning_, "Time binning for the sensor");
    time_cmd.SetUnitCategory("Time");
    time_cmd.SetParameterName("time_binning", false);
    time_cmd.SetRange("time_binning>0.");
  }

  SiPMpetVUV::~SiPMpetVUV()
  {
  }

  void SiPMpetVUV::Construct()
  {

    // PACKAGE ///////////////////////////////////////////////////////

    // Hamamatsu 3X3MM-50UM VUV3
    // G4double sipm_x = 6.5* mm;
    // G4double sipm_y = 6.5 * mm;
    // G4double sipm_z = 1.55 * mm;

    // array-style
    G4double sipm_x = 3.* mm;
    G4double sipm_y = 3. * mm;
    G4double sipm_z = 1.55 * mm;

    SetDimensions(G4ThreeVector(sipm_x, sipm_y, sipm_z));

    G4Box* sipm_solid = new G4Box("SIPMpet", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* epoxy = MaterialsList::Epoxy();
    G4cout << "Epoxy used with constant refraction index = " <<  refr_index_ << G4endl;
    epoxy->SetMaterialPropertiesTable(OpticalMaterialProperties::EpoxyFixedRefr(refr_index_));

    G4LogicalVolume* sipm_logic =
      new G4LogicalVolume(sipm_solid, epoxy, "SIPMpet");

    this->SetLogicalVolume(sipm_logic);


    // PCB ///////////////////////////////////////////////////////

    // G4double pcb_z = 0.550 * mm;

    // G4Material* plastic = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYCARBONATE");

    // G4Box* plastic_solid = new G4Box("PLASTIC", sipm_x/2., sipm_y/2., pcb_z/2);

    // G4LogicalVolume* plastic_logic =
    // new G4LogicalVolume(plastic_solid, plastic, "PLASTIC");

    // G4double epoxy_z = 0.300 * mm;

    // new G4PVPlacement(0, G4ThreeVector(0, 0., epoxy_z/2), plastic_logic,
    // 		      "PLASTIC", sipm_logic, false, 0, false);

    // ACTIVE WINDOW /////////////////////////////////////////////////

    G4double active_side     = 3.0   * mm;
    G4double active_depth    = 0.01   * mm;

    G4Box* active_solid =
      new G4Box("PHOTODIODES", active_side/2., active_side/2., active_depth/2);

    G4Material* silicon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    new G4PVPlacement(0, G4ThreeVector(0., 0., sipm_z/2. - active_depth/2. - .1*mm), active_logic,
		      "PHOTODIODES", sipm_logic, false, 0, true);


    // OPTICAL SURFACES //////////////////////////////////////////////

    // SiPM efficiency set using the official Hamamatsu specs.

    const G4int entries = 12;


    G4double energies[entries] = {1.5*eV, 6.19919*eV, 6.35814*eV, 6.52546*eV,
				   6.70182*eV, 6.88799*eV, 7.08479*eV,
				   7.29316*eV, 7.51417*eV, 7.74898*eV,
				   7.99895*eV, 8.26558*eV};
    G4double reflectivity[entries] = {0., 0., 0., 0.,
				      0., 0., 0.,
                                      0., 0., 0.,
				      0., 0.};
    G4double efficiency[entries]   = {eff_, eff_, eff_,
                                      eff_, eff_, eff_,
                                      eff_, eff_, eff_,
                                      eff_, eff_};


    G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
    //sipm_mt->AddProperty("EFFICIENCY", energies, efficiency_red, entries);
    sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
    sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

    G4OpticalSurface* sipm_opsurf =
      new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
    sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);

    new G4LogicalSkinSurface("SIPM_OPSURF", active_logic, sipm_opsurf);


    // SENSITIVE DETECTOR ////////////////////////////////////////////

    G4String sdname = "/SIPM/SiPMpetVUV";
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

    if (!sdmgr->FindSensitiveDetector(sdname, false)) {
      PmtSD* sipmsd = new PmtSD(sdname);
      sipmsd->SetDetectorVolumeDepth(0);
      sipmsd->SetDetectorNamingOrder(1000.);
      sipmsd->SetTimeBinning(5.*picosecond);
      //sipmsd->SetMotherVolumeDepth(1);
      //     sipmsd->SetGrandMotherVolumeDepth(3);
      sipmsd->SetMotherVolumeDepth(2);
      //sipmsd->SetGrandMotherVolumeDepth(2);

      G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
      sipm_logic->SetSensitiveDetector(sipmsd);
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
