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
#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>

#include <stdexcept>

namespace nexus {

  FullRing::FullRing():
    BaseGeometry(),
    // Detector dimensions
    det_thickness_(0.*mm),
    n_modules_(24),
    z_size_(3.*cm),
    ring_diameter_(25.*cm)
  {
     // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/FullRing/", "Control commands of geometry FullRing.");

    // z size
     G4GenericMessenger::Command& zsize_cmd = 
       msg_->DeclareProperty("z_size", z_size_, "z dimension");
     zsize_cmd.SetUnitCategory("Length");
     zsize_cmd.SetParameterName("z_size", false);
     zsize_cmd.SetRange("z_size>0.");

     // Number of modules of the ring
     msg_->DeclareProperty("n_modules", n_modules_, "number of modules");

     // diameter of the ring
     G4GenericMessenger::Command& diameter_cmd = 
       msg_->DeclareProperty("ring_diameter", ring_diameter_, "ring diameter");
     diameter_cmd.SetUnitCategory("Length");
     diameter_cmd.SetParameterName("ring_diameter", false);
     diameter_cmd.SetRange("ring_diameter>0.");

     module_ = new PetaloTrap();
     kdb_ = new PetKDBFixedPitch();

     phantom_diam_ = 12.*cm;
     phantom_length_ = 10.*cm;

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
  
    // BuildDetector();
    //   BuildOneModule();

     // Dimensions
    G4double size1 = 2.*ring_diameter_/2.*tan(pi/n_modules_);
    G4double size2 = size1 + 2.*z_size_*tan(pi/n_modules_);
    G4cout << size1 << ", vs " << size2 << G4endl;

    
    module_->SetParameters(size1, size2, z_size_);
    module_->Construct();

    //    PmtR7378A pmt;
    //   pmt.Construct();
    //    G4LogicalVolume* module_logic = pmt.GetLogicalVolume();
    G4LogicalVolume* module_logic = module_->GetLogicalVolume();
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), module_logic,
     		      "MODULE", lab_logic_, false, 0, true);   
  }

  void FullRing::BuildDetector()
  {

    // Dimensions
    G4double size1 = 2.*ring_diameter_/2.*tan(pi/n_modules_);
    G4double size2 = size1 + 2.*z_size_*tan(pi/n_modules_);
    G4cout << size1 << ", vs " << size2 << G4endl;

    
    module_->SetParameters(size1, size2, z_size_);
    module_->Construct();

    //    PmtR7378A pmt;
    //   pmt.Construct();
    //    G4LogicalVolume* module_logic = pmt.GetLogicalVolume();
    G4LogicalVolume* module_logic = module_->GetLogicalVolume();

    G4double step = 2.*pi/n_modules_;
    G4double radius = ring_diameter_/2.+z_size_/2.+det_thickness_;
    G4ThreeVector position(0.,radius, 0.);
    G4RotationMatrix rot;
    rot.rotateX(-pi/2.);
    // The first must be positioned outside the loop
    new G4PVPlacement(G4Transform3D(rot, position), module_logic,
		      "MODULE0", lab_logic_, false, 0, true);   

    for (G4int i=1; i<n_modules_;++i) {
      G4double angle = i*step;
      rot.rotateZ(step);
      position.setX(-radius*sin(angle));
      position.setY(radius*cos(angle));
      G4String vol_name = "MODULE" + i;
      new G4PVPlacement(G4Transform3D(rot, position), module_logic,
		      vol_name, lab_logic_, false, i, true);   
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

 void FullRing::BuildOneModule()
  {

     // Dimensions
    G4double size1 = 2.*ring_diameter_/2.*tan(pi/n_modules_);
    G4double size2 = size1 + 2.*z_size_*tan(pi/n_modules_);
    G4cout << size1 << ", vs " << size2 << G4endl;

    
    module_->SetParameters(size1, size2, z_size_);
    module_->Construct();

    //    PmtR7378A pmt;
    //   pmt.Construct();
    //    G4LogicalVolume* module_logic = pmt.GetLogicalVolume();
    G4LogicalVolume* module_logic = module_->GetLogicalVolume();
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), module_logic,
     		      "MODULE", lab_logic_, false, 0, true);   


    kdb_->SetXYsize(size1);
    kdb_->Construct();
    G4double db_z = kdb_->GetDimensions().z();
    
   G4double displ = (z_size_-det_thickness_)/2. + db_z/2.;

   G4LogicalVolume* db_logic = kdb_->GetLogicalVolume();
   // new G4PVPlacement(0, G4ThreeVector(0.,0., -displ), db_logic,
   // 		     "LXE_DICE", lab_logic_, false, 0, true);

  }

  

  G4ThreeVector FullRing::GenerateVertex(const G4String& region) const
  {
   
    G4ThreeVector vertex(0.,0.,0.);

    // ACTIVE
    // if (region == "ACTIVE") {
    //   vertex = active_gen_->GenerateVertex(region);
    // } else if (region == "OUTSIDE") {
    //   vertex = G4ThreeVector(0., 0., -20.*cm);
    // } else if (region == "CENTER") {
    //   vertex = G4ThreeVector(0., 5.*mm, -5.*mm);
    // } else if (region == "SURFACE") {
    //   vertex = surf_gen_->GenerateVertex("Z_SURF");
    // }

    if (region == "PHANTOM") {
      vertex = cylindric_gen_->GenerateVertex("BODY_VOL");
    } else {
      G4Exception("[FullRing]", "GenerateVertex()", FatalException,
                  "Unknown vertex generation region!");     
    }
    
    return vertex;
  }

}
