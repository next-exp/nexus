// ----------------------------------------------------------------------------
//  $Id: PetaloTrapAlone.cc  $
//
//  Author:  <jmunoz@ific.uv.es>   
//  Created: January 2014
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "PetaloTrapAlone.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "PetKDB.h"
#include "PetKDBFixedPitch.h"
#include "PetPlainDice.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Trap.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>
#include <G4NistManager.hh>
#include <G4UniformMagField.hh>
#include <G4FieldManager.hh>
#include <G4TransportationManager.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4RotationMatrix.hh>


namespace nexus {

  PetaloTrapAlone::PetaloTrapAlone():
    BaseGeometry(),

    // Detector dimensions
    //    vacuum_thickn_(1.*mm),
    //   outer_wall_thickn_(3.*mm),
    det_thickness_(1.*mm),
    //  det_size_(20.*cm),
    active_size_ (5.*cm),
    z_size_(3.*cm),
    size1_(31.6312*mm),
    size2_(40.2579*mm)

  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetalX/", "Control commands of geometry PetalX.");

    // z size
     G4GenericMessenger::Command& zsize_cmd = 
       msg_->DeclareProperty("z_size", z_size_, "z dimension");
     zsize_cmd.SetUnitCategory("Length");
     zsize_cmd.SetParameterName("z_size", false);
     zsize_cmd.SetRange("z_size>0.");
   
    // Maximum Step Size
    G4GenericMessenger::Command& step_cmd = 
      msg_->DeclareProperty("max_step_size", max_step_size_, 
								  "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");

