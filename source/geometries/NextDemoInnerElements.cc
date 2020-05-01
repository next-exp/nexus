// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 18 Sept 2013
//
//  Copyright (c) 2013 NEXT Collaboration
//
//  Original NextNewInnerElements.cc
//
//  Updated to NextDemo++  by  Ruth Weiss Babai  <ruty.wb@gmail.com>
//  From: NextNewInnerElements.cc
//  Date:      June-Aug 2019
// ----------------------------------------------------------------------------

#include "NextDemoInnerElements.h"
#include "NextDemoFieldCage.h"
#include "NextDemoTrackingPlane.h"
#include "NextDemoEnergyPlane.h"
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

  NextDemoInnerElements::NextDemoInnerElements(const G4double vessel_length):
    BaseGeometry(),
    _mother_logic(nullptr),
    _mother_phys(nullptr),
    _verbosity(0)

  {

    _msg = new G4GenericMessenger(this, "/Geometry/NextDemo/", "Control commands of geometry NextDemo.");
    _msg->DeclareProperty("inner_elements_verbosity", _verbosity, "Inner Elements verbosity");

   /// Needed External variables
    _vessel_length = vessel_length;

    // Build the internal objects that live there
    _field_cage = new NextDemoFieldCage(_vessel_length);
    _tracking_plane = new NextDemoTrackingPlane();
    _energy_plane = new NextDemoEnergyPlane();
  }

  void NextDemoInnerElements::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  void NextDemoInnerElements::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
  {
    _mother_phys = mother_phys;
  }

  void NextDemoInnerElements::Construct()
  {
    // Reading material
    _gas = _mother_logic->GetMaterial();
    _pressure =    _gas->GetPressure();
    _temperature = _gas->GetTemperature();

    //INNER ELEMENTS
    _field_cage->SetLogicalVolume(_mother_logic);
    _field_cage->SetMotherPhysicalVolume(_mother_phys);
    _field_cage->Construct();
    // SetELzCoord(_field_cage->GetELzCoord());   //  Ruty 25-03-2020 transfer to TrackPl ELzEdge
    _anode_z_pos = _field_cage->GetAnodezCoord();   //  Ruty 25-03-2020 transfer to TrackPl ELzEdg
   if (_verbosity) {
      G4cout << "******************************" << G4endl;
      G4cout << "INNER_ELEMENTS: _anode_z_pos: " << _anode_z_pos  <<  G4endl;
      G4cout << "******************************" << G4endl;
   }
    _tracking_plane->SetLogicalVolume(_mother_logic);
    _tracking_plane->SetAnodeZCoord(_anode_z_pos);
    //_tracking_plane->SetELzCoord(_field_cage->GetELzCoord());
    _tracking_plane->Construct();
    _energy_plane->SetLogicalVolume(_mother_logic);
    // _energy_plane->SetELzCoord(_field_cage->GetELzCoord());
    _energy_plane->Construct();
  }

  NextDemoInnerElements::~NextDemoInnerElements()
  {
    delete _field_cage;
    delete _tracking_plane;
    delete _energy_plane;
  }


  G4ThreeVector NextDemoInnerElements::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

   if       ( (region == "ACTIVE") ||
            // (region == "DRIFT_TUBE") ||
	    // (region == "ANODE_QUARTZ") ||
	    // (region == "CENTER") ||
	    // (region == "CATHODE") ||
	    // (region == "XENON") ||
	    // (region == "BUFFER") ||
	     (region== "EL_TABLE") ||
	     (region == "AD_HOC")) {
      vertex = _field_cage->GenerateVertex(region);
    }
    else {
      G4Exception("[NextDemoInnerElements]", "GenerateVertex()", FatalException,
		  "Unknown vertex generation region!");
    }
    return vertex;
  }
}//end namespace nexus
