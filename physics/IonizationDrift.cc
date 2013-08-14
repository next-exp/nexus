// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 1 June 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "IonizationDrift.h"

#include "AlongStepRandomGenerator.h"

#include <G4Step.hh>
#include <G4StepPoint.hh>
#include <Randomize.hh>
#include <G4Poisson.hh>

// Random generators from CLHEP (slower but more precise)
//#include <CLHEP/Random/RandPoisson.h>
//#include <CLHEP/Random/RandGauss.h>


namespace nexus {


  IonizationDrift::IonizationDrift():
    _drift_velocity(0.), _transv_diff(0.), _longit_diff(0.), _attachment(0.),
    _ioniz_energy(0.), _fano_factor(0.), _rnd(new AlongStepRandomGenerator)
  {
  }
  
  
  
  IonizationDrift::IonizationDrift(G4double drift_velocity,
				   G4double transverse_diffusion,
				   G4double longitudinal_diffusion,
				   G4double attachment,
				   G4double ionization_energy,
				   G4double max_drift_length):
    _drift_velocity(drift_velocity), _transv_diff(transverse_diffusion),
    _longit_diff(longitudinal_diffusion), _attachment(attachment),
    _ioniz_energy(ionization_energy), _max_drift_length(max_drift_length),
    _rnd(new AlongStepRandomGenerator)
  {
  }
  
  

  IonizationDrift::~IonizationDrift()
  {
    delete _rnd;
  }
  
  
  
  void IonizationDrift::SetStep(const G4Step& step)
  {
    G4StepPoint* pre_point  = step.GetPreStepPoint();
    G4StepPoint* post_point = step.GetPostStepPoint();
    
    G4LorentzVector p0(pre_point->GetPosition(), pre_point->GetGlobalTime());
    G4LorentzVector p1(post_point->GetPosition(), post_point->GetGlobalTime());

    _rnd->SetPoints(p0, p1);

    G4double energy_dep = 
      step.GetTotalEnergyDeposit() - step.GetNonIonizingEnergyDeposit();

    G4double mean = energy_dep / _ioniz_energy;
    
    if (mean > 10.)
      _num_electrons = G4int(G4RandGauss::shoot(mean, sqrt(mean)) + 0.5);
    else
      _num_electrons = G4int(G4Poisson(mean));
  }
  
  
  
  G4LorentzVector IonizationDrift::GenerateOne()
  {
    // Generate a random point along the step as time-space origin
    // of the drifting electron.
    G4LorentzVector origin = _rnd->Shoot();
    G4double drift_length = _max_drift_length/2. - origin.z();
    G4double drift_time = drift_length / _drift_velocity;

    // Drift lines are parallel to the z-axis. Electrons deviate from
    // that straight path due to diffusion, that can be modeled as a
    // 3-dimensional gaussian spread.

    // Calculate standard deviations in transverse and longitudinal directions
    G4double transv_sigma = _transv_diff * sqrt(drift_length);
    G4double longit_sigma = _longit_diff * sqrt(drift_length);
    
    G4double x = G4RandGauss::shoot(origin.x(), transv_sigma);
    G4double y = G4RandGauss::shoot(origin.y(), transv_sigma);
    G4double z = _max_drift_length/2.;
    
    G4double deltat = G4RandGauss::shoot(0., longit_sigma) / _drift_velocity;
    G4double t = origin.t() + drift_time + deltat;
    
    return G4LorentzVector(x, y, z, t);
  }
  
  
} // end namespace nexus
