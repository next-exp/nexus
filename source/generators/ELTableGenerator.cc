// ----------------------------------------------------------------------------
// nexus | ELTableGenerator.cc
//
// This generator simulates ionization electrons to produce look-up tables.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ELTableGenerator.h"

#include "DetectorConstruction.h"
#include "GeometryBase.h"
#include "IonizationElectron.h"
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4OpticalPhoton.hh>

#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(ELTableGenerator, G4VPrimaryGenerator)

ELTableGenerator::ELTableGenerator():
  G4VPrimaryGenerator(), msg_(0), num_ie_(1)
{
  msg_ = new G4GenericMessenger(this, "/Generator/ELTableGenerator/",
    "Control commands of the EL lookup table primary generator.");

  //G4GenericMessenger::Command& num_ie_cmd =
    msg_->DeclareProperty("num_ie", num_ie_, "Set number of ionization electrons to be generated.");

  // Retrieve pointer to detector geometry from the run manager
  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();
}


ELTableGenerator::~ELTableGenerator()
{
  delete msg_;
}


void ELTableGenerator::GeneratePrimaryVertex(G4Event* event)
{
  // Select an initial position for the ionization electrons using the geometry
  G4ThreeVector position = geom_->GenerateVertex("EL_TABLE");

  // Ionization electrons generated at start-of-event
  G4double time = 0.;

  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);

  // We set a value for the electron's momentum as required by Geant4
  G4double kinetic_energy = 1. * eV;
  G4double mass = IonizationElectron::Definition()->GetPDGMass();
  G4double total_energy = kinetic_energy + mass;
  G4double pz = std::sqrt(total_energy*total_energy - mass*mass);

  // Add as many ionization electrons to the vertex as requested by the user
  for (G4int i=0; i<num_ie_; i++) {
    G4PrimaryParticle* particle =
      new G4PrimaryParticle(IonizationElectron::Definition(), 0., 0., pz);
    vertex->SetPrimary(particle);
  }

  // Add vertex to the event
  event->AddPrimaryVertex(vertex);
}
