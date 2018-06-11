// ----------------------------------------------------------------------------
//  $Id$
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
#include "Visibilities.h"

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

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>


namespace nexus{

  using namespace CLHEP;

  Enclosure::Enclosure():

    // Enclosures dimensions
    _enclosure_in_diam (80. * mm), 
    _enclosure_length (200.0 * mm),
    _enclosure_thickness (6. * mm),
    _enclosure_endcap_diam (119. * mm), 
    _enclosure_endcap_thickness (60. * mm),
    _enclosure_window_diam (85. * mm), 
    _enclosure_window_thickness (6. * mm), //???
    _enclosure_pad_thickness (1. * mm),//max 60  ??????
    //   _enclosure_tpb_thickness(1.*micrometer),
    _pmt_base_diam (47. *mm),
    _pmt_base_thickness (5. *mm),
    _pmt_base_z (50. *mm), //distance from window
    _visibility(1)
  {
    /// Initializing the geometry navigator (used in vertex generation)
    _geom_navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("enclosure_vis", _visibility, "Vessel Visibility");

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
    G4Material* optical_coupler = MaterialsList::OpticalSilicone();
    optical_coupler->SetMaterialPropertiesTable(OpticalMaterialProperties::OptCoupler());

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
       new G4LogicalVolume(enclosure_gas_solid, vacuum, "ENCLOSURE_GAS");
     G4double gas_pos = 
       (_enclosure_endcap_thickness/2-_enclosure_window_thickness-
     	_enclosure_pad_thickness)/2;
     new G4PVPlacement(0,G4ThreeVector(0.,0.,gas_pos), enclosure_gas_logic,
		       "ENCLOSURE_GAS", enclosure_logic, false, 0, false);

     // Adding the sapphire window
     G4Tubs* enclosure_window_solid = 
       new G4Tubs("ENCLOSURE_WINDOW", 0., _enclosure_window_diam/2., 
		  _enclosure_window_thickness/2., 
		  0., twopi);
     G4LogicalVolume* enclosure_window_logic = 
       new G4LogicalVolume(enclosure_window_solid, sapphire,
			   "ENCLOSURE_WINDOW");
    
     _window_z_pos = _enclosure_length/2 - _enclosure_window_thickness/2.;
     new G4PVPlacement(0, G4ThreeVector(0.,0.,_window_z_pos),
		       enclosure_window_logic, "ENCLOSURE_WINDOW", 
		       enclosure_logic, false, 0, false);

     // Adding the optical pad
     G4Tubs* enclosure_pad_solid =
       new G4Tubs("OPTICAL_PAD", 0., _enclosure_in_diam/2., _enclosure_pad_thickness/2.,0.,twopi);
     G4LogicalVolume* enclosure_pad_logic = 
       new G4LogicalVolume(enclosure_pad_solid, optical_coupler, "OPTICAL_PAD");
     G4double pad_z_pos = 
       _window_z_pos-_enclosure_window_thickness/2.-_enclosure_pad_thickness/2.;
     new G4PVPlacement(0, G4ThreeVector(0.,0.,pad_z_pos),
		       enclosure_pad_logic,"OPTICAL_PAD",
		       enclosure_logic, false, 0, false);
     
     // Adding the PMT
     _pmt->Construct();
     G4LogicalVolume* pmt_logic = _pmt->GetLogicalVolume();
     G4double pmt_rel_z_pos = _pmt->GetRelPosition().z();//return G4ThreeVector(0.,0., _front_body_length/2.);
     _pmt_z_pos = gas_length/2.- pmt_rel_z_pos;
     new G4PVPlacement(0, G4ThreeVector(0.,0.,_pmt_z_pos), pmt_logic,
		       "PMT", enclosure_gas_logic, false, 0, false);
     // Adding the PMT base
     G4Tubs* pmt_base_solid = 
       new G4Tubs("PMT_BASE", 0., _pmt_base_diam/2., _pmt_base_thickness, 0.,twopi);
     G4LogicalVolume* pmt_base_logic = 
       new G4LogicalVolume(pmt_base_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON"),
			   "PMT_BASE"); 
     new G4PVPlacement(0, G4ThreeVector(0.,0., -_pmt_base_z),
		       pmt_base_logic, "PMT_BASE", enclosure_gas_logic, false, 0, false);
     //std::cout<<"1_pmt_base_z_pos  "<< -_pmt_base_z<<std::endl; 
     
     /////  SETTING VISIBILITIES   //////////   
     if (_visibility) { 
       G4VisAttributes copper_col = nexus::CopperBrown();
       //      copper_col.SetForceSolid(true);
       enclosure_logic->SetVisAttributes(copper_col);
       G4VisAttributes sapphire_col = nexus::Lilla();
       sapphire_col.SetForceSolid(true);
       enclosure_window_logic->SetVisAttributes(sapphire_col);
       G4VisAttributes pad_col = nexus::LightGreen();
       pad_col.SetForceSolid(true);
       enclosure_pad_logic->SetVisAttributes(pad_col);
       G4VisAttributes base_col = nexus::Yellow();
       base_col.SetForceSolid(true);
       pmt_base_logic->SetVisAttributes(base_col);
     } else {
       enclosure_logic->SetVisAttributes(G4VisAttributes::Invisible);        
       enclosure_window_logic->SetVisAttributes(G4VisAttributes::Invisible);
       enclosure_pad_logic->SetVisAttributes(G4VisAttributes::Invisible);
       pmt_base_logic->SetVisAttributes(G4VisAttributes::Invisible);
     }
     
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
     _enclosure_pad_gen =
       new CylinderPointSampler(0., _enclosure_pad_thickness, _enclosure_in_diam/2., 0., G4ThreeVector(0.,0.,pad_z_pos));

     _pmt_base_gen =
       new CylinderPointSampler(0., _pmt_base_thickness, _pmt_base_diam/2., 0., G4ThreeVector(0.,0., -_pmt_base_z +10.*mm ));//10mm fitting???

     _enclosure_surf_gen =
       new CylinderPointSampler(gas_diam/2., gas_length, 0., 0., G4ThreeVector (0., 0., gas_pos));

     _enclosure_cap_surf_gen =
       new CylinderPointSampler(0., 0.1 * micrometer, gas_diam/2., 0.,
					       G4ThreeVector (0., 0., - gas_pos/2. + 0.05 * micrometer));

     
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
     // std::cout<<"ENCLOSURE VOLUME: \t"<<total_vol<<std::endl;
     // std::cout<<"ENCLOSURE WINDOW  VOLUME: \t"<<enclosure_window_solid->GetCubicVolume()<<std::endl;
     G4double enclosure_int_surf = 2. * pi * gas_diam/2. * gas_length;
     G4double enclosure_int_cap_surf = pi * gas_diam/2. * gas_diam/2.;
     G4double total_surf = enclosure_int_surf + enclosure_int_cap_surf;
     _int_surf_perc = enclosure_int_surf / total_surf;
     _int_cap_surf_perc = enclosure_int_cap_surf / total_surf;

   }

