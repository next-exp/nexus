// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 10 May 2010
//  
//  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
// ---------------------------------------------------------------------------- 

#include "IonizationClustering.h"

#include "BaseDriftField.h"
#include "IonizationElectron.h"
#include "DriftTrackInfo.h"
#include "SegmentPointSampler.h"

#include <G4ParticleDefinition.hh>
#include <G4OpticalPhoton.hh>
#include <G4Gamma.hh>
#include <G4Poisson.hh>
#include <Randomize.hh>
#include <G4LorentzVector.hh>


namespace nexus {

  
  IonizationClustering::IonizationClustering(const G4String& process_name, 
					     G4ProcessType type):
    G4VRestDiscreteProcess(process_name, type), 
    _rnd(new SegmentPointSampler())
  {
    pParticleChange = &_ParticleChange;
  }
  
  
  
  IonizationClustering::~IonizationClustering() 
  {
  }
  
  
  
  G4bool IonizationClustering::IsApplicable(const G4ParticleDefinition& pdef)
  {
    if (pdef == *IonizationElectron::Definition() ||
	pdef == *G4OpticalPhoton::Definition())
      return false;
    else
      return true;
  }
  
  
  
  G4VParticleChange*
  IonizationClustering::AtRestDoIt(const G4Track& track, const G4Step& step)
  {
    // The method simply calls the equivalent PostStepDoIt, 
    // proceeding as in any other step.
    return IonizationClustering::PostStepDoIt(track, step);
  }
  
  
  
  G4VParticleChange*
  IonizationClustering::PostStepDoIt(const G4Track& track, const G4Step& step)
  {
    // Initialize particle change with current track values
    _ParticleChange.Initialize(track);

    // The clustering process makes sense only for those regions with
    // a drift field defined. Therefore, check whether the current region
    // has a drift field attached. Stop the process if that is not the case.
    G4Region* region = track.GetVolume()->GetLogicalVolume()->GetRegion();
    BaseDriftField* field = dynamic_cast<BaseDriftField*>
      (region->GetUserInformation());
    if (!field) 
      return G4VRestDiscreteProcess::PostStepDoIt(track, step);
    
    // Get energy deposited through ionization
    G4double energy_dep = 
      step.GetTotalEnergyDeposit() - step.GetNonIonizingEnergyDeposit();
    
    // If no energy was deposited in the last step, stop the process
    if (energy_dep <= 0)
      return G4VRestDiscreteProcess::PostStepDoIt(track, step);
    
    // Get material properties relevant for the process
    // G4MaterialPropertiesTable* mpt = 
    //   track.GetMaterial()->GetMaterialPropertiesTable();
    // if (!mpt)
    //   return G4VRestDiscreteProcess::PostStepDoIt(track, step);
    
    //G4double ioni_energy = mpt->GetConstProperty("IONIZATIONENERGY");
    //G4double fano_factor = mpt->GetConstProperty("FANOFACTOR");
    G4double ioni_energy = 22.4 * eV;
    G4double fano_factor = 1.;
    
    G4double mean = energy_dep / ioni_energy;

    G4int num_charges = 0;
    
    if (mean > 10.) {
      G4double sigma = sqrt(mean*fano_factor);
      num_charges = G4int(G4RandGauss::shoot(mean, sigma) + 0.5);
    }
    else {
      num_charges = G4int(G4Poisson(mean));
    }
    
    
    // Generate the ionization electrons.
    // N.B.- A kinetic energy and momentum direction must be defined for
    // the ionization electron or Geant4 will throw an exception.
    // However, these are dummy values that won't be used anywhere
    // because the transportation process is switched off for this 
    // type of particles.

    _ParticleChange.SetNumberOfSecondaries(num_charges);

    G4double kinetic_energy = 5.*eV;
    G4ThreeVector momentum_direction(0.,0.,1.);

    // Set pre and post points of the step in the random generator
    G4LorentzVector prestep (step.GetPreStepPoint()->GetPosition(),
			     step.GetPreStepPoint()->GetGlobalTime());
    G4LorentzVector poststep(step.GetPostStepPoint()->GetPosition(),
			     step.GetPostStepPoint()->GetGlobalTime());
    _rnd->SetPoints(prestep, poststep);

    
    for (G4int i=0; i<num_charges; i++) {

      G4DynamicParticle* ionielectron =
	new G4DynamicParticle(IonizationElectron::Definition(),
			      momentum_direction, kinetic_energy);

      // Calculate position and time. We distribute the charges along
      // the step for all particles but gammas, where we use the post-step point
      G4LorentzVector point;
      
      if (track.GetDefinition() == G4Gamma::Definition()) 
	point = poststep;
      else
	point = _rnd->Shoot();

      // Create new track associated to the ionization electron
      G4Track* aSecondaryTrack = new G4Track(ionielectron, point.t(), point.v());
      aSecondaryTrack->SetTouchableHandle(step.GetPreStepPoint()->GetTouchableHandle());
      aSecondaryTrack->SetUserInformation(new DriftTrackInfo);
      _ParticleChange.AddSecondary(aSecondaryTrack);
    }

    return G4VRestDiscreteProcess::PostStepDoIt(track, step);
  }
  
  
  
  G4double IonizationClustering::GetMeanFreePath(const G4Track&, G4double, 
						 G4ForceCondition* condition)
  {
    *condition = StronglyForced;
    return DBL_MAX;
  }
  
  
  
  G4double IonizationClustering::GetMeanLifeTime(const G4Track&,
						 G4ForceCondition* condition)
  {
    *condition = Forced;
    return DBL_MAX;
  }
  

} // end namespace nexus