    //db_ = new PetKDB();
    db_ = new PetKDBFixedPitch();
    //    pdb_ = new PetPlainDice();
  }


  PetaloTrapAlone::~PetaloTrapAlone()
  {    
    delete active_gen_;
    delete surf_gen_;
  }


  void PetaloTrapAlone::Construct()
  {

    db_->SetXYsize( size1_);
    db_->Construct();
    db_->SetPitchSize(6.2*mm);

    // Vertex Generators
    active_gen_ = new BoxPointSampler(active_size_, active_size_, z_size_, 0.,
				      G4ThreeVector(0.,0.,0.) ,0);

    double zpos_generation = z_size_/2. + db_z_ + det_thickness_ + 1.*mm;
    surf_gen_ =  new BoxPointSampler(active_size_, active_size_, 0., 0., 
				     G4ThreeVector(0.,0., -zpos_generation) ,0);

    // LAB. This is just a volume of air surrounding the detector
    G4double lab_size = 1.*m;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
    
    lab_logic_ = 
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic_);

    lXe_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    lXe_->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
  
    BuildDetector();
    /*
    BuildLXe() ;
    BuildActive();
    */
    BuildSiPMPlane();
  }

  // void PetaloTrapAlone::BuildOuterWall() 
  // {
  //   G4Box* outwall_solid = 
  //     new G4Box("OUTER_WALL", (det_size_+2.*mm)/2., (det_size_+2.*mm)/2., (det_size_+2.*mm)/2.);

  //   G4Material* steel = MaterialsList::Steel();
    
  //   outwall_logic_ = new G4LogicalVolume(outwall_solid, steel, "OUTER_WALL");
  //   outwall_logic_->SetVisAttributes(G4VisAttributes::Invisible);
  //   new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), outwall_logic_,
  // 		      "OUTER_WALL", lab_logic_, false, 0, true);

  //     G4Box* vacuum_solid = 
  //     new G4Box("VACUUM", (det_size_+ 2.*mm - outer_wall_thick_*2)/2., (det_size_+2.*mm)/2., (det_size_+2.*mm)/2.);

  //   G4Material* steel = MaterialsList::Steel();
    
  //   outwall_logic_ = new G4LogicalVolume(outwall_solid, steel, "VACUUM");
  //   outwall_logic_->SetVisAttributes(G4VisAttributes::Invisible);
  //   new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), outwall_logic_,
  // 		      "VACUUM", lab_logic_, false, 0, true);
  // }

  void PetaloTrapAlone::BuildDetector() 
  {
    /*
    G4double det_size = active_size_ + 2.*db_z_ + 2.*det_thickness_;
    G4double det_size2 = z_size_+ 2.*db_z_ + 2.*det_thickness_;
    G4Box* det_solid = 
      //   new G4Box("WALL", det_size/2., det_size/2., det_size/2.);
      new G4Box("WALL", det_size/2., det_size/2., det_size2/2.);
    G4Material* steel = MaterialsList::Steel();
    
    det_logic_ = new G4LogicalVolume(det_solid, steel, "WALL");
    det_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), det_logic_,
		      "WALL", lab_logic_, false, 0, true);
    */

     G4double det_size1 = size1_;
    G4double det_size2 = size2_;
    G4cout << det_size1  << ", vs " << det_size2 << G4endl;
    G4Trap* det_solid = 
      //   new G4Box("WALL", det_size/2., det_size/2., det_size/2.);
      new G4Trap("TRAP", det_size1/2., det_size2/2., det_size1/2., det_size2/2., (z_size_)/2.);
   
    G4Material* steel = MaterialsList::Steel();
    
    det_logic_ = new G4LogicalVolume(det_solid, steel, "TRAP");
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), det_logic_,
		      "ACTIVE", lab_logic_, false, 0, true);
     

    G4Colour myColour(.6, .8, .79); 
    G4VisAttributes grey_color(myColour); 
    grey_color.SetForceSolid(true);
    // det_logic_->SetVisAttributes(grey_color);

    G4Trap* lxe_solid = 
      new G4Trap("ACTIVE", (size1_-2.*det_thickness_)/2., (size2_-2.*det_thickness_)/2., (size1_-2.*det_thickness_)/2., (size2_-2.*det_thickness_)/2., (z_size_-det_thickness_)/2.);
   
    G4Material* lXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    lXe->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    
    G4LogicalVolume* lxe_logic = new G4LogicalVolume(lxe_solid, lXe, "ACTIVE");
    // lxe_logic->SetVisAttributes(white_color);  
   
    // new G4PVPlacement(0, G4ThreeVector(0.,0.,-det_thickness_/2.), lxe_logic,
    // 		  "ACTIVE", det_logic_, false, 0, true);

    // Set the ACTIVE volume as an ionization sensitive active
    IonizationSD* ionisd = new IonizationSD("/PETALX/ACTIVE");
    lxe_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    std::cout << "*** Maximum Step Size (mm): " << max_step_size_/mm << std::endl;
    lxe_logic->SetUserLimits(new G4UserLimits(max_step_size_));
  }

 void PetaloTrapAlone::BuildLXe() 
  {
    G4double lXe_size = active_size_ + 2.*db_z_ ;
    G4double lXe_size2 = z_size_ + 2.*db_z_ ;
    G4Box* lXe_solid = 
      new G4Box("LXE", lXe_size/2., lXe_size/2., lXe_size2/2.);
    
    lXe_logic_ = new G4LogicalVolume(lXe_solid, lXe_, "LXE");
    lXe_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), lXe_logic_,
 		      "LXE", det_logic_, false, 0, true);
  }

  void PetaloTrapAlone::BuildActive() 
  {
    G4Box* active_solid = 
      new G4Box("ACTIVE", active_size_/2., active_size_/2., z_size_/2.);

    G4LogicalVolume* active_logic = new G4LogicalVolume(active_solid, lXe_, "ACTIVE");
    active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    // G4VisAttributes red_color;
    // red_color.SetColor(1., 0., 0.);
    // red_color.SetForceSolid(true);
    // active_logic->SetVisAttributes(red_color);

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic,
		      "ACTIVE", lXe_logic_, false, 0, true);
    
    // Set the ACTIVE volume as an ionization sensitive active
    IonizationSD* ionisd = new IonizationSD("/PETALX/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    std::cout << "*** Maximum Step Size (mm): " << max_step_size_/mm << std::endl;
    active_logic->SetUserLimits(new G4UserLimits(max_step_size_));

  }

  void PetaloTrapAlone::BuildSiPMPlane()
  {
    G4LogicalVolume* db_logic = db_->GetLogicalVolume();
    G4double db_z = db_->GetDimensions().z();
    G4double displ = (z_size_-det_thickness_)/2. + db_z/2.;

   
    new G4PVPlacement(0, G4ThreeVector(0.,0., -displ), db_logic,
		      "LXE_DICE", det_logic_, false, 0, true);

    
    db_->SetXYsize( size2_);
    db_->SetPitchSize(6.3*mm);
    db_->Construct();
    db_logic = db_->GetLogicalVolume();
    displ = (z_size_-det_thickness_)/2. + db_z/2.; 
    new G4PVPlacement(0, G4ThreeVector(0.,0., displ), db_logic,
		      "LXE_DICE", det_logic_, false, 1, true);
  
    
    /*
    pdb_->SetSize(z_size_, active_size_);
    pdb_->Construct();

    G4LogicalVolume* db_logic = db_->GetLogicalVolume();
    G4LogicalVolume* pdb_logic = pdb_->GetLogicalVolume();
    //   G4double db_xsize = db.GetDimensions().x();
    // G4double db_ysize = db.GetDimensions().y();
    G4double db_zsize = db_->GetDimensions().z();
    // G4cout << "dice board x = " << db_xsize << ", y = " 
    // 	   << db_ysize << ", z = " <<  db_zsize << std::endl;
    G4double displ = active_size_/2. + db_zsize/2.;
    G4double displ2 = z_size_/2. + db_zsize/2.;
    
    new G4PVPlacement(0, G4ThreeVector(0.,0., -displ2), db_logic,
     		      "LXE_DICE", lXe_logic_, false, 0, true);
    
    //  G4cout << " LXe outer box starts at " << displ  - db_zsize/2. << "and ends at " << displ + db_zsize/2. << G4endl;

    G4RotationMatrix rot;
    
    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(-displ, 0., 0.)), pdb_logic,
		      "LXE_DICE", lXe_logic_, false, 1, true);

    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., 0., displ2)), db_logic,
		      "LXE_DICE", lXe_logic_, false, 2, true);
    
    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(displ, 0., 0.)), pdb_logic,
		      "LXE_DICE", lXe_logic_, false, 3, true);

    rot.rotateZ(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., displ, 0.)), pdb_logic,
    		      "LXE_DICE", lXe_logic_, false, 4, true);
    
    rot.rotateZ(pi);
     new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., -displ, 0.)), pdb_logic,
    		      "LXE_DICE", lXe_logic_, false, 5, true);
    */
    
  }
    
  G4ThreeVector PetaloTrapAlone::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // ACTIVE
    if (region == "ACTIVE") {
      vertex = active_gen_->GenerateVertex(region);
    } else if (region == "OUTSIDE") {
      vertex = G4ThreeVector(0., 0., -20.*cm);
    } else if (region == "CENTER") {
      vertex = G4ThreeVector(0., 5.*mm, -5.*mm);
    } else if (region == "SURFACE") {
      vertex = surf_gen_->GenerateVertex("Z_SURF");
    }
   
    return vertex;
  }
  
  
} //end namespace nexus
