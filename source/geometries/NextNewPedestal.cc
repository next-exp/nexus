// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 29 Nov 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewPedestal.h"
#include "MaterialsList.h"
#include <G4GenericMessenger.hh>

#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Box.hh>
#include <G4NistManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>


namespace nexus {

  using namespace CLHEP;

  NextNewPedestal::NextNewPedestal():

    BaseGeometry(),

    // Body dimensions
    _table_x (1200. *mm),
    _table_y (15. *mm),
    _table_z (2500. *mm),
    _y_pos(-600. *mm)
    
  {
   
   /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/NextNew/", "Control commands of geometry NextNew.");
    _msg->DeclareProperty("table_vis", _visibility, "TABLE Visibility");
    
  }
   
  void NextNewPedestal::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }

  void NextNewPedestal::Construct()
  {
    ////// SUPPORT TABLE  ///////////
    G4Box* table_solid = new G4Box("TABLE", _table_x/2., _table_y/2., _table_z/2.);
   
    G4LogicalVolume* table_logic = new G4LogicalVolume(table_solid, 
						       MaterialsList::Steel316Ti(),
						       "TABLE");
    G4PVPlacement* _table_physi = 
      new G4PVPlacement(0, G4ThreeVector(0.,_y_pos,0.), table_logic, "TABLE", _mother_logic, false, 0,false);
   
    // SETTING VISIBILITIES   //////////
    if (_visibility) {
    G4VisAttributes steel_col(G4Colour(.88, .87, .86));
    steel_col.SetForceSolid(true);
    table_logic->SetVisAttributes(steel_col);
    }
    else {
      table_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }


    // VERTEX GENERATORS   //////////
    _table_gen = new BoxPointSampler(_table_x,_table_y,_table_z,0., G4ThreeVector(0.,_y_pos,0.), 0);
   		 
    // Calculating some probs
    G4double table_vol = table_solid->GetCubicVolume();
    // std::cout<<"TABLE VOLUME:\t"<<table_vol<<std::endl;
  }

  NextNewPedestal::~NextNewPedestal()
  {
    delete _table_gen;
  }
  
  G4ThreeVector NextNewPedestal::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0., 0., 0.);
    if (region == "PEDESTAL") {
      vertex = _table_gen->GenerateVertex("INSIDE");
    }
    return vertex;
  }

} //end namespace nexus
