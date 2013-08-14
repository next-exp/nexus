// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 27 Mar 2009
//
//  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SingleParticle.h"
#include "ConfigService.h"
#include "ParamStore.h"
#include "DetectorConstruction.h"
#include "BaseGeometry.h"

#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>



namespace nexus {


  SingleParticle::SingleParticle()
  {
    Initialize();
  }
  
  
  
  void SingleParticle::Initialize()
  {
    // Retrieve configuration parameters for generation
    const ParamStore& cfg = ConfigService::Instance().Generation();
    
    // Set particle definition (i.e., type) by name
    G4String particle_name = cfg.GetSParam("particle_name");
    _particle_definition = 
      (G4ParticleTable::GetParticleTable())->FindParticle(particle_name);

    // Set particle basic properties from definition
    _mass   = _particle_definition->GetPDGMass();
    _charge = _particle_definition->GetPDGCharge();
    
    _energy_min = cfg.GetDParam("energy_min") * MeV;
    _energy_max = cfg.GetDParam("energy_max") * MeV;

    _region = cfg.GetSParam("region");

    if (cfg.PeekDVParam("direction"))
      const bhep::vdouble& direction = cfg.GetDVParam("direction");
    
  }
  

  
  void SingleParticle::GeneratePrimaryVertex(G4Event* event)
  {
    //_particle_definition = 
    //(G4ParticleTable::GetParticleTable())->GetIon(95, 241, 0.);
    
    // Generation time at start-of-event (t=0)
    G4double particle_time = 0.;

    // Get geometry from G4RunManager to generate a vertex
    DetectorConstruction* detConst = (DetectorConstruction*)
      G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    G4ThreeVector particle_position = 
      detConst->GetGeometry()->GenerateVertex(_region);

    // Create a new vertex
    G4PrimaryVertex* vertex = 
      new G4PrimaryVertex(particle_position, particle_time);
    
    // Generate uniform random energy in [E_min, E_max]
    G4double kinetic_energy = GenerateRandomEnergy(_energy_min, _energy_max);
    
    // Generate random direction
    G4ThreeVector momentum_direction = G4RandomDirection();
    
    // Calculate cartesian components of momentum
    G4double energy = kinetic_energy + _mass;
    G4double pmod = std::sqrt(energy*energy - _mass*_mass);
    G4double px = pmod * momentum_direction.x();
    G4double py = pmod * momentum_direction.y();
    G4double pz = pmod * momentum_direction.z();
    
    // Create new primary and add it to the vertex
    G4PrimaryParticle* particle = 
      new G4PrimaryParticle(_particle_definition, px, py, pz);
    particle->SetMass(_mass);
    particle->SetCharge(_charge);
    particle->SetPolarization(0.,0.,0.);
    vertex->SetPrimary(particle);
    event->AddPrimaryVertex(vertex);
  }
  
  
  
  G4double SingleParticle::GenerateRandomEnergy(G4double min, G4double max)
  {
    return (G4UniformRand() * (max - min) + min);
  }


} // end namespace nexus


