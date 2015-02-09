// ----------------------------------------------------------------------------
//  $Id: PMT_QE_setup.cc  $
//
//  Author:  P. Ferrario <paolafer@ific.uv.es>    
//  Created: 14 Dec 2012
//  
//  Copyright (c) 2012 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "PMT_QE_setup.h"

#include "MaterialsList.h"
#include "SiPM11.h"
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
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>


namespace nexus {

  using namespace CLHEP;

  PMT_QE_setup::PMT_QE_setup(): BaseGeometry()
  {
    _msg = new G4GenericMessenger(this, "/Geometry/PMT_QE_setup/",
				"Control commands of geometry PMT_QE_setup.");
    _msg->DeclareProperty("z_dist", _z_dist,
			"Distance of the generation point in z from the surface of the PMT window.");
  }



  PMT_QE_setup::~PMT_QE_setup()
  {
  }
 
  
  
  void PMT_QE_setup::Construct()
  {
    // CHAMBER ///////////////////////////////////////////////////////

    // The parameterized width, height and length are internal dimensions.
    // The chamber thickness is added to obtain the external (total) size.
    G4double width = 50.*cm;
    G4double height = 50.*cm;
    _length = 50.*cm;
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
    G4double gxe_pressure = 10*bar;

    G4Box* gas_solid = new G4Box("GAS", width/2., height/2., _length/2.);
    
    G4Material* gxe = MaterialsList::GXe(gxe_pressure);
    gxe->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(gxe_pressure));
    // G4Material* air = 
    //   G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    
    G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gxe, "GAS");
       
    G4PVPlacement* gas_physi = 
      new G4PVPlacement(0, G4ThreeVector(0,0,0), gas_logic, "GAS",
			chamber_logic, false, 0, true);

    // Positioning of the PMT /////////////////////////////////////////
    _pmt.Construct();
    G4LogicalVolume* pmt_logic = _pmt.GetLogicalVolume();
    //   _pmt_length = _pmt.Length() // this is R7378A
    _pmt_length = 20*cm; // this is R11410
    
    G4PVPlacement* pmt_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,0.,-_length/2.+_pmt_length/2.), 
			pmt_logic, "PMT",
     			gas_logic, false, 0, true);

    // Positioning of the teflon panel
    // G4Box* teflon_solid = new G4Box("GAS", 22.*cm/2., 50.*cm/2., thickn/2.); 
    // G4Material* ptfe = MaterialsList::PEEK();
    // G4LogicalVolume* teflon_logic = new G4LogicalVolume(teflon_solid, ptfe, "TEFLON");

    // G4RotationMatrix* rotdb = new G4RotationMatrix();
    // rotdb->rotateY(-pi/2.);
    // new G4PVPlacement(0, G4ThreeVector(0.*cm, 0. ,-_length/2.+_pmt_length + _z_dist + 1.*cm), 
    // 		      teflon_logic, "TEFLON", gas_logic, false, 0, true);
    // G4RotationMatrix* rotdb2 = new G4RotationMatrix();
    // rotdb2->rotateY(pi/2.);
    // new G4PVPlacement(rotdb2, G4ThreeVector(-10.*cm, 0., -_length/2.+_pmt_length + _z_dist + 1.*cm), 
    //  		      teflon_logic, "TEFLON", gas_logic, false, 2, true);
    
    // Optical surface
    // G4OpticalSurface* teflon_opsur = new G4OpticalSurface("TEFLON_OPSURF");
    // teflon_opsur->SetType(dielectric_metal);
    // teflon_opsur->SetModel(unified);
    // teflon_opsur->SetFinish(ground);
    // teflon_opsur->SetSigmaAlpha(0.0000001); // it does not affect, because the reflection is totally lambertian.
    // teflon_opsur->SetMaterialPropertiesTable(OpticalMaterialProperties::PTFE());
    
    // new G4LogicalSkinSurface("TEFLON_OPSURF", teflon_logic, teflon_opsur);

  }
  
  G4ThreeVector PMT_QE_setup::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector point;

    if (region == "CIRCLE"){
      //     G4double pmt_diameter = _pmt.Diameter(); // this is R7378A
      G4double pmt_diameter = 64.*mm;  // this is R11410
      G4double r_max = pmt_diameter/2.;
      G4double r = G4UniformRand()*r_max;
      G4double phi = G4UniformRand()*2*pi;
      G4double x = r*cos(phi);
      G4double y = r*sin(phi);
      
      point =  G4ThreeVector(x, y,-_length/2.+_pmt_length+_z_dist);

    } else if (region == "POINT") {     
      point = G4ThreeVector(0, 0, -_length/2.+_pmt_length+_z_dist);
    }
    
    return point;
  }


} //end namespace nexus
