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
//#include "IonizationCluster.h"
#include "IonizationElectron.h"
//#include "DriftTrackInfo.h"

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
    return ((pdef == *IonizationElectron::Definition())); 
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

    // Get the drift info associated to the cluster
    // DriftTrackInfo* drift_info =
    //   dynamic_cast<DriftTrackInfo*>(track.GetUserInformation());
    
    // If the region has no field, the particle won't move and therefore
    // the step length is zero.
    if (!field) {
      step_length = 0.;
      return step_length;
    }

    // Get displacement from current position due to drift field
    _xyzt.set(track.GetGlobalTime(), track.GetPosition());
    //step_length = field->Drift(_xyzt, drift_info);
    step_length = field->Drift(_xyzt);
    
    return step_length;
  }
  
  
  
  G4VParticleChange* 
  IonizationDrift::AlongStepDoIt(const G4Track& track, const G4Step& step)
  {
    // Initialize the particle-change (sets all its members equal to
    // the corresponding members in the track).
    _ParticleChange.Initialize(track);

    if (step.GetStepLength() > 0) {
      
      G4MaterialPropertiesTable* mpt =
	track.GetMaterial()->GetMaterialPropertiesTable();

      if (!mpt)
	G4cout << "No attachment" << G4endl;

      const G4double attach = mpt->GetConstProperty("ATTACHMENT");
      
      G4double rnd = -attach * log(G4UniformRand());

      G4double time = _xyzt.t();
      
      if (time > rnd)
	_ParticleChange.ProposeTrackStatus(fStopAndKill);
      
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
    _nav->LocateGlobalPointAndUpdateTouchableHandle
      (track.GetPosition(), track.GetMomentumDirection(), touchable, false);
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
    
    // if the particle is an ionization electron, we are done
    // if (track.GetDefinition() == IonizationElectron::Definition())
    //   return G4VContinuousDiscreteProcess::PostStepDoIt(track, step);
    
    return G4VContinuousDiscreteProcess::PostStepDoIt(track, step);
  }

  
} // end namespace nexus
