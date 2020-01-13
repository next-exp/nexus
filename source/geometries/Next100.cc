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
    _bottom_nozzle_ypos(-53. * cm)
  {

    _msg = new G4GenericMessenger(this, "/Geometry/Next100/",
				  "Control commands of geometry Next100.");

    G4GenericMessenger::Command&  specific_vertex_X_cmd =
      _msg->DeclareProperty("specific_vertex_X", _specific_vertex_X,
                            "If region is AD_HOC, x coord of primary particles");
    specific_vertex_X_cmd.SetParameterName("specific_vertex_X", true);
    specific_vertex_X_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Y_cmd =
      _msg->DeclareProperty("specific_vertex_Y", _specific_vertex_Y,
                            "If region is AD_HOC, y coord of primary particles");
    specific_vertex_Y_cmd.SetParameterName("specific_vertex_Y", true);
    specific_vertex_Y_cmd.SetUnitCategory("Length");
    G4GenericMessenger::Command&  specific_vertex_Z_cmd =
      _msg->DeclareProperty("specific_vertex_Z", _specific_vertex_Z,
                            "If region is AD_HOC, z coord of primary particles");
    specific_vertex_Z_cmd.SetParameterName("specific_vertex_Z", true);
    specific_vertex_Z_cmd.SetUnitCategory("Length");


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

    _lab_logic =
      new G4LogicalVolume(lab_solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),
			  "LAB");
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
    _gate_zpos_in_vessel = _vessel->GetELzCoord();
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), vessel_logic,
    		      "VESSEL", shielding_air_logic, false, 0);

    G4LogicalVolume* vessel_internal_logic = _vessel->GetInternalLogicalVolume();

    // Inner Elements
    _inner_elements->SetLogicalVolume(vessel_internal_logic);
    _inner_elements->SetELzCoord(_gate_zpos_in_vessel);
    _inner_elements->Construct();

    // Internal Copper Shielding
    _ics->SetLogicalVolume(vessel_internal_logic);
    _ics->Construct();

    new G4PVPlacement(0, G4ThreeVector(0., 0., -_gate_zpos_in_vessel), shielding_logic,
     		      "LEAD_BOX", _lab_logic, false, 0);


    //// VERTEX GENERATORS   //
    _lab_gen =
      new BoxPointSampler(_lab_size - 1.*m, _lab_size - 1.*m, _lab_size  - 1.*m,
			  1.*m,G4ThreeVector(0., 0., 0.), 0);

    G4ThreeVector shielding_dim = _shielding->GetDimensions();
    _muon_gen = new MuonsPointSampler(shielding_dim.x()/2. + 50.*cm,
				      shielding_dim.y()/2. + 1.*cm,
				      shielding_dim.z()/2. + 50.*cm);

  }



  G4ThreeVector Next100::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // Air around shielding
    if (region == "LAB") {
      vertex = _lab_gen->GenerateVertex("INSIDE");
    } else if (region == "MUONS") {
      vertex = _muon_gen->GenerateVertex();
    // Shielding regions
    } else if ((region == "SHIELDING_LEAD")  ||
	       (region == "SHIELDING_STEEL") ||
	       (region == "EXTERNAL") ||
	       (region == "INNER_AIR") ||
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
    else if ((region == "CENTER") ||
	     (region == "ACTIVE") ||
	     (region == "BUFFER") ||
	     (region == "CATHODE_GRID") ||
	     (region == "XENON") ||
	     (region == "LIGHT_TUBE_DRIFT") ||
	     (region == "LIGHT_TUBE_BUFFER") ||
	     (region == "ENERGY_COPPER_PLATE") ||
	     (region == "SAPPHIRE_WINDOW") ||
	     (region == "OPTICAL_PAD") ||
	     (region == "PMT_BODY") ||
	     (region == "PMT") ||
	     (region == "INTERNAL_PMT_BASE") ||
	     (region == "EXTERNAL_PMT_BASE") ||
	     (region == "TRK_SUPPORT") ||
	     (region == "DICE_BOARD") ||
	     (region == "AXIAL_PORT") ||
	     (region == "EL_TABLE") ) {
      vertex = _inner_elements->GenerateVertex(region);
    }
    else if (region == "AD_HOC") {
      // AD_HOC does not need to be shifted because it is passed by the user
      vertex =
	G4ThreeVector(_specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z);
      return vertex;
    }
    else {
      G4Exception("[Next100]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }

    G4ThreeVector displacement = G4ThreeVector(0., 0., -_gate_zpos_in_vessel);
    vertex = vertex + displacement;

    return vertex;
  }


} //end namespace nexus
