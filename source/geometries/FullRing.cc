// ----------------------------------------------------------------------------
//  $Id: FullRing.cc  $
//
//  Author:  <paolafer@ific.uv.es>   
//  Created: January 2015
//  
//  Copyright (c) 2015 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "FullRing.h"
#include "PetaloTrap.h"
#include "PmtR7378A.h"
#include "CylinderPointSampler.h"
#include "MaterialsList.h"
#include "PetKDBFixedPitch.h"
#include "OpticalMaterialProperties.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <G4LogicalVolume.hh>

#include <stdexcept>

namespace nexus {

  FullRing::FullRing():
    BaseGeometry(),
    // Detector dimensions
    det_thickness_(0.*mm),
    n_modules_(12),
    r_dim_(5.*cm),
    internal_diam_(21.5*cm),
    cryo_width_(8.*cm),
    cryo_thickn_(1.*mm),
    n_cells_(12)
  {
     // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/FullRing/", "Control commands of geometry FullRing.");

    module_ = new PetaloTrap();
    //kdb_ = new PetKDBFixedPitch();

    phantom_diam_ = 12.*cm;
    phantom_length_ = 10.*cm;

    external_diam_ = internal_diam_ + 2.*r_dim_*tan(pi/n_cells_);
    
    cylindric_gen_ = 
      new CylinderPointSampler(0., phantom_length_, phantom_diam_/2., 0., G4ThreeVector (0., 0., 0.));
    
  }

  FullRing::~FullRing()
  {

  }

  void FullRing::Construct()
  { 
    // LAB. This is just a volume of air surrounding the detector
    G4double lab_size = 1.*m;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
    
    lab_logic_ = 
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic_);

    BuildCryostat();
    BuildDetector();
  }

  void FullRing::BuildCryostat()
  {
    const G4double int_diam_cryo = internal_diam_ - 2. * cm;
    const G4double ext_diam_cryo = external_diam_ + 10. * cm;

    G4Tubs* cryostat_solid =
      new G4Tubs("CRYOSTAT", int_diam_cryo/2., ext_diam_cryo/2., cryo_width_/2., 0, twopi);
    G4Material* steel = MaterialsList::Steel();
    G4LogicalVolume* cryostat_logic =
      new G4LogicalVolume(cryostat_solid, steel, "CRYOSTAT");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), cryostat_logic,
		      "CRYOSTAT", lab_logic_, false, 0, true);

    G4Tubs* LXe_solid =
      new G4Tubs("LXE", (int_diam_cryo + 2.*cryo_thickn_)/2., (ext_diam_cryo - 2.*cryo_thickn_)/2.,
                 (cryo_width_ - 2.*cryo_thickn_)/2., 0, twopi);
    G4Material* LXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    LXe->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    LXe_logic_ =
      new G4LogicalVolume(LXe_solid, steel, "LXE");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), LXe_logic_,
		      "LXE", cryostat_logic, false, 0, true);

  }

  void FullRing::BuildDetector()
  {

    // Dimensions
    // G4double size1 = 2.*ring_diameter_/2.*tan(pi/n_modules_);
    //G4double size2 = size1 + 2.*z_size_*tan(pi/n_modules_);
    //G4cout << "Dimensions: "<< size1 << " and " << size2 << G4endl;
  
    // module_->SetParameters(size1, size2, z_size_);
    module_->Construct();
    G4LogicalVolume* module_logic = module_->GetLogicalVolume();

    G4double step = 2.*pi/n_modules_;
    G4double radius = internal_diam_/2.+r_dim_/2.+det_thickness_;
    G4ThreeVector position(0.,radius, 0.);
    G4RotationMatrix rot;
    rot.rotateX(pi/2.);
    // The first must be positioned outside the loop
    new G4PVPlacement(G4Transform3D(rot, position), module_logic,
		      "MODULE_1", LXe_logic_, false, 1, true);

    for (G4int i=2; i<=n_modules_;++i) {
      G4double angle = (i-1)*step;
      rot.rotateZ(step);
      position.setX(-radius*sin(angle));
      position.setY(radius*cos(angle));
      G4String vol_name = "MODULE_" + std::to_string(i);
      new G4PVPlacement(G4Transform3D(rot, position), module_logic,
                        vol_name, LXe_logic_, false, i, true);
    }
  }

 void FullRing::BuildPhantom()
  {
     

    G4Tubs* phantom_solid = new G4Tubs("PHANTOM",  0., phantom_diam_/2., 
                 phantom_length_/2., 0, twopi);
    G4LogicalVolume* phantom_logic =  
      new G4LogicalVolume(phantom_solid, MaterialsList::PEEK(), "PHANTOM");
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), phantom_logic,
		       "PAHNTOM", lab_logic_, false, 0, true);   

 
  }
  

  G4ThreeVector FullRing::GenerateVertex(const G4String& region) const
  {
   
    G4ThreeVector vertex(0.,0.,0.);

    if (region == "CENTER") {
      vertex = vertex;
    } else if (region == "PHANTOM") {
      vertex = cylindric_gen_->GenerateVertex("BODY_VOL");
    } else {
      G4Exception("[FullRing]", "GenerateVertex()", FatalException,
                  "Unknown vertex generation region!");     
    }
    
    return vertex;
  }

}
