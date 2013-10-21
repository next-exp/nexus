// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <miquel.nebot@ific.uv.es>
//  Created: 18 Sept 2013
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "NextNewOpticalGeometry.h"

#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"

#include "NextNewInnerElements.h"

#include <G4Material.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4GenericMessenger.hh>

using namespace nexus;

NextNewOpticalGeometry::NextNewOpticalGeometry():
  BaseGeometry()
{
 // Build the internal gas volume with all the objects that live there 

  _inner_elements = new NextNewInnerElements();//gas_logic

  _msg = new G4GenericMessenger(this, "/Geometry/NextNewOpticalGeometry/",
				"Control commands of geometry NextNew");
  //Gas pressure
  G4GenericMessenger::Command& pressure_cmd =
    _msg->DeclareProperty("pressure", _pressure,
			  "Set pressure for gaseous xenon.");
  pressure_cmd.SetUnitCategory("Pressure");
  pressure_cmd.SetParameterName("pressure", false);
  pressure_cmd.SetRange("pressure>0.");

}

NextNewOpticalGeometry::~NextNewOpticalGeometry()
{
  delete _inner_elements;
}

void NextNewOpticalGeometry::Construct()
{
  ///MOTHER VOLUME
  // Build a big box of gaseous xenon which hosts the optical geometry
  G4Material* gxe = MaterialsList::GXe(_pressure, 303);
  gxe->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, 303));

  G4double gas_size = 3.*m;
  G4Box* gas_solid = new G4Box("GAS", gas_size/2., gas_size/2., gas_size/2.);

  G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gxe, "GAS");

  // Set this volume as the wrapper for the whole geometry 
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(gas_logic);

  // Visibilities
  gas_logic->SetVisAttributes(G4VisAttributes::Invisible);

  ///INNER ELEMENTS
  _inner_elements->SetLogicalVolume(gas_logic);
  _inner_elements->Construct();
  
}
 

G4ThreeVector NextNewOpticalGeometry::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex(0.,0.,0.);   
  vertex = _inner_elements->GenerateVertex(region);   
  return vertex; 
}
