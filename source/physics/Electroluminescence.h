// ----------------------------------------------------------------------------
///  \file   Electroluminescence.h
///  \brief  Physics process describing the generation of EL light.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     28 Oct 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef ELECTROLUMINESCENCE_H
#define ELECTROLUMINESCENCE_H

#include <G4VDiscreteProcess.hh>


class G4ParticleChange;


namespace nexus {

  /// This class implements a model for the electroluminescence (in gaseous
  /// detectors) as a Geant4 discrete physics process.

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

    void BuildThePhysicsTable();
    void ComputeCumulativeDistribution(const G4PhysicsOrderedFreeVector&,
                                       G4PhysicsOrderedFreeVector&);

  private:
    G4ParticleChange* _ParticleChange;

    G4PhysicsTable* _theSlowIntegralTable;
    G4PhysicsTable* _theFastIntegralTable;
  };

} // end namespace nexus

#endif
