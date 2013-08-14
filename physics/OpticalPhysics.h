// ----------------------------------------------------------------------------
///  \file   OpticalPhysics.h
///  \brief  Optical physics processes.
///
///  \author   F. Monrabal <franmon4@ific.uv.es>
///  \date     27 Jan 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __OPTICAL_PHYSICS__
#define __OPTICAL_PHYSICS__

#include <G4VPhysicsConstructor.hh>
#include <globals.hh>

namespace nexus {

  /// This class provides construction of the optical physics processes.

  class OpticalPhysics: public G4VPhysicsConstructor
  {
  public:
    /// Constructor
    OpticalPhysics(G4int verbosity=0, const G4String& name="Optical");
    /// Destructor
    ~OpticalPhysics();

  private:
    /// Construct all required particles (Geant4 mandatory method)
    virtual void ConstructParticle();
    /// Construct all required physics processes (Geant4 mandatory method)
    virtual void ConstructProcess();

  private:
    G4int verbose;
  };

} // end namespace nexus

#endif
