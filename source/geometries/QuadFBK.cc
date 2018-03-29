#include "QuadFBK.h"
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
  
  QuadFBK::QuadFBK(): BaseGeometry(),
                      visibility_(0),
                      refr_index_(1.54),
                      eff_(1.),
                      time_binning_(200.*nanosecond)

  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/QuadFBK/", "Control commands of geometry.");
    msg_->DeclareProperty("visibility", visibility_, "SiPMpet Visibility");
    msg_->DeclareProperty("refr_index", refr_index_, "Refraction index for epoxy");
    msg_->DeclareProperty("efficiency", eff_, "Efficiency of SiPM");
    
    G4GenericMessenger::Command& time_cmd = 
      msg_->DeclareProperty("time_binning", time_binning_, "Time binning for the sensor");
    time_cmd.SetUnitCategory("Time");
    time_cmd.SetParameterName("time_binning", false);
    time_cmd.SetRange("time_binning>0.");
  }
  
  QuadFBK::~QuadFBK()
  {
  }  
  
  void QuadFBK::Construct()
  {
   
    // PACKAGE ///////////////////////////////////////////////////////
    
    // FBK quad
    G4double quad_x = 5.9 * mm;
    G4double quad_y = 5.5 * mm;
    G4double quad_z = 1.50 * mm; 
    
    SetDimensions(G4ThreeVector(quad_x, quad_y, quad_z));   

    G4Box* quad_solid = new G4Box("QUAD", quad_x/2., quad_y/2., quad_z/2);

    G4Material* epoxy = MaterialsList::Epoxy();
    G4cout << "Epoxy used with constant refraction index = " <<  refr_index_ << G4endl;
    epoxy->SetMaterialPropertiesTable(OpticalMaterialProperties::EpoxyFixedRefr(refr_index_));
    
    G4LogicalVolume* quad_logic = 
      new G4LogicalVolume(quad_solid, epoxy, "QUAD");

    this->SetLogicalVolume(quad_logic);


    // ACTIVE WINDOW /////////////////////////////////////////////////

    G4double active_x = quad_x;
    G4double active_y = quad_y;
    G4double active_depth = 0.01 * mm;
    
    G4Box* active_solid =
      new G4Box("PHOTODIODES", active_x/2., active_y/2., active_depth/2);
    
    G4Material* silicon = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, silicon, "PHOTODIODES");

    new G4PVPlacement(0, G4ThreeVector(0., 0., quad_z/2. - active_depth/2. - .1*mm),
                      active_logic, "PHOTODIODES", quad_logic, false, 0, true);
    
    
    // OPTICAL SURFACES //////////////////////////////////////////////
    
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

    G4String sdname = "/SIPM/QuadFBK";
    G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
    
    if (!sdmgr->FindSensitiveDetector(sdname, false)) {
      ToFSD* sipmsd = new ToFSD(sdname);
      sipmsd->SetDetectorVolumeDepth(0);
      sipmsd->SetDetectorNamingOrder(1000.);
      sipmsd->SetTimeBinning(time_binning_);
      //sipmsd->SetMotherVolumeDepth(2); // for Petit + PetitModule geometry
      sipmsd->SetMotherVolumeDepth(1); // for FullRingInfinity geometry
      
      G4SDManager::GetSDMpointer()->AddNewDetector(sipmsd);
      quad_logic->SetSensitiveDetector(sipmsd);
    }

    // Visibilities
    if (visibility_) {
      G4VisAttributes sipm_col = nexus::Yellow();
      quad_logic->SetVisAttributes(sipm_col);
      G4VisAttributes active_col = nexus::Blue();
      active_col.SetForceSolid(true);
      active_logic->SetVisAttributes(active_col);
    }
    else {
      quad_logic->SetVisAttributes(G4VisAttributes::Invisible);
      active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }
  
  
} // end namespace nexus
