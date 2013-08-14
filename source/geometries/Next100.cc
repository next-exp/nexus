// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <justo.martin-albo@ific.uv.es>, <jmunoz@ific.uv.es>
//  Created: 21 Nov 2011
//  
//  Copyright (c) 2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next100.h"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>



namespace nexus {

  
  Next100::Next100():
    BaseGeometry(),
    // Lab dimensions
    _lab_size (5. * m),
    // Buffer gas dimensions
    _buffer_gas_size (4. * m),
    // Nozzles external diam and y positions
    _nozzle_ext_diam (9. * cm),
    _up_nozzle_ypos (20. * cm),
    _central_nozzle_ypos (0. * cm),
    _down_nozzle_ypos (-20. * cm),
    _bottom_nozzle_ypos(-53. * cm)
  {

  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones


  // The lab
  BuildLab();

  // Shielding
  _shielding = new Next100Shielding(_nozzle_ext_diam, _up_nozzle_ypos, _central_nozzle_ypos,
				    _down_nozzle_ypos, _bottom_nozzle_ypos);
  G4LogicalVolume* shielding_logic = _shielding->GetLogicalVolume();
  G4PVPlacement*   shielding_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), shielding_logic,
						       "LEAD_BOX", _buffer_gas_logic, false, 0);
  //G4LogicalVolume* shielding_internal_logic = _shielding.GetInternalLogicalVolume();

  // Vessel
  _vessel = new Next100Vessel(_nozzle_ext_diam, _up_nozzle_ypos, _central_nozzle_ypos,
			      _down_nozzle_ypos, _bottom_nozzle_ypos);
  G4LogicalVolume* vessel_logic = _vessel->GetLogicalVolume();
  G4PVPlacement*   vessel_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), vessel_logic,
						    "VESSEL", _buffer_gas_logic, false, 0);
  G4LogicalVolume* vessel_internal_logic = _vessel->GetInternalLogicalVolume();

  // Internal copper shielding
  _ics = new Next100Ics(_nozzle_ext_diam, _up_nozzle_ypos, _central_nozzle_ypos,
			_down_nozzle_ypos, _bottom_nozzle_ypos);
  G4LogicalVolume* ics_logic = _ics->GetLogicalVolume();
  G4PVPlacement*   ics_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), ics_logic,
						 "ICS", vessel_internal_logic, false, 0);

  // INNER ELEMENTS
  _inner_elements = new Next100InnerElements(vessel_internal_logic);


  }
  
  
  
  Next100::~Next100()
  {
    delete _inner_elements;
    delete _ics;
    delete _vessel;
    delete _shielding;
  }
  

  void Next100::BuildLab()
  {
    // LAB /////////////////////////////////////////////////////////////
    // This is just a volume of air surrounding the detector so that
    // events (from calibration sources or cosmic rays) can be generated 
    // on the outside.
    
    G4Box* lab_solid = 
      new G4Box("LAB", _lab_size/2., _lab_size/2., _lab_size/2.);
    
    _lab_logic = new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    _lab_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Set this volume as the wrapper for the whole geometry 
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(_lab_logic);


    // BUFFER GAS   ////////////////////////////////////////////////////////////
    // This is a volume, initially made of air, defined to be the mother volume
    // of Shielding and Vessel

    G4Box* buffer_gas_solid = 
      new G4Box("BUFFER_GAS", _buffer_gas_size/2., _buffer_gas_size/2., _buffer_gas_size/2.);
    
    _buffer_gas_logic = new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "BUFFER_GAS");
    _buffer_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);

    G4PVPlacement* buffer_gas_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), _buffer_gas_logic,
						       "BUFFER_GAS", _lab_logic, false, 0);

  }
  




  G4ThreeVector Next100::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // Shielding regions
    if ((region == "SHIELDING_LEAD") || (region == "SHIELDING_STEEL") ||
	(region == "EXTERNAL")       || (region == "SHIELDING_GAS") ) {
      vertex = _shielding->GenerateVertex(region);
    }

    // Vessel regions
    else if ((region == "VESSEL") || (region == "VESSEL_FLANGES") ||
	     (region == "VESSEL_TRACKING_ENDCAP") || (region == "VESSEL_ENERGY_ENDCAP")) {
      vertex = _vessel->GenerateVertex(region);
    }

    // ICS regions
    else if (region == "ICS") {
      vertex = _ics->GenerateVertex(region);
    }

    // INNER ELEMENTS regions
    else if ((region == "FIELD_CAGE") || (region == "ACTIVE") ||
	     (region == "CARRIER_PLATE") || (region == "ENCLOSURE_BODY") ||
	     (region == "ENCLOSURE_WINDOW") || (region == "PMT_BODY") ||
	     (region == "TRK_SUPPORT") || (region == "DICE_BOARD") ) {
      vertex = _inner_elements->GenerateVertex(region);
    }

    return vertex;
  }


} //end namespace nexus
