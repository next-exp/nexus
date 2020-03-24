// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : P Ferrario <paolafer@ific.uv.es>    
//  Created: 
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
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
G4VPrimaryGenerator(), _msg(0), _particle_definition(0),
_energy_min(0.), _energy_max(0.), _geom(0)
{
  _msg = new G4GenericMessenger(this, "/Generator/DoubleParticle/",
    "Control commands of single-particle generator.");

  _msg->DeclareMethod("particle", &DoubleParticle::SetParticleDefinition, 
    "Set particle to be generated.");

  G4GenericMessenger::Command& min_energy =
    _msg->DeclareProperty("min_energy", _energy_min, 
      "Set minimum kinetic energy of the particle.");
  min_energy.SetUnitCategory("Energy");
  min_energy.SetParameterName("min_energy", false);
  min_energy.SetRange("min_energy>0.");

  G4GenericMessenger::Command& max_energy =
    _msg->DeclareProperty("max_energy", _energy_max, 
      "Set maximum kinetic energy of the particle");
  max_energy.SetUnitCategory("Energy");

  _msg->DeclareProperty("region", _region, 
    "Set the region of the geometry where the vertex will be generated.");

  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  _geom = detconst->GetGeometry();
}



DoubleParticle::~DoubleParticle()
{
  delete _msg;
}



void DoubleParticle::SetParticleDefinition(G4String particle_name)
{
  _particle_definition = 
    G4ParticleTable::GetParticleTable()->FindParticle(particle_name);

  if (!_particle_definition)
    G4Exception("SetParticleDefinition()", "[DoubleParticle]",
      FatalException, "User gave an unknown particle name.");
}



void DoubleParticle::GeneratePrimaryVertex(G4Event* event)
{
  // Generate an initial position for the particle using the geometry
  std::pair<G4ThreeVector, G4ThreeVector>  positions = _geom->GenerateVertices(_region);
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
  G4ThreeVector _momentum_direction = G4RandomDirection();

    // Calculate cartesian components of momentum
  G4double mass   = _particle_definition->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod = std::sqrt(energy*energy - mass*mass);
  G4double px = pmod * _momentum_direction.x();
  G4double py = pmod * _momentum_direction.y();
  G4double pz = pmod * _momentum_direction.z();



  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle = 
    new G4PrimaryParticle(_particle_definition, px, py, pz);

  // Set random polarization
  if (_particle_definition == G4OpticalPhoton::Definition()) {
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
  G4ThreeVector _momentum_direction2 = G4RandomDirection();

    // Calculate cartesian components of momentum
  // G4double mass   = _particle_definition->GetPDGMass();
  G4double energy2 = kinetic_energy2 + mass;
  G4double pmod2 = std::sqrt(energy2*energy2 - mass*mass);
  G4double px2 = pmod2 * _momentum_direction2.x();
  G4double py2 = pmod2 * _momentum_direction2.y();
  G4double pz2 = pmod2 * _momentum_direction2.z();

  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle2 = 
    new G4PrimaryParticle(_particle_definition, px2, py2, pz2);

  // Set random polarization
  if (_particle_definition == G4OpticalPhoton::Definition()) {
    G4ThreeVector polarization2 = G4RandomDirection();
    particle2->SetPolarization(polarization2);
  }
 
    // Add particle to the vertex and this to the event
  vertex2->SetPrimary(particle2);
  event->AddPrimaryVertex(vertex2);
}



G4double DoubleParticle::RandomEnergy() const
{
  if (_energy_max == _energy_min) 
    return _energy_min;
  else
    return (G4UniformRand()*(_energy_max - _energy_min) + _energy_min);
}
