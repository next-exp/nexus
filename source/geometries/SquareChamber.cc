// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 1 Aug 2009
//  
//  Copyright (c) 2009-2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "SquareChamber.h"

#include "ConfigService.h"
#include "MaterialsList.h"
#include "SiPM11.h"
#include "IonizationSD.h"
#include "BoxPointSampler.h"
#include "ELParamSimulation.h"
#include "OpticalMaterialProperties.h"
#include "UniformElectricDriftField.h"

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

    // distance between sensors
    _pitch = cfg.GetDParam("pitch");

    // length of the electroluminescence gap
    _elgap_length = cfg.GetDParam("elgap_length");
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
    gxe->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(15.*bar, 303));

    G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gxe, "GAS");

    G4PVPlacement* gas_physi = 
      new G4PVPlacement(0, G4ThreeVector(0,0,0), gas_logic, "GAS",
			chamber_logic, false, 0);
    
    // EL GAP ////////////////////////////////////////////////////////

    G4double elgap_length = 5. * mm;
    G4double grid_thickn = 0.5 * mm; 

    G4Box* elgap_solid = 
      new G4Box("EL_GAP", _width/2., _height/2., elgap_length/2.);
    
    G4LogicalVolume* elgap_logic = 
      new G4LogicalVolume(elgap_solid, gxe, "EL_GAP");
    
    G4ThreeVector elgap_position(0., 0., (_length/2. - 10.*cm));

    G4PVPlacement* elgap_physi = 
      new G4PVPlacement(0, elgap_position, elgap_logic, "EL_GAP",
			gas_logic, false, 0);

    // DRIFT AND EL FIELDS ///////////////////////////////////////////

    UniformElectricDriftField* drift_field = new UniformElectricDriftField();
    drift_field->SetCathodePosition(-_length/2.);
    drift_field->SetAnodePosition(elgap_position.z());
    drift_field->SetDriftVelocity(1.*mm/microsecond);
    drift_field->SetTransverseDiffusion(1.*mm/sqrt(cm));
    drift_field->SetLongitudinalDiffusion(.5*mm/sqrt(cm));
  
    G4Region* drift_region = new G4Region("DRIFT");
    drift_region->SetUserInformation(drift_field);
    drift_region->AddRootLogicalVolume(gas_logic);
  }
  
  
  
  G4ThreeVector SquareChamber::GenerateVertex(const G4String& region) const
  {
    return _chamber_vertex_gen->GenerateVertex(region);
  }
  

} //end namespace nexus
