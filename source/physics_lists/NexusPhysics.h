// ----------------------------------------------------------------------------
///  \file   NexusPhysics.h
///  \brief  
///
///  \author   J. Martín-Albo
///  \date     28 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXUS_PHYSICS__
#define __NEXUS_PHYSICS__

#include <G4VPhysicsConstructor.hh>


namespace nexus {

  /// This class provides construction of the standard electromagnetic
  /// physics processes.

  class NexusPhysics: public G4VPhysicsConstructor
  {
  public:
    /// Constructor
    NexusPhysics(const G4String& name="Nexus");
    /// Destructor
    ~NexusPhysics();

  private:
    /// Construct all required particles (Geant4 mandatory method)
    virtual void ConstructParticle();
    /// Construct all required physics processes (Geant4 mandatory method)
    virtual void ConstructProcess();
  };

} // end namespace nexus

#endif
