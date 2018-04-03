#include "QuadFBK.h"
#include "PmtSD.h"
#include "ToFSD.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"
#include "SiPMpetFBK.h"

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
                      visibility_(0)
  {
    /// Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/QuadFBK/", "Control commands of geometry.");
    msg_->DeclareProperty("visibility", visibility_, "SiPMpet Visibility");


    sipm_ = new SiPMpetFBK();
  }
  
  QuadFBK::~QuadFBK()
  {
  }  
  
  void QuadFBK::Construct()
  {

    // FBK quad
    G4double quad_x = 5.9 * mm;
    G4double quad_y = 5.5 * mm;
    G4double quad_z = 1.50 * mm; 
    
    SetDimensions(G4ThreeVector(quad_x, quad_y, quad_z));   

    G4Box* quad_solid = new G4Box("QUAD", quad_x/2., quad_y/2., quad_z/2);

    G4Material* epoxy = MaterialsList::Epoxy();
    // G4cout << "Epoxy used with constant refraction index = " <<  refr_index_ << G4endl;
    //epoxy->SetMaterialPropertiesTable(OpticalMaterialProperties::EpoxyFixedRefr(refr_index_));
    
    G4LogicalVolume* quad_logic = 
      new G4LogicalVolume(quad_solid, epoxy, "QUAD");

    this->SetLogicalVolume(quad_logic);

    sipm_->Construct();
    G4ThreeVector sipm_dim = sipm_->GetDimensions();

    G4LogicalVolume* sipm_logic = sipm_->GetLogicalVolume();
    
    new G4PVPlacement(0, G4ThreeVector(-quad_x/2. + sipm_dim.x()/2., -quad_y/2. + sipm_dim.y()/2.,
                                   quad_z/2. - sipm_dim.z()/2.),
                  sipm_logic, "SiPMpetFBK", quad_logic, false, 0, true);
    new G4PVPlacement(0, G4ThreeVector(quad_x/2. - sipm_dim.x()/2., -quad_y/2. + sipm_dim.y()/2.,
                                   quad_z/2. - sipm_dim.z()/2.),
                  sipm_logic, "SiPMpetFBK", quad_logic, false, 1, true);
    new G4PVPlacement(0, G4ThreeVector(-quad_x/2. + sipm_dim.x()/2., quad_y/2. - sipm_dim.y()/2.,
                                   quad_z/2. - sipm_dim.z()/2.),
                  sipm_logic, "SiPMpetFBK", quad_logic, false, 2, true);
    new G4PVPlacement(0, G4ThreeVector(quad_x/2. - sipm_dim.x()/2., quad_y/2. - sipm_dim.y()/2.,
                                   quad_z/2. - sipm_dim.z()/2.),
                  sipm_logic, "SiPMpetFBK", quad_logic, false, 3, true);
    

    // Visibilities
    if (visibility_) {
      G4VisAttributes sipm_col = nexus::Lilla();
      quad_logic->SetVisAttributes(sipm_col);
    }
    else {
      quad_logic->SetVisAttributes(G4VisAttributes::Invisible);

    }
  }
  
  
} // end namespace nexus
