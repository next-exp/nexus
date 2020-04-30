// ----------------------------------------------------------------------------
//  \file   MuonGenerator.cc
//  \brief  Point Sampler on the surface of a rectangular used for muon generation.
//          Control plots of the generation variables available.
//
//  Author: Neus Lopez March <neus.lopez@ific.uv.es>
//
//  Created: 30 Jan 2015
//
//  Copyright (c) 2015 NEXT Collaboration
// ----------------------------------------------------------------------------
#include "MuonGenerator.h"
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
#include "MuonsPointSampler.h"
#include "AddUserInfoToPV.h"

#include "TF1.h"

#include <TMath.h>
#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;
using namespace CLHEP;


MuonGenerator::MuonGenerator():
  G4VPrimaryGenerator(), _msg(0), _particle_definition(0),
  _energy_min(0.), _energy_max(0.), _geom(0), _momentum_X(0.),
  _momentum_Y(0.), _momentum_Z(0.)
{
  _msg = new G4GenericMessenger(this, "/Generator/MuonGenerator/",
				"Control commands of muongenerator.");

  G4GenericMessenger::Command& min_energy =
    _msg->DeclareProperty("min_energy", _energy_min, "Set minimum kinetic energy of the particle.");
  min_energy.SetUnitCategory("Energy");
  min_energy.SetParameterName("min_energy", false);
  min_energy.SetRange("min_energy>0.");

  G4GenericMessenger::Command& max_energy =
    _msg->DeclareProperty("max_energy", _energy_max, "Set maximum kinetic energy of the particle");
  max_energy.SetUnitCategory("Energy");
  max_energy.SetParameterName("max_energy", false);
  max_energy.SetRange("max_energy>0.");

  _msg->DeclareProperty("region", _region,
			"Set the region of the geometry where the vertex will be generated.");

  _msg->DeclareProperty("momentum_X", _momentum_X,"x coord of momentum");
  _msg->DeclareProperty("momentum_Y", _momentum_Y,"y coord of momentum");
  _msg->DeclareProperty("momentum_Z", _momentum_Z,"z coord of momentum");


  DetectorConstruction* detconst = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  _geom = detconst->GetGeometry();

}



MuonGenerator::~MuonGenerator()
{

  delete _msg;
}

void MuonGenerator::GeneratePrimaryVertex(G4Event* event)
{
  _particle_definition = G4ParticleTable::GetParticleTable()->FindParticle(MuonCharge());
  if (!_particle_definition)
    G4Exception("SetParticleDefinition()", "[MuonGenerator]",
                FatalException, " can not create a muon ");

  // Generate an initial position for the particle using the geometry
  G4ThreeVector position = _geom->GenerateVertex(_region);
  // Particle generated at start-of-event
  G4double time = 0.;
  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);
  // Generate uniform random energy in [E_min, E_max]
  G4double kinetic_energy = RandomEnergy();

  // Particle propierties
  G4double mass   = _particle_definition->GetPDGMass();
  G4double energy = kinetic_energy + mass;
  G4double pmod = std::sqrt(energy*energy - mass*mass);

  // Generate momentum direction in spherical coordinates
  G4double theta = GetTheta();
  G4double phi   = GetPhi();

  G4double x, y, z;

  // NEXT axis convention (z<->y) and generate with -y! towards the detector
  x = sin(theta) * cos(phi);
  y = -cos(theta);
  z = sin(theta) * sin(phi);

  G4ThreeVector _p_dir(x,y,z);

  G4double px = pmod * _p_dir.x();
  G4double py = pmod * _p_dir.y();
  G4double pz = pmod * _p_dir.z();


  // If user provides a momentum direction, this one is used
  if (_momentum_X != 0. || _momentum_Y != 0. || _momentum_Z != 0.) {
    // Normalize if needed
    G4double mom_mod = std::sqrt(_momentum_X * _momentum_X +
				 _momentum_Y * _momentum_Y +
				 _momentum_Z * _momentum_Z);
    px = pmod * _momentum_X/mom_mod;
    py = pmod * _momentum_Y/mom_mod;
    pz = pmod * _momentum_Z/mom_mod;
  }

  // Create the new primary particle and set it some properties
  G4PrimaryParticle* particle =
    new G4PrimaryParticle(_particle_definition, px, py, pz);

  // Add info to PrimaryVertex to be accessed from EventAction type class to make histos of variables generated here.
  AddUserInfoToPV *info = new AddUserInfoToPV(theta, phi);

  vertex->SetUserInformation(info);

  // Add particle to the vertex and this to the event
  vertex->SetPrimary(particle);
  event->AddPrimaryVertex(vertex);

}

G4double MuonGenerator::RandomEnergy() const
{
  if (_energy_max == _energy_min)
    return _energy_min;
  else
    return (G4UniformRand()*(_energy_max - _energy_min) + _energy_min);
}

G4String MuonGenerator::MuonCharge() const
{
  G4double rndCh = 2.3 *G4UniformRand(); //From PDG cosmic muons  mu+/mu- = 1.3
  if (rndCh <1.3)
    return "mu+";
  else
    return "mu-";
}


G4double MuonGenerator::GetTheta() const
{
  TF1 *f1 = new TF1("f1","pow(cos(x),2)",0,pi/2);
  G4double theta = f1->GetRandom();
  return ( theta );
}


G4double MuonGenerator::GetPhi() const
{
  return ( twopi*G4UniformRand());
}
