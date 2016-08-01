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
#include "NextNewPedestal.h"
//#include "NextNewCuCastle.h"
#include "NextNewVessel.h"
#include "NextNewIcs.h"
#include "NextNewInnerElements.h"
#include "Na22Source.h"
#include "BoxPointSampler.h"
#include "OpticalMaterialProperties.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>
#include <G4NistManager.hh>
#include <G4RotationMatrix.hh>
#include <G4Transform3D.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  NextNew::NextNew():
    BaseGeometry(),
    // Lab dimensions
    _lab_size (5. * m)   
    // Buffer gas dimensions
  {
    //Shielding
    _shielding = new Next100Shielding();
    //Pedestal
    _pedestal = new NextNewPedestal();
    //Copper Castle
    // _cu_castle = new NextNewCuCastle();
    
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
    delete _pedestal;
    // delete _cu_castle;
   
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

    this->SetDrift(true);
    
    //_lab_logic->SetVisAttributes(G4VisAttributes (G4Colour(.46, .46, .46)));
    _lab_logic->SetVisAttributes(G4VisAttributes::Invisible);

    // Set this volume as the wrapper for the whole geometry 
    // (i.e., this is the volume that will be placed in the world)
    this->SetLogicalVolume(_lab_logic);

    /* 
    // BUFFER GAS   ///////////////////////////////////////////////////////////////////////////////////
    // This is a volume, initially made of air, defined to be the mother volume of Shielding and Vessel

    G4Box* buffer_gas_solid = 
      new G4Box("BUFFER_GAS", _buffer_gas_size/2., _buffer_gas_size/2., _buffer_gas_size/2.);
    //G4Material* _air= G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    //    _air->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(1. *bar, 303.*kelvin));
    
    _buffer_gas_logic = new G4LogicalVolume(buffer_gas_solid,
					    G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),
					    "BUFFER_GAS");
    ////////////////////////////////////////
    ////Limit the uStepMax=Maximum step length, uTrakMax=Maximum total track length,
    //uTimeMax= Maximum global time for a track, uEkinMin= Minimum remaining kinetic energy for a track
    //uRangMin=	 Minimum remaining range for a track
    // _buffer_gas_logic->SetUserLimits(new G4UserLimits( _lab_size*1E6, _lab_size*1E6,1E12 *s,100.*keV,0.));
    _buffer_gas_logic->SetUserLimits(new G4UserLimits( DBL_MAX, DBL_MAX, DBL_MAX,100.*keV,0.));
   
    // _buffer_gas_logic->SetVisAttributes(G4VisAttributes (G4Colour(.86, .86, .86)));
    _buffer_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), _buffer_gas_logic,
		      "BUFFER_GAS", _lab_logic, false, 0, false);
    
    */
    //SHIELDING
    _shielding->Construct();
    G4LogicalVolume* shielding_logic = _shielding->GetLogicalVolume();
    new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),shielding_logic, "LEAD_BOX",
		      _lab_logic, false, 0, false);
    
    //COPPER CASTLE 
    // _cu_castle->SetLogicalVolume(_buffer_gas_logic);
    // _cu_castle->Construct();

    G4LogicalVolume* shielding_air_logic = _shielding->GetAirLogicalVolume();
  
    //PEDESTAL
    _pedestal->SetLogicalVolume(shielding_air_logic);
    _pedestal->Construct();

    
    //VESSEL
    _vessel->Construct();
    G4LogicalVolume* vessel_logic = _vessel->GetLogicalVolume();
    G4ThreeVector position(0.,0.,0.); 
    new G4PVPlacement(0, position, vessel_logic, 
		      "VESSEL", shielding_air_logic, false, 0, false);
    G4LogicalVolume* vessel_gas_logic = _vessel->GetInternalLogicalVolume();

    G4ThreeVector lat_pos = _vessel->GetLatExtSourcePosition(); // this is the position of the end of the port tube
    G4RotationMatrix* lat_rot = new G4RotationMatrix();
    lat_rot->rotateY(-pi/2.);
    
    Na22Source* na22 = new Na22Source();
    na22->Construct();
    G4LogicalVolume* na22_logic = na22->GetLogicalVolume();

    // This is the position of the whole Na22 source + plastic support.
    // G4ThreeVector lat_pos_na22 = G4ThreeVector(lat_pos.getX() + na22->GetSupportThickness()/2. - na22->GetSourceThickness()/2., lat_pos.getY(), lat_pos.getZ());
    G4ThreeVector lat_pos_source = G4ThreeVector(lat_pos.getX() + na22->GetSupportThickness()/2., lat_pos.getY(), lat_pos.getZ());

    new G4PVPlacement(G4Transform3D(*lat_rot, lat_pos_source), na22_logic, "NA22_SOURCE",
		      shielding_air_logic, false, 0, false);

    G4ThreeVector up_pos = _vessel->GetUpExtSourcePosition(); // this is the position of the end of the port tube
    G4RotationMatrix* up_rot = new G4RotationMatrix();
    up_rot->rotateX(pi/2.);

    // G4ThreeVector up_pos_na22 = G4ThreeVector(up_pos.getX(), up_pos.getY() + na22->GetSupportThickness()/2. - na22->GetSourceThickness()/2., up_pos.getZ());
    G4ThreeVector up_pos_source = G4ThreeVector(up_pos.getX(), up_pos.getY() + na22->GetSupportThickness()/2., up_pos.getZ());

    new G4PVPlacement(G4Transform3D(*up_rot, up_pos_source), na22_logic, "NA22_SOURCE",
		      shielding_air_logic, false, 1, false);

    G4VisAttributes light_brown_col = nexus::CopperBrown();
    na22_logic->SetVisAttributes(light_brown_col);
    
    //ICS
    _ics->SetLogicalVolume(vessel_gas_logic);
    _ics->SetNozzlesZPosition( _vessel->GetLATNozzleZPosition(),_vessel->GetUPNozzleZPosition());
    _ics->Construct();

   
    //INNER ELEMENTS
    _inner_elements->SetLogicalVolume(vessel_gas_logic);
    _inner_elements->Construct();
    SetELzCoord(_inner_elements->GetELzCoord());


    //// VERTEX GENERATORS   //
    _lab_gen = 
      new BoxPointSampler(_lab_size - 1.*m, _lab_size - 1.*m, _lab_size  - 1.*m, 1.*m,G4ThreeVector(0.,0.,0.),0);

    G4double source_diam = na22->GetSourceDiameter();
    G4double source_thick = na22->GetSourceThickness();
    G4ThreeVector lat_pos_na22 = G4ThreeVector(lat_pos.getX() + na22->GetSourceThickness()/2., lat_pos.getY(), lat_pos.getZ());
    G4ThreeVector up_pos_na22 = G4ThreeVector(up_pos.getX(), up_pos.getY() + na22->GetSourceThickness()/2., up_pos.getZ());
    _source_gen_lat = new CylinderPointSampler(0., source_thick, source_diam/2., 0., lat_pos_na22, lat_rot);
    _source_gen_up = new CylinderPointSampler(0., source_thick, source_diam/2., 0., up_pos_na22, up_rot);
    

  }

 
    
  G4ThreeVector NextNew::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);
    //AIR AROUND SHIELDING
    if (region == "LAB") {
      vertex = _lab_gen->GenerateVertex("INSIDE");
    }
    else if (region == "NA22_PORT_ANODE_EXT") {
      vertex =  _source_gen_lat->GenerateVertex("BODY_VOL");
    }
    else if (region == "NA22_PORT_UP_EXT") {
      vertex =  _source_gen_up->GenerateVertex("BODY_VOL");
    }
    else if ( (region == "SHIELDING_LEAD") || (region == "SHIELDING_STEEL") || 
	      (region == "SHIELDING_GAS") || (region=="SHIELDING_STRUCT") ||
	      (region == "EXTERNAL") || (region == "SOURCE_PORT_AXIAL_EXT") ) {
      vertex = _shielding->GenerateVertex(region);   
    }
    //PEDESTAL
    else if (region == "PEDESTAL") {
      vertex = _pedestal->GenerateVertex(region);
    }
    //  //COPPER CASTLE
   // else if (region == "CU_CASTLE"){
   //   vertex = _cu_castle->GenerateVertex(region);
   //}
   //  //RADON 
   //  //on the inner lead surface (SHIELDING_GAS) and on the outer copper castle surface (RN_CU_CASTLE)
   // else if (region == "RN_CU_CASTLE") {
   //   vertex = _cu_castle->GenerateVertex(region);
   //  }

    //VESSEL REGIONS
    else if ( (region == "VESSEL") || 
	      (region == "SOURCE_PORT_ANODE") ||
	      (region == "SOURCE_PORT_UP") ||
	      (region == "SOURCE_PORT_AXIAL")) {
      vertex = _vessel->GenerateVertex(region);
    }
    // ICS REGIONS
    else if (region == "ICS"){  
      vertex = _ics->GenerateVertex(region);
    }
    //INNER ELEMENTS
    else if ( (region == "CENTER") ||
	      (region == "CARRIER_PLATE") || (region == "ENCLOSURE_BODY") || (region == "ENCLOSURE_WINDOW") || 
	      (region=="OPTICAL_PAD") || (region == "PMT_BODY") || (region=="PMT_BASE") ||
	      (region == "DRIFT_TUBE") || (region== "REFLECTOR_DRIFT") ||
	      (region == "BUFFER_TUBE") || (region== "REFLECTOR_BUFFER") ||
	      (region == "ANODE_QUARTZ")||
	      (region == "ACTIVE") || (region== "EL_TABLE") || (region == "AD_HOC") ||
	      (region == "SUPPORT_PLATE") || (region == "DICE_BOARD") || (region == "DB_PLUG") ){
      vertex = _inner_elements->GenerateVertex(region);
    }
    else {
      G4Exception("[NextNew]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");     
    }
    return vertex;
  }
  
  
} //end namespace nexus
