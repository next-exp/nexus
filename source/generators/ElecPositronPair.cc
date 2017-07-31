// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : P Ferrario <paolafer@ific.uv.es>    
//  Created: 
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ElecPositronPair.h"
#include "BaseGeometry.h"

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


ElecPositronPair::ElecPositronPair():
G4VPrimaryGenerator(), _msg(0), _particle_definition(0),
_energy_min(0.), _energy_max(0.), 
_geom(0)
{
  _msg = new G4GenericMessenger(this, "/Generator/ElecPositronPair/",
    "Control commands of single-particle generator.");

  // _msg->DeclareMethod("particle", &ElecPositronPair::SetParticleDefinition, 
  //   "Set particle to be generated.");

  G4GenericMessenger::Command& min_energy =
    _msg->DeclareProperty("min_energy", _energy_min, 
                          "Set minimum total kinetic energy of the two particles.");
  min_energy.SetUnitCategory("Energy");
  min_energy.SetParameterName("min_energy", false);
  min_energy.SetRange("min_energy>0.");

  G4GenericMessenger::Command& max_energy =
    _msg->DeclareProperty("max_energy", _energy_max, 
                          "Set maximum total kinetic energy of the two particles.");
  max_energy.SetUnitCategory("Energy");
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  _msg->DeclareProperty("region", _region, 
    "Set the region of the geometry where the vertex will be generated.");

  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  _geom = detconst->GetGeometry();
}



ElecPositronPair::~ElecPositronPair()
{
  delete _msg;
}


void ElecPositronPair::GeneratePrimaryVertex(G4Event* event)
{

  _particle_definition = 
    G4ParticleTable::GetParticleTable()->FindParticle("e-");

  if (!_particle_definition)
    G4Exception("SetParticleDefinition()", "[ElecPositronPair]",
      FatalException, "User gave an unknown particle name.");
  
  // Generate an initial position for the particle using the geometry
  G4ThreeVector pos = _geom->GenerateVertex(_region);
  
  // Particle generated at start-of-event
  G4double time = 0.;

  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(pos, time);

  // Generate uniform random energy in [E_min, E_max]
  G4double tot_kinetic_energy = RandomEnergy(_energy_min, _energy_max);
  G4double kinetic_energy = G4UniformRand() * tot_kinetic_energy;

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

  // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);


   _particle_definition = 
    G4ParticleTable::GetParticleTable()->FindParticle("e+");

  if (!_particle_definition)
    G4Exception("SetParticleDefinition()", "[ElecPositronPair]",
      FatalException, "User gave an unknown particle name.");
 
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
    new G4PrimaryParticle(_particle_definition, px2, py2, pz2);
 
    // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle2);
  event->AddPrimaryVertex(vertex);
}



G4double ElecPositronPair::RandomEnergy(G4double emin, G4double emax) const
{
  if (emax == emin) 
    return emin;
  else
    return (G4UniformRand()*(emax - emin) + emin);
}
