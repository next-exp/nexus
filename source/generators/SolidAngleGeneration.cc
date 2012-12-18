// ----------------------------------------------------------------------------
// 
//  Author : P Ferrario <paolafer@ific.uv.es>    
//  Created: 17 Dec 2012
//
//  Copyright (c) 2012 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SolidAngleGeneration.h"

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

#include <math.h>



namespace nexus {

   SolidAngleGeneration::SolidAngleGeneration():
    _energy_min(0.), _energy_max(0.),
    _geom(0)
  {
    DetectorConstruction* detconst = (DetectorConstruction*)
      G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    _geom = detconst->GetGeometry();

    ReadConfigParams();
  }

void SolidAngleGeneration::ReadConfigParams()
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

    // Set limits for the solid angle of generation
    if (cfg.PeekDParam("costheta_min")) {
      _costheta_min = cfg.GetDParam("costheta_min");
    } else {
      _costheta_min = -1.;
    }
    if (cfg.PeekDParam("costheta_max")) {
      _costheta_max = cfg.GetDParam("costheta_max");
    } else {
      _costheta_max = 1.;
    }
    if (cfg.PeekDParam("phi_min")) {
      _phi_min = cfg.GetDParam("phi_min");
    } else {
      _phi_min = 0.;
    }
    if (cfg.PeekDParam("phi_max")) {
      _phi_max = cfg.GetDParam("phi_max");
    } else {
      _phi_max = twopi;
    }

  }  

  
  void SolidAngleGeneration::GeneratePrimaryVertex(G4Event* event)
  {
    // Ask the geometry to generate a position for the particle
    G4ThreeVector position = _geom->GenerateVertex(_region);

    // Particle generated at start-of-event
    G4double time = 0.;
    
    // Create a new vertex
    G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);
    
    // Generate uniform random energy in [E_min, E_max]
    G4double kinetic_energy = RandomEnergy();

    G4bool mom_dir = false;
    G4ThreeVector momentum_direction;
     
    while (mom_dir == false) {
      
      G4double cosTheta  = 2.*G4UniformRand()-1.;
      G4double sinTheta2 = 1. - cosTheta*cosTheta;
      if( sinTheta2 < 0.)  sinTheta2 = 0.;
      G4double sinTheta  = std::sqrt(sinTheta2); 
      G4double phi       = twopi*G4UniformRand();
      
	
	  // 	G4double angle = std::atan(position.getY()/position.getX());
	  // 	if ((phi > 0 && phi < 2.094) || (phi > 5.24 && phi < 6.28)){	//sideport in avanti
	  // 	  //	  if (phi > 3.14){ // towards negative y
	  // 	//	  if (phi > angle + 3.14/2. && phi < angle + 3./2.*3.14){ // around a cylinder towards inside
	  // 	  // 	//  	if (phi > 0 && phi < 3.14){ //superior half 
	  // 	//	if (cosTheta > 0. && cosTheta < 1.){ // positive z
	  if (cosTheta > _costheta_min && cosTheta < _costheta_max) {
	    if (phi > _phi_min && phi < _phi_max) {
	    // 	//if (cosTheta > 0.9987523 && cosTheta < 1){ // positive z, Na22 from behind
	    // 	//if (cosTheta > 0.9987523 && cosTheta < 1){
	    // 	  //       	  // G4cout << "Angoli accettati" << G4endl;
	    // 	  //       	  // G4cout << "phi = " << phi << " and cosTheta =" << cosTheta << G4endl;
	    mom_dir = true;
	    momentum_direction = G4ThreeVector(sinTheta*std::cos(phi),
						sinTheta*std::sin(phi), cosTheta).unit();
	 
	    // 	  // break;
	    }
	  }
	}
 
      
    

      // Calculate cartesian components of momentum
      	G4double energy = kinetic_energy + _mass;
      	G4double pmod = std::sqrt(energy*energy - _mass*_mass);
      	G4double px = pmod * momentum_direction.x();
      	G4double py = pmod * momentum_direction.y();
      	G4double pz = pmod * momentum_direction.z();

	//	G4cout << _momentum_direction.x() << ", " << _momentum_direction.y() << ", " << _momentum_direction.z() << G4endl;

      // G4double px;
      // G4double py;
      // G4double pz;

      // while (mom_dir == false) {
      // 	_momentum_direction = G4RandomDirection();

      // 	// Calculate cartesian components of momentum
      // 	G4double energy = kinetic_energy + _mass;
      // 	G4double pmod = std::sqrt(energy*energy - _mass*_mass);
      // 	px = pmod * _momentum_direction.x();
      // 	py = pmod * _momentum_direction.y();
      // 	pz = pmod * _momentum_direction.z();
	
      // 	G4double prho = sqrt(pow(px,2)+pow(py,2)+pow(pz,2));
      // 	G4double ptheta = acos(pz/prho);
      // 	G4double pphi = atan(py/px);
      // 	if ((pphi > 4.5 && pphi < 7) && ( ptheta > 0.15 && ptheta < 1.78 ) ){
      // 	  mom_dir = true;
      // 	}
      // }

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
  
  
  
  G4double SolidAngleGeneration::RandomEnergy() const
  {
    if (_energy_max == _energy_min)
      return _energy_min;
    
    return (G4UniformRand() * (_energy_max - _energy_min) + _energy_min);
  }

}
