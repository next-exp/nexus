// ----------------------------------------------------------------------------
// nexus | IonizationDrift.h
//
// This class implements the process of drifting the ionization electrons
// under the influence of a field.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef IONIZATION_DRIFT_H
#define IONIZATION_DRIFT_H

#include <G4VContinuousDiscreteProcess.hh>


class G4Navigator;
class G4ParticleChangeForTransport;

namespace nexus {

  class IonizationDrift: public G4VContinuousDiscreteProcess
  {
  public:
    
    /// Constructor
    IonizationDrift(const G4String& name="Drift",
		                G4ProcessType type=fUserDefined);
    /// Destructor
    ~IonizationDrift();

    /// The process is applicable only to ionization electrons
    G4bool IsApplicable(const G4ParticleDefinition&);

    G4VParticleChange* AlongStepDoIt(const G4Track&, const G4Step&);
    
    G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);
        
  private:
    
    /// Returns infinity; i.e., the process does not limit the step,
    /// but sets the 'StronglyForced' condition so that the PostStepDoIt 
    /// is invoked at every step.
    G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*);

    /// Returns zero if no electric field is defined for the region.
    G4double GetContinuousStepLimit(const G4Track&, G4double, 
				    G4double, G4double&);
    
  private:
    G4LorentzVector xyzt_;
    G4ParticleChangeForTransport* ParticleChange_;
    G4Navigator* nav_; ///< Pointer to the G4 navigator for tracking
  };

} // end namespace nexus

#endif