  Enclosure::~Enclosure()
  {
    delete _enclosure_body_gen;
    delete _enclosure_flange_gen;
    delete _enclosure_cap_gen;
    delete _enclosure_window_gen;
    delete _enclosure_pad_gen;
    delete _pmt_base_gen;
    delete _enclosure_surf_gen;
  }

  G4ThreeVector Enclosure::GetObjectCenter()
  { return G4ThreeVector(0., 0., _enclosure_length/2.);}

  G4double Enclosure::GetWindowDiameter()
  { return _enclosure_window_diam;}
  
  G4ThreeVector Enclosure::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    
    /// Enclosures bodies
    if (region == "ENCLOSURE_BODY") {
      G4double rand1 = G4UniformRand();
      // Generating in the cilindric part of the enclosure
      if (rand1 < _body_perc) {
	vertex = _enclosure_body_gen->GenerateVertex("BODY_VOL");
      }
      // Generating in the union/flange
      else if (rand1 < _flange_perc){
       	vertex = _enclosure_flange_gen->GenerateVertex("BODY_VOL");
      }
      // Generating in the rear cap of the enclosure
      else {
       	vertex = _enclosure_cap_gen->GenerateVertex("BODY_VOL");
      }
     
    }
    /// Enclosures windows
    else if (region == "ENCLOSURE_WINDOW") {
      vertex = _enclosure_window_gen->GenerateVertex("BODY_VOL");
      //z translation made in CylinderPointSampler
    }
    //Optical pad
    else if (region=="OPTICAL_PAD"){
      vertex =_enclosure_pad_gen->GenerateVertex("BODY_VOL");
      //z translation made in CylinderPointSampler    
    }
    //PMT base 
    else if (region=="PMT_BASE"){
      vertex =_pmt_base_gen->GenerateVertex("BODY_VOL");
      //z translation made in CylinderPointSampler 
      //std::cout<<"vertx z  "<< vertex.z()<<std::endl;  
    }
    //PMTs bodies 
    else if (region == "PMT_BODY") {
      vertex = _pmt->GenerateVertex(region);
      vertex.setZ(vertex.z() + _pmt_z_pos);
    }
    // Internal surface of enclosure
    else if (region == "INT_ENCLOSURE_SURF") {
      G4double rand1 = G4UniformRand();
      if (rand1 < _int_surf_perc) {
        vertex = _enclosure_surf_gen->GenerateVertex("BODY_SURF");
      }
      else {
        vertex = _enclosure_cap_surf_gen->GenerateVertex("WHOLE_VOL");
      }
    }
    // External surface of PMT
    else if (region == "PMT_SURF") {
      vertex = _pmt->GenerateVertex(region);
      vertex.setZ(vertex.z() + _pmt_z_pos);
    }
    return vertex;
  }

}//end namespace nexus
