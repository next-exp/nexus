// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 1 Aug 2009
//  
//  Copyright (c) 2009-2012 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "SquareChamber.h"

#include "ConfigService.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "BoxPointSampler.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4NistManager.hh>
#include <G4Region.hh>


namespace nexus {


  SquareChamber::SquareChamber(): BaseGeometry()
  {
    SetParameters();
    BuildGeometry();
    
    // Creating the vertex generator
    _chamber_vertex_gen = 
      new BoxPointSampler(_width, _height, _length, _thickn);
  }



  SquareChamber::~SquareChamber()
  {
    delete _chamber_vertex_gen;
  }
  


  void SquareChamber::SetParameters()
  {
    // get geometry configuration parameters
    const ParamStore& cfg = ConfigService::Instance().Geometry();
    
    // chamber dimensions
    _width  = cfg.GetDParam("chamber_width");
    _height = cfg.GetDParam("chamber_height");
    _length = cfg.GetDParam("chamber_length");    
    _thickn = cfg.GetDParam("chamber_thickness");

    // xenon gas pressure
    _gxe_pressure = cfg.GetDParam("gxe_pressure");
  }
  
  
  
  void SquareChamber::BuildGeometry()
  {
    // CHAMBER ///////////////////////////////////////////////////////
    // The parameterized width, height and length are internal dimensions.
    // The chamber thickness is added to obtain the external (total) size.
    G4double X = _width  + 2.*_thickn;
    G4double Y = _height + 2.*_thickn;
    G4double Z = _length + 2.*_thickn;

    G4Box* chamber_solid = new G4Box("CHAMBER", X/2., Y/2., Z/2.);

    G4Material* copper = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

    G4LogicalVolume* chamber_logic = 
      new G4LogicalVolume(chamber_solid, copper, "CHAMBER");
    this->SetLogicalVolume(chamber_logic);
    
    // GAS ///////////////////////////////////////////////////////////

    G4Box* gas_solid = new G4Box("GAS", _width/2., _height/2., _length/2.);

    G4Material* gxe = MaterialsList::GXe(15.*bar, 303*kelvin);
    
    G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gxe, "GAS");

    G4PVPlacement* gas_physi = 
      new G4PVPlacement(0, G4ThreeVector(0,0,0), gas_logic, "GAS",
			chamber_logic, false, 0);

    // Set the gas as an ionization sensitive detector
    IonizationSD* ionisd = new IonizationSD("/SQUARE_CHAMBER/GAS");
    gas_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);
  }
  
  
  
  G4ThreeVector SquareChamber::GenerateVertex(const G4String& region) const
  {
    return _chamber_vertex_gen->GenerateVertex(region);
  }
  

} //end namespace nexus
