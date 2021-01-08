// ----------------------------------------------------------------------------
// nexus | NexusPhysics.h
//
// This class registers any new physics process defined in nexus.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXUS_PHYSICS_H
#define NEXUS_PHYSICS_H

#include <G4VPhysicsConstructor.hh>

class G4GenericMessenger;


namespace nexus {

  class NexusPhysics: public G4VPhysicsConstructor
  {
  public:
    /// Constructor
    NexusPhysics();
    /// Destructor
    ~NexusPhysics();

    /// Construct all required particles (Geant4 mandatory method)
    virtual void ConstructParticle();
    /// Construct all required physics processes (Geant4 mandatory method)
    virtual void ConstructProcess();

  private:
    G4bool clustering_;          ///< Switch on/of the ionization clustering
    G4bool drift_;               ///< Switch on/of the ionization drift
    G4bool electroluminescence_; ///< Switch on/off the electroluminescence
    G4bool photoelectric_;       ///< Switch on/off the photoelectric effect

    G4GenericMessenger* msg_;
  };

} // end namespace nexus

#endif
