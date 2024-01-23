// ----------------------------------------------------------------------------
// nexus | SingleParticleGenerator.cc
//
// This class is the primary generator for events consisting of
// a single particle. The user must specify via configuration
// parameters the particle type, a kinetic energy interval and, optionally,
// a momentum direction.
// Particle energy is generated with flat random probability
// between E_min and E_max.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SingleParticleGenerator.h"

#include "DetectorConstruction.h"
#include "GeometryBase.h"
#include "RandomUtils.h"
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

REGISTER_CLASS(SingleParticleGenerator, G4VPrimaryGenerator)


SingleParticleGenerator::SingleParticleGenerator():
G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
energy_min_(0.), energy_max_(0.), geom_(0), momentum_{},
costheta_min_(-1.), costheta_max_(1.), phi_min_(0.), phi_max_(2.*pi)
{
  msg_ = new G4GenericMessenger(this, "/Generator/SingleParticle/",
    "Control commands of single-particle generator.");

  msg_->DeclareMethod("particle", &SingleParticleGenerator::SetParticleDefinition,
    "Set particle to be generated.");

  G4GenericMessenger::Command& min_energy =
    msg_->DeclareProperty("min_energy", energy_min_, "Minimum kinetic energy of the particle.");
  min_energy.SetUnitCategory("Energy");
  min_energy.SetParameterName("min_energy", false);
  min_energy.SetRange("min_energy>0.");

  G4GenericMessenger::Command& max_energy =
    msg_->DeclareProperty("max_energy", energy_max_, "Maximum kinetic energy of the particle");
  max_energy.SetUnitCategory("Energy");
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  msg_->DeclareProperty("region", region_,
                        "Region of the geometry where the vertex will be generated.");


  msg_->DeclarePropertyWithUnit("momentum", "mm",  momentum_, "Particle 3-momentum.");

  msg_->DeclareProperty("min_costheta", costheta_min_,
			"Minimum cosTheta for the direction of the particle.");
  msg_->DeclareProperty("max_costheta", costheta_max_,
			"Maximum cosTheta for the direction of the particle.");
  msg_->DeclareProperty("min_phi", phi_min_,
			"Minimum phi for the direction of the particle.");
  msg_->DeclareProperty("max_phi", phi_max_,
			"Maximum phi for the direction of the particle.");


  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();
}



SingleParticleGenerator::~SingleParticleGenerator()
{
  delete msg_;
}



void SingleParticleGenerator::SetParticleDefinition(G4String particle_name)
{
  particle_definition_ =
    G4ParticleTable::GetParticleTable()->FindParticle(particle_name);

  if (!particle_definition_)
    G4Exception("[SingleParticleGenerator]", "SetParticleDefinition()",
      FatalException, "User gave an unknown particle name.");
}



void SingleParticleGenerator::GeneratePrimaryVertex(G4Event* event)
{
  // Generate uniform random energy in [E_min, E_max]
  G4double kinetic_energy = nexus::UniformRandomInRange(energy_max_, energy_min_);

    // Calculate cartesian components of momentum
  G4double mass   = particle_definition_->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod = std::sqrt(energy*energy - mass*mass);

  bool fixed_momentum = momentum_ != G4ThreeVector{};
  bool restrict_angle = costheta_min_ != -1. || costheta_max_ != 1. || phi_min_ != 0. || phi_max_ !=2.*pi;

  G4ThreeVector p_dir; // it will be set in the if branches below
  if (fixed_momentum) { // if the user provides a momentum direction
    p_dir = momentum_.unit();
  } else if (restrict_angle) { // if the user provides a range of angles
    p_dir = RandomDirectionInRange(costheta_min_, costheta_max_, phi_min_, phi_max_);
  } else {
    p_dir = G4RandomDirection();
  }

  G4ThreeVector p = pmod * p_dir;

  // Create the new primary particle and set it some properties
  auto particle = new G4PrimaryParticle(particle_definition_, p.x(), p.y(), p.z());

  // Set random polarization
  if (particle_definition_ == G4OpticalPhoton::Definition()) {
    G4ThreeVector polarization = G4RandomDirection();
    particle->SetPolarization(polarization);
  }

  // Generate an initial position for the particle using the geometry
  G4ThreeVector position = geom_->GenerateVertex(region_);

  // Particle generated at start-of-event
  G4double time = 0.;

  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);

    // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);
  event->AddPrimaryVertex(vertex);
}
