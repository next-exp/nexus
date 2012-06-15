// ----------------------------------------------------------------------------
///  \file   Electroluminescence.h
///  \brief  Physics process describing the generation of EL light
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     28 Oct 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __ELECTROLUMINESCENCE__
#define __ELECTROLUMINESCENCE__

#include <G4VDiscreteProcess.hh>

class G4PhysicsTable;


namespace nexus {

  /// This class implements a model for the electroluminescence (in gaseous
  /// detectors) as a Geant4 discrete physics process

  class Electroluminescence: public G4VDiscreteProcess
  {
  public:
    /// Constructor
    Electroluminescence(const G4String& process_name = "Electroluminescence",
			G4ProcessType type=fUserDefined);
    /// Destructor
    ~Electroluminescence();

    /// Returns true if particle is an ionization electron
    G4bool IsApplicable(const G4ParticleDefinition&);

    /// If set, the ionization electron tracking is interrupted and
    /// any produced electroluminescence photon is tracked next. When all
    /// are done, the tracking of the electron resumes.
    void SetTrackSecondariesFirst(G4bool);

    /// Returns the state of the flag for tracking EL photons first
    G4bool GetTrackSecondariesFirst() const;

  public:
    /// This is the method that implements the EL light emission
    /// as a post-step process, that is, photons are generated as
    /// secondaries at the end of the step.
    G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);
    
  private:
    /// Returns infinity; i.e., the process does not limit the step,
    /// but sets the 'StronglyForced' condition for the DoIt to be
    /// invoked at every step.
    G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*);

  private:
    G4bool _secondaries_first;
    G4ParticleChange _ParticleChange;
    G4double _pressure;
    G4PhysicsTable* _SlowIntegralTable;
    G4PhysicsTable* _FastIntegralTable;
  };

  // INLINE METHODS //////////////////////////////////////////////////

  inline void Electroluminescence::SetTrackSecondariesFirst(G4bool sf)
  { _secondaries_first = sf; }

  inline G4bool Electroluminescence::GetTrackSecondariesFirst() const
  { return _secondaries_first; }
  
} // end namespace nexus

#endif
