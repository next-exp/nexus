// ----------------------------------------------------------------------------
//
//  Authors: <paola.ferrario@dipc.org>
//  Created: 30 Dec 2019
//
//  Copyright (c) 2019 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100OpticalGeometry.h"
#include "Next100InnerElements.h"

#include "OpticalMaterialProperties.h"
#include "MaterialsList.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Box.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <G4UnitsTable.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

using namespace CLHEP;

namespace nexus {

  Next100OpticalGeometry::Next100OpticalGeometry(): BaseGeometry(),
						    _pressure(15. * bar),
						    _temperature (300 * kelvin),
						    _sc_yield(25510. * 1/MeV),
						    _gas("naturalXe")
  {
    /// Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/",
				  "Control commands of geometry Next100.");

    G4GenericMessenger::Command& pressure_cmd =
      _msg->DeclareProperty("pressure", _pressure, "Pressure of gasn.");
    pressure_cmd.SetUnitCategory("Pressure");
    pressure_cmd.SetParameterName("pressure", false);
    pressure_cmd.SetRange("pressure>0.");

    new G4UnitDefinition("1/MeV","1/MeV", "1/Energy", 1/MeV);

    G4GenericMessenger::Command& sc_yield_cmd =
      _msg->DeclareProperty("sc_yield", _sc_yield,
			    "Scintillation yield of gas. It is in photons/MeV");
    sc_yield_cmd.SetParameterName("sc_yield", true);
    sc_yield_cmd.SetUnitCategory("1/Energy");

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

    _msg->DeclareProperty("gas", _gas, "Gas being used");


    _inner_elements = new Next100InnerElements();
  }


  void Next100OpticalGeometry::Construct()
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

  lab_logic->SetVisAttributes(G4VisAttributes::Invisible);

  // Set this volume as the wrapper for the whole geometry
  // (i.e., this is the volume that will be placed in the world)
  this->SetLogicalVolume(lab_logic);

    ///MOTHER VOLUME
  // Build a big box of gas which hosts the optical geometry

  G4Material* gas_mat = nullptr;

  if (_gas == "naturalXe") {
    gas_mat = MaterialsList::GXe(_pressure, _temperature);
    gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure,
								       _temperature,
								       _sc_yield));
  } else if (_gas == "enrichedXe") {
    gas_mat =  MaterialsList::GXeEnriched(_pressure, _temperature);
    gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure,
								       _temperature,
								       _sc_yield));
  } else if  (_gas == "depletedXe") {
    gas_mat =  MaterialsList::GXeDepleted(_pressure, _temperature);
    gas_mat->SetMaterialPropertiesTable(OpticalMaterialProperties::GXe(_pressure,
								       _temperature,
								       _sc_yield));
  }  else {
    G4Exception("[Next100OpticalGeometry]", "Construct()", FatalException,
                "Unknown kind of gas, valid options are: naturalXe, enrichedXe, depletedXe.");
  }

  G4double gas_size = lab_size - 1.*cm;
  G4Box* gas_solid = new G4Box("GAS", gas_size/2., gas_size/2., gas_size/2.);
  G4LogicalVolume* gas_logic = new G4LogicalVolume(gas_solid, gas_mat, "GAS");

  _gate_zpos_in_gas = 0. * mm;
  G4VPhysicalVolume* gas_phys =
    new G4PVPlacement(0, G4ThreeVector(0, 0, -_gate_zpos_in_gas), gas_logic,
                      "GAS", lab_logic, false, 0, false);

  ///INNER ELEMENTS
  _inner_elements->SetLogicalVolume(gas_logic);
  _inner_elements->SetPhysicalVolume(gas_phys);
  _inner_elements->SetELzCoord(_gate_zpos_in_gas);
  _inner_elements->Construct();

  // Visibilities
  gas_logic->SetVisAttributes(G4VisAttributes::Invisible);

  }


  Next100OpticalGeometry::~Next100OpticalGeometry()
  {

  }


  G4ThreeVector Next100OpticalGeometry::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);
    if (region == "AD_HOC") {
      // AD_HOC does not need to be shifted because it is passed by the user
      vertex =
	G4ThreeVector(_specific_vertex_X, _specific_vertex_Y, _specific_vertex_Z);
      return vertex;
    }
    else {
      vertex = _inner_elements->GenerateVertex(region);
    }

    G4ThreeVector displacement = G4ThreeVector(0., 0., -_gate_zpos_in_gas);
    vertex = vertex + displacement;
    return vertex;
  }


} // end namespace nexus
