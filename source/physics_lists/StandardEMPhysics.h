// ----------------------------------------------------------------------------
///  \file   StandardEMPhysics.h
///  \brief  Standard electromagnetic physics processes.
///
///  \author   F. Monrabal <franmon4@ific.uv.es>
///  \date     27 Jan 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __STANDARD_EM_PHYSICS__
#define __STANDARD_EM_PHYSICS__

#include <G4VPhysicsConstructor.hh>


namespace nexus {

  /// This class provides construction of the standard electromagnetic
  /// physics processes.

  class StandardEMPhysics: public G4VPhysicsConstructor
  {
  public:
    /// Constructor
    StandardEMPhysics(const G4String& name="StandardEM");
    /// Destructor
    ~StandardEMPhysics();

  private:
    /// Construct all required particles (Geant4 mandatory method)
    virtual void ConstructParticle();
    /// Construct all required physics processes (Geant4 mandatory method)
    virtual void ConstructProcess();
  };

} // end namespace nexus

#endif
