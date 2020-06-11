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
#include "DetectorConstruction.h"
#include "BaseGeometry.h"

#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

namespace nexus {

  using namespace CLHEP;

 Na22Generation::Na22Generation() : geom_(0) 
  {
    /// For the moment, only random direction are allowed. To be fixes if needed
     msg_ = new G4GenericMessenger(this, "/Generator/Na22Generator/",
    "Control commands of Na22 generator.");

     msg_->DeclareProperty("region", region_, 
			   "Set the region of the geometry where the vertex will be generated.");

    DetectorConstruction* detconst = (DetectorConstruction*)
      G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    geom_ = detconst->GetGeometry();
  }

  Na22Generation::~Na22Generation()
  {
  }

  void Na22Generation::GeneratePrimaryVertex(G4Event* evt)
  {
    // Ask the geometry to generate a position for the particle

    G4ThreeVector position = geom_->GenerateVertex(region_); 
    G4double time = 0.;
    G4PrimaryVertex* vertex = 
        new G4PrimaryVertex(position, time);

    G4ParticleDefinition* particle_definition = 
      G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    // Set masses to PDG values
    G4double mass = particle_definition->GetPDGMass();
    // Set charges to PDG value
    G4double charge = particle_definition->GetPDGCharge();

    G4double rand = G4UniformRand();

    if (rand < 0.903) {

    G4ThreeVector momentum_direction = G4RandomDirection();     

    // Calculate cartesian components of momentum
    G4double energy = 510.999*keV + mass;
    G4double pmod = std::sqrt(energy*energy - mass*mass);
    G4double px = pmod * momentum_direction.x();
    G4double py = pmod * momentum_direction.y();
    G4double pz = pmod * momentum_direction.z();
   
     G4PrimaryParticle* particle1 =
       new G4PrimaryParticle(particle_definition, px, py, pz);
    particle1->SetMass(mass);
    particle1->SetCharge(charge);
    particle1->SetPolarization(0.,0.,0.);
    vertex->SetPrimary(particle1);

    G4PrimaryParticle* particle2 =
      new G4PrimaryParticle(particle_definition, -px, -py, -pz);
    particle2->SetMass(mass);
    particle2->SetCharge(charge);
    particle2->SetPolarization(0.,0.,0.);
    vertex->SetPrimary(particle2);

 }

    G4ThreeVector momentum_direction_dis = G4RandomDirection();

    // Calculate cartesian components of momentum of disexcitation gamma
    G4double energy_dis = 1274.537*keV + mass;
    G4double pmod_dis = std::sqrt(energy_dis*energy_dis - mass*mass);
    G4double px_dis = pmod_dis * momentum_direction_dis.x();
    G4double py_dis = pmod_dis * momentum_direction_dis.y();
    G4double pz_dis = pmod_dis * momentum_direction_dis.z();
  

     G4PrimaryParticle* gamma_dis =
       new G4PrimaryParticle(particle_definition, px_dis, py_dis, pz_dis);
    gamma_dis->SetMass(mass);
    gamma_dis->SetCharge(charge);
    gamma_dis->SetPolarization(0.,0.,0.);
    vertex->SetPrimary(gamma_dis);


    evt->AddPrimaryVertex(vertex);
  }

}
