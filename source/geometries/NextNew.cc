// ----------------------------------------------------------------------------
//  $Id: NextNew.cc  $
//
//  Author:  Miquel Nebot Guinot <miquel.nebot@ific.uv.es>
//           <jmunoz@ific.uv.es>, <justo.martin-albo@ific.uv.es>   
//  Created: Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNew.h"

//#include "MaterialsList.h"
#include "Next100Shielding.h"
#include "NextNewVessel.h"
#include "NextNewIcs.h"
#include "NextNewInnerElements.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>

#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>


namespace nexus {

  NextNew::NextNew():
    BaseGeometry(),
    // Lab dimensions
    _lab_size (5. * m),
    // Buffer gas dimensions
    _buffer_gas_size (4. * m)
  {
    //Shielding
    _shielding = new Next100Shielding(100*mm,0.,0.,0.,0.);
    //Vessel
    _vessel = new NextNewVessel();
    //ICS
    _ics = new NextNewIcs();   
    //Inner elements
    _inner_elements = new NextNewInnerElements();
  }

  NextNew::~NextNew()
  {
    //deletes
    delete _shielding;
    delete _vessel;
    delete _ics;
    delete _inner_elements;
    
    delete _lab_gen;
  }

 void NextNew::Construct()
  {
    // LAB /////////////////////////////////////////////////////////////
    // This is just a volume of air surrounding the detector so that events 
    //(from calibration sources or cosmic rays) can be generated on the outside.
    
    G4Box* lab_solid = 
      new G4Box("LAB", _lab_size/2., _lab_size/2., _lab_size/2.);
    
    _lab_logic = new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    
    //_lab_logic->SetVisAttributes(G4VisAttributes (G4Colour(.46, .46, .46)));
    _lab_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Set this volume as the wrapper for the whole geometry 
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(_lab_logic);

   
    // BUFFER GAS   ///////////////////////////////////////////////////////////////////////////////////
    // This is a volume, initially made of air, defined to be the mother volume of Shielding and Vessel

    G4Box* buffer_gas_solid = 
      new G4Box("BUFFER_GAS", _buffer_gas_size/2., _buffer_gas_size/2., _buffer_gas_size/2.);
    
    _buffer_gas_logic = new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "BUFFER_GAS");
    // _buffer_gas_logic->SetVisAttributes(G4VisAttributes (G4Colour(.86, .86, .86)));
    _buffer_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);

    G4PVPlacement* buffer_gas_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), _buffer_gas_logic,
                                                       "BUFFER_GAS", _lab_logic, false, 0);


    //SHIELDING
    _shielding->Construct();
    G4LogicalVolume* shielding_logic = _shielding->GetLogicalVolume();
    G4PVPlacement* shielding_physi = new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),shielding_logic, "SHIELDING",
						       _buffer_gas_logic, false, 0, true);

    //VESSEL
    _vessel->Construct();
    G4LogicalVolume* vessel_logic = _vessel->GetLogicalVolume();
    G4ThreeVector position(0.,0.,0.);
    G4PVPlacement* vessel_physi = new G4PVPlacement(0, position, vessel_logic, "VESSEL",
     						    _buffer_gas_logic, false, 0, true);
    G4LogicalVolume* vessel_gas_logic = _vessel->GetInternalLogicalVolume();
      
    //ICS
    _ics->SetLogicalVolume(vessel_gas_logic);
    _ics->SetNozzlesZPosition( _vessel->GetLATNozzleZPosition(),_vessel->GetUPNozzleZPosition());
    _ics->Construct();

    // G4LogicalVolume* ics_logic = _ics->GetLogicalVolume();
    // G4PVPlacement* ics_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), ics_logic, "ICS", _buffer_gas_logic,false, 0, true);
 
    //INNER ELEMENTS
    _inner_elements->SetLogicalVolume(vessel_gas_logic);
    _inner_elements->Construct();

    //// VERTEX GENERATORS   //
    _lab_gen = new BoxPointSampler(_buffer_gas_size,_buffer_gas_size,_buffer_gas_size, _lab_size-_buffer_gas_size,G4ThreeVector(0.,0.,0.),0);


  }
    
  G4ThreeVector NextNew::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);
    //BUFFER GAS
    if (region == "LAB") {
      vertex = _lab_gen->GenerateVertex(region);
    }
    //LEAD CASTLE
    if ( (region == "SHIELDING_LEAD") || (region == "SHIELDING_STEEL") || 
	 (region == "SHIELDING_GAS") ||  (region == "EXTERNAL") ) {
      vertex = _shielding->GenerateVertex(region);
    }
    //VESSEL REGIONS
    if ( (region == "VESSEL") || 
	 (region == "SOURCE_PORT_ANODE") ||
	 (region == "SOURCE_PORT_CATHODE")) {
      vertex = _vessel->GenerateVertex(region);
    }
    // ICS REGIONS
    else if (region == "ICS"){  
      vertex = _ics->GenerateVertex(region);
    }
    //INNER ELEMENTS
    else if ( (region == "CENTER") ||
	      (region == "CARRIER_PLATE") || (region == "ENCLOSURE_BODY") || (region == "ENCLOSURE_WINDOW") || 
	      (region=="OPTICAL_PAD") || (region == "PMT_BODY") ||
	      (region == "FIELD_CAGE") || (region== "REFLECTOR") || (region == "ACTIVE") ||
	      (region == "SUPPORT_PLATE") || (region == "DICE_BOARD") ){
      vertex = _inner_elements->GenerateVertex(region);
    }
   
    return vertex;
  }
  
  
} //end namespace nexus
