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
#include "NextNewKDB.h"

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


namespace nexus {

  PetalX::PetalX():
    BaseGeometry(),

    // Detector dimensions
    det_size_(20.*cm),
    active_size_ (10.*cm)

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
  }


  PetalX::~PetalX()
  {    
    delete active_gen_;
  }


  void PetalX::Construct()
  {
    // LAB. This is just a volume of air surrounding the detector
    G4double lab_size = 50.*cm;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
    
    lab_logic_ = new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    lab_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic_);

    BuildActive();
  }

  void PetalX::BuildDetector() 
  {
     G4Box* det_solid = 
       new G4Box("ACTIVE", det_size_/2., det_size_/2., det_size_/2.);

     G4Material* steel = MaterialsList::Steel();
    
    det_logic_ = new G4LogicalVolume(det_solid, steel, "WALL");
    det_logic_->SetVisAttributes(G4VisAttributes::Invisible);
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), det_logic_,
		      "WALL", lab_logic_, false, 0, true);
  }

  void PetalX::BuildActive() 
  {
    G4Box* active_solid = 
      new G4Box("ACTIVE", active_size_/2., active_size_/2., active_size_/2.);

    lXe_ = G4NistManager::Instance()->FindOrBuildMaterial("G4_lXe");

    G4LogicalVolume* active_logic = new G4LogicalVolume(active_solid, lXe_, "ACTIVE");
    active_logic->SetVisAttributes(G4VisAttributes::Invisible);

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic,
		      "ACTIVE", det_logic_, false, 0, true);
    
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
    NextNewKDB db(9,9);
    db.Construct();

    G4LogicalVolume* db_logic = db.GetLogicalVolume();
    G4double db_xsize = db.GetDimensions().x();
    G4double db_ysize = db.GetDimensions().y();
    G4double db_zsize = db.GetDimensions().z();

    G4double displ = active_size_/2. + db_zsize/2.;
  }
    
  G4ThreeVector PetalX::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // ACTIVE
    if (region == "ACTIVE") {
      vertex = active_gen_->GenerateVertex(region);
    }
   
    return vertex;
  }
  
  
} //end namespace nexus
