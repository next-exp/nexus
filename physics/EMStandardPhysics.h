// ----------------------------------------------------------------------------
///  \file   EMStandardPhysics.h
///  \brief  Standard electromagnetic physics processes.
///
///  \author   F. Monrabal <franmon4@ific.uv.es>
///  \date     27 Jan 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __EM_STANDARD_PHYSICS__
#define __EM_STANDARD_PHYSICS__

#include <G4VPhysicsConstructor.hh>
#include <globals.hh>

namespace nexus {

  /// This class provides construction of the standard electromagnetic
  /// physics processes.

  class EMStandardPhysics: public G4VPhysicsConstructor
  {
  public:
    /// Constructor
    EMStandardPhysics(G4int verbosity=0, const G4String& name="EMStandard");
    /// Destructor
    ~EMStandardPhysics();

  private:
    /// Construct all required particles (Geant4 mandatory method)
    virtual void ConstructParticle();
    /// Construct all required physics processes (Geant4 mandatory method)
    virtual void ConstructProcess();
    
  private:
    G4int _verbose;
  };

} // end namespace nexus

#endif
