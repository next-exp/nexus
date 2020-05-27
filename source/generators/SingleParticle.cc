// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>
//  Created: 27 Mar 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SingleParticle.h"

#include "DetectorConstruction.h"
#include "BaseGeometry.h"

#include <G4GenericMessenger.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4OpticalPhoton.hh>

#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;
using namespace CLHEP;


SingleParticle::SingleParticle():
G4VPrimaryGenerator(), _msg(0), _particle_definition(0),
_energy_min(0.), _energy_max(0.), _geom(0), _momentum_X(0.),
_momentum_Y(0.), _momentum_Z(0.), _costheta_min(-1.), _costheta_max(1.),
_phi_min(0.), _phi_max(2.*pi)
{
  _msg = new G4GenericMessenger(this, "/Generator/SingleParticle/",
    "Control commands of single-particle generator.");

  _msg->DeclareMethod("particle", &SingleParticle::SetParticleDefinition,
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
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  _msg->DeclareProperty("region", _region,
    "Set the region of the geometry where the vertex will be generated.");

  /// Temporary
  //G4GenericMessenger::Command&  momentum_X_cmd =
  _msg->DeclareProperty("momentum_X", _momentum_X,
			"x coord of momentum");
  //G4GenericMessenger::Command&  momentum_Y_cmd =
  _msg->DeclareProperty("momentum_Y", _momentum_Y,
			"y coord of momentum");
  //G4GenericMessenger::Command&  momentum_Z_cmd =
  _msg->DeclareProperty("momentum_Z", _momentum_Z,
			"z coord of momentum");

  _msg->DeclareProperty("min_costheta", _costheta_min,
			"Set minimum cosTheta for the direction of the particle.");
  _msg->DeclareProperty("max_costheta", _costheta_max,
			"Set maximum cosTheta for the direction of the particle.");
  _msg->DeclareProperty("min_phi", _phi_min,
			"Set minimum phi for the direction of the particle.");
  _msg->DeclareProperty("max_phi", _phi_max,
			"Set maximum phi for the direction of the particle.");



  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  _geom = detconst->GetGeometry();
}



SingleParticle::~SingleParticle()
{
  delete _msg;
}



void SingleParticle::SetParticleDefinition(G4String particle_name)
{
  _particle_definition =
    G4ParticleTable::GetParticleTable()->FindParticle(particle_name);

  if (!_particle_definition)
    G4Exception("SetParticleDefinition()", "[SingleParticle]",
      FatalException, "User gave an unknown particle name.");
}



void SingleParticle::GeneratePrimaryVertex(G4Event* event)
{
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

  // If user provides a momentum direction, this one is used
  if (_momentum_X != 0. || _momentum_Y != 0. || _momentum_Z != 0.) {
    // Normalize if needed
    G4double mom_mod = std::sqrt(_momentum_X * _momentum_X +
				 _momentum_Y * _momentum_Y +
				 _momentum_Z * _momentum_Z);
    px = pmod * _momentum_X/mom_mod;
    py = pmod * _momentum_Y/mom_mod;
    pz = pmod * _momentum_Z/mom_mod;
  } else if (_costheta_min != -1. || _costheta_max != 1. || _phi_min != 0. || _phi_max !=2.*pi) {
    G4bool mom_dir = false;
    while (mom_dir == false) {
      G4double cosTheta  = 2.*G4UniformRand()-1.;
      if (cosTheta > _costheta_min && cosTheta < _costheta_max){
	G4double sinTheta2 = 1. - cosTheta*cosTheta;
	if( sinTheta2 < 0.)  sinTheta2 = 0.;
	G4double sinTheta  = std::sqrt(sinTheta2);
	G4double phi = twopi*G4UniformRand();
	  if (phi > _phi_min && phi < _phi_max){
	    mom_dir = true;
	    _momentum_direction = G4ThreeVector(sinTheta*std::cos(phi),
						sinTheta*std::sin(phi), cosTheta).unit();
	    px = pmod * _momentum_direction.x();
	    py = pmod * _momentum_direction.y();
	    pz = pmod * _momentum_direction.z();
	  }
      }
    }
  }

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
}



G4double SingleParticle::RandomEnergy() const
{
  if (_energy_max == _energy_min)
    return _energy_min;
  else
    return (G4UniformRand()*(_energy_max - _energy_min) + _energy_min);
}
