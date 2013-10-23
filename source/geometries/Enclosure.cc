// ----------------------------------------------------------------------------
//  $Id: Enclosure.cc  $
//
//  Author:  <miquel.nebot@ific.uv.es>
//  Created: 24 Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "Enclosure.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "CylinderPointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4RotationMatrix.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4SDManager.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <Randomize.hh>
#include <G4OpticalSurface.hh>


namespace nexus{
  Enclosure::Enclosure():

    // Enclosures dimensions
    _enclosure_length (200.0 * mm),
    _enclosure_in_diam (80. * mm), 
    _enclosure_thickness (6. * mm),
    _enclosure_endcap_diam (119. * mm), 
    _enclosure_endcap_thickness (20. * mm),
    _enclosure_window_diam (85. * mm), 
    _enclosure_window_thickness (6. * mm), //???
    _enclosure_pad_thickness (2. * mm)//max 60  ??????
  {
    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    
    /// The PMT
    _pmt = new PmtR11410(); 
  }

  void Enclosure::Construct()
  {
    /// Assign optical properties to materials ///
    G4Material* sapphire = MaterialsList::Sapphire();
    sapphire->SetMaterialPropertiesTable(OpticalMaterialProperties::Sapphire());
    G4Material* vacuum = 
      G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    vacuum->SetMaterialPropertiesTable(OpticalMaterialProperties::Vacuum());

     /////   ENCLOSURES  /////
     G4Tubs* enclosure_body = 
      new G4Tubs("ENCLOSURE_BODY", 0.,  
		 _enclosure_in_diam/2.+ _enclosure_thickness, 
		 _enclosure_length/2., 0., twopi);
     G4Tubs* enclosure_endcap = 
       new G4Tubs("ENCLOSURE_ENDCAP", 0., _enclosure_endcap_diam/2,
		  _enclosure_endcap_thickness/2, 0., twopi); 
     G4ThreeVector transl(0., 0., 
			  -_enclosure_length/2 + _enclosure_endcap_thickness/2);
     G4UnionSolid* enclosure_solid = 
       new G4UnionSolid("ENCLOSURE_SOLID", enclosure_body, enclosure_endcap, 
			0, transl);

     G4LogicalVolume* enclosure_logic = 
       new G4LogicalVolume(enclosure_solid, 
			   G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu"), "ENCLOSURE");     
     this->SetLogicalVolume(enclosure_logic);

     // Filling the enclosure with vacuum
     G4double gas_diam = _enclosure_in_diam ;
     G4double gas_length = 
       _enclosure_length-_enclosure_endcap_thickness/2-
       _enclosure_window_thickness-_enclosure_pad_thickness;
     G4Tubs* enclosure_gas_solid = 
       new G4Tubs("ENCLOSURE_GAS", 0., gas_diam/2., gas_length/2., 0., twopi);
     G4LogicalVolume* enclosure_gas_logic = 
       new G4LogicalVolume(enclosure_gas_solid,	vacuum, "ENCLOSURE_GAS");
     G4double gas_pos = 
       (_enclosure_endcap_thickness/2-_enclosure_window_thickness-
     	_enclosure_pad_thickness)/2;
     G4PVPlacement* enclosure_gas_physi = 
       new G4PVPlacement(0,G4ThreeVector(0.,0.,gas_pos), enclosure_gas_logic,
			 "ENCLOSURE_GAS", enclosure_logic, false, 0, true);

     // Adding the sapphire window
     G4Tubs* enclosure_window_solid = 
       new G4Tubs("ENCLOSURE_WINDOW", 0., _enclosure_window_diam/2., 
		  _enclosure_window_thickness/2., 0., twopi);
     G4LogicalVolume* enclosure_window_logic = 
       new G4LogicalVolume(enclosure_window_solid, sapphire,
			   "ENCLOSURE_WINDOW");
    
     _window_z_pos = _enclosure_length/2 - _enclosure_window_thickness/2.;
     G4PVPlacement* enclosure_window_physi =
       new G4PVPlacement(0, G4ThreeVector(0.,0.,_window_z_pos),
     			 enclosure_window_logic, "ENCLOSURE_WINDOW", 
     			 enclosure_logic, false, 0, true);
     
         
     // Adding the optical pad
     G4Tubs* enclosure_pad_solid =
       new G4Tubs("OPTICAL_PAD", 0., _enclosure_in_diam/2., _enclosure_pad_thickness/2.,0.,twopi);
     G4LogicalVolume* enclosure_pad_logic = 
       new G4LogicalVolume(enclosure_pad_solid, vacuum, "OPTICAL_PAD");
     G4double pad_z_pos = 
       _window_z_pos-_enclosure_window_thickness/2.-_enclosure_pad_thickness/2.;
     G4PVPlacement* enclosure_pad_physi = 
       new G4PVPlacement(0, G4ThreeVector(0.,0.,pad_z_pos),
     			 enclosure_pad_logic,"OPTICAL_PAD",
     			 enclosure_logic, false, 0, true);

     // Adding the PMT
     _pmt->Construct();
     G4LogicalVolume* pmt_logic = _pmt->GetLogicalVolume();
     G4double pmt_rel_z_pos = _pmt->GetRelPosition().z();//return G4ThreeVector(0.,0., _front_body_length/2.);
     _pmt_z_pos = gas_length/2.- pmt_rel_z_pos;
     G4PVPlacement* pmt_physi = 
       new G4PVPlacement(0, G4ThreeVector(0.,0.,_pmt_z_pos), pmt_logic,
			 "PMT", enclosure_gas_logic, false, 0, true);


     ///OPTICAL PROPERTIES//
     //Sapphire window
     //Optical Pad
     
     /////  SETTING VISIBILITIES   //////////    
     G4VisAttributes copper_col(G4Colour(.58, .36, .16));
     //copper_col.SetForceSolid(true);// ==/vis/viewer/set/style surface
     enclosure_logic->SetVisAttributes(copper_col);
     G4VisAttributes Vacuum_col(G4Colour(.48,.45,.58));
     Vacuum_col.SetForceSolid(true);
     enclosure_gas_logic->SetVisAttributes(Vacuum_col);  
     enclosure_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);
     G4VisAttributes Sapphire_col(G4Colour(.8,.8,1.));
     Sapphire_col.SetForceSolid(true);
     enclosure_window_logic->SetVisAttributes(Sapphire_col);
     G4VisAttributes Pad_col(G4Colour(.6,.9,.2));
     Pad_col.SetForceSolid(true);
     enclosure_pad_logic->SetVisAttributes(Pad_col);

     
     // VERTEX GENERATORS   //////////
     _enclosure_body_gen = 
       new CylinderPointSampler(_enclosure_in_diam/2., 
				_enclosure_length - _enclosure_endcap_thickness,
				_enclosure_thickness, 0.,
				G4ThreeVector(0., 0., 0.), 0);
     _enclosure_flange_gen = 
       new CylinderPointSampler(_enclosure_in_diam/2., 
				_enclosure_endcap_thickness, 
				_enclosure_endcap_diam/2.-_enclosure_in_diam/2.
				, 0., G4ThreeVector(0., 0., - _enclosure_length/2. +  _enclosure_endcap_thickness/2.));
     _enclosure_cap_gen = 
       new CylinderPointSampler(0., _enclosure_endcap_thickness/2., 
				_enclosure_in_diam/2., 0.,
				G4ThreeVector (0., 0., - _enclosure_length/2. + _enclosure_endcap_thickness/4.));
     _enclosure_window_gen = 
       new CylinderPointSampler(0., _enclosure_window_thickness,
				_enclosure_window_diam/2., 0.,
				G4ThreeVector (0., 0., _enclosure_length/2. - _enclosure_window_thickness/2.));
     _enclosure_pad_gen = new CylinderPointSampler(0., _enclosure_pad_thickness, _enclosure_in_diam/2., 0., G4ThreeVector(0.,0.,pad_z_pos));
     
     // Getting the enclosure body volume over total
     G4double body_vol = 
       (_enclosure_length-_enclosure_endcap_thickness) * pi * 
       ((_enclosure_in_diam/2.+_enclosure_thickness)*
	(_enclosure_in_diam/2.+_enclosure_thickness) - 
	(_enclosure_in_diam/2.)*(_enclosure_in_diam/2.) );
     G4double flange_vol = 
       _enclosure_endcap_thickness * pi * 
       ((_enclosure_endcap_diam/2.)*(_enclosure_endcap_diam/2.)-
	(_enclosure_in_diam/2.)*(_enclosure_in_diam/2.));
     G4double cap_vol = 
       _enclosure_endcap_thickness/2 * pi * 
       (_enclosure_in_diam/2.) * (_enclosure_in_diam/2.);
     G4double total_vol = body_vol + flange_vol + cap_vol;
     _body_perc = body_vol / total_vol;
     _flange_perc =  (flange_vol + body_vol) / total_vol;
    
   }

