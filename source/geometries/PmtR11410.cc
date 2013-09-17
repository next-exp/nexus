// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  Javier Muñoz Vidal <jmunoz@ific.uv.es>
//  Created: 1 January 2013
//  
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "PmtR11410.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "PmtSD.h"
#include "CylinderPointSampler.h"

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



namespace nexus {
  
  
  PmtR11410::PmtR11410():
    BaseGeometry(),
    // Dimensions
    _front_body_diam (76. * mm),
    _front_body_length (38. * mm),
    _rear_body_diam (53. * mm),
    _rear_body_length (76. * mm),
    _body_thickness (.5 * mm),       // To be checked
    _window_thickness (2. * mm),
    _photocathode_diam (64. * mm),
    _photocathode_thickness (.1 * mm)
  {
  }
  


  void PmtR11410::Construct()
  {
    // PMT BODY //////////////////////////////////////////////////////

    G4Tubs* front_body_solid = 
      new G4Tubs("FRONT_BODY", 0., _front_body_diam/2., _front_body_length/2., 
        0., twopi);

    G4Tubs* rear_body_solid = 
      new G4Tubs("REAR_BODY", 0., _rear_body_diam/2., _rear_body_length/2., 
        0., twopi);

    // Union of the two volumes of the phototube body
    G4double z_transl = -_front_body_length/2. - _rear_body_length/2.;
    G4ThreeVector transl(0., 0., z_transl);
    G4UnionSolid* pmt_solid = 
      new G4UnionSolid("PMT_R11410",front_body_solid,rear_body_solid,0,transl);
 
 
    G4Material* Kovar = MaterialsList::Kovar();

    G4LogicalVolume* pmt_logic = 
      new G4LogicalVolume(pmt_solid, Kovar, "PMT_R11410");
    this->SetLogicalVolume(pmt_logic);


    // PMT GAS  //////////////////////////////////////////////////////

    G4double front_body_gas_diam = _front_body_diam - 2. * _body_thickness;
    G4double front_body_gas_length = _front_body_length - _body_thickness;
    G4Tubs* front_body_gas_solid = 
      new G4Tubs("FRONT_BODY_GAS", 0., front_body_gas_diam/2., front_body_gas_length/2., 0., twopi);

    G4double rear_body_gas_diam = _rear_body_diam - 2. * _body_thickness;
    G4double rear_body_gas_length = _rear_body_length;
    G4Tubs* rear_body_gas_solid = 
      new G4Tubs("REAR_BODY_GAS", 0., rear_body_gas_diam/2., rear_body_gas_length/2., 0., twopi);

    // Union of the two volumes of the phototube body gas
    G4double z_gas_transl = -front_body_gas_length/2. - rear_body_gas_length/2.;
    G4ThreeVector gas_transl(0., 0., z_gas_transl);
    G4UnionSolid* pmt_gas_solid = new G4UnionSolid("PMT_GAS", front_body_gas_solid,
						   rear_body_gas_solid, 0, gas_transl);

    G4Material* pmt_gas_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    G4LogicalVolume* pmt_gas_logic = new G4LogicalVolume(pmt_gas_solid, pmt_gas_mat, "PMT_GAS");

    G4double pmt_gas_posz = _body_thickness/2.;
    G4PVPlacement* pmt_gas_physi = new G4PVPlacement(0, G4ThreeVector(0., 0., pmt_gas_posz), pmt_gas_logic,
						     "PMT_GAS", pmt_logic, false, 0);


    // PMT WINDOW ////////////////////////////////////////////////////

    _window_diam = front_body_gas_diam;
    G4Tubs* window_solid =
      new G4Tubs("PMT_WINDOW", 0, _window_diam/2., _window_thickness/2., 0., twopi);

    G4Material* silica = MaterialsList::FusedSilica();
    silica->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
    G4LogicalVolume* window_logic = new G4LogicalVolume(window_solid, silica, "PMT_WINDOW");

    G4double window_posz = front_body_gas_length/2. - _window_thickness/2.;
    G4PVPlacement* window_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,window_posz), window_logic,
    						    "PMT_WINDOW", pmt_gas_logic, false, 0);


    // PMT PHOTOCATHODE  /////////////////////////////////////////////

    G4Tubs* photocathode_solid =
      new G4Tubs("PMT_PHOTOCATHODE", 0, _photocathode_diam/2., _photocathode_thickness/2., 0., twopi);

    G4Material* aluminum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");	
    G4LogicalVolume* photocathode_logic = new G4LogicalVolume(photocathode_solid, aluminum, "PMT_PHOTOCATHODE");

    G4double photocathode_posz = window_posz - _window_thickness/2. - _photocathode_thickness/2.;
    G4PVPlacement* photocathode_physi = new G4PVPlacement(0, G4ThreeVector(0., 0., photocathode_posz), photocathode_logic,
     							  "PMT_PHOTOCATHODE", pmt_gas_logic, false, 0);

    // Optical properties
    G4OpticalSurface* pmt_opt_surf = GetPhotOptSurf();
    new G4LogicalSkinSurface("PMT_PHOTOCATHODE", photocathode_logic, pmt_opt_surf);

    // Sensitive detector
    PmtSD* pmtsd = new PmtSD("/PMT_R11410/PMT_PHOTOCATHODE");
    pmtsd->SetDetectorVolumeDepth(4);
    G4SDManager::GetSDMpointer()->AddNewDetector(pmtsd);
    window_logic->SetSensitiveDetector(pmtsd);


    // VISIBILITIES //////////////////////////////////////////////////

    G4VisAttributes grey(G4Colour(.7, .7, .7));
    pmt_logic->SetVisAttributes(grey);
    pmt_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);
    window_logic->SetVisAttributes(G4VisAttributes::Invisible);

    G4VisAttributes brown(G4Colour(.93, .87, .8));
    brown.SetForceSolid(true);
    photocathode_logic->SetVisAttributes(brown);


    // VERTEX GENERATORS /////////////////////////////////////////////

    G4double front_body_irad = _front_body_diam/2. - _body_thickness;
    G4double rear_body_irad  = _rear_body_diam/2. - _body_thickness;

    _front_body_gen = new CylinderPointSampler(front_body_irad, _front_body_length, _body_thickness, 0.,
					       G4ThreeVector (0., 0., 0.));

    _medium_body_gen = new CylinderPointSampler(_rear_body_diam/2., _body_thickness, front_body_irad - _rear_body_diam/2., 0.,
						G4ThreeVector (0., 0., -_front_body_length/2. + _body_thickness/2.));

    _rear_body_gen = new CylinderPointSampler(rear_body_irad, _rear_body_length+_body_thickness, _body_thickness, 0.,
					      G4ThreeVector (0., 0., -_front_body_length/2. - _rear_body_length/2. + _body_thickness/2.));

    _rear_cap_gen = new CylinderPointSampler(rear_body_irad, _body_thickness, 0., 0.,
					     G4ThreeVector (0., 0., -_front_body_length/2. - _rear_body_length + _body_thickness/2.));



    // Getting the enclosure body volume over total
    G4double front_body_vol  = _front_body_length * pi * ((_front_body_diam/2.)*(_front_body_diam/2.) - front_body_irad*front_body_irad);
    G4double medium_body_vol = _body_thickness * pi * (front_body_irad*front_body_irad - (_rear_body_diam/2.)*(_rear_body_diam/2.));
    G4double rear_body_vol   = (_rear_body_length+_body_thickness) * pi * ((_rear_body_diam/2.)*(_rear_body_diam/2.) - rear_body_irad*rear_body_irad);
    G4double rear_cap_vol    = _body_thickness * pi * rear_body_irad*rear_body_irad;
    G4double total_vol       = front_body_vol + medium_body_vol + rear_body_vol + rear_cap_vol;
    _front_body_perc  = front_body_vol / total_vol;
    _fr_med_body_perc = (front_body_vol + medium_body_vol) / total_vol;
    _fr_med_re_body_perc = (front_body_vol + medium_body_vol + rear_body_vol) / total_vol;
  }  
  


  PmtR11410::~PmtR11410()
  {
  }


  
  G4ThreeVector PmtR11410::GetRelPosition()
  {
    return G4ThreeVector(0., 0., _front_body_length/2.);
  }

  

  G4ThreeVector PmtR11410::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);

    if (region == "PMT_BODY") {
      G4double rand1 = G4UniformRand();
      if (rand1 < _front_body_perc) 
        vertex = _front_body_gen->GenerateVertex(VOLUME);
      else if (rand1 < _fr_med_body_perc)
        vertex = _medium_body_gen->GenerateVertex(VOLUME);
      else if (rand1 < _fr_med_re_body_perc)
        vertex = _rear_body_gen->GenerateVertex(VOLUME);
      else
        vertex = _rear_cap_gen->GenerateVertex(INSIDE);
    }

    return vertex;
  }



  G4OpticalSurface* PmtR11410::GetPhotOptSurf() 
  {
  
    const G4int entries = 63;

    G4double ENERGIES[entries] =
      {	1.96800306364 *eV,  1.99974504854 *eV,  2.03252775425 *eV,  2.06640321682 *eV,  
	2.10142700016 *eV,  2.13765850016 *eV,  2.17516128086 *eV,  2.21400344659 *eV,  
	2.25425805471 *eV,  2.29600357425 *eV,  2.3393243964 *eV,   2.38431140402 *eV,  
	2.43106260802 *eV,  2.47968386018 *eV,  2.53028965325 *eV,  2.58300402103 *eV,  
	2.63796155339 *eV,  2.69530854368 *eV,  2.75520428909 *eV,  2.81782256839 *eV, 
	2.8833533258 *eV,   2.95200459546 *eV,  3.02400470754 *eV,  3.09960482523 *eV,  
	3.17908187203 *eV,  3.2627419213 *eV,   3.35092413538 *eV,  3.44400536137 *eV, 
	3.54240551455 *eV,  3.64659391204 *eV,  3.75709675786 *eV,  3.87450603154 *eV,  
	3.99949009707 *eV,  4.13280643364 *eV,  4.27531700032 *eV,  4.42800689319 *eV,  
	4.59200714849 *eV,  4.76862280805 *eV,  4.95936772037 *eV,  5.16600804205 *eV, 
	5.39061708736 *eV,  5.63564513678 *eV,  5.90400919092 *eV,  6.19920965046 *eV,  
	6.35816374406 *eV,  6.70184827077 *eV,  7.0848110291 *eV, 7.12553 *eV, 
	7.16672 *eV, 7.18749 *eV, 7.20838 *eV, 7.2294 *eV, 
	7.25054 *eV, 7.2718 *eV, 7.29319 *eV, 7.3147 *eV, 
	7.33634 *eV, 7.38001 *eV, 7.40204 *eV, 7.4242 *eV, 
	7.46893 *eV, 7.51419351571 *eV, 7.60639220916 *eV};

    G4double EFFICIENCY[entries] =
      { 0.0105, 0.0152, 0.0210, 0.0270,  
	0.0362, 0.0455, 0.0558, 0.0695,  
	0.0820, 0.1020, 0.1178, 0.1340,  
	0.1490, 0.1590, 0.1742, 0.1845,  
	0.1950, 0.2130, 0.2240, 0.2380,  
	0.2495, 0.2605, 0.2698, 0.2740,  
	0.2798, 0.2840, 0.2898, 0.2920,  
	0.2968, 0.3030, 0.3030, 0.3020,  
	0.2970, 0.2860, 0.2707, 0.2600,  
	0.2390, 0.2295, 0.2240, 0.2200,  
	0.2078, 0.2090, 0.2148, 0.2320,  
	0.2528, 0.2740, 0.2840, 0.2700, 
	0.2550, 0.2450, 0.2360, 0.2210,
	0.2080, 0.1980, 0.1900, 0.1780, 
	0.1600,	0.1470, 0.1350, 0.1200, 
	0.1000, 0.0850, 0.0480  };

    G4double REFLECTIVITY[entries] =
      { 0., 0., 0., 0., 0.,
    	0., 0., 0., 0., 0.,
    	0., 0., 0., 0., 0.,
    	0., 0., 0., 0., 0.,
    	0., 0., 0., 0., 0.,
    	0., 0., 0., 0., 0. };
    
    G4MaterialPropertiesTable* phcath_mpt = new G4MaterialPropertiesTable();
    phcath_mpt->AddProperty("EFFICIENCY", ENERGIES, EFFICIENCY, entries);
    phcath_mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, entries);
    
    G4OpticalSurface* opt_surf = new G4OpticalSurface("PHOTOCATHODE", unified, polished, dielectric_metal);
    opt_surf->SetMaterialPropertiesTable(phcath_mpt);

    return opt_surf;
  }
  
} // end namespace nexus
