// ----------------------------------------------------------------------------
// nexus | IonGenerator.cc
//
// This class is the primary generator for events consisting in the decay
// of a radioactive ion. The user must specify via configuration parameters
// the atomic number, mass number and energy level of the isotope of interest.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "IonGenerator.h"

#include "BaseGeometry.h"
#include "DetectorConstruction.h"

#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>
#include <G4ParticleDefinition.hh>
#include <G4IonTable.hh>
#include <G4PrimaryVertex.hh>

using namespace nexus;


IonGenerator::IonGenerator():
  G4VPrimaryGenerator(),
  atomic_number_(0), mass_number_(0), energy_level_(0.),
  decay_at_time_zero_(true),
  region_(""),
  msg_(nullptr), geom_(nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Generator/IonGenerator/",
                                "Control commands of the ion gun primary generator.");

  G4GenericMessenger::Command& atomic_number_cmd =
    msg_->DeclareProperty("atomic_number", atomic_number_, "Atomic number of the ion.");
  atomic_number_cmd.SetParameterName("atomic_number", false);
  atomic_number_cmd.SetRange("atomic_number > 0");

  G4GenericMessenger::Command& mass_number_cmd =
    msg_->DeclareProperty("mass_number", mass_number_, "Mass number of the ion.");
  mass_number_cmd.SetParameterName("mass_number", false);
  mass_number_cmd.SetRange("mass_number > 0");

  G4GenericMessenger::Command& energy_level_cmd =
    msg_->DeclarePropertyWithUnit("energy_level", "MeV", energy_level_,
                                  "Energy level of the ion.");
  energy_level_cmd.SetParameterName("energy_level", false);
  energy_level_cmd.SetRange("energy_level >= 0");

  msg_->DeclareProperty("decay_at_time_zero", decay_at_time_zero_,
                        "Set to true to make unstable ions decay at t=0.");

  msg_->DeclareProperty("region", region_,
                        "Region of the geometry where vertices will be generated.");

  // Load the detector geometry, which will be used for the generation of vertices
  const DetectorConstruction* detconst = dynamic_cast<const DetectorConstruction*>
    (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  if (detconst) geom_ = detconst->GetGeometry();
  else G4Exception("[IonGenerator]", "IonGenerator()", FatalException,
                   "Unable to load geometry.");
}


IonGenerator::~IonGenerator()
{
  delete msg_;
}


G4ParticleDefinition* IonGenerator::IonDefinition()
{
  G4ParticleDefinition* pdef =
    G4IonTable::GetIonTable()->GetIon(atomic_number_, mass_number_, energy_level_);

  if (!pdef) G4Exception("[IonGenerator]", "IonDefinition()",
                         FatalException, "Unable to find the requested ion.");

  // Unstable ions decay by default at a random time t sampled from an exponential
  // decay distribution proportional to their mean lifetime. This, even for
  // not so long-lived nuclides, pushes the global time of the event to scales
  // orders of magnitude longer than the nanosecond precision required in NEXUS
  // for the correct simulation of the ionization drift and photon tracing.
  // To prevent this behaviour, the lifetime of unstable isotopes is reset here
  // (unless the configuration variable 'decay_at_time_zero' has been set to false)
  // to an arbitrary, short value (1 ps).
  if (decay_at_time_zero_ && !(pdef->GetPDGStable())) pdef->SetPDGLifeTime(1.*ps);

  return pdef;
}


void IonGenerator::GeneratePrimaryVertex(G4Event* event)
{
  // Pointer declared as static so that it gets allocated only once
  // (i.e. the ion definition is only looked up in the first event).
  static G4ParticleDefinition* pdef = IonDefinition();
  // Create the new primary particle (i.e. the ion)
  G4PrimaryParticle* ion = new G4PrimaryParticle(pdef);

  // Generate an initial position for the ion using the geometry
  G4ThreeVector position = geom_->GenerateVertex(region_);
  // Ion generated at the start-of-event time
  G4double time = 0.;
  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);

  // Add ion to the vertex and this to the event
  vertex->SetPrimary(ion);
  event->AddPrimaryVertex(vertex);
}
