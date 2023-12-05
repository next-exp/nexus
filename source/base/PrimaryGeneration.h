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
#include <globals.hh>
#include <memory>

class G4VPrimaryGenerator;

namespace nexus {

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
    void SetGenerator(std::unique_ptr<G4VPrimaryGenerator> pg);
    /// Returns a pointer to the primary generator
    const G4VPrimaryGenerator* GetGenerator() const;

  private:
    std::unique_ptr<G4VPrimaryGenerator> generator_; ///< Pointer to the primary generator
  };

  // INLINE DEFINITIONS //////////////////////////////////////////////

  inline void PrimaryGeneration::SetGenerator(std::unique_ptr<G4VPrimaryGenerator> pg)
  { generator_ = std::move(pg); }

  inline const G4VPrimaryGenerator* PrimaryGeneration::GetGenerator() const
  { return generator_.get(); }

} // end namespace nexus

#endif
