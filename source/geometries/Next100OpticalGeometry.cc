// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <paola.ferrario@ific.uv.es>, <jmunoz@ific.uv.es>
//  Created: 1 Mar 2012
//  
//  Copyright (c) 2012 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Next100OpticalGeometry.h"
#include "MaterialsList.h"
#include "ConfigService.h"
#include "OpticalMaterialProperties.h"

#include <G4Material.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>

using namespace nexus;



Next100OpticalGeometry::Next100OpticalGeometry():
  BaseGeometry()
{

  // Read configuration parameters
  ReadParameters();

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

  // Build the internal gas volume with all the objects that live there 
  // (EL gap, cathode...)
  _inner_elements = new Next100InnerElements(gas_logic);
 
}



Next100OpticalGeometry::~Next100OpticalGeometry()
{
}



void Next100OpticalGeometry::ReadParameters()
{
  const ParamStore& cfg_geom = ConfigService::Instance().Geometry();
  // Xenon gas pressure
  _pressure = cfg_geom.GetDParam("pressure");
}



G4ThreeVector Next100OpticalGeometry::GenerateVertex(const G4String& region) const
{
    G4ThreeVector vertex(0.,0.,0.);

  if (region == "CENTER") {
    return vertex;
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
