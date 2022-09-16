// ----------------------------------------------------------------------------
// nexus | LambertianGenerator.cc
//
// This class is a generator for particle events generated in a given cone
// following a Lambertian distribution. The user must specify via configuration
// parameters the particle type, a kinetic energy interval, momentum direction
// and theta limits.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "LambertianGenerator.h"
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
#include <G4RandomTools.hh>
#include <G4OpticalPhoton.hh>

#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(LambertianGenerator, G4VPrimaryGenerator)


LambertianGenerator::LambertianGenerator():
G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
energy_min_(0.), energy_max_(0.), geom_(0), momentum_{},
costheta_min_(0), costheta_max_(1.)
{
  msg_ = new G4GenericMessenger(this, "/Generator/LambertianGenerator/",
    "Control commands of Lambertian generator.");

  msg_->DeclareMethod("particle", &LambertianGenerator::SetParticleDefinition,
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


  msg_->DeclarePropertyWithUnit("momentum", "mm",  momentum_, "Set particle 3-momentum.");

  msg_->DeclareProperty("min_costheta", costheta_min_,
			"Set minimum cosTheta for the direction of the particle.");
  msg_->DeclareProperty("max_costheta", costheta_max_,
			"Set maximum cosTheta for the direction of the particle.");

  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();
}



LambertianGenerator::~LambertianGenerator()
{
  delete msg_;
}



void LambertianGenerator::SetParticleDefinition(G4String particle_name)
{
  particle_definition_ =
    G4ParticleTable::GetParticleTable()->FindParticle(particle_name);

  if (!particle_definition_)
    G4Exception("[LambertianGenerator]", "SetParticleDefinition()",
      FatalException, "User gave an unknown particle name.");
}



void LambertianGenerator::GeneratePrimaryVertex(G4Event* event)
{
  // Generate uniform random energy in [E_min, E_max]
  G4double kinetic_energy = nexus::UniformRandomInRange(energy_max_, energy_min_);

  // Calculate cartesian components of momentum
  G4double mass   = particle_definition_->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod = std::sqrt(energy*energy - mass*mass);

  G4ThreeVector p_dir = G4LambertianRand(momentum_);

  G4double cosine = p_dir.dot(momentum_);

  // The theta angle is the angle between the momentum generated (p_dir) and
  // Lambertian generator axis (in this code called momentum_).
  while((costheta_min_ > cosine) || (cosine > costheta_max_)){
      p_dir = G4LambertianRand(momentum_);
      cosine = p_dir.dot(momentum_);
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
