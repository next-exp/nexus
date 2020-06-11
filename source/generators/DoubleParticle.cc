// ----------------------------------------------------------------------------
// nexus | DoubleParticle.cc
//
// This generator is based on the SingleParticle generator, but simulates
// two particles instead of one. The region passed to the GenerateVertex()
// method must provide two positions.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DoubleParticle.h"

#include "DetectorConstruction.h"
#include "Pet2boxes.h"

#include <G4GenericMessenger.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4OpticalPhoton.hh>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

using namespace nexus;
using namespace CLHEP;


DoubleParticle::DoubleParticle():
G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
energy_min_(0.), energy_max_(0.), geom_(0)
{
  msg_ = new G4GenericMessenger(this, "/Generator/DoubleParticle/",
    "Control commands of single-particle generator.");

  msg_->DeclareMethod("particle", &DoubleParticle::SetParticleDefinition, 
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

  msg_->DeclareProperty("region", region_, 
    "Set the region of the geometry where the vertex will be generated.");

  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();
}



DoubleParticle::~DoubleParticle()
{
  delete msg_;
}



void DoubleParticle::SetParticleDefinition(G4String particle_name)
{
  particle_definition_ = 
    G4ParticleTable::GetParticleTable()->FindParticle(particle_name);

  if (!particle_definition_)
    G4Exception("SetParticleDefinition()", "[DoubleParticle]",
      FatalException, "User gave an unknown particle name.");
}



void DoubleParticle::GeneratePrimaryVertex(G4Event* event)
{
  // Generate an initial position for the particle using the geometry
  std::pair<G4ThreeVector, G4ThreeVector>  positions = geom_->GenerateVertices(region_);
  G4ThreeVector pos1 = positions.first;
  G4ThreeVector pos2 = positions.second;

  G4double time1 = sqrt(pos1.getX()*pos1.getX() + pos1.getY()*pos1.getY() + pos1.getZ()*pos1.getZ())/c_light;
  G4double time2 = sqrt(pos2.getX()*pos2.getX() + pos2.getY()*pos2.getY() + pos2.getZ()*pos2.getZ())/c_light;

  // Particle generated at start-of-event
  // G4cout << time1/picosecond << ", " << time2/picosecond << G4endl;
  //G4double time = 0.;

  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(pos1, time1);

  // Generate uniform random energy in [E_min, E_max]
  G4double kinetic_energy = RandomEnergy();

  // Generate random direction by default
  G4ThreeVector momentum_direction_ = G4RandomDirection();

    // Calculate cartesian components of momentum
  G4double mass   = particle_definition_->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod = std::sqrt(energy*energy - mass*mass);
  G4double px = pmod * momentum_direction_.x();
  G4double py = pmod * momentum_direction_.y();
  G4double pz = pmod * momentum_direction_.z();



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

   // Create a new vertex
  G4PrimaryVertex* vertex2 = new G4PrimaryVertex(pos2, time2);

  // Generate uniform random energy in [E_min, E_max]
  G4double kinetic_energy2 = RandomEnergy();

  // Generate random direction by default
  G4ThreeVector momentum_direction2_ = G4RandomDirection();

    // Calculate cartesian components of momentum
  // G4double mass   = _particle_definition->GetPDGMass();
  G4double energy2 = kinetic_energy2 + mass;
  G4double pmod2 = std::sqrt(energy2*energy2 - mass*mass);
  G4double px2 = pmod2 * momentum_direction2_.x();
  G4double py2 = pmod2 * momentum_direction2_.y();
  G4double pz2 = pmod2 * momentum_direction2_.z();

  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle2 = 
    new G4PrimaryParticle(particle_definition_, px2, py2, pz2);

  // Set random polarization
  if (particle_definition_ == G4OpticalPhoton::Definition()) {
    G4ThreeVector polarization2 = G4RandomDirection();
    particle2->SetPolarization(polarization2);
  }
 
    // Add particle to the vertex and this to the event
  vertex2->SetPrimary(particle2);
  event->AddPrimaryVertex(vertex2);
}



G4double DoubleParticle::RandomEnergy() const
{
  if (energy_max_ == energy_min_) 
    return energy_min_;
  else
    return (G4UniformRand()*(energy_max_ - energy_min_) + energy_min_);
}
