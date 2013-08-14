// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 1 Aug 2009
//  
//  Copyright (c) 2009 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "SquareChamber.h"

#include "ConfigService.h"
#include "IonizationSD.h"
#include "MaterialsList.h"
#include "BoxVertexGenerator.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>


namespace nexus {


  SquareChamber::SquareChamber(): BaseGeometry()
  {
    SetParameters();
    BuildGeometry();
    
    // Creating the vertex generator
    _chamber_vertex_gen = 
      new BoxVertexGenerator(_width, _height, _length, _thickn);
  }



  SquareChamber::~SquareChamber()
  {
    delete _chamber_vertex_gen;
  }
  


  void SquareChamber::SetParameters()
  {
    // get geometry configuration parameters
    const ParamStore& config = ConfigService::Instance().Geometry();
    
    // chamber dimensions
    _width  = config.GetDParam("chamber_width");
    _height = config.GetDParam("chamber_height");
    _length = config.GetDParam("chamber_length");    
    _thickn = config.GetDParam("chamber_thickness");

    // chamber pressure
    _pressure = config.GetDParam("GXe_pressure");
  }
  
  
  
  void SquareChamber::BuildGeometry()
  {
    // Materials .....................................................
    G4Material* steel = MaterialsList::Steel();
    G4Material* gxe = MaterialsList::GXe(_pressure);

    // CHAMBER volume ................................................

    // The parameterized width, height and length are internal dimensions.
    // The chamber thickness is added to obtain the external (total) size.
    G4double X = _width  + 2.*_thickn;
    G4double Y = _height + 2.*_thickn;
    G4double Z = _length + 2.*_thickn;
    
    G4Box* chamber_solid = new G4Box("CHAMBER", X/2., Y/2., Z/2.);

    G4LogicalVolume* chamber_logic =
      new G4LogicalVolume(chamber_solid, steel, "CHAMBER");

    SetLogicalVolume(chamber_logic);

    // ACTIVE volume .................................................

    G4Box* active_solid = new G4Box("ACTIVE", _width/2., _height/2., _length/2.);

    G4LogicalVolume* active_logic = 
      new G4LogicalVolume(active_solid, gxe, "ACTIVE");
    
    active_logic->SetUserLimits(new G4UserLimits(5.*mm));

    G4PVPlacement* active_physi =
      new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic, "ACTIVE",
			chamber_logic, false, 0);

    // ACTIVE is defined as an ionization sensitive volume.
    G4String SD_name = "/SQUARE_CHAMBER/CHAMBER/ACTIVE";
    IonizationSD* ionSD = new IonizationSD(SD_name);
    G4SDManager* SDMan = G4SDManager::GetSDMpointer();
    SDMan->AddNewDetector(ionSD);
    active_logic->SetSensitiveDetector(ionSD);
    
    // Visualization properties
    //G4VisAttributes white(G4Colour(1., 1., 1.));
    //active_logic->SetVisAttributes(white);
  }

  
  
  G4ThreeVector SquareChamber::GenerateVertex(const G4String& region) const
  {
    if (region == "CENTER") {
      return G4ThreeVector(0,0,0);
    }

    // Generating in any part of the CHAMBER
    else {
      return _chamber_vertex_gen->GenerateVertex(region);
    }

  }


} //end namespace nexus
