// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 18 Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewInnerElements.h"
#include "NextNewEnergyPlane.h"
#include "NextNewFieldCage.h"
#include "NextNewTrackingPlane.h"
#include "MaterialsList.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4Region.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

namespace nexus {

  using namespace CLHEP;

  NextNewInnerElements::NextNewInnerElements():
    BaseGeometry()
  {
    // Build the internal objects that live there 
    _energy_plane = new NextNewEnergyPlane();
    _field_cage = new NextNewFieldCage();
    _tracking_plane = new NextNewTrackingPlane();
  }
   
  void NextNewInnerElements::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  void NextNewInnerElements::Construct()
  {
    // Reading material
    _gas = _mother_logic->GetMaterial();
    _pressure =    _gas->GetPressure();
    _temperature = _gas->GetTemperature();
    //INNER ELEMENTS
    _field_cage->SetLogicalVolume(_mother_logic);
    _field_cage->Construct();
    SetELzCoord(_field_cage->GetELzCoord());
    _tracking_plane->SetLogicalVolume(_mother_logic);
    _tracking_plane->SetELzCoord(_field_cage->GetELzCoord());
    _tracking_plane->Construct();
    _energy_plane->SetLogicalVolume(_mother_logic);
    _energy_plane->SetELzCoord(_field_cage->GetELzCoord());
    _energy_plane->Construct();
  }

  NextNewInnerElements::~NextNewInnerElements()
  {
    delete _energy_plane;
    delete _field_cage;    
    delete _tracking_plane;
  }

  
  G4ThreeVector NextNewInnerElements::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);
 
    // INNER ELEMENTS regions
   if ( (region == "CARRIER_PLATE") || (region == "ENCLOSURE_BODY") ||
	      (region == "ENCLOSURE_WINDOW") || (region=="OPTICAL_PAD") || 
	      (region == "PMT_BODY")||(region=="PMT_BASE") ||
              (region == "INT_ENCLOSURE_SURF") || (region == "PMT_SURF")) {
      vertex = _energy_plane->GenerateVertex(region);
    }
    else if ( (region == "DRIFT_TUBE") ||
	      (region == "ANODE_QUARTZ") || (region== "CENTER") || (region == "CATHODE") || 
	      (region == "ACTIVE") || (region== "EL_TABLE") || (region == "AD_HOC")) {
      vertex=_field_cage->GenerateVertex(region);
    }
    else if ( (region == "SUPPORT_PLATE") || (region == "DICE_BOARD")|| (region == "DB_PLUG") ) {
      vertex = _tracking_plane->GenerateVertex(region);
    }
    else {
      G4Exception("[NextNewInnerElements]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");     
    } 
    return vertex;
  }
}//end namespace nexus
