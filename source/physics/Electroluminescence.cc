// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 28 Oct 2009
//  
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Electroluminescence.h"

#include "IonizationElectron.h"
#include "DriftTrackInfo.h"
#include "BaseDriftField.h"

#include <G4TransportationManager.hh>
#include <G4OpticalPhoton.hh>
#include <Randomize.hh>
#include <G4Poisson.hh>



namespace nexus {

  
  Electroluminescence::Electroluminescence(const G4String& process_name,
					   G4ProcessType type):
    G4VDiscreteProcess(process_name, type), _secondaries_first(true)
  {
    pParticleChange = &_ParticleChange;
  }
  
  
  
  Electroluminescence::~Electroluminescence()
  {
  }
  
  
  
  G4bool Electroluminescence::IsApplicable(const G4ParticleDefinition& pdef)
  {
    return (pdef == *IonizationElectron::Definition());
  }



  G4VParticleChange*
  Electroluminescence::PostStepDoIt(const G4Track& track, const G4Step& step)
  {
    // Initialize particle change with current track values
    _ParticleChange.Initialize(track);
    
    // Get the current region and its associated drift field
    G4Region* region = track.GetVolume()->GetLogicalVolume()->GetRegion();
    BaseDriftField* field =
      dynamic_cast<BaseDriftField*>(region->GetUserInformation());

    // If no drift field is defined (i.e., the dynamic_cast returned a null
    // pointer), kill the track and finish the process.
    if (!field) {
      _ParticleChange.ProposeTrackStatus(fStopAndKill);
      return G4VDiscreteProcess::PostStepDoIt(track, step);
    }
    
    // Get the average light yield for the last step from the 
    // drift track info
    DriftTrackInfo* drift_info = 
      dynamic_cast<DriftTrackInfo*>(track.GetUserInformation());
    
    if (!drift_info) {
      G4cout << "[Electroluminescence] WARNING: "
	     << "No DriftTrackInfo attached to the ionization electron. "
	     << "Therefore, EL light emission cannot be simulated.\n" 
	     << G4endl;
      return G4VDiscreteProcess::PostStepDoIt(track, step);
    }

    G4double yield = drift_info->GetELLightYield();

    // Generate a random number of photons around mean 'yield'
    G4int num_photons;
        
    if (yield < 10.) { // Poissonian regime
      num_photons = G4int(G4Poisson(yield));
    }
    else {             // Gaussian regime            
      G4double sigma = sqrt(yield);
      num_photons = G4int(G4RandGauss::shoot(yield, sigma) + 0.5);
    }
    
    _ParticleChange.SetNumberOfSecondaries(num_photons);
    
    if (_secondaries_first) {
      if (track.GetTrackStatus() == fAlive)
	aParticleChange.ProposeTrackStatus(fSuspend);
    }

    G4ThreeVector position = step.GetPreStepPoint()->GetPosition();
    G4double time = step.GetPreStepPoint()->GetGlobalTime();
    G4LorentzVector initial_position(position, time);

    for (G4int i=0; i<num_photons; i++) {
      
      // Generate a random direction for the photon 
      // (EL is supposed isotropic)
      
      G4double cos_theta = 1. - 2.*G4UniformRand();
      G4double sin_theta = sqrt((1.-cos_theta)*(1.+cos_theta));

      G4double phi = twopi * G4UniformRand();
      G4double sin_phi = sin(phi);
      G4double cos_phi = cos(phi);
      
      G4double px = sin_theta * cos_phi;
      G4double py = sin_theta * sin_phi;
      G4double pz = cos_theta;

      G4ThreeVector momentum(px, py, pz);

      // Determine photon polarization accordingly

      G4double sx = cos_theta * cos_phi;
      G4double sy = cos_theta * sin_phi; 
      G4double sz = -sin_theta;
      
      G4ThreeVector polarization(sx, sy, sz);
      
      G4ThreeVector perp = momentum.cross(polarization);
      
      phi = twopi * G4UniformRand();
      sin_phi = sin(phi);
      cos_phi = cos(phi);

      polarization = cos_phi * polarization + sin_phi * perp;
      polarization = polarization.unit();
      
      // Generate a new photon and set properties
      
      G4DynamicParticle* photon =
	new G4DynamicParticle(G4OpticalPhoton::Definition(), momentum);
      
      photon->SetPolarization(polarization.x(), 
			      polarization.y(), 
			      polarization.z());
      
      // FIXME. Energy should be sampled from integral (like it is
      // done in G4Scintillation)
      G4double kinetic_energy = 6. * eV;

      photon->SetKineticEnergy(kinetic_energy);
      
      // Generate a random position (and related time) along the
      // electron drift line

      G4LorentzVector xyzt = 
	field->GeneratePointAlongDriftLine(initial_position);

      // Create the track
      G4Track* secondary = new G4Track(photon, xyzt.t(), xyzt.v());
      secondary->SetParentID(track.GetTrackID());
      //secondary->
      //SetTouchableHandle(step.GetPreStepPoint()->GetTouchableHandle());

      _ParticleChange.AddSecondary(secondary);
    }
    
    return G4VDiscreteProcess::PostStepDoIt(track, step);
  }
  
  
  
  G4double Electroluminescence::GetMeanFreePath
  (const G4Track&, G4double, G4ForceCondition* condition)
  {
    *condition = StronglyForced;
    return DBL_MAX;
  }
  

} // end namespace nexus
