// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 27 Mar 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SingleParticle.h"

#include "ConfigService.h"
#include "DetectorConstruction.h"
#include "BaseGeometry.h"
#include "BhepUtils.h"

#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>



namespace nexus {


  SingleParticle::SingleParticle():
    _energy_min(0.), _energy_max(0.), _direction_is_random(true),
    _geom(0)
  {
    DetectorConstruction* detconst = (DetectorConstruction*)
      G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    _geom = detconst->GetGeometry();

    ReadConfigParams();
  }
  
  
  
  void SingleParticle::ReadConfigParams()
  {
    // Get configuration parameters for generation
    const ParamStore& cfg = ConfigService::Instance().Generation();
    
    // Set particle type searching in particle table by name
    _particle_definition = G4ParticleTable::GetParticleTable()->
      FindParticle(cfg.GetSParam("particle_name"));

    // Set mass and charge to PDG values
    _charge = _particle_definition->GetPDGCharge();
    _mass   = _particle_definition->GetPDGMass();
    
    // Set limits for kinetic energy
    _energy_min = cfg.GetDParam("energy_min");
    _energy_max = cfg.GetDParam("energy_max");

    // Set momentum direction if specified; it will be random otherwise.
    if (cfg.PeekDVParam("momentum_direction")) {
      _direction_is_random = false;
      _momentum_direction = 
	BhepUtils::DVto3Vector(cfg.GetDVParam("momentum_direction"));
      // normalize the vector if needed
      _momentum_direction = _momentum_direction.unit();
    }

    _region = cfg.GetSParam("region");
  }
  

  
  void SingleParticle::GeneratePrimaryVertex(G4Event* event)
  {
    // Ask the geometry to generate a position for the particle
    G4ThreeVector position = _geom->GenerateVertex(_region);

    // Particle generated at start-of-event
    G4double time = 0.;
    
    // Create a new vertex
    G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);
    
    // Generate uniform random energy in [E_min, E_max]
    G4double kinetic_energy = RandomEnergy();
    
    // Generate random direction
    if (_direction_is_random)
      _momentum_direction = G4RandomDirection();

    // Calculate cartesian components of momentum
    G4double energy = kinetic_energy + _mass;
    G4double pmod = std::sqrt(energy*energy - _mass*_mass);
    G4double px = pmod * _momentum_direction.x();
    G4double py = pmod * _momentum_direction.y();
    G4double pz = pmod * _momentum_direction.z();
    
    // Create the new primary particle and set it some properties
    G4PrimaryParticle* particle = 
      new G4PrimaryParticle(_particle_definition, px, py, pz);

    particle->SetCharge(_charge);
    particle->SetMass(_mass);
    particle->SetPolarization(0.,0.,0.);

    // Add particle to the vertex and this to the event
    vertex->SetPrimary(particle);
    event->AddPrimaryVertex(vertex);
  }
  
  
  
  G4double SingleParticle::RandomEnergy() const
  {
    if (_energy_max == _energy_min)
      return _energy_min;
    
    return (G4UniformRand() * (_energy_max - _energy_min) + _energy_min);
  }


} // end namespace nexus


