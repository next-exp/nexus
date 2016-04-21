// ----------------------------------------------------------------------------
//  $Id: Lab.cc 10054 2015-02-09 14:50:49Z paola $
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 29 August 2013
//  
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "Lab.h"

#include "Pet2boxes.h"
#include "PetLXeCell.h"
#include "PetLYSOCell.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "UniformElectricDriftField.h"
#include "IonizationSD.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

#include <TTree.h>


namespace nexus {
  
  using namespace CLHEP;

  Lab::Lab(): 
    BaseGeometry(), _msg(0), type_("LXe")
  {
    _msg = new G4GenericMessenger(this, "/Geometry/Lab/", 
				  "Control commands of geometry Lab.");
    _msg->DeclareProperty("starting_point", starting_point_, "");
    _msg->DeclareProperty("file_name", filename_, "");
     // Which material are we using?
     _msg->DeclareProperty("det_type", type_, "type of detector");

    //module_ = new Pet2boxes();
    // module_ = new PetLYSObox();

     //  module_ = new PetLYSOCell();
     module_ = new PetLXeCell();
  
     //   moduleLXe_ = new PetLXeCell();
     //   moduleLYSO_ = new PetLYSOCell();
    
  }



  Lab::~Lab()
  {
    delete _msg;
  } 



  void Lab::Construct()
  {
        // To read a TTree
    
    file_ = new TFile(filename_.c_str(),"READ");
    G4cout << filename_.c_str() << G4endl;
    
    TTree* tree = dynamic_cast<TTree*>(file_->Get("tpg"));

    
    tree->SetBranchAddress("px1", &px1_);
    tree->SetBranchAddress("py1", &py1_);
    tree->SetBranchAddress("pz1", &pz1_);
    tree->SetBranchAddress("px2", &px2_);
    tree->SetBranchAddress("py2", &py2_);
    tree->SetBranchAddress("pz2", &pz2_);
    for (G4int i=0; i<tree->GetEntries(); ++i) {
      tree->GetEntry(i);
      //      G4ThreeVector pos1(px1_, py1_, pz1_ );
      //G4ThreeVector pos2(px2_, py2_, pz2_ );
      // New vertices (Jan 2016)
      G4ThreeVector pos1(px1_, py1_, - pz1_ - 100.);
      G4ThreeVector pos2(px2_, py2_, pz2_ + 100.);
     
 // if ( px1_ < 12. && px1_ > -12. && py1_ < 12. && py1_ > -12. &&  pz1_ < -100. && pz1_  > -150. &&
 // 	   px2_ < 12. && px2_ > -12. && py2_ < 12. && py2_ > -12. &&  pz2_ > 100. && pz2_ < 150.) {
      if ( px1_ < 12. && px1_ > -12. && py1_ < 12. && py1_ > -12. &&  (-pz1_-100) < -100. && (- pz1_ - 100) > -150. &&
      	   px2_ < 12. && px2_ > -12. && py2_ < 12. && py2_ > -12. &&  (pz2_+100) > 100. && (pz2_+100) < 150.) {
	//    G4cout << px1_ << ", " << py1_ << ", " << pz1_  << G4endl;
	//    G4cout << px2_ << ", " << py2_ << ", " << pz2_  << G4endl;
	std::pair<G4ThreeVector, G4ThreeVector> positions = std::make_pair(pos1, pos2);
	//	G4cout << pos1 << ", " << pos2 << G4endl;
	vertices_.push_back(positions);
      }
    }
    file_ ->Close();
    
    //   G4cout << "Size of points: " << vertices_.size() << G4endl;
    // LAB /////////////////////////////////////////////////////////////
    // This is just a volume of air surrounding the detector so that
  // events (from calibration sources or cosmic rays) can be generated 
  // on the outside.

    G4double lab_size (2. * m);
    G4Box* lab_solid = 
      new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
    
    G4LogicalVolume* lab_logic =
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "AIR");
    lab_logic->SetVisAttributes(G4VisAttributes::Invisible);
    
    // Set this volume as the wrapper for the whole geometry 
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(lab_logic);
    
    // if (type_ == "LYSO" ) {
    //   G4cout << "LYSO cell being instantiated" << G4endl;
    //   module_ = moduleLYSO_;
    // } else {
    //   G4cout << "LXe cell being instantiated" << G4endl;
    //   module_ = moduleLXe_;
    // }

    module_->Construct();

    G4LogicalVolume* module_logic = module_->GetLogicalVolume();
    new G4PVPlacement(0, G4ThreeVector(0.,0., -10.*cm - 2.5*cm), module_logic, "MODULE_0",
        lab_logic, false, 0, true);

    G4RotationMatrix rot;
    rot.rotateY(pi);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0.,0., 10.*cm + 2.5*cm)), module_logic, "MODULE_1",
        lab_logic, false, 1, true);
    
  }



  G4ThreeVector Lab::GenerateVertex(const G4String& /*region*/) const
  {   
    return G4ThreeVector(0.,0.,0.);
  }

  std::pair<G4ThreeVector, G4ThreeVector> Lab::GenerateVertices(const G4String& /*region*/) const
  {
    std::pair<G4ThreeVector, G4ThreeVector> vertices;
    unsigned int i = starting_point_ + index_;

    if (i == (vertices_.size()-1)) {
      G4Exception("[Pet2boxes]", "GenerateVertex()", 
		  RunMustBeAborted, "Reached last event in vertices list.");
    }

    try {
      vertices = vertices_.at(i);
      index_++;
    }
    catch (const std::out_of_range& oor) {
      G4Exception("[Pet2boxes]", "GenerateVertex()", FatalErrorInArgument, "Point out of range.");
    }
    
    return vertices;
  }
  

} // end namespace nexus
