// ----------------------------------------------------------------------------
//  $Id: PMT_QE_setup.cc  $
//
//  Author:  P. Ferrario <paolafer@ific.uv.es>    
//  Created: 14 Dec 2012
//  
//  Copyright (c) 2012 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "PMT_QE_setup.h"

#include "ConfigService.h"
#include "MaterialsList.h"
#include "SiPM11.h"
#include "PmtR7378A.h"
#include "IonizationSD.h"
#include "BoxPointSampler.h"
#include "OpticalMaterialProperties.h"

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4SDManager.hh>
#include <G4UserLimits.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4SubtractionSolid.hh>
#include <Randomize.hh>


namespace nexus {


  PMT_QE_setup::PMT_QE_setup(): BaseGeometry()
  {
    SetParameters();
    BuildGeometry();
  }



  PMT_QE_setup::~PMT_QE_setup()
  {
  }
  


  void PMT_QE_setup::SetParameters()
  {
    // get geometry configuration parameters
    const ParamStore& cfg_geom = ConfigService::Instance().Geometry();

    // xenon gas pressure
    _gxe_pressure = cfg_geom.GetDParam("gxe_pressure");

    const ParamStore& cfg_gen = ConfigService::Instance().Generation();
    // distance of the generation point in z from the surface of the PMT window
    _z_dist = cfg_gen.GetDParam("z_dist");

   
  }
  
  
  
  void PMT_QE_setup::BuildGeometry()
  {
    // CHAMBER ///////////////////////////////////////////////////////

    // The parameterized width, height and length are internal dimensions.
    // The chamber thickness is added to obtain the external (total) size.
    G4double width = 1.2*m;
    G4double height = 1.2*m;
    _length = 1.2*m;
    G4double thickn = 1.*cm;
    G4double X = width  + 2.*thickn;
    G4double Y = height + 2.*thickn;
    G4double Z = _length + 2.*thickn;

    G4Box* chamber_solid = new G4Box("CHAMBER", X/2., Y/2., Z/2.);

    G4Material* copper = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");

    G4LogicalVolume* chamber_logic = 
      new G4LogicalVolume(chamber_solid, copper, "CHAMBER");
    this->SetLogicalVolume(chamber_logic);
    
    // GAS ///////////////////////////////////////////////////////////
    
    G4Box* gas_solid = new G4Box("GAS", width/2., height/2., _length/2.);
    
    G4Material* gxe = MaterialsList::GXe(_gxe_pressure);
    gxe->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_gxe_pressure));
    G4Material* air = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    
    G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gxe, "GAS");
   
    
    G4PVPlacement* gas_physi = 
      new G4PVPlacement(0, G4ThreeVector(0,0,0), gas_logic, "GAS",
			chamber_logic, false, 0, true);

    
    G4LogicalVolume* pmt_logic = _pmt.GetLogicalVolume();
    
    G4PVPlacement* pmt_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,0.,-_length/2.+_pmt.Length()/2.), 
			pmt_logic, "PMT",
     			gas_logic, false, 0, true);
   

  }
  
  G4ThreeVector PMT_QE_setup::GenerateVertex(const G4String& region) const
  {
    if (region == "CIRCLE"){
      G4double pmt_diameter = _pmt.Diameter();
      G4double r_max = pmt_diameter/2.;
      G4double r = G4UniformRand()*r_max;
      G4double phi = G4UniformRand()*2*pi;
      G4double x = r*cos(phi);
      G4double y = r*sin(phi);
      
      return G4ThreeVector(x, y,-_length/2.+_pmt.Length()+_z_dist);

    } else if (region == "POINT") {     
      return G4ThreeVector(0, 0, -_length/2.+_pmt.Length()+_z_dist);
    }
  }


} //end namespace nexus
