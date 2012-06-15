// ----------------------------------------------------------------------------
//
//
//  Author : P. Ferrario <paola.ferrario@ific.uv.es>    
//  Created: 6 Dic 2011
//
//  Copyright (c) 2011 NEXT Collaboration
// ----------------------------------------------------------------------------
#include "Na22Generation.h"

#include "G4Event.hh"
#include "ConfigService.h"
#include "DetectorConstruction.h"
#include "BaseGeometry.h"
#include "BhepUtils.h"

#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>

namespace nexus {

  Na22Generation::Na22Generation() : _energy_min(0.), _energy_max(0.), 
				     _direction_is_random(true), _geom(0),
				     _direction_dis_is_random(true)

  {
    DetectorConstruction* detconst = (DetectorConstruction*)
      G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    _geom = detconst->GetGeometry();

    ReadConfigParams();
  }

  Na22Generation::~Na22Generation()
  {
  }

   void Na22Generation::ReadConfigParams()
  {
    // Get configuration parameters for generation
    const ParamStore& cfg = ConfigService::Instance().Generation();
    
    // Set particle type searching in particle table by name
    _particle_definition = G4ParticleTable::GetParticleTable()->
      FindParticle(cfg.GetSParam("particle_name"));
    _particle_dis_definition = G4ParticleTable::GetParticleTable()->
      FindParticle(cfg.GetSParam("particle_dis_name"));

    
    // Set limits for kinetic energies
    _energy_min = cfg.GetDParam("energy_min");
    _energy_max = cfg.GetDParam("energy_max");
    _energy_min_dis = cfg.GetDParam("energy_min_dis");
    _energy_max_dis = cfg.GetDParam("energy_max_dis");

    // Set momentum direction if specified;
    if (cfg.PeekDVParam("momentum_direction")) {
      _direction_is_random = false;
      _momentum_direction = 
	BhepUtils::DVto3Vector(cfg.GetDVParam("momentum_direction"));
      // normalize the vector if needed
      _momentum_direction = _momentum_direction.unit();
    }

      // Set momentum direction if specified; 
    if (cfg.PeekDVParam("momentum_direction_dis")) {
      _direction_dis_is_random = false;
      _momentum_direction_dis = 
	BhepUtils::DVto3Vector(cfg.GetDVParam("momentum_direction_dis"));
      // normalize the vector if needed
      _momentum_direction_dis = _momentum_direction_dis.unit();
    }

    // Specify the fraction of solid angle that is chosen
    if (cfg.PeekSParam("solid_angle")){
      _solid_angle =  cfg.GetSParam("solid_angle");
    }
    if (cfg.PeekSParam("solid_angle_dis")){
      _solid_angle_dis =  cfg.GetSParam("solid_angle_dis");
    }


    _region = cfg.GetSParam("region");
  }