  Enclosure::~Enclosure()
  {
    delete _enclosure_body_gen;
    delete _enclosure_flange_gen;
    delete _enclosure_cap_gen;
    delete _enclosure_window_gen;
    delete _enclosure_pad_gen;
  }

  G4ThreeVector Enclosure::GetObjectCenter(){ return G4ThreeVector(0., 0., _enclosure_length/2.);}
  
  G4ThreeVector Enclosure::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    
    /// Enclosures bodies
    if (region == "ENCLOSURE_BODY") {
      G4double rand1 = G4UniformRand();
      // Generating in the cilindric part of the enclosure
      if (rand1 < _body_perc) {
	vertex = _enclosure_body_gen->GenerateVertex(VOLUME);
      }
      // Generating in the union/flange
      else if (rand1 < _flange_perc){
       	vertex = _enclosure_flange_gen->GenerateVertex(VOLUME);
      }
      // Generating in the rear cap of the enclosure
      else {
       	vertex = _enclosure_cap_gen->GenerateVertex(VOLUME);
      }
     
    }
    /// Enclosures windows
    else if (region == "ENCLOSURE_WINDOW") {
      vertex = _enclosure_window_gen->GenerateVertex(VOLUME);
      //z translation made in CylinderPointSampler
    }
    //Optical pad
    else if (region=="OPTICAL_PAD"){
      vertex =_enclosure_pad_gen->GenerateVertex(VOLUME);
      //z translation made in CylinderPointSampler    
    }
    //PMTs bodies 
    else if (region == "PMT_BODY") {
      vertex = _pmt->GenerateVertex(region);
      vertex.setZ(vertex.z() + _pmt_z_pos);
    }   
    return vertex;
  }

}//end namespace nexus
