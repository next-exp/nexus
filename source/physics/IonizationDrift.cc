// ----------------------------------------------------------------------------
// nexus | IonizationDrift.cc
//
// This class implements the process of drifting the ionization electrons
// under the influence of a field.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "IonizationDrift.h"

#include "IonizationElectron.h"
#include "BaseDriftField.h"

#include <G4ParticleChangeForTransport.hh>
#include <G4RegionStore.hh>
#include <G4TransportationManager.hh>
#include <G4TouchableHandle.hh>
#include <G4Navigator.hh>


namespace nexus {


  IonizationDrift::IonizationDrift(const G4String& name, G4ProcessType type):
    G4VContinuousDiscreteProcess(name, type)
  {
    ParticleChange_ = new G4ParticleChangeForTransport();
    pParticleChange = ParticleChange_;

    nav_ = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  }
  
  
  
  IonizationDrift::~IonizationDrift()
  {
    delete ParticleChange_;
  }
  


  G4bool IonizationDrift::IsApplicable(const G4ParticleDefinition& pdef)
  {
    return ((pdef == *IonizationElectron::Definition())); 
  }
  
  
  
  G4double IonizationDrift::GetContinuousStepLimit(const G4Track& track, G4double, G4double, G4double&)
  {
    G4double step_length = 0.;
    
    // Get current region
    G4Region* region = track.GetVolume()->GetLogicalVolume()->GetRegion();
    
    // Get the drift field attached to this region
    BaseDriftField* field = 
      dynamic_cast<BaseDriftField*>(region->GetUserInformation());

    // If the region has no field, the particle won't move 
    // and therefore the step length is zero.
    if (!field) return step_length;

    // Get displacement from current position due to drift field
    xyzt_.set(track.GetGlobalTime(), track.GetPosition());
    step_length = field->Drift(xyzt_);
    
    return step_length;
  }
  
  
  
  G4VParticleChange* 
  IonizationDrift::AlongStepDoIt(const G4Track& track, const G4Step& step)
  {
    // Initialize the particle-change (sets all its members equal to
    // the corresponding members in the track).
    ParticleChange_->Initialize(track);

    if (step.GetStepLength() > 0) {
      ParticleChange_->ProposeGlobalTime(xyzt_.t());
      ParticleChange_->ProposePosition(xyzt_.vect());
    }
    else {
      // Kill the particle (it didn't move)
      ParticleChange_->ProposeTrackStatus(fStopAndKill);
    }
    
    return G4VContinuousDiscreteProcess::AlongStepDoIt(track, step);
  }
  
  
  
  G4double IonizationDrift::GetMeanFreePath(const G4Track&, G4double, 
    G4ForceCondition* condition)
  {
    // This ensures that the PostStep action is always called,
    // so that navigator and touchable can be relocated
    *condition = StronglyForced;
    return DBL_MAX;
  }
  
  
  
  G4VParticleChange* 
  IonizationDrift::PostStepDoIt(const G4Track& track, const G4Step& step)
  {
    ParticleChange_->Initialize(track);

    // Update navigator and touchable handle
    G4TouchableHandle touchable = track.GetTouchableHandle();
    nav_->LocateGlobalPointAndUpdateTouchableHandle
      (track.GetPosition(), track.GetMomentumDirection(), touchable, false);
    ParticleChange_->SetTouchableHandle(touchable);
    
    // Get the volume where the particle currently lives
    const G4VPhysicalVolume* new_volume = touchable->GetVolume();

    // Check whether the particle has left the world volume.
    // If so, we kill it.
    if (!new_volume) ParticleChange_->ProposeTrackStatus(fStopAndKill);

    // Set the material corresponding to the new volume
    // (we "cast away" the constness of the pointer because the particle 
    // change expects a non-constant object).
    G4Material* new_material = 
      const_cast<G4Material*>(new_volume->GetLogicalVolume()->GetMaterial());
    
    ParticleChange_->SetMaterialInTouchable(new_material);
       
    return G4VContinuousDiscreteProcess::PostStepDoIt(track, step);
  }

  
} // end namespace nexus