  void Na22Generation::GeneratePrimaryVertex(G4Event* evt)
  {
    // Ask the geometry to generate a position for the particle
    G4ThreeVector position = _geom->GenerateVertex(_region);

    // Generate uniform random energy in [E_min, E_max]
    G4double kinetic_energy = 0;
    if (_energy_max == _energy_min){
     kinetic_energy = _energy_min;
    } else {
      kinetic_energy = G4UniformRand() * (_energy_max - _energy_min) + _energy_min;
    }
    // Generate uniform random energy in [E_min, E_max] for disexcitation gamma
    G4double kinetic_energy_dis = 0;
    if (_energy_max_dis == _energy_min_dis){
     kinetic_energy_dis = _energy_min_dis;
    } else {
      kinetic_energy_dis = G4UniformRand() * (_energy_max_dis - _energy_min_dis) + _energy_min_dis;
    }
   
    // Generate random direction
    if (_direction_is_random){
       //// To generate uniformly in 4*pi
      if (_solid_angle == "4Pi"){
	_momentum_direction = G4RandomDirection();
      } else {
	///To generate just in a part of the solid angle
	G4bool mom_dir = false;
	while (mom_dir == false) {
	  G4double cosTheta  = 2.*G4UniformRand()-1.;
	  G4double sinTheta2 = 1. - cosTheta*cosTheta;
	  if( sinTheta2 < 0.)  sinTheta2 = 0.;
	  G4double sinTheta  = std::sqrt(sinTheta2); 
	  G4double phi       = twopi*G4UniformRand();
	  if (_solid_angle == "2Piforward"){
	    if ((phi > 0 && phi < 2.094) || (phi > 5.24 && phi < 6.28)){    //forward from sideport
	      mom_dir = true;
	      _momentum_direction = G4ThreeVector(sinTheta*std::cos(phi),
						  sinTheta*std::sin(phi), cosTheta).unit();
	    }
	  }
	}
      }
    }
    
  
    // Generate random direction for disexcitation gamma
    if (_direction_dis_is_random){
       //// To generate uniformly in 4*Pi
      if (_solid_angle_dis == "4Pi"){
      _momentum_direction_dis = G4RandomDirection();
      } else {
	///To generate just in a part of the solid angle
	G4bool mom_dir = false;
	while (mom_dir == false) {
	  G4double cosTheta  = 2.*G4UniformRand()-1.;
	  G4double sinTheta2 = 1. - cosTheta*cosTheta;
	  if( sinTheta2 < 0.)  sinTheta2 = 0.;
	  G4double sinTheta  = std::sqrt(sinTheta2); 
	  G4double phi       = twopi*G4UniformRand();
	  if (_solid_angle_dis == "2Piforward"){
	    if ((phi > 0 && phi < 2.094) || (phi > 5.24 && phi < 6.28)){    //forward from sideport
	      mom_dir = true;
	      _momentum_direction_dis = G4ThreeVector(sinTheta*std::cos(phi),
						      sinTheta*std::sin(phi), cosTheta).unit();
	    }
	  }
	}
      }
    }
    
    // Set masses to PDG values
    G4double mass = _particle_definition->GetPDGMass();
    G4double mass_dis = _particle_dis_definition->GetPDGMass();
    // Set charges to PDG value
    G4double charge = _particle_definition->GetPDGCharge();
    G4double charge_dis = _particle_dis_definition->GetPDGCharge();

    // Calculate cartesian components of momentum
    G4double energy = kinetic_energy + mass;
    G4double pmod = std::sqrt(energy*energy - mass*mass);
    G4double px = pmod * _momentum_direction.x();
    G4double py = pmod * _momentum_direction.y();
    G4double pz = pmod * _momentum_direction.z();

    // Calculate cartesian components of momentum of disexcitation gamma
    G4double energy_dis = kinetic_energy_dis + mass;
    G4double pmod_dis = std::sqrt(energy_dis*energy_dis - mass*mass);
    G4double px_dis = pmod_dis * _momentum_direction_dis.x();
    G4double py_dis = pmod_dis * _momentum_direction_dis.y();
    G4double pz_dis = pmod_dis * _momentum_direction_dis.z();

    // create a new vertex

    // Set starting time of generation
    G4double time = 0;

    G4PrimaryVertex* vertex = 
      new G4PrimaryVertex(position, time);

    // create new primaries and set them to the vertex
    
    G4PrimaryParticle* particle1 =
      new G4PrimaryParticle(_particle_definition);
    particle1->SetMass(mass);
    particle1->SetMomentum(px, py, pz);
    particle1->SetCharge(charge);
    particle1->SetPolarization(0.,0.,0.);
    vertex->SetPrimary(particle1);

    G4PrimaryParticle* particle2 =
      new G4PrimaryParticle(_particle_definition);
    particle2->SetMass(mass);
    particle2->SetMomentum(-px, -py, -pz);
    particle2->SetCharge(charge);
    particle2->SetPolarization(0.,0.,0.);
    //   vertex->SetPrimary(particle2);

     G4PrimaryParticle* gamma_dis =
      new G4PrimaryParticle(_particle_dis_definition);
    gamma_dis->SetMass(mass_dis);
    gamma_dis->SetMomentum(px_dis, py_dis, pz_dis);
    gamma_dis->SetCharge(charge_dis);
    gamma_dis->SetPolarization(0.,0.,0.);
    vertex->SetPrimary(gamma_dis);


    evt->AddPrimaryVertex(vertex);
  }

}
