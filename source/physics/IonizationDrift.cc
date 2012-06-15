// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 1 June 2009
//
//  Copyright (c) 2009-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "IonizationDrift.h"

#include "BaseDriftField.h"
#include "DriftTrackInfo.h"
#include "IonizationElectron.h"

#include <G4TransportationManager.hh>


namespace nexus {
  
  
  IonizationDrift::IonizationDrift(const G4String& name, G4ProcessType type):
    G4VContinuousDiscreteProcess(name, type)
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
    return (pdef == *IonizationElectron::Definition());
  }
  
  
  
  G4double IonizationDrift::GetContinuousStepLimit
  (const G4Track& track, G4double, G4double, G4double&)
  {
    G4double step_length;
    
    // Get current region
    G4Region* region = track.GetVolume()->GetLogicalVolume()->GetRegion();
    
    // Get the drift field attached to this region
    // (The dynamic_cast ensures at runtime that the type of the user info
    // object is compatible; it returns a null pointer otherwise.)
    BaseDriftField* field = 
      dynamic_cast<BaseDriftField*>(region->GetUserInformation());

    // If the region has no field, the particle won't move and therefore
    // the step length is zero
    if (!field) {
      step_length = 0.;
      return step_length;
    }

    // Get the drift info associated to the track
    DriftTrackInfo* drift_info =
      dynamic_cast<DriftTrackInfo*>(track.GetUserInformation());
    
    if (!drift_info) {
      G4ExceptionDescription ed;
      ed << "Ionization drift cannot be done on track with no DriftTrackInfo.";
      G4Exception("[IonizationDrift]", "GetContinuousStepLimit()",
		  EventMustBeAborted, ed);
    }

    // Set properties in drift info
    G4LorentzVector prepoint(track.GetPosition(), track.GetGlobalTime());
    drift_info->SetPreDriftPoint(prepoint);

    // Get displacement from current position due to drift field
    step_length = field->Drift(drift_info);
    
    return step_length;
  }
  
  
  
  G4VParticleChange* 
  IonizationDrift::AlongStepDoIt(const G4Track& track, const G4Step& step)
  {
    // Initialize the particle-change (sets all its members equal to
    // the corresponding members in the track).
    _ParticleChange.Initialize(track);

    // Get the drift info associated to the track
    DriftTrackInfo* drift_info =
      dynamic_cast<DriftTrackInfo*>(track.GetUserInformation());
    if (!drift_info) {
      G4ExceptionDescription ed;
      ed << "Ionization drift cannot be done on track without DriftTrackInfo.";
      G4Exception("[IonizationDrift]", "AlongStepDoIt()",
		  EventMustBeAborted, ed);
    }
    
    // If the track drifted, propose a new position and time
    if (step.GetStepLength() > 0.) {
      G4LorentzVector post = drift_info->GetPostDriftPoint();
      _ParticleChange.ProposeGlobalTime(post.t());
      _ParticleChange.ProposePosition(post.vect());
      if (drift_info->GetDriftStatus() == ATTACHED)
	_ParticleChange.ProposeTrackStatus(fStopAndKill);
    }
    // Kill the particle otherwise (no drift field)
    else {
      _ParticleChange.ProposeGlobalTime(track.GetGlobalTime());
      _ParticleChange.ProposePosition(track.GetPosition());
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
    _nav->LocateGlobalPointAndUpdateTouchableHandle
      (track.GetPosition(), track.GetMomentumDirection(), touchable, false);
    _ParticleChange.SetTouchableHandle(touchable);
    
    // Get the volume where the particle currently lives
    const G4VPhysicalVolume* new_volume = touchable->GetVolume();

    // Check whether the new volume has a region with a drift field.
    // If not, kill the particle
    G4Region* region = new_volume->GetLogicalVolume()->GetRegion();
    BaseDriftField* field = 
      dynamic_cast<BaseDriftField*>(region->GetUserInformation());
    if (!field)
      _ParticleChange.ProposeTrackStatus(fStopAndKill);

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
    
    return G4VContinuousDiscreteProcess::PostStepDoIt(track, step);
  }

} // namespace nexus
