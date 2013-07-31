// ----------------------------------------------------------------------------
//  $Id: NEW.cc  $
//
//  Author:  Miquel Nebot Guinot <miquel.nebot@ific.uv.es>    
//  Created: 25 Jul 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NEW.h"

#include "MaterialsList.h"
#include "BoxPointSampler.h"
#include "CylinderPointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>


namespace nexus {

  NEW::NEW():
    BaseGeometry(),
    // Lab dimensions
    _lab_size (5. * m),
    // Buffer gas dimensions
    _buffer_gas_size (4. * m),
    //Vessel Diameter
    _vessel_dim (65.* cm),
    //Vessel length
    _vessel_length (90. * cm),
    //inner copper shielding width
    _ics_width (6. * cm),
    //Pressure Gas Xenon
    _pressure (10. * bar)
  {}

  NEW::~NEW()
  {}

 void NEW::Construct()
  {
    // LAB /////////////////////////////////////////////////////////////
    // This is just a volume of air surrounding the detector so that events 
    //(from calibration sources or cosmic rays) can be generated on the outside.
    
    G4Box* lab_solid = 
      new G4Box("LAB", _lab_size/2., _lab_size/2., _lab_size/2.);
    
    _lab_logic = new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    _lab_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Set this volume as the wrapper for the whole geometry 
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(_lab_logic);

    // Internal Copper Shielding
    G4Material* copper= G4NistManager::Instance()->FindOrBuildMaterial("Cu");
    G4Tubs* _ics=
      new G4Tubs ("ICS",0.*cm,_vessel_dim,_vessel_length,0.*degree,360.*degree);
    G4LogicalVolume* ics_logic = new G4LogicalVolume(_ics,copper,"ics_logic");
    G4PVPlacement*   ics_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), ics_logic,
						   "ICS", _lab_logic, false, 0);
    
    //Xenon Fiducial Volume
    G4Tubs* _xefv=
      new G4Tubs ("XEFV",0.*cm,_vessel_dim -_ics_width,_vessel_length,0.*degree,360.*degree);
    G4Material* xenon = 0;
    xenon = MaterialsList::GXe(_pressure);
    G4LogicalVolume* xefv_logic= new G4LogicalVolume(_xefv,xenon,"xefv_logic");
    G4PVPlacement* xefv_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), xefv_logic, 
						  "XEFV",xefv_logic,false, 0);

    // Creating the vertex generators   //////////
    _lab_gen = new BoxPointSampler(0,0,0,_lab_size/2,G4ThreeVector(0,0,0),0);
    _ics_gen = new CylinderPointSampler(_vessel_dim -_ics_width,_vessel_length-_ics_width,_ics_width,_ics_width,G4ThreeVector(0,0,0),0);
    _xefv_gen = new CylinderPointSampler(0.*cm,0.*cm, _vessel_dim -_ics_width,_vessel_length-_ics_width,G4ThreeVector(0,0,0),0);


  }
    
  G4ThreeVector NEW::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    //BUFFER GAS
    if (region == "LAB") {
      vertex = _lab_gen->GenerateVertex(region);
    }
    // ICS regions
    else if (region == "ICS") {
      vertex = _ics_gen->GenerateVertex(region);
    }
    //Xenon Fiducial Volume
    else 
      vertex=_xefv_gen->GenerateVertex("XEFV");
    
    return vertex;
  }
  
  
} //end namespace nexus
