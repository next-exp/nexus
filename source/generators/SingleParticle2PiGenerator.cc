// ----------------------------------------------------------------------------
// nexus | SingleParticle2PiGenerator.cc
//
// This class is the primary generator for events consisting of
// a single particle, generated in the direction towards the detector,
// from the NEXT-DEMO sideport. The user must specify via configuration
// parameters the particle type, a kinetic energy interval.
// Particle energy is generated with flat random probability
// between E_min and E_max.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SingleParticle2PiGenerator.h"

#include "DetectorConstruction.h"
#include "BaseGeometry.h"

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


SingleParticle2PiGenerator::SingleParticle2PiGenerator():
G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
energy_min_(0.), energy_max_(0.), geom_(0)
{
  msg_ = new G4GenericMessenger(this, "/Generator/SingleParticle2Pi/",
                                "Control commands of single-particle generator.");

  msg_->DeclareMethod("particle", &SingleParticle2PiGenerator::SetParticleDefinition,
                      "Set particle to be generated.");

  G4GenericMessenger::Command& min_energy =
    msg_->DeclareProperty("min_energy", energy_min_,
      "Set minimum kinetic energy of the particle.");
  min_energy.SetUnitCategory("Energy");
  min_energy.SetParameterName("min_energy", false);
  min_energy.SetRange("min_energy>0.");

  G4GenericMessenger::Command& max_energy =
    msg_->DeclareProperty("max_energy", energy_max_,
      "Set maximum kinetic energy of the particle");
  max_energy.SetUnitCategory("Energy");
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  msg_->DeclareProperty("region", region_,
    "Set the region of the geometry where the vertex will be generated.");



  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();
}



SingleParticle2PiGenerator::~SingleParticle2PiGenerator()
{
  delete msg_;
}



void SingleParticle2PiGenerator::SetParticleDefinition(G4String particle_name)
{
  particle_definition_ =
    G4ParticleTable::GetParticleTable()->FindParticle(particle_name);

  if (!particle_definition_)
    G4Exception("[SingleParticle2PiGenerator]", "SetParticleDefinition()",
      FatalException, "User gave an unknown particle name.");
}



void SingleParticle2PiGenerator::GeneratePrimaryVertex(G4Event* event)
{
  // Generate an initial position for the particle using the geometry
  G4ThreeVector position = geom_->GenerateVertex(region_);

  // Particle generated at start-of-event
  G4double time = 0.;

  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);

  // Generate uniform random energy in [E_min, E_max]
  G4double kinetic_energy = RandomEnergy();

  G4ThreeVector momentum_direction;

  ///To generate just in a part of the solid angle
  G4bool mom_dir = false;
  while (mom_dir == false) {
    G4double cosTheta  = 2.*G4UniformRand()-1.;
    G4double sinTheta2 = 1. - cosTheta*cosTheta;
    if( sinTheta2 < 0.)  sinTheta2 = 0.;
    G4double sinTheta  = std::sqrt(sinTheta2);
    G4double phi       = twopi*G4UniformRand();

      if ((phi > 0 && phi < 2.094) || (phi > 5.24 && phi < 6.28)){
        //forward from Next1EL sideport
	mom_dir = true;
	momentum_direction = G4ThreeVector(sinTheta*std::cos(phi),
					    sinTheta*std::sin(phi), cosTheta).unit();
      }

  }

    // Calculate cartesian components of momentum
  G4double mass   = particle_definition_->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod = std::sqrt(energy*energy - mass*mass);
  G4double px = pmod * momentum_direction.x();
  G4double py = pmod * momentum_direction.y();
  G4double pz = pmod * momentum_direction.z();

  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle =
    new G4PrimaryParticle(particle_definition_, px, py, pz);

  // Set random polarization
  if (particle_definition_ == G4OpticalPhoton::Definition()) {
    G4ThreeVector polarization = G4RandomDirection();
    particle->SetPolarization(polarization);
  }

    // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);
  event->AddPrimaryVertex(vertex);
}



G4double SingleParticle2PiGenerator::RandomEnergy() const
{
  if (energy_max_ == energy_min_)
    return energy_min_;
  else
    return (G4UniformRand()*(energy_max_ - energy_min_) + energy_min_);
}
