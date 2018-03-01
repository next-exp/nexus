// ----------------------------------------------------------------------------
//  $Id: PetaloTrap.cc  $
//
//  Author:  <jmunoz@ific.uv.es>   
//  Created: January 2014
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "PetaloTrap.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "OpticalMaterialProperties.h"
#include "BoxPointSampler.h"
#include "PetitPlainDice.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Trd.hh>
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

  PetaloTrap::PetaloTrap():
    BaseGeometry(),
    internal_diam_(20.*cm),
    r_dim_(5.*cm),
    n_cells_(12),
    dim_int_(5.2*cm)
  {
    
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetaloTrap/",
                                  "Control commands of geometry PetaloTrap.");
    
    // Maximum Step Size
    G4GenericMessenger::Command& step_cmd = 
      msg_->DeclareProperty("max_step_size", max_step_size_, 
			    "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");
    
    // size1_ = 2.*ring_diameter_/2.*tan(pi/n_modules_);
    dim_ext_ = dim_int_ + 2.*r_dim_*tan(pi/n_cells_);
    // G4cout << size1_  << ", vs " << size2_ << G4endl;

    //  SetParameters(31.6312*mm, 40.2579*mm, 30.*mm);

    pdb_ = new PetitPlainDice();
  }


  PetaloTrap::~PetaloTrap()
  {    
    // delete active_gen_;
    // delete surf_gen_;
  }


  void PetaloTrap::Construct()
  {  
    pdb_->Construct();

    const G4double thickness = 1. * mm;

    G4Trd* det_solid =
      new G4Trd("TRAP", dim_int_/2., dim_ext_/2., dim_int_/2., dim_ext_/2., r_dim_/2.);
   
    G4Material* kapton =
      G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");
    
    det_logic_ = new G4LogicalVolume(det_solid, kapton, "TRAP");

    this->SetLogicalVolume(det_logic_);
    //   det_logic_->SetVisAttributes(G4VisAttributes::Invisible);    

    G4VisAttributes trap_color = nexus::LightGrey();
    trap_color.SetForceSolid(true);
    det_logic_->SetVisAttributes(trap_color);

    G4Trd* lxe_solid =
      new G4Trd("ACTIVE", (dim_int_-2.*thickness)/2., (dim_ext_-2.*thickness)/2.,
                (dim_int_-2.*thickness)/2., (dim_ext_-2.*thickness)/2.,
                (r_dim_-thickness)/2.);
   
    G4Material* lXe = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    lXe->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
    
    G4LogicalVolume* lxe_logic = new G4LogicalVolume(lxe_solid, lXe, "ACTIVE");
    // lxe_logic->SetVisAttributes(white_color);  
   
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), lxe_logic,
		  "ACTIVE", det_logic_, false, 0, true);

    // Set the ACTIVE volume as an ionization sensitive active
    IonizationSD* ionisd = new IonizationSD("/PETALO/ACTIVE");
    lxe_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    std::cout << "*** Maximum Step Size (mm): " << max_step_size_/mm << std::endl;
    lxe_logic->SetUserLimits(new G4UserLimits(max_step_size_));

    //   G4Colour otherColour(.6, .8, .79); 
    //   G4VisAttributes myAttr(otherColour); 
    //   myAttr.SetForceSolid(true);
    // lxe_logic->SetVisAttributes(myAttr);
    
    BuildSiPMPlanes();
  }

 void PetaloTrap::BuildSiPMPlanes()
 {
   // G4double db_z = db_->GetDimensions().z();

   // G4double displ = (z_size_-det_thickness_)/2. + db_z/2.;

   // G4LogicalVolume* db_logic = db_->GetLogicalVolume();
   // new G4PVPlacement(0, G4ThreeVector(0.,0., -displ), db_logic,
   //      	     "LXE_DICE", lXe_logic_, false, 0, true);

   // db_->SetXYsize( size2_);
   // db_->Construct();
   // db_logic = db_->GetLogicalVolume();
   // new G4PVPlacement(0, G4ThreeVector(0.,0., displ), db_logic,
   // 		     "LXE_DICE", lXe_logic_, false, 1, true);
   
 }


  /*
 void PetaloTrap::BuildLXe() 
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

  void PetaloTrap::BuildActive() 
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

  void PetaloTrap::BuildSiPMPlane()
  {
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
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(-displ, 0., 0.)), db_logic,
		      "LXE_DICE", lXe_logic_, false, 1, true);

    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., 0., displ2)), db_logic,
		      "LXE_DICE", lXe_logic_, false, 2, true);
    
    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(displ, 0., 0.)), db_logic,
		      "LXE_DICE", lXe_logic_, false, 3, true);

    rot.rotateZ(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., displ, 0.)), db_logic,
    		      "LXE_DICE", lXe_logic_, false, 4, true);
    
    rot.rotateZ(pi);
     new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., -displ, 0.)), db_logic,
    		      "LXE_DICE", lXe_logic_, false, 5, true);
    
  }

  */

  G4ThreeVector PetaloTrap::GetParameters() const {
    return G4ThreeVector(dim_int_, dim_ext_, r_dim_);
  }
    
  G4ThreeVector PetaloTrap::GenerateVertex(const G4String& region) const
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
  
} //end namespace nexus
