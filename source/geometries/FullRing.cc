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

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>

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
  
    BuildDetector();
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
		      "TRAP", lab_logic_, false, 0, true);   

    for (G4int i=1; i<n_modules_;++i) {
      G4double angle = i*step;
      rot.rotateZ(step);
      position.setX(-radius*sin(angle));
      position.setY(radius*cos(angle));
      new G4PVPlacement(G4Transform3D(rot, position), module_logic,
		      "TRAP", lab_logic_, false, i, true);   
    }
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
    
    return vertex;
  }

}
