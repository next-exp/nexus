// ----------------------------------------------------------------------------
// nexus | MagBox.cc
//
// Detector with magnetic field.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "MagBox.h"
#include "MaterialsList.h"
#include "IonizationSD.h"
#include "BoxPointSamplerLegacy.h"
#include "FactoryBase.h"

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

  REGISTER_CLASS(MagBox, GeometryBase)

  MagBox::MagBox():
    GeometryBase(),

    // Detector dimensions
    detector_size_ (1.*m)

  {
    // Messenger
    msg_ =
      new G4GenericMessenger(this, "/Geometry/MagBox/",
                             "Control commands of geometry MagBox.");

    // Active gas name
    // G4GenericMessenger::Command& gas_cmd =
    msg_->DeclareProperty("gas_name", gas_name_, "Gas Name");
    //msg_->DeclareProperty("gas_name", gas_name_, "Active Gas Name");

    // Pressure
    G4GenericMessenger::Command& pressure_cmd =
      msg_->DeclareProperty("pressure", pressure_, "Gas Pressure");
    pressure_cmd.SetUnitCategory("Pressure");

    // Magnetic Field Intensity
    G4GenericMessenger::Command& mag_cmd =
      msg_->DeclareProperty("mag_intensity", mag_intensity_,
                            "Magnetic Field Intensity");
    mag_cmd.SetUnitCategory("Magnetic flux density");

    // Maximum Step Size
    G4GenericMessenger::Command& step_cmd =
      msg_->DeclareProperty("max_step_size", max_step_size_,
                            "Maximum Step Size");
    step_cmd.SetUnitCategory("Length");
    step_cmd.SetParameterName("max_step_size", false);
    step_cmd.SetRange("max_step_size>0.");
  }


  MagBox::~MagBox()
  {
    delete active_gen_;
  }


  void MagBox::Construct()
  {
    // LAB. This is just a volume of air surrounding the detector
    G4double lab_size = detector_size_ + 1.*cm;
    G4Box* lab_solid = new G4Box("LAB", lab_size/2., lab_size/2., lab_size/2.);

    G4LogicalVolume* lab_logic =
      new G4LogicalVolume(lab_solid,
                          G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"),
                          "LAB");
    lab_logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    this->SetLogicalVolume(lab_logic);

    // ACTIVE
    G4Box* active_solid =
      new G4Box("ACTIVE", detector_size_/2., detector_size_/2.,
                detector_size_/2.);

    // Define the gas
    G4NistManager* man = G4NistManager::Instance();

    if (gas_name_ == "Xenon") {
      std::cout << "*** Xenon Pressure (atm): " << pressure_/bar << std::endl;
      gas_ = materials::GXe(pressure_, 303.*kelvin);
    }
    else if (gas_name_ == "Hydrogen") {
      std::cout << "*** Hydrogen Pressure (atm): " << pressure_/bar << std::endl;
      gas_ = man->FindOrBuildMaterial("G4_H");
    }
    else if (gas_name_ == "SeF6") {
      std::cout << "*** Selenium HexaFluoride (atm): " << pressure_/bar << std::endl;
      gas_ = materials::SeF6(pressure_, 303.*kelvin);
    }
    else {
      std::cout << "ERROR: GAS NOT KNOWN" << std::endl;
      exit(0);
    }


    G4LogicalVolume* active_logic =
      new G4LogicalVolume(active_solid, gas_, "ACTIVE");
    active_logic->SetVisAttributes(G4VisAttributes::GetInvisible());

    new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), active_logic,
		      "ACTIVE", lab_logic, false, 0, false);

    // Set the ACTIVE volume as an ionization sensitive detector
    IonizationSD* ionisd = new IonizationSD("/MAGBOX/ACTIVE");
    active_logic->SetSensitiveDetector(ionisd);
    G4SDManager::GetSDMpointer()->AddNewDetector(ionisd);

    // Limit the step size in ACTIVE volume for better tracking precision
    std::cout << "*** Maximum Step Size (mm): " << max_step_size_/mm << std::endl;
    active_logic->SetUserLimits(new G4UserLimits(max_step_size_));

    // Magnetic Field
    std::cout << "*** Magnetic field intensity (tesla): "
              << mag_intensity_/tesla << std::endl;
    G4UniformMagField* mag_field =
      new G4UniformMagField(G4ThreeVector(0., 0., mag_intensity_));
    G4FieldManager* field_mgr =
      G4TransportationManager::GetTransportationManager()->GetFieldManager();
    field_mgr->SetDetectorField(mag_field);
    field_mgr->CreateChordFinder(mag_field);
    active_logic->SetFieldManager(field_mgr, true);


    // Vertex Generator
    active_gen_ =
      new BoxPointSamplerLegacy(detector_size_, detector_size_, detector_size_, 0.,
                                G4ThreeVector(0.,0.,0.) ,0);

  }


  G4ThreeVector MagBox::GenerateVertex(const G4String& region) const
  {
    G4ThreeVector vertex(0.,0.,0.);

    // ACTIVE
    if (region == "ACTIVE") {
      vertex = active_gen_->GenerateVertex(region);
    }

    return vertex;
  }


} //end namespace nexus
