// ----------------------------------------------------------------------------
// nexus | OpPhotoelectricEffect.h
//
// Add photoelectric effect physics to optical photons hitting
// specific materials.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef OPTICAL_PHOTOELECTRIC_H
#define OPTICAL_PHOTOELECTRIC_H


#include <G4VDiscreteProcess.hh>

class G4Material;

namespace nexus {

  class OpPhotoelectricEffect: public G4VDiscreteProcess
  {
  public:

    /// Constructor
    OpPhotoelectricEffect(const G4String& process_name="OpPhotoelectricEffect",
			                   G4ProcessType type = fUserDefined);
    /// Destructor
    ~OpPhotoelectricEffect();

    /// Whether a particle is sensitive to this physics.
    /// Only optical photons apply
    G4bool IsApplicable(const G4ParticleDefinition&);

    /// Calculates ionization electron emission probability
    /// and generates new particles if necessary.
    G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);

  private:

    /// Returns infinity; i. e. the process does not limit the step,
    /// but sets the 'StronglyForced' condition for the PostStepDoIt
    /// to be invoked at every step
    G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*);

    /// Returns infinity; i. e. the process does not limit the time,
    /// but sets the 'StronglyForced' condition for the AtRestDoIt
    /// to be invoked at every step
    G4double GetMeanLifeTime(const G4Track&, G4ForceCondition*);

    G4bool ValidMaterial(const G4Material*);

  private:
    G4ParticleChange* particle_change_;

  };

} // end namespace nexus

#endif
