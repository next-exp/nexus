// ----------------------------------------------------------------------------
//
//
//  Author : P. Ferrario <paola.ferrario@ific.uv.es>    
//  Created: 6 Dic 2011
//
//  Copyright (c) 2011 NEXT Collaboration
// ----------------------------------------------------------------------------
#include "Back2backGammas.h"

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

#include <TFile.h>
#include <TH1F.h>

namespace nexus {

  using namespace CLHEP;

  Back2backGammas::Back2backGammas() : _geom(0), _costheta_min(-1.), _costheta_max(1.),
				       _phi_min(0.), _phi_max(2.*pi)
  {
    G4cout << "Limits = " << std::numeric_limits<unsigned int>::max() << G4endl;
    /// For the moment, only random direction are allowed. To be fixes if needed
     _msg = new G4GenericMessenger(this, "/Generator/Back2back/",
    "Control commands of 511-keV back to back gammas generator.");

     _msg->DeclareProperty("region", _region,
			   "Set the region of the geometry where the vertex will be generated.");

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

    theta_angle_ = new TH1F("CosTheta", "CosTheta", 180, -2, 2);
    theta_angle_->GetXaxis()->SetTitle("Cos(theta)");
    theta_angle_->GetYaxis()->SetTitle("Entries");

    phi_angle_ = new TH1F("Phi", "Phi", 360, -pi-2, pi+2);
    phi_angle_->GetXaxis()->SetTitle("Phi (rad)");
    phi_angle_->GetYaxis()->SetTitle("Entries");
  }

  Back2backGammas::~Back2backGammas()
  {
    out_file_ = new TFile("GenerationAngles.root", "recreate");
    theta_angle_->Write();
    phi_angle_->Write();
    out_file_->Close();
  }

  void Back2backGammas::GeneratePrimaryVertex(G4Event* evt)
  {
    // Ask the geometry to generate a position for the particle

    G4ThreeVector position = _geom->GenerateVertex(_region);
    G4double time = 0.;
    G4PrimaryVertex* vertex =
        new G4PrimaryVertex(position, time);

    G4ParticleDefinition* particle_definition =
      G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    // Set masses to PDG values
    G4double mass = particle_definition->GetPDGMass();
    // Set charges to PDG value
    G4double charge = particle_definition->GetPDGCharge();

    G4ThreeVector momentum_direction = G4RandomDirection();

    // Calculate cartesian components of momentum
    G4double energy = 510.999*keV + mass;
    G4double pmod = std::sqrt(energy*energy - mass*mass);
    G4double px = pmod * momentum_direction.x();
    G4double py = pmod * momentum_direction.y();
    G4double pz = pmod * momentum_direction.z();

    if (_costheta_min != -1. || _costheta_max != 1. || _phi_min != 0. || _phi_max != 2.*pi) {
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
	    momentum_direction = G4ThreeVector(sinTheta*std::cos(phi),
					       sinTheta*std::sin(phi), cosTheta).unit();
	    px = pmod * momentum_direction.x();
	    py = pmod * momentum_direction.y();
	    pz = pmod * momentum_direction.z();
	  }
	}
      }
    }

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

    evt->AddPrimaryVertex(vertex);

    theta_angle_->Fill(momentum_direction.z());
    phi_angle_->Fill(std::atan2(momentum_direction.y(), momentum_direction.x()));
  }

}
