// ----------------------------------------------------------------------------
// nexus | SiPMpetVUV.cc
//
// Variable size SiPM geometry with no wavelength shifter
// and a window with perfect transparency and configurable refractive index.
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


namespace nexus {

  using namespace CLHEP;

  SiPMpetVUV::SiPMpetVUV(): BaseGeometry(),
			    visibility_(0),
			    refr_index_(1.6),
			    eff_(1.),
                            time_binning_(1.*microsecond),
                            sensor_depth_(-1),
			    mother_depth_(0),
                            naming_order_(0)
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/SiPMpet/", "Control commands of geometry.");
    msg_->DeclareProperty("visibility", visibility_, "SiPMpet Visibility");
    msg_->DeclareProperty("refr_index", refr_index_, "Refraction index for sipm window");
    msg_->DeclareProperty("efficiency", eff_, "Efficiency of SiPM");

    G4GenericMessenger::Command& time_cmd =
      msg_->DeclareProperty("time_binning", time_binning_, "Time binning for the sensor");
    time_cmd.SetUnitCategory("Time");
    time_cmd.SetParameterName("time_binning", false);
    time_cmd.SetRange("time_binning>0.");

    G4GenericMessenger::Command& size_cmd =
      msg_->DeclareProperty("size", sipm_size_, "Size of SiPMs");
    size_cmd.SetUnitCategory("Length");
    size_cmd.SetParameterName("size", false);
    size_cmd.SetRange("size>0.");
  }

  SiPMpetVUV::~SiPMpetVUV()
  {
  }

  void SiPMpetVUV::Construct()
  {
    G4double sipm_x = sipm_size_;
    G4double sipm_y = sipm_size_;
    G4double sipm_z = 0.7 * mm;

    SetDimensions(G4ThreeVector(sipm_x, sipm_y, sipm_z));

    G4Box* sipm_solid = new G4Box("SIPMpet", sipm_x/2., sipm_y/2., sipm_z/2);

    G4Material* sipm_mat = MaterialsList::FR4();
    G4LogicalVolume* sipm_logic =
      new G4LogicalVolume(sipm_solid, sipm_mat, "SIPMpet");

    G4Material* window_mat = MaterialsList::FusedSilica();
    G4cout << "Quartz used with constant refraction index = " <<  refr_index_ << G4endl;
    window_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::FakeGenericMaterial(refr_index_));

    this->SetLogicalVolume(sipm_logic);

    
    // QUARTZ WINDOW
    
    G4double window_thickness = 0.6 * mm;
    G4Box* window_solid_vol =
    new G4Box("SIPM_WNDW", sipm_size_/2., sipm_size_/2., window_thickness/2.);

    G4LogicalVolume* window_logic_vol =
      new G4LogicalVolume(window_solid_vol, window_mat, "SIPM_WNDW");
    
    G4double window_zpos = sipm_z/2. - window_thickness/2.;
    
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., window_zpos), window_logic_vol,
                      "SIPM_WNDW", sipm_logic, false, 0, true);

   
    // ACTIVE WINDOW /////////////////////////////////////////////////

    G4double active_x = sipm_x - 0.001 * mm;
    G4double active_y = sipm_y - 0.001 * mm;
    G4double active_depth    = 0.01   * mm;

    G4Box* active_solid =
      new G4Box("PHOTODIODES", active_x/2., active_y/2., active_depth/2);

    G4Material* silicon =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    new G4PVPlacement(0, G4ThreeVector(0., 0., sipm_z/2. - window_thickness - active_depth/2.),
                      active_logic, "PHOTODIODES", sipm_logic, false, 0, true);


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
      ToFSD* sipmsd = new ToFSD(sdname);

      if (sensor_depth_ == -1) 
        G4Exception("[SiPMpetVUV]", "Construct()", FatalException,
                    "Sensor depth must be set before constructing");
      sipmsd->SetDetectorVolumeDepth(sensor_depth_);
      sipmsd->SetMotherVolumeDepth(mother_depth_);
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
    }
    else {
      sipm_logic->SetVisAttributes(G4VisAttributes::Invisible);
      active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }


} // end namespace nexus
