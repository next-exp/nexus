// ----------------------------------------------------------------------------
///  \file   IonizationClustering.h
///  \brief  
///  
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     10 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef IONIZATION_CLUSTERING_H
#define IONIZATION_CLUSTERING_H

#include <G4VRestDiscreteProcess.hh>


namespace nexus {

  class SegmentPointSampler;

  
  /// FIXME. Class description.

  class IonizationClustering: public G4VRestDiscreteProcess
  {
  public:
    
    /// Constructor
    IonizationClustering(const G4String& process_name="Clustering",
			                   G4ProcessType type = fUserDefined);
    /// Destructor
    ~IonizationClustering();
    
    /// Returns true (that is, the process is applicable) 
    /// for any particle but ionization electrons and optical photons.
    /// Notice that even though neutral particles cannot ionize
    /// a medium, Geant4 may assign energy depositions to them under
    /// some circumstances. For instance, the deexcitation energy of an
    /// atom after photoelectric effect is assigned to the incident gamma
    /// in the standard electromagnetic version of the process.
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
    /// to be invoked at every step
    G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*);

    /// Returns infinity; i. e. the process does not limit the time,
    /// but sets the 'StronglyForced' condition for the AtRestDoIt 
    /// to be invoked at every step
    G4double GetMeanLifeTime(const G4Track&, G4ForceCondition*);

  private:
    G4ParticleChange* _ParticleChange;
    SegmentPointSampler* _rnd;
  };
  
} // end namespace nexus

#endif
