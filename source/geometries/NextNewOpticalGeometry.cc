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
#include <G4NistManager.hh>
#include <G4UnitsTable.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

using namespace nexus;
using namespace CLHEP;

NextNewOpticalGeometry::NextNewOpticalGeometry():
  BaseGeometry(),
  _pressure(1. * bar),
  _temperature (300 * kelvin),
  _sc_yield(25510. * 1/MeV),
  _e_lifetime(1000. * ms),
  _gas("naturalXe"),
  _rot_angle(pi)
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

  new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);

  G4GenericMessenger::Command& sc_yield_cmd =
    _msg->DeclareProperty("sc_yield", _sc_yield,
			  "Set scintillation yield for GXe. It is in photons/MeV");
  sc_yield_cmd.SetParameterName("sc_yield", true);
  sc_yield_cmd.SetUnitCategory("1/Energy");

  G4GenericMessenger::Command& e_lifetime_cmd =
    _msg->DeclareProperty("e_lifetime", _e_lifetime,
        "Electron lifetime in gas.");
  e_lifetime_cmd.SetParameterName("e_lifetime", false);
  e_lifetime_cmd.SetUnitCategory("Time");
  e_lifetime_cmd.SetRange("e_lifetime>0.");

  _msg->DeclareProperty("gas", _gas, "Gas being used");

}

NextNewOpticalGeometry::~NextNewOpticalGeometry()
{
  delete _inner_elements;
}

void NextNewOpticalGeometry::Construct()
{

  // LAB /////////////////////////////////////////////////////////////
  // This is just a volume of air without optical properties surrounding
  // the gas so that optical photons die there.

  // AIR
  G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

  G4double lab_size = 4.*m;
  G4Box* lab_solid =
    new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
  G4LogicalVolume* lab_logic =
    new G4LogicalVolume(lab_solid, air, "LAB");

  this->SetDrift(true);

  lab_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Set this volume as the wrapper for the whole geometry
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(lab_logic);

  ///MOTHER VOLUME
  // Build a big box of gas which hosts the optical geometry

  G4Material* gas_mat = nullptr;

  if (_gas == "naturalXe") {
    gas_mat = MaterialsList::GXe(_pressure, _temperature);
    gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, _temperature, _sc_yield, _e_lifetime));
  } else if (_gas == "enrichedXe") {
    gas_mat =  MaterialsList::GXeEnriched(_pressure, _temperature);
    gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, _temperature, _sc_yield, _e_lifetime));
  } else if  (_gas == "depletedXe") {
    gas_mat =  MaterialsList::GXeDepleted(_pressure, _temperature);
    gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure, _temperature, _sc_yield, _e_lifetime));
  } else if (_gas == "Ar") {
    gas_mat =  MaterialsList::GAr(_pressure, _temperature);
    gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GAr(_sc_yield, _e_lifetime));
  } else {
    G4Exception("[NextNewOpticalGeometry]", "Construct()", FatalException,
		"Unknown kind of gas, valid options are: naturalXe, enrichedXe, depletedXe, Ar.");
  }

  G4double gas_size = 3.*m;
  G4Box* gas_solid = new G4Box("GAS", gas_size/2., gas_size/2., gas_size/2.);
  G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gas_mat, "GAS");

  _displ = G4ThreeVector(0., 0., _inner_elements->GetELzCoord());
  G4RotationMatrix rot;
  rot.rotateY(_rot_angle);

  G4VPhysicalVolume* gas_phys =
    new G4PVPlacement(G4Transform3D(rot, _displ), gas_logic,
                      "GAS", lab_logic, false, 0, false);

  ///INNER ELEMENTS
  _inner_elements->SetMotherLogicalVolume(gas_logic);
  _inner_elements->SetMotherPhysicalVolume(gas_phys);
  _inner_elements->Construct();


  // Set this volume as the wrapper for the whole geometry
  // (i.e., this is the volume that will be placed in the world)
  //  this->SetLogicalVolume(gas_logic);

  // Visibilities
  gas_logic->SetVisAttributes(G4VisAttributes::Invisible);

}


G4ThreeVector NextNewOpticalGeometry::GenerateVertex(const G4String& region) const
{
  G4ThreeVector vertex = _inner_elements->GenerateVertex(region);

  // AD_HOC is the only vertex that is not rotated and shifted because it is passed by the user
  if  (region == "AD_HOC")
    return vertex;

  vertex.rotate(_rot_angle, G4ThreeVector(0., 1., 0.));
  vertex = vertex + _displ;
  return vertex;
}
