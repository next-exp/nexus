// ----------------------------------------------------------------------------
///  \file   IonizationClustering.h
///  \brief  
///  
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     10 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __IONIZATION_CLUSTERING__
#define __IONIZATION_CLUSTERING__

#include <G4VRestDiscreteProcess.hh>


namespace nexus {

  class SegmentPointSampler;

  
  /// FIXME. Class description.

  class IonizationClustering: public G4VRestDiscreteProcess
  {
  public:
    /// Constructor
    IonizationClustering(const G4String& process_name="IonizClust",
			 G4ProcessType type = fUserDefined);
    /// Destructor
    ~IonizationClustering();
    
    /// Returns true (that is, the process is applicable) for any
    /// particle but optical photons, ionization electrons and
    /// ionization clusters
    G4bool IsApplicable(const G4ParticleDefinition&);

    /// Implements the clusterization for energy depositions of
    /// particles in flight
    G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);
    
    /// Implements the clusterization for energy given to the medium
    /// by particles at rest
    G4VParticleChange* AtRestDoIt(const G4Track&, const G4Step&);
    
  private:
    
    /// Returns infinity; i. e. the process does not limit the step,
    /// but sets the 'StronglyForced' condition for the PostStepDoIt 
    /// to be invoked at every step.
    G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*);

    /// Returns infinity; i. e. the process does not limit the time,
    /// but sets the 'StronglyForced' condition for the AtRestDoIt 
    /// to be invoked at every step.
    G4double GetMeanLifeTime(const G4Track&, G4ForceCondition*);

  private:
    G4ParticleChange _ParticleChange;
    SegmentPointSampler* _rnd;
  };
  
} // namespace nexus

#endif
