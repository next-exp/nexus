// ----------------------------------------------------------------------------
// nexus | SiPMpet.cc
//
// Basic SiPM geometry with TPB coating.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SiPMpet.h"
#include "PmtSD.h"
#include "ToFSD.h"
#include "MaterialsList.h"
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
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>


namespace nexus {

  using namespace CLHEP;

  SiPMpet::SiPMpet(): BaseGeometry(),
                      visibility_(0),
                      time_binning_(1.*microsecond),
                      box_geom_(false)

  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/SiPMpet/", "Control commands of geometry.");
    msg_->DeclareProperty("SiPMpet_vis", visibility_, "SiPMpet Visibility");

    G4GenericMessenger::Command& time_cmd =
      msg_->DeclareProperty("time_binning", time_binning_, "Time binning for the sensor");
    time_cmd.SetUnitCategory("Time");
    time_cmd.SetParameterName("time_binning", false);
    time_cmd.SetRange("time_binning>0.");

    msg_->DeclareProperty("box_geom", box_geom_,
      "To indicate whether Box geometry is being used and so the sensor id correctly set.");
  }



  SiPMpet::~SiPMpet()
  {
  }



  G4ThreeVector SiPMpet::GetDimensions() const
  {
    return dimensions_;
  }



  void SiPMpet::Construct()
  {
    // PACKAGE ///////////////////////////////////////////////////////

    G4double sipm_x = 6.2 * mm;
    G4double sipm_y = 6.2 * mm;
    G4double sipm_z = 0.850 * mm;

    dimensions_.setX(sipm_x);
    dimensions_.setY(sipm_y);
    dimensions_.setZ(sipm_z);

    G4Box* sipm_solid = new G4Box("SIPMpet", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* epoxy = MaterialsList::Epoxy();
    epoxy->SetMaterialPropertiesTable(OpticalMaterialProperties::GlassEpoxy());

    G4LogicalVolume* sipm_logic =
      new G4LogicalVolume(sipm_solid, epoxy, "SIPMpet");

    this->SetLogicalVolume(sipm_logic);

    // TPB coating
    G4double tpb_z = 0.001 * mm;
    G4Box* tpb_solid = new G4Box("TPB", sipm_x/2., sipm_y/2., tpb_z/2);
    G4Material* TPB = MaterialsList::TPB();
    TPB->SetMaterialPropertiesTable(OpticalMaterialProperties::TPB_LXe());
    G4LogicalVolume* tpb_logic =
      new G4LogicalVolume(tpb_solid, TPB, "TPB");

    G4double pos_z = (sipm_z - tpb_z) / 2.;

    new G4PVPlacement(0, G4ThreeVector(0.,0.,pos_z), tpb_logic,
                      "TPB", sipm_logic, false, 0, true);
    //   G4cout << "TPB starts in " << pos_z - tpb_z/2 << " in the ref system of SiPM" << G4endl;

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

    G4double active_side     = 6.0   * mm;
    G4double active_depth    = 0.01   * mm;

    G4Box* active_solid =
      new G4Box("PHOTODIODES", active_side/2., active_side/2., active_depth/2);

    G4Material* silicon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), active_logic,
		      "PHOTODIODES", sipm_logic, false, 0, false);


    // OPTICAL SURFACES //////////////////////////////////////////////

    // SiPM efficiency set using the official Hamamatsu specs.

    const G4int entries = 21;


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
                                      0.06};


    G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
    sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
    sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

    G4OpticalSurface* sipm_opsurf =
      new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
    sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);

    new G4LogicalSkinSurface("SIPM_OPSURF", active_logic, sipm_opsurf);


    // SENSITIVE DETECTOR ////////////////////////////////////////////

    G4String sdname = "/SIPMpet/SiPM";
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

    if (!sdmgr->FindSensitiveDetector(sdname, false)) {
      //      PmtSD* sipmsd = new PmtSD(sdname);
      ToFSD* sipmsd = new ToFSD(sdname);
      sipmsd->SetDetectorVolumeDepth(0);
      sipmsd->SetDetectorNamingOrder(1000.);
      sipmsd->SetTimeBinning(time_binning_);
      sipmsd->SetMotherVolumeDepth(1);

      G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
      sipm_logic->SetSensitiveDetector(sipmsd);
    }

    // Visibilities
    if (visibility_) {
      G4VisAttributes red_col = nexus::Red();
      sipm_logic->SetVisAttributes(red_col);

      G4VisAttributes active_col(G4Colour(.6, .8, .79));
      active_col.SetForceSolid(true);
      active_logic->SetVisAttributes(active_col);
    }
    else {
      sipm_logic->SetVisAttributes(G4VisAttributes::Invisible);
      active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }


} // end namespace nexus
