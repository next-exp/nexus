// ----------------------------------------------------------------------------
// nexus | ElecPositronPairGenerator.cc
//
// This generator simulates an electron and a positron from the same vertex,
// with total kinetic energy settable by parameter.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ElecPositronPairGenerator.h"
#include "GeometryBase.h"
#include "FactoryBase.h"
#include "RandomUtils.h"
#include "DetectorConstruction.h"

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

REGISTER_CLASS(ElecPositronPairGenerator, G4VPrimaryGenerator)

ElecPositronPairGenerator::ElecPositronPairGenerator():
G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
energy_min_(0.), energy_max_(0.),
geom_(0)
{
  msg_ = new G4GenericMessenger(this, "/Generator/ElecPositronPair/",
    "Control commands of single-particle generator.");

  G4GenericMessenger::Command& min_energy =
    msg_->DeclareProperty("min_energy", energy_min_, "Minimum total kinetic energy of the two particles.");
  min_energy.SetUnitCategory("Energy");
  min_energy.SetParameterName("min_energy", false);
  min_energy.SetRange("min_energy>0.");

  G4GenericMessenger::Command& max_energy =
    msg_->DeclareProperty("max_energy", energy_max_, "Maximum total kinetic energy of the two particles.");
  max_energy.SetUnitCategory("Energy");
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  msg_->DeclareProperty("region", region_,
                        "Region of the geometry where the vertex will be generated.");

  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();
}



ElecPositronPairGenerator::~ElecPositronPairGenerator()
{
  delete msg_;
}


void ElecPositronPairGenerator::GeneratePrimaryVertex(G4Event* event)
{

  particle_definition_ =
    G4ParticleTable::GetParticleTable()->FindParticle("e-");

  // Generate an initial position for the particle using the geometry
  G4ThreeVector pos = geom_->GenerateVertex(region_);

  // Particle generated at start-of-event
  G4double time = 0.;

  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(pos, time);

  // Generate uniform random energy in [E_min, E_max]
  G4double tot_kinetic_energy = UniformRandomInRange(energy_max_, energy_min_);
  G4double kinetic_energy = G4UniformRand() * tot_kinetic_energy;

  // Generate random direction by default
  G4ThreeVector _momentum_direction = G4RandomDirection();

    // Calculate cartesian components of momentum
  G4double mass   = particle_definition_->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod = std::sqrt(energy*energy - mass*mass);
  G4double px = pmod * _momentum_direction.x();
  G4double py = pmod * _momentum_direction.y();
  G4double pz = pmod * _momentum_direction.z();

  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle =
    new G4PrimaryParticle(particle_definition_, px, py, pz);

  // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);


   particle_definition_ =
    G4ParticleTable::GetParticleTable()->FindParticle("e+");

  G4double kinetic_energy2 = tot_kinetic_energy - kinetic_energy;

  // Generate random direction by default
  G4ThreeVector _momentum_direction2 = G4RandomDirection();

    // Calculate cartesian components of momentum
  G4double energy2 = kinetic_energy2 + mass;
  G4double pmod2 = std::sqrt(energy2*energy2 - mass*mass);
  G4double px2 = pmod2 * _momentum_direction2.x();
  G4double py2 = pmod2 * _momentum_direction2.y();
  G4double pz2 = pmod2 * _momentum_direction2.z();

  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle2 =
    new G4PrimaryParticle(particle_definition_, px2, py2, pz2);

    // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle2);
  event->AddPrimaryVertex(vertex);
}

