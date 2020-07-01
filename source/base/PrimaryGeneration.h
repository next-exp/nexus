// ----------------------------------------------------------------------------
// nexus | PrimaryGeneration.h
//
// This is a mandatory class which initializes the generation of
// primary particles in a nexus event.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PRIMARY_GENERATION_H
#define PRIMARY_GENERATION_H

#include <G4VUserPrimaryGeneratorAction.hh>

class G4VPrimaryGenerator;


namespace nexus {

  /// Geant4 (mandatory) initialization class for event generation.

  class PrimaryGeneration: public G4VUserPrimaryGeneratorAction
  {
  public:
    /// Constructor
    PrimaryGeneration();
    /// Destructor
    ~PrimaryGeneration();

    ///
    void GeneratePrimaries(G4Event*);

    /// Sets the primary generator
    void SetGenerator(G4VPrimaryGenerator*);
    /// Returns a pointer to the primary generator
    const G4VPrimaryGenerator* GetGenerator() const;

  private:
    G4VPrimaryGenerator* generator_; ///< Pointer to the primary generator
  };

  // INLINE DEFINITIONS //////////////////////////////////////////////

  inline void PrimaryGeneration::SetGenerator(G4VPrimaryGenerator* pg)
  { generator_ = pg; }

  inline const G4VPrimaryGenerator* PrimaryGeneration::GetGenerator() const
  { return generator_; }

} // end namespace nexus

#endif
