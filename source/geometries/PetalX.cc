// ----------------------------------------------------------------------------
//  $Id: PetalX.cc  $
//
//  Author:  <jmunoz@ific.uv.es>   
//  Created: January 2014
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "PetalX.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "PetKDB.h"
#include "PetPlainDice.h"
#include "OpticalMaterialProperties.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>

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

  PetalX::PetalX():
    BaseGeometry(),

    // Detector dimensions
    //    vacuum_thickn_(1.*mm),
    //   outer_wall_thickn_(3.*mm),
    det_thickness_(1.*mm),
    //  det_size_(20.*cm),
    active_size_ (5.*cm)
  {
    // Messenger
    msg_ = new G4GenericMessenger(this, "/Geometry/PetalX/", "Control commands of geometry PetalX.");
   
    // Maximum Step Size
    G4GenericMessenger::Command& step_cmd = 
      msg_->DeclareProperty("max_step_size", max_step_size_, 
								  "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");

     db_ = new PetKDB(8,8);
     db_->Construct(); //here, otherwise we don't know its dimensions
     pdb_ = new PetPlainDice(8,8);

     db_z_ = db_->GetDimensions().z();

  }


  PetalX::~PetalX()
  {    
    delete active_gen_;
  }


  void PetalX::Construct()
  {
    // LAB. This is just a volume of air surrounding the detector
    G4double lab_size = 1.*m;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
    
    lab_logic_ = new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic_);

    lXe_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");
    lXe_->SetMaterialPropertiesTable(OpticalMaterialProperties::LXe());
  
    BuildDetector();
    BuildLXe() ;
    BuildActive();
    BuildSiPMPlane();
  }

  // void PetalX::BuildOuterWall() 
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

  void PetalX::BuildDetector() 
  {
    G4Box* det_solid = 
      new G4Box("WALL", det_size_/2., det_size_/2., det_size_/2.);

    G4Material* steel = MaterialsList::Steel();
    
    det_logic_ = new G4LogicalVolume(det_solid, steel, "WALL");
    det_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), det_logic_,
		      "WALL", lab_logic_, false, 0, true);
  }

 void PetalX::BuildLXe() 
  {
    G4double lXe_size_ = det_size_ - 2.* det_thickness_;
    G4Box* lXe_solid = 
      new G4Box("LXE", lXe_size_/2., lXe_size_/2., lXe_size_/2.);
    
    lXe_logic_ = new G4LogicalVolume(lXe_solid, lXe_, "LXE");
    lXe_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), lXe_logic_,
		      "LXE", det_logic_, false, 0, true);
  }

  void PetalX::BuildActive() 
  {
    G4Box* active_solid = 
      new G4Box("ACTIVE", active_size_/2., active_size_/2., active_size_/2.);

   

    G4LogicalVolume* active_logic = new G4LogicalVolume(active_solid, lXe_, "ACTIVE");
    //active_logic->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes red_color;
    red_color.SetColor(1., 0., 0.);
    red_color.SetForceSolid(true);
    active_logic->SetVisAttributes(red_color);

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic,
		      "ACTIVE", lXe_logic_, false, 0, true);
    
    // Set the ACTIVE volume as an ionization sensitive active
    IonizationSD* ionisd = new IonizationSD("/PETALX/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    std::cout << "*** Maximum Step Size (mm): " << max_step_size_/mm << std::endl;
    active_logic->SetUserLimits(new G4UserLimits(max_step_size_));

    // Vertex Generator
    active_gen_ = new BoxPointSampler(active_size_, active_size_, active_size_, 0.,
				      G4ThreeVector(0.,0.,0.) ,0);
  }

  void PetalX::BuildSiPMPlane()
  {
    
    db_->Construct();
    pdb_->Construct();

    G4LogicalVolume* db_logic = db_->GetLogicalVolume();
    G4LogicalVolume* pdb_logic = pdb_->GetLogicalVolume();
    //   G4double db_xsize = db.GetDimensions().x();
    // G4double db_ysize = db.GetDimensions().y();
    G4double db_zsize = db_->GetDimensions().z();
    // G4cout << "dice board x = " << db_xsize << ", y = " 
    // 	   << db_ysize << ", z = " <<  db_zsize << std::endl;
    G4double displ = active_size_/2. + db_zsize/2.;

    
    new G4PVPlacement(0, G4ThreeVector(0.,0., -displ), db_logic,
     		      "LXE_DICE", lXe_logic_, false, 0, true);
    
    //  G4cout << " LXe outer box starts at " << displ  - db_zsize/2. << "and ends at " << displ + db_zsize/2. << G4endl;

    G4RotationMatrix rot;
    
    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(-displ, 0., 0.)), db_logic,
		      "LXE_DICE", lXe_logic_, false, 1, true);

    rot.rotateY(pi/2.);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0., 0., displ)), db_logic,
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
    
  G4ThreeVector PetalX::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // ACTIVE
    if (region == "ACTIVE") {
      vertex = active_gen_->GenerateVertex(region);
    } else if (region == "OUTSIDE") {
      vertex = G4ThreeVector(0., 0., -60.*cm);
    } else if (region == "CENTER") {
      vertex = G4ThreeVector(0., 5.*mm, -5.*mm);
    }
   
    return vertex;
  }
  
  
} //end namespace nexus
