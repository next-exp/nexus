// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 1 June 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "IonizationDrift.h"

#include "BaseDriftField.h"
#include "IonizationCluster.h"
#include "IonizationElectron.h"
#include "DriftTrackInfo.h"

#include <G4ParticleChange.hh>
#include <G4RegionStore.hh>
#include <G4TransportationManager.hh>
#include <G4TouchableHandle.hh>
#include <G4Navigator.hh>


namespace nexus {


  IonizationDrift::IonizationDrift(const G4String& name, G4ProcessType type):
    G4VContinuousDiscreteProcess(name, type), _shoot_charges(false)
  {
    _nav = G4TransportationManager::GetTransportationManager()->
      GetNavigatorForTracking();
    
    pParticleChange = &_ParticleChange;
  }
  
  
  
  IonizationDrift::~IonizationDrift()
  {
  }
  


  G4bool IonizationDrift::IsApplicable(const G4ParticleDefinition& pdef)
  {
    return ((pdef == *IonizationCluster::Definition()) || 
	    (pdef == *IonizationElectron::Definition())); 
  }
  
  
  
  G4double IonizationDrift::GetContinuousStepLimit
  (const G4Track& track, G4double, G4double, G4double&)
  {
    G4double step_length;
    
    // Get current region
    G4Region* region = track.GetVolume()->GetLogicalVolume()->GetRegion();
    
    // Get the drift field attached to this region.
    // (The dynamic_cast ensures at runtime that the type of the user info
    // object is compatible; it returns a null pointer otherwise.)
    BaseDriftField* field = 
      dynamic_cast<BaseDriftField*>(region->GetUserInformation());
    
    // If the region has no field, the particle won't move and therefore
    // the step length is zero.
    if (!field) {
      step_length = 0.;
      return step_length;
    }

    // Get drift info object associated to track 
    DriftTrackInfo* drift_info =
      dynamic_cast<DriftTrackInfo*>(track.GetUserInformation());

    // Get displacement from current position due to drift field
    G4LorentzVector current_position(track.GetPosition(),
				     track.GetGlobalTime());
    
    _xyzt = field->Drift(current_position, drift_info);
    
    // Calculate step length as euclidean distance between initial
    // and final positions
    G4ThreeVector displacement = _xyzt.vect() - track.GetPosition();
    step_length = displacement.mag();
    
    return step_length;
  }
  
  
  
  G4VParticleChange* 
  IonizationDrift::AlongStepDoIt(const G4Track& track, const G4Step& step)
  {
    // Initialize the particle-change (sets all its members equal to
    // the corresponding members in the track).
    _ParticleChange.Initialize(track);

    // If the step length calculated in GetContinuousStepLimit is not
    // zero, propose a new position and time for the particle after drifting
    if (step.GetStepLength() > 0) {
      G4double time = _xyzt.t();
      _ParticleChange.ProposeGlobalTime(time);
      _ParticleChange.ProposePosition(_xyzt.vect());
    }
    // Kill the particle otherwise (no drift field)
    else {
      _ParticleChange.ProposeTrackStatus(fStopAndKill);
    }
    
    return G4VContinuousDiscreteProcess::AlongStepDoIt(track, step);
  }
  
  
  
  G4double IonizationDrift::GetMeanFreePath(const G4Track&, G4double, 
					    G4ForceCondition* condition)
  {
    // This ensures that the PostStep action is always called,
    // so that navigator and touchable can be relocated.
    *condition = StronglyForced;
    return DBL_MAX;
  }
  
  
  
  G4VParticleChange* 
  IonizationDrift::PostStepDoIt(const G4Track& track, const G4Step& step)
  {
    _ParticleChange.Initialize(track);

    // Update navigator and touchable handle
    G4TouchableHandle touchable = track.GetTouchableHandle();

//     G4cout << "Position: " << track.GetPosition() << G4endl;
//     G4cout << "Volume: " << track.GetVolume()->GetName() << G4endl;
//     G4cout << "Volume: " << touchable->GetVolume()->GetName() << G4endl;

    _nav->LocateGlobalPointAndUpdateTouchableHandle
      (track.GetPosition(), track.GetMomentumDirection(), touchable, false);
    
//     G4cout << "Position: " << track.GetPosition() << G4endl;
//     G4cout << "Volume: " << track.GetVolume()->GetName() << G4endl;
//     G4cout << "Volume: " << touchable->GetVolume()->GetName() << G4endl;

    _ParticleChange.SetTouchableHandle(touchable);
    
    // Get the volume where the particle currently lives
    const G4VPhysicalVolume* new_volume = touchable->GetVolume();

    // Check whether the particle has left the world volume.
    // If so, we kill it.
    if (!new_volume)
      _ParticleChange.ProposeTrackStatus(fStopAndKill);

    // Set the material corresponding to the new volume
    // (we "cast away" the constness of the pointer because the particle 
    // change expects a non-constant object).
    G4Material* new_material = 
      const_cast<G4Material*>(new_volume->GetLogicalVolume()->GetMaterial());
    
    _ParticleChange.SetMaterialInTouchable(new_material);

    // Get the drift info
    DriftTrackInfo* drift_info =
      dynamic_cast<DriftTrackInfo*>(track.GetUserInformation());

    G4bool drift_status = drift_info->GetDriftStatus();

    // if (!drift_status)
    //   _ParticleChange.ProposeTrackStatus(fStopAndKill);
    
    // if the particle is an ionization electron, we are done
    if (track.GetDefinition() == IonizationElectron::Definition())
      return G4VContinuousDiscreteProcess::PostStepDoIt(track, step);
    

    if (_shoot_charges && !drift_status) {
      
      G4double transv_sigma = drift_info->GetTransverseSpread();
      G4double longit_sigma = drift_info->GetLongitudinalSpread();
      
      G4double num_charges = drift_info->GetNumberOfCharges();
      
      _ParticleChange.SetNumberOfSecondaries(num_charges);
      
      for (G4int i=0; i<num_charges; i++) {
	
	G4ThreeVector momentum_direction(0.,0.,1.);
	G4double kinetic_energy = 10.*eV;

	G4DynamicParticle* ioniz_electron =
	  new G4DynamicParticle(IonizationElectron::Definition(),
				momentum_direction, kinetic_energy);
	
	G4ThreeVector cluster_position = track.GetPosition();
	
	G4double x = G4RandGauss::shoot(cluster_position.x(), transv_sigma);
	G4double y = G4RandGauss::shoot(cluster_position.y(), transv_sigma);
	G4double z = cluster_position.z();
	G4ThreeVector position(x,y,z);

	G4double deltat = G4RandGauss::shoot(0., longit_sigma);
	//G4cout << deltat/microsecond << G4endl;
	G4double time = track.GetGlobalTime() + deltat;
	
	G4Track* secondary = new G4Track(ioniz_electron, time, position);
	DriftTrackInfo* info = new DriftTrackInfo();
	secondary->SetUserInformation(info);
	//secondary->SetTouchableHandle(touchable);

	_ParticleChange.AddSecondary(secondary);
      }
      
      // We kill the ionization cluster so that it doesn't
      // generate electrons again
      _ParticleChange.ProposeTrackStatus(fStopAndKill);
    }
  
    return G4VContinuousDiscreteProcess::PostStepDoIt(track, step);
  }

  
} // end namespace nexus
