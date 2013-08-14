// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 10 May 2010
//  
//  Copyright (c) 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "IonizationClustering.h"

#include "BaseDriftField.h"
#include "IonizationCluster.h"
#include "DriftTrackInfo.h"

#include <G4Poisson.hh>
#include <Randomize.hh>


namespace nexus {

  
  IonizationClustering::IonizationClustering(const G4String& process_name,
					     G4ProcessType type):
    G4VRestDiscreteProcess(process_name, type),
    _cluster_per_step(true)
  {
    pParticleChange = &_ParticleChange;
  }
  
  
  
  IonizationClustering::~IonizationClustering() 
  {
  }
  
  
  
  G4bool IonizationClustering::IsApplicable(const G4ParticleDefinition& pdef)
  {
    return (pdef.GetPDGCharge() != 0);
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
    // a drift field defined. Therefore, get the current region and
    // check whether it has a drift field attached. Stop the process
    // if that is not the case.
    G4Region* region = track.GetVolume()->GetLogicalVolume()->GetRegion();
    BaseDriftField* field =
      dynamic_cast<BaseDriftField*>(region->GetUserInformation());

    if (!field)
      return G4VRestDiscreteProcess::PostStepDoIt(track, step);
    
    // Get energy deposited through ionization
    G4double energy_dep = 
      step.GetTotalEnergyDeposit() - step.GetNonIonizingEnergyDeposit();
    
    if (energy_dep <= 0)
      return G4VRestDiscreteProcess::PostStepDoIt(track, step);
    
    // // Get material properties
    // const G4Material* material = track.GetMaterial();
    // G4MaterialPropertiesTable* mpt = material->GetMaterialPropertiesTable();

    // if (!mpt)
    //   return G4VRestDiscreteProcess::PostStepDoIt(track, step);

    // // Calculate number of charges produced in the ionization process
    // G4double ion_potential = mpt->GetConstProperty("IONIZATION_POTENTIAL");
    // G4double fano_factor = mpt->GetConstProperty("FANO_FACTOR");

    G4double ion_potential = 22. * eV;
    G4double fano_factor = 1.;

    G4double mean = energy_dep / ion_potential;
    G4int num_charges = 0;
    
    if (mean < 10.) {
      num_charges = G4int(G4Poisson(mean));
    }
    else {
      G4double sigma = sqrt(mean*fano_factor);
      num_charges = G4int(G4RandGauss::shoot(mean, sigma) + 0.5);
    }
    
    // Create a new ionization cluster. 
    // A momentum direction and kinetic energy must be defined for
    // the ionization cluster or Geant4 will throw an exception.
    // However, these are dummy values that won't be used anywhere
    // because the transportation process is (should be) switched off 
    // for this type of particles.
    
    G4ThreeVector momentum_direction(0.,0.,1.);
    G4double kinetic_energy = 10.*eV;
    
    G4DynamicParticle* cluster =  
      new G4DynamicParticle(IonizationCluster::Definition(), 
			    momentum_direction, kinetic_energy);
    
    // Calculate position and time 
    // (FIXME. For now we'll place the ionization cluster in the 
    // middle of the step segment. Other possibilities may be
    // implemented in the future if needed.)
    
    G4StepPoint* pre_point = step.GetPreStepPoint();

    G4ThreeVector position = 
      step.GetPreStepPoint()->GetPosition() + 0.5 * step.GetDeltaPosition();
    
    G4double time = step.GetPreStepPoint()->GetGlobalTime();

    // Create new track associated to the cluster
    G4Track* aSecondaryTrack = new G4Track(cluster, time, position);
    aSecondaryTrack->SetTouchableHandle(pre_point->GetTouchableHandle());
    
    DriftTrackInfo* drift_info = new DriftTrackInfo();
    drift_info->SetNumberOfCharges(num_charges);
    aSecondaryTrack->SetUserInformation(drift_info);

    _ParticleChange.SetNumberOfSecondaries(1);
    _ParticleChange.AddSecondary(aSecondaryTrack);
  
  
    return G4VRestDiscreteProcess::PostStepDoIt(track, step);
  }


  G4double IonizationClustering::GetMeanFreePath(const G4Track&,
						 G4double,
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
