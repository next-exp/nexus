// ----------------------------------------------------------------------------
///  \file   IonizationDrift.h
///  \brief  Drift of ionization electrons
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     1 June 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __IONIZATION_DRIFT__
#define __IONIZATION_DRIFT__

#include <G4VContinuousDiscreteProcess.hh>
#include <G4ParticleChangeForTransport.hh>

class G4Navigator;


namespace nexus {
  
  class BaseDriftField;


  /// Physics process describing the drift of an ionization electron
  /// under the influence of a field

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


  public:
    G4VParticleChange* AlongStepDoIt(const G4Track&, const G4Step&);
    G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);
        
  private:

    /// Returns zero if no electric field is defined for the region.
    G4double GetContinuousStepLimit(const G4Track& track, 
				    G4double, G4double, G4double&);
    
    /// Returns infinity; i.e., the process does not limit the step,
    /// but sets the 'StronglyForced' condition so that the PostStepDoIt 
    /// is invoked at every step.
    G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*);

  private:
    /// Pointer to the geometry navigator used for tracking
    G4Navigator* _nav; 
    /// Particle change (store of final-state properties) for the process
    G4ParticleChangeForTransport _ParticleChange;
  };

} // end namespace nexus

#endif
