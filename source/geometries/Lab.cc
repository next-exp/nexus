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

#include <TTree.h>


namespace nexus {
  
  using namespace CLHEP;

  Lab::Lab(): 
    BaseGeometry(), _msg(0)
  {
    module_ = new Pet2boxes();

        // To read a TTree
    file_ = new TFile("GeneratePhotonsTree.root","READ");
  
    
    TTree* tree = dynamic_cast<TTree*>(file_->Get("tpg"));

    
    tree->SetBranchAddress("px1", &px1_);
    tree->SetBranchAddress("py1", &py1_);
    tree->SetBranchAddress("pz1", &pz1_);
    tree->SetBranchAddress("px2", &px2_);
    tree->SetBranchAddress("py2", &py2_);
    tree->SetBranchAddress("pz2", &pz2_);
    for (G4int i=0; i<tree->GetEntries(); ++i) {
      tree->GetEntry(i);
      G4ThreeVector pos1(px1_, py1_, pz1_);
      G4ThreeVector pos2(px2_, py2_, pz2_);
      std::pair<G4ThreeVector, G4ThreeVector> positions = std::make_pair(pos1, pos2);
      vertices_.push_back(positions);
    }
  }



  Lab::~Lab()
  {
    delete _msg;
  } 



  void Lab::Construct()
  {
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
   
    module_->Construct();

    G4LogicalVolume* module_logic = module_->GetLogicalVolume();
    new G4PVPlacement(0, G4ThreeVector(0.,0., -10.*cm - 1.5*cm), module_logic, "MODULE_0",
        lab_logic, false, 0, true);

    G4RotationMatrix rot;
    rot.rotateY(pi);
    new G4PVPlacement(G4Transform3D(rot, G4ThreeVector(0.,0., 10.*cm + 1.5*cm)), module_logic, "MODULE_1",
        lab_logic, false, 1, true);
    
  }



  G4ThreeVector Lab::GenerateVertex(const G4String& /*region*/) const
  {
    
    return G4ThreeVector(0.,0.,0.);
  }

  std::pair<G4ThreeVector, G4ThreeVector> Lab::GenerateVertices(const G4String& /*region*/) const
  {
    std::pair<G4ThreeVector, G4ThreeVector> vertices;
    unsigned int i = index_;

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
