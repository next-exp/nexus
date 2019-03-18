// ---------------------------------------------------------------------------
//
//  Author : Miquel Nebot Guinot <miquel.nebot@ific.uv.es>
//           A. Laing
//
//  Created : Aug 2016
//  Modified: March 2019
//
//  Copyright (c) 2016 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------


#include "NeutronGenerator.h"

#include "DetectorConstruction.h"
#include "BaseGeometry.h"

#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryVertex.hh>
#include <G4RandomDirection.hh>


using namespace nexus;


NeutronGenerator::NeutronGenerator():
  G4VPrimaryGenerator(), _msg(0), _particle_definition(0), _energy_min(0.), _energy_max(0.),
  _geom(0)
{
  _msg = new G4GenericMessenger(this, "/Generator/NeutronGenerator/",
                                "Control commands of the neutron primary generator.");

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
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  _msg->DeclareProperty("region", _region,
			"Set the region of the geometry where the vertex will be generated.");

  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  _geom = detconst->GetGeometry();
}



NeutronGenerator::~NeutronGenerator()
{
  delete _msg;
}

void NeutronGenerator::GeneratePrimaryVertex(G4Event* event)
{
  _particle_definition = G4ParticleTable::GetParticleTable()->
    FindParticle("neutron");

  if (!_particle_definition)
    G4Exception("SetParticleDefinition()", "[NeutronGenerator]",
                FatalException, " can not create a neutron ");
  // Generate an initial position for the particle using the geometry                                                                                                                
  G4ThreeVector position = _geom->GenerateVertex(_region);
  // Particle generated at start-of-event                                                                                                                                            
  G4double time = 0.;
  // Create a new vertex                                                                                                                                                             
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);
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
    new G4PrimaryParticle(_particle_definition,px,py,pz);

  // Add particle to the vertex and this to the event                                                                                                                                
  vertex->SetPrimary(particle);
  event->AddPrimaryVertex(vertex);
}

G4double NeutronGenerator::RandomEnergy() const
{
  if (_energy_max == _energy_min)
    return _energy_min;
  else
    return (G4UniformRand()*(_energy_max - _energy_min) + _energy_min);
}



