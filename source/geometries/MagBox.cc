// ----------------------------------------------------------------------------
//  $Id: MagBox.cc  $
//
//  Author:  <jmunoz@ific.uv.es>   
//  Created: January 2014
//  
//  Copyright (c) 2013 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "MagBox.h"
#include "MaterialsList.h"
#include "IonizationSD.h"

#include <G4GenericMessenger.hh>
#include <G4Box.hh>

#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>
#include <G4NistManager.hh>
#include <G4UniformMagField.hh>
#include <G4FieldManager.hh>
#include <G4TransportationManager.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>


namespace nexus {

  MagBox::MagBox():
    BaseGeometry(),

    // Detector dimensions
    _detector_size (1.*m)

  {
    // Messenger
    _msg = new G4GenericMessenger(this, "/Geometry/MagBox/", "Control commands of geometry MagBox.");

    // Active gas name
    G4GenericMessenger::Command& gas_cmd = _msg->DeclareProperty("gas_name", _gas_name, "Gas Name");
    //_msg->DeclareProperty("gas_name", _gas_name, "Active Gas Name");
    
    // Pressure
    G4GenericMessenger::Command& pressure_cmd = _msg->DeclareProperty("pressure", _pressure, "Gas Pressure");
    pressure_cmd.SetUnitCategory("Pressure");

    // Magnetic Field Intensity
    G4GenericMessenger::Command& mag_cmd = _msg->DeclareProperty("mag_intensity", _mag_intensity,
								 "Magnetic Field Intensity"); 
    mag_cmd.SetUnitCategory("Magnetic flux density");

    // Maximum Step Size
    G4GenericMessenger::Command& step_cmd = _msg->DeclareProperty("max_step_size", _max_step_size, 
								  "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");
  }


  MagBox::~MagBox()
  {    
    delete _active_gen;
  }


  void MagBox::Construct()
  {
    // LAB. This is just a volume of air surrounding the detector
    G4double lab_size = _detector_size + 1.*cm;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);
    
    G4LogicalVolume* lab_logic = new G4LogicalVolume(lab_solid,
						     G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"), "LAB");
    lab_logic->SetVisAttributes(G4VisAttributes::Invisible);
    this->SetLogicalVolume(lab_logic);

    // ACTIVE
    G4Box* active_solid = new G4Box("ACTIVE", _detector_size/2., _detector_size/2., _detector_size/2.);

    // Define the gas
    G4NistManager* man = G4NistManager::Instance();

    if (_gas_name == "Xenon") {
      std::cout << "*** Xenon Pressure (atm): " << _pressure/bar << std::endl;
      _gas = MaterialsList::GXe(_pressure, 303.*kelvin);
    }
    else if (_gas_name == "Hydrogen") {
      std::cout << "*** Hydrogen Pressure (atm): " << _pressure/bar << std::endl;
      _gas = man->FindOrBuildMaterial("G4_H");
    }	
    else if (_gas_name == "SeF6") {
      std::cout << "*** Selenium HexaFluoride (atm): " << _pressure/bar << std::endl;
      _gas = MaterialsList::SeF6(_pressure, 303.*kelvin);
    }
    else {
      std::cout << "ERROR: GAS NOT KNOWN" << std::endl;
      exit(0);
    }


    G4LogicalVolume* active_logic = new G4LogicalVolume(active_solid, _gas, "ACTIVE");
    active_logic->SetVisAttributes(G4VisAttributes::Invisible);

    G4PVPlacement* active_physi = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic,
						    "ACTIVE", lab_logic, false, 0, false);

    // Set the ACTIVE volume as an ionization sensitive detector
    IonizationSD* ionisd = new IonizationSD("/MAGBOX/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    std::cout << "*** Maximum Step Size (mm): " << _max_step_size/mm << std::endl;
    active_logic->SetUserLimits(new G4UserLimits(_max_step_size));

    // Magnetic Field
    std::cout << "*** Magnetic field intensity (tesla): " << _mag_intensity/tesla << std::endl;
    G4UniformMagField* mag_field = new G4UniformMagField(G4ThreeVector(0., 0., _mag_intensity));
    G4FieldManager* field_mgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    field_mgr->SetDetectorField(mag_field);
    field_mgr->CreateChordFinder(mag_field);
    active_logic->SetFieldManager(field_mgr, true);


    // Vertex Generator
    _active_gen = new BoxPointSampler(_detector_size, _detector_size, _detector_size, 0.,
				      G4ThreeVector(0.,0.,0.) ,0);

  }

    
  G4ThreeVector MagBox::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // ACTIVE
    if (region == "ACTIVE") {
      vertex = _active_gen->GenerateVertex(region);
    }
   
    return vertex;
  }
  
  
} //end namespace nexus
