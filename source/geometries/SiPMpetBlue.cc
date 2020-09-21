// ----------------------------------------------------------------------------
// nexus | SiPMpetBlue.cc
//
// 6x6 mm2 Hamamatsu Blue SiPM geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SiPMpetBlue.h"
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

  SiPMpetBlue::SiPMpetBlue(): BaseGeometry(),
			      visibility_(0),
			      refr_index_(1.55), //given by Hammamatsu datasheet
                              eff_(1.),
                              sensor_depth_(-1),
                              mother_depth_(-1),
                              naming_order_(-1),
                              time_binning_(200.*nanosecond),
                              box_geom_(false)

  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/SiPMpet/", "Control commands of geometry.");
    msg_->DeclareProperty("visibility", visibility_, "SiPMpet Visibility");
    msg_->DeclareProperty("refr_index", refr_index_, "Refraction index for epoxy");
    msg_->DeclareProperty("efficiency", eff_, "Efficiency of SiPM");

    msg_->DeclareProperty("naming_order", naming_order_, "To start numbering at different place than zero.");

    G4GenericMessenger::Command& time_cmd =
      msg_->DeclareProperty("time_binning", time_binning_, "Time binning for the sensor");
    time_cmd.SetUnitCategory("Time");
    time_cmd.SetParameterName("time_binning", false);
    time_cmd.SetRange("time_binning>0.");

    msg_->DeclareProperty("box_geom", box_geom_, "To indicate whether Box geometry is being used and so naming ordering correctly set.");

  }

  SiPMpetBlue::~SiPMpetBlue()
  {
  }

  void SiPMpetBlue::Construct()
  {

    // PACKAGE ///////////////////////////////////////////////////////
    G4double sipm_x = 6.0 * mm;
    G4double sipm_y = 6.0 * mm;
    G4double sipm_z = 0.6 * mm;

    SetDimensions(G4ThreeVector(sipm_x, sipm_y, sipm_z));

    G4Box* sipm_solid = new G4Box("SiPMpetBlue", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* plastic = MaterialsList::FR4();
    G4LogicalVolume* sipm_logic =
      new G4LogicalVolume(sipm_solid, plastic, "SiPMpetBlue");

    this->SetLogicalVolume(sipm_logic);


    // EPOXY PROTECTIVE LAYER ////////////////////////////////////////

    G4double epoxy_depth = 0.1 * mm;

    G4Box* epoxy_solid =
      new G4Box("Epoxy", sipm_x/2., sipm_y/2., epoxy_depth/2);

    G4Material* epoxy = MaterialsList::Epoxy();
      epoxy->SetMaterialPropertiesTable(OpticalMaterialProperties::EpoxyFixedRefr(refr_index_));

    G4LogicalVolume* epoxy_logic =
      new G4LogicalVolume(epoxy_solid, epoxy, "Epoxy");

    new G4PVPlacement(0, G4ThreeVector(0., 0., sipm_z/2. - epoxy_depth/2.),
    epoxy_logic, "Epoxy", sipm_logic, false, 0, true);


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

    new G4PVPlacement(0, G4ThreeVector(0., 0., sipm_z/2. - epoxy_depth - active_depth/2.),
                      active_logic, "PHOTODIODES", sipm_logic, false, 0, true);


    // OPTICAL SURFACES /////////////////////////////////////////////

    const G4int entries = 17;
    G4double energies[entries] = {1.37760*eV, 1.54980*eV,
                                  1.79687*eV, 1.90745*eV, 1.99974*eV,
                                  2.06640*eV, 2.21400*eV, 2.47968*eV,
                                  2.75520*eV, 2.91727*eV, 3.09960*eV,
                                  3.22036*eV, 3.44400*eV, 3.54240*eV,
                                  3.62526*eV, 3.73446*eV, 3.87450*eV};

    G4double reflectivity[entries] = {0., 0., 0., 0., 0.,
                                      0., 0., 0., 0., 0.,
                                      0., 0., 0., 0., 0.,
                                      0., 0.};

    G4double efficiency[entries] = {0.0445, 0.1045, 0.208 ,
                                    0.261 , 0.314 , 0.3435,
                                    0.420 , 0.505 , 0.528 ,
                                    0.502 , 0.460 , 0.4195,
                                    0.3145, 0.2625, 0.211 ,
                                    0.1055, 0.026};

    G4MaterialPropertiesTable* sipm_mt = new G4MaterialPropertiesTable();
    sipm_mt->AddProperty("EFFICIENCY", energies, efficiency, entries);
    sipm_mt->AddProperty("REFLECTIVITY", energies, reflectivity, entries);

    G4OpticalSurface* sipm_opsurf =
      new G4OpticalSurface("SIPM_OPSURF", unified, polished, dielectric_metal);
    sipm_opsurf->SetMaterialPropertiesTable(sipm_mt);

    new G4LogicalSkinSurface("SIPM_OPSURF", active_logic, sipm_opsurf);


    // SENSITIVE DETECTOR ////////////////////////////////////////////

    G4String sdname = "/SIPM/SiPMpetBlue";
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();

    if (!sdmgr->FindSensitiveDetector(sdname, false)) {
      ToFSD* sipmsd = new ToFSD(sdname);
      if (sensor_depth_ == -1)
        G4Exception("[SiPMpetBlue]", "Construct()", FatalException,
                    "Sensor Depth must be set before constructing");
      sipmsd->SetDetectorVolumeDepth(sensor_depth_);

      if (mother_depth_ == -1)
        G4Exception("[SiPMpetBlue]", "Construct()", FatalException,
                    "Mother Depth must be set before constructing");
      sipmsd->SetMotherVolumeDepth(mother_depth_);

      if (naming_order_ == -1)
        G4Exception("[SiPMpetBlue]", "Construct()", FatalException,
                    "Naming Order must be set before constructing");
      sipmsd->SetDetectorNamingOrder(naming_order_);
      sipmsd->SetTimeBinning(time_binning_);

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
      G4VisAttributes epoxy_col = nexus::Red();
      //epoxy_col.SetForceSolid(true);
      epoxy_logic->SetVisAttributes(epoxy_col);
    }
    else {
      sipm_logic->SetVisAttributes(G4VisAttributes::Invisible);
      active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }


} // end namespace nexus
