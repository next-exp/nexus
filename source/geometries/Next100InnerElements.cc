// ----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <jmunoz@ific.uv.es>, <paola.ferrario@dipc.org>
//  Created: 2 Mar 2012
//
//  Copyright (c) 2012-2020 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Next100InnerElements.h"

#include "Next100FieldCage.h"
#include "Next100EnergyPlane.h"
#include "Next100TrackingPlane.h"
#include "IonizationSD.h"
#include "PmtSD.h"
#include "UniformElectricDriftField.h"
#include "OpticalMaterialProperties.h"
#include "MaterialsList.h"
#include "XenonGasProperties.h"
#include "Visibilities.h"

#include <G4GenericMessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Tubs.hh>
#include <G4PVPlacement.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4Region.hh>
#include <G4UserLimits.hh>
#include <G4SDManager.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>
#include <G4TransportationManager.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <stdexcept>

using namespace CLHEP;


namespace nexus {


  Next100InnerElements::Next100InnerElements():
    BaseGeometry(),
    _gate_sapphire_wdw_distance  (1460.5 * mm), // active length + cathode thickness + buffer length
    _gate_tracking_plane_distance(25.6 * mm),
    _mother_logic(nullptr),
    _mother_phys (nullptr),
    _gas(nullptr),
    _field_cage    (new Next100FieldCage()),
    _energy_plane  (new Next100EnergyPlane()),
    _tracking_plane(new Next100TrackingPlane(_gate_tracking_plane_distance)),
    _msg(nullptr)
  {
    // Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/Next100/",
                                  "Control commands of geometry Next100.");
  }


  void Next100InnerElements::SetLogicalVolume(G4LogicalVolume* mother_logic)
  {
    _mother_logic = mother_logic;
  }


  void Next100InnerElements::SetPhysicalVolume(G4VPhysicalVolume* mother_phys)
  {
    _mother_phys = mother_phys;
  }


  void Next100InnerElements::Construct()
  {
    // Position in Z of the beginning of the drift region
    G4double gate_zpos = GetELzCoord();
    // Reading mother material
    _gas = _mother_logic->GetMaterial();
    _pressure =    _gas->GetPressure();
    _temperature = _gas->GetTemperature();

    // Field Cage
    _field_cage->SetMotherLogicalVolume(_mother_logic);
    _field_cage->SetMotherPhysicalVolume(_mother_phys);
    _field_cage->SetELzCoord(gate_zpos);
    _field_cage->Construct();

    // Energy Plane
    _energy_plane->SetMotherLogicalVolume(_mother_logic);
    _energy_plane->SetELzCoord(gate_zpos);
    _energy_plane->SetSapphireSurfaceZPos(_gate_sapphire_wdw_distance);
    _energy_plane->Construct();

    // Tracking plane
    _tracking_plane->SetMotherPhysicalVolume(_mother_phys);
    _tracking_plane->SetELzCoord(gate_zpos);
    _tracking_plane->Construct();
  }


  Next100InnerElements::~Next100InnerElements()
  {
    delete _field_cage;
    delete _energy_plane;
    delete _tracking_plane;
  }


  G4ThreeVector Next100InnerElements::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // Field Cage regions
    if ((region == "CENTER") ||
	(region == "ACTIVE") ||
	(region == "BUFFER") ||
	(region == "XENON") ||
	(region == "LIGHT_TUBE")) {
      vertex = _field_cage->GenerateVertex(region);
    }
    // Energy Plane regions
    else if ((region == "EP_COPPER_PLATE") ||
             (region == "SAPPHIRE_WINDOW") ||
             (region == "OPTICAL_PAD") ||
	     (region == "PMT") ||
             (region == "PMT_BODY") ||
	     (region == "INTERNAL_PMT_BASE") ||
	     (region == "EXTERNAL_PMT_BASE")) {
      vertex = _energy_plane->GenerateVertex(region);
    }
    // Tracking Plane regions
    else if ((region == "TP_COPPER_PLATE") ||
             (region == "SIPM_BOARD")) {
      vertex = _tracking_plane->GenerateVertex(region);
    }
    else {
      G4Exception("[Next100InnerElements]", "GenerateVertex()", FatalException,
        "Unknown vertex generation region!");
    }

    return vertex;
  }

} // end namespace nexus
