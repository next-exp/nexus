//
//  SingleParticle.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 27 Mar 2009
//     Updated:    Mar 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#include "SingleParticle.h"
#include "InputManager.h"
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <Randomize.hh>
#include <bhep/gstore.h>


namespace nexus {


  SingleParticle::SingleParticle():
    _input(InputManager::Instance().GetGenerationInput())
  {
    Initialize();
  }
  
  
  
  void SingleParticle::Initialize()
  {
    /// Set particle definition (i.e., type) by name
    G4String particle_name = _input.fetch_sstore("particle_name");
    G4ParticleTable* particle_table = G4ParticleTable::GetParticleTable();
    _particle_definition = particle_table->FindParticle(particle_name);
    
    /// Set particle basic properties from definition
    _mass   = _particle_definition->GetPDGMass();
    _charge = _particle_definition->GetPDGCharge();
    
    /// Set user-options
    _energy_min = _input.fetch_dstore("energy_min");
    _energy_max = _input.fetch_dstore("energy_max");
  }
  

  
  void SingleParticle::GeneratePrimaryVertex(G4Event* evt)
  {
    G4ThreeVector particle_position = (0., 0., 0.);
    G4double particle_time = 0.;
    
    /// Create a new vertex
    G4PrimaryVertex* vertex = new G4PrimaryVertex(particle_position, particle_time);
    
    /// Generate random energy in [E_min, E_max]
    G4double kinetic_energy = GenerateRandomEnergy(_energy_min, _energy_max);
    
    /// Generate random direction
    G4ThreeVector momentum_direction = GenerateRandomDirection();
    
    /// Create new primaries and add them to the vertex
    G4double energy = kinetic_energy + _mass;
    G4double pmom = std::sqrt(energy*energy - _mass*_mass);
    G4double px = pmom * momentum_direction.x();
    G4double py = pmom * momentum_direction.y();
    G4double pz = pmom * momentum_direction.z();
    
    G4PrimaryParticle* particle = new G4PrimaryParticle(_particle_definition, 
							px, py, pz);
    particle->SetMass(_mass);
    particle->SetCharge(_charge);
    particle->SetPolarization(0.,0.,0.);
    vertex->SetPrimary(particle);
    evt->AddPrimaryVertex(vertex);
  }
  
  
  
  G4ThreeVector SingleParticle::GenerateRandomDirection()
  {
    /// Random distribution uniform in solid angle
    G4double cos_theta = 2. * G4UniformRand() - 1.;
    G4double sin_theta = std::sqrt(1. - cos_theta*cos_theta);
    G4double phi = twopi * G4UniformRand();
    
    G4double ux = sin_theta * std::cos(phi);
    G4double uy = sin_theta * std::sin(phi);
    G4double uz = cos_theta;
    
    return G4ThreeVector(ux, uy, uz);
  }
  


  G4double SingleParticle::GenerateRandomEnergy(G4double min, G4double max)
  {
    return (G4UniformRand() * (max - min) + min);
  }



} // end namespace nexus


