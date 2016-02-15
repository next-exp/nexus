// ----------------------------------------------------------------------------
//  $Id: ScintillationGenerator.cc 9593 2014-02-13 16:38:56Z paola $
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 27 Mar 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ScintillationGenerator.h"

#include "DetectorConstruction.h"
#include "BaseGeometry.h"

#include <G4GenericMessenger.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>
#include <G4OpticalPhoton.hh>

#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;
using namespace CLHEP;


ScintillationGenerator::ScintillationGenerator() :
  G4VPrimaryGenerator(), _msg(0), _particle_definition(G4OpticalPhoton::Definition()),
_geom(0), _position_X(0.),_position_Y(0.), _position_Z(0.),
_nphotons(1000000)
{
  _msg = new G4GenericMessenger(this, "/Generator/S1generator/",
    "Control commands of scintillation generator.");

  _msg->DeclareMethod("particle", &ScintillationGenerator::SetParticleDefinition, 
    "Set particle to be generated.");
  
  G4GenericMessenger::Command& energy = _msg->DeclareProperty("energy", _energy,"Set kinetic energy of the particle.");
  energy.SetUnitCategory("Energy");
  energy.SetParameterName("energy", false);
  energy.SetRange("energy>0.");

  _msg->DeclareProperty("xpos", _position_X, "Set x position");
  _msg->DeclareProperty("ypos", _position_Y, "Set y position");
  _msg->DeclareProperty("zpos", _position_Z, "Set z position");
  _msg->DeclareProperty("nphotons", _nphotons, "Set number of photons");
  
  DetectorConstruction* detconst = 
    (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  _geom = detconst->GetGeometry();
}

ScintillationGenerator::~ScintillationGenerator()
{
  delete _msg;
}

void ScintillationGenerator::SetParticleDefinition(G4String particle_name)
{
  _particle_definition = G4ParticleTable::GetParticleTable()->FindParticle(particle_name);

  if (!_particle_definition)
    G4Exception("SetParticleDefinition()", "[S1gen]",  FatalException, "User gave an unknown particle name.");
}

void ScintillationGenerator::GeneratePrimaryVertex(G4Event* event)
{
  // Generate an initial position for the particle using the geometry and set time to 0.
  G4ThreeVector position = G4ThreeVector( _position_X, _position_Y, _position_Z );
  G4double time = 0.;
  
  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);
  
  for ( G4int i = 0; i<_nphotons; i++)
    {
      // Generate random direction by default
      G4ThreeVector _momentum_direction = G4RandomDirection();
      G4double pmod = GetEnergy();
      G4double px = pmod * _momentum_direction.x();
      G4double py = pmod * _momentum_direction.y();
      G4double pz = pmod * _momentum_direction.z();
      
      // Create the new primary particle and set it some properties
      G4PrimaryParticle* particle = new G4PrimaryParticle(_particle_definition, px, py, pz);
      
      G4ThreeVector polarization = G4RandomDirection();
      particle->SetPolarization(polarization);
      
      // Add particle to the vertex and this to the event
      vertex->SetPrimary(particle);
    }
  event->AddPrimaryVertex(vertex);
}

void ScintillationGenerator::SetEnergy(double e) { _energy = e; }
G4double ScintillationGenerator::GetEnergy() { return _energy; }
