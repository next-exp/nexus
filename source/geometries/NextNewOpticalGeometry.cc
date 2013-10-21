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
#include "NextNewVessel.h"
#include "NextNewInnerElements.h"
#include "NextNewIcs.h"
// #include "Enclosure.h"
//#include "NextNewEnergyPlane.h"
//#include "NextNewTrackingPlane.h"
//#include "NextNewKDB.h"
//#include "SiPM11.h"
//#include "NextNewFieldCage.h"

#include <G4Material.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>

using namespace nexus;

NextNewOpticalGeometry::NextNewOpticalGeometry():
  BaseGeometry()
{
 // Build the internal gas volume with all the objects that live there 

  _vessel = new NextNewVessel();
  _inner_elements = new NextNewInnerElements();//gas_logic
  _ics = new NextNewIcs();
  //_enclosure = new Enclosure();
  //_energy_plane = new NextNewEnergyPlane();
  //_field_cage = new NextNewFieldCage();
  //_kdb = new NextNewKDB(8,8);
  //_sipm = new SiPM11();
  //_tracking_plane = new NextNewTrackingPlane();
}

NextNewOpticalGeometry::~NextNewOpticalGeometry()
{
  delete _vessel;
  delete _inner_elements;
  delete _ics;
  //delete _enclosure;
  //delete _energy_plane;
  //delete _field_cage;
  //delete _kdb;
  //delete _sipm;
  //delete _tracking_plane;
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

  _vessel->Construct();
  G4LogicalVolume* vessel_logic = _vessel->GetLogicalVolume();
  G4ThreeVector position(0.,0.,0.);
  G4PVPlacement* vessel_physi = new G4PVPlacement(0, position, vessel_logic, "VESSEL", gas_logic, false, 0, true);

  ///INNER ELEMENTS
  _inner_elements->SetLogicalVolume(gas_logic);
  _inner_elements->Construct();
 
  _ics->Construct();
  G4LogicalVolume* ics_logic = _ics->GetLogicalVolume();
  G4ThreeVector pos(0.,0.,0.);
  G4PVPlacement* ics_physi = new G4PVPlacement(0, pos, ics_logic, "ICS", gas_logic,false, 0, true);
 
  // _enclosure->Construct();
  // G4LogicalVolume* enc_logic = _enclosure->GetLogicalVolume();
  // G4ThreeVector pos(0.,0.,0.);
  // G4PVPlacement* enc_physi = new G4PVPlacement (0,pos, enc_logic, "ENC",gas_logic,false,0);

  // _energy_plane->SetLogicalVolume(gas_logic);   
  // _energy_plane->Construct();
   
  // _field_cage->SetLogicalVolume(gas_logic);
  // _field_cage->Construct();

  // _kdb->Construct();
  // G4LogicalVolume* kdb_logic = _kdb->GetLogicalVolume();
  // G4ThreeVector pos(0.,0.,0.);
  // G4PVPlacement* kdb_physi = new G4PVPlacement(0,pos,kdb_logic,"KDB",gas_logic,false,0);

  // _sipm->Construct();
  // G4LogicalVolume* sipm_logic= _sipm->GetLogicalVolume();
  // G4ThreeVector pos(0.,0.,0.);
  // G4PVPlacement* sipm_physi = new G4PVPlacement(0,pos,sipm_logic,"SIPM",gas_logic,false,0);

  // _tracking_plane->SetLogicalVolume(gas_logic);
  // _tracking_plane->Construct();
 
}
 

G4ThreeVector NextNewOpticalGeometry::GenerateVertex(const G4String& region) const
{
    G4ThreeVector vertex(0.,0.,0.);
    if (region == "VESSEL"){
      vertex = _vessel->GenerateVertex(region);
    }
    else if (region == "ICS"){  
      // vertex = _ics->GenerateVertex(region);
    }
    else {
      //vertex = _inner_elements->GenerateVertex(region);
    }
    
    return vertex; 
}
