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

  class WavelengthShifting;

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
    G4bool risetime_; ///< Rise time for LYSO

    G4bool noCompt_; ///< Switch off Compton scattering

    G4GenericMessenger* msg_;
    WavelengthShifting* wls_;
  };

} // end namespace nexus

#endif
