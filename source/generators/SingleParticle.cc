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
G4VPrimaryGenerator(), msg_(0), particle_definition_(0),
energy_min_(0.), energy_max_(0.), geom_(0), momentum_X_(0.),
momentum_Y_(0.), momentum_Z_(0.), costheta_min_(-1.), costheta_max_(1.),
phi_min_(0.), phi_max_(2.*pi)
{
  msg_ = new G4GenericMessenger(this, "/Generator/SingleParticle/",
    "Control commands of single-particle generator.");

  msg_->DeclareMethod("particle", &SingleParticle::SetParticleDefinition,
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
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  msg_->DeclareProperty("region", region_,
    "Set the region of the geometry where the vertex will be generated.");

  /// Temporary
  //G4GenericMessenger::Command&  momentum_X_cmd =
  msg_->DeclareProperty("momentum_X", momentum_X_,
			"x coord of momentum");
  //G4GenericMessenger::Command&  momentum_Y_cmd =
  msg_->DeclareProperty("momentum_Y", momentum_Y_,
			"y coord of momentum");
  //G4GenericMessenger::Command&  momentum_Z_cmd =
  msg_->DeclareProperty("momentum_Z", momentum_Z_,
			"z coord of momentum");

  msg_->DeclareProperty("min_costheta", costheta_min_,
			"Set minimum cosTheta for the direction of the particle.");
  msg_->DeclareProperty("max_costheta", costheta_max_,
			"Set maximum cosTheta for the direction of the particle.");
  msg_->DeclareProperty("min_phi", phi_min_,
			"Set minimum phi for the direction of the particle.");
  msg_->DeclareProperty("max_phi", phi_max_,
			"Set maximum phi for the direction of the particle.");



  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();
}



SingleParticle::~SingleParticle()
{
  delete msg_;
}



void SingleParticle::SetParticleDefinition(G4String particle_name)
{
  particle_definition_ =
    G4ParticleTable::GetParticleTable()->FindParticle(particle_name);

  if (!particle_definition_)
    G4Exception("SetParticleDefinition()", "[SingleParticle]",
      FatalException, "User gave an unknown particle name.");
}



void SingleParticle::GeneratePrimaryVertex(G4Event* event)
{
  // Generate an initial position for the particle using the geometry
  G4ThreeVector position = geom_->GenerateVertex(region_);

  // Particle generated at start-of-event
  G4double time = 0.;

  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);

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

  // If user provides a momentum direction, this one is used
  if (momentum_X_ != 0. || momentum_Y_ != 0. || momentum_Z_ != 0.) {
    // Normalize if needed
    G4double mom_mod = std::sqrt(momentum_X_ * momentum_X_ +
				 momentum_Y_ * momentum_Y_ +
				 momentum_Z_ * momentum_Z_);
    px = pmod * momentum_X_/mom_mod;
    py = pmod * momentum_Y_/mom_mod;
    pz = pmod * momentum_Z_/mom_mod;
  } else if (costheta_min_ != -1. || costheta_max_ != 1. || phi_min_ != 0. || phi_max_ !=2.*pi) {
    G4bool mom_dir = false;
    while (mom_dir == false) {
      G4double cosTheta  = 2.*G4UniformRand()-1.;
      if (cosTheta > costheta_min_ && cosTheta < costheta_max_){
	G4double sinTheta2 = 1. - cosTheta*cosTheta;
	if( sinTheta2 < 0.)  sinTheta2 = 0.;
	G4double sinTheta  = std::sqrt(sinTheta2);
	G4double phi = twopi*G4UniformRand();
	  if (phi > phi_min_ && phi < phi_max_){
	    mom_dir = true;
	    momentum_direction_ = G4ThreeVector(sinTheta*std::cos(phi),
						sinTheta*std::sin(phi), cosTheta).unit();
	    px = pmod * momentum_direction_.x();
	    py = pmod * momentum_direction_.y();
	    pz = pmod * momentum_direction_.z();
	  }
      }
    }
  }

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
}



G4double SingleParticle::RandomEnergy() const
{
  if (energy_max_ == energy_min_)
    return energy_min_;
  else
    return (G4UniformRand()*(energy_max_ - energy_min_) + energy_min_);
}
