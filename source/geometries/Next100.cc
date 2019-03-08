// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <justo.martin-albo@ific.uv.es>, <jmunoz@ific.uv.es>
//  Created: 21 Nov 2011
//  
//  Copyright (c) 2011 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next100.h"
#include "BoxPointSampler.h"
#include "MuonsPointSampler.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4UserLimits.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

namespace nexus {

  using namespace CLHEP;

  Next100::Next100():
    BaseGeometry(),
    // Lab dimensions
    _lab_size (5. * m),
    // Nozzles external diam and y positions
    _nozzle_ext_diam (9. * cm),
    _up_nozzle_ypos (20. * cm),
    _central_nozzle_ypos (0. * cm),
    _down_nozzle_ypos (-20. * cm),
    _bottom_nozzle_ypos(-53. * cm),
    _rot_angle(pi)
  {

  // The following methods must be invoked in this particular
  // order since some of them depend on the previous ones


  // Shielding
  _shielding = new Next100Shielding();

  // Vessel
  _vessel = new Next100Vessel(_nozzle_ext_diam, _up_nozzle_ypos, _central_nozzle_ypos,
			      _down_nozzle_ypos, _bottom_nozzle_ypos);

  // Internal copper shielding
  _ics = new Next100Ics(_nozzle_ext_diam, _up_nozzle_ypos, _central_nozzle_ypos,
			_down_nozzle_ypos, _bottom_nozzle_ypos);

  // Inner Elements
  _inner_elements = new Next100InnerElements();

  }
  
  
  
  Next100::~Next100()
  {
    delete _inner_elements;
    delete _ics;
    delete _vessel;
    delete _shielding;
    delete _lab_gen;
    delete _muon_gen;
  }
  


  void Next100::Construct()
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

 
    // SHIELDING
    _shielding->Construct();
    G4LogicalVolume* shielding_logic = _shielding->GetLogicalVolume();
   
    // VESSEL
    _vessel->Construct();
    G4LogicalVolume* shielding_air_logic = _shielding->GetAirLogicalVolume();
    G4LogicalVolume* vessel_logic = _vessel->GetLogicalVolume();
    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), vessel_logic,
 		      "VESSEL", shielding_air_logic, false, 0);
    G4LogicalVolume* vessel_internal_logic = _vessel->GetInternalLogicalVolume();

    // Inner Elements
    _inner_elements->SetLogicalVolume(vessel_internal_logic);
    _inner_elements->Construct();

    _vessel->SetELzCoord(_inner_elements->GetELzCoord());
    
    // Internal Copper Shielding
    _ics->SetLogicalVolume(vessel_internal_logic);
    _ics->SetELzCoord(_inner_elements->GetELzCoord());
    _ics->Construct();  

    // Placement of the shielding volume, rotated and translated to have a right-handed ref system with z = z drift.
    _displ = G4ThreeVector(0., 0., _inner_elements->GetELzCoord());

    G4RotationMatrix rot;
    rot.rotateY(_rot_angle);
    
    new G4PVPlacement(G4Transform3D(rot, _displ), shielding_logic,
		      "LEAD_BOX", _lab_logic, false, 0);
   

    //// VERTEX GENERATORS   //
    _lab_gen = 
      new BoxPointSampler(_lab_size - 1.*m, _lab_size - 1.*m, _lab_size  - 1.*m, 1.*m,G4ThreeVector(0.,0.,0.),0);

    G4ThreeVector shielding_dim = _shielding->GetDimensions();
    _muon_gen = new MuonsPointSampler(shielding_dim.x()/2. + 50.*cm, shielding_dim.y()/2. + 1.*cm, shielding_dim.z()/2. + 50.*cm);

  }
  


  G4ThreeVector Next100::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

     //AIR AROUND SHIELDING
    if (region == "LAB") {
      vertex = _lab_gen->GenerateVertex("INSIDE");
    } else if (region == "MUONS") {
      vertex = _muon_gen->GenerateVertex();
    // Shielding regions
    } else if ((region == "SHIELDING_LEAD")  || 
    	(region == "SHIELDING_STEEL") ||
		(region == "EXTERNAL")        || 
		(region == "SHIELDING_GAS")   || 
		(region == "SHIELDING_STRUCT") ) {
      vertex = _shielding->GenerateVertex(region);
    }

    // Vessel regions
    else if ((region == "VESSEL") || 
	     (region == "VESSEL_FLANGES") ||
	     (region == "VESSEL_TRACKING_ENDCAP") || 
	     (region == "VESSEL_ENERGY_ENDCAP")) {
      vertex = _vessel->GenerateVertex(region);
    }

    // Inner copper shielding
    else if ((region == "ICS") ||
	     (region == "DB_PLUG")) {
      vertex = _ics->GenerateVertex(region);
    }

    // Inner elements (photosensors' planes and field cage)
    else if ((region == "FIELD_CAGE") ||
             (region == "CATHODE") ||
	     (region == "XENON") ||
	     (region == "ACTIVE") ||
	     (region == "BUFFER") ||
	     (region == "ANODE_QUARTZ") ||
	     (region == "CARRIER_PLATE") || 
	     (region == "ENCLOSURE") ||
	     (region == "ENCLOSURE_WINDOW") ||
	     (region == "ENCLOSURE_PAD") ||
	     (region == "PMT_BODY") ||
	     (region == "PMT") ||
	     (region == "PMT_BASE") ||
	     (region == "TRK_SUPPORT") ||
	     (region == "DICE_BOARD") ||
	     (region == "AD_HOC") ||
	     (region == "AXIAL_PORT") ||
	     (region == "EL_TABLE") ) {
      vertex = _inner_elements->GenerateVertex(region);
    }
    else {
      G4Exception("[Next100]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");     
    }

    // AD_HOC is the only vertex that is not rotated and shifted because it is passed by the user
    if  (region == "AD_HOC")
      return vertex;
    
    // First rotate, then shift
    vertex.rotate(_rot_angle, G4ThreeVector(0., 1., 0.));
    vertex = vertex + _displ;
    
    return vertex;
  }


} //end namespace nexus
