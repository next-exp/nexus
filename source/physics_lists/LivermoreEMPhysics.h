// ----------------------------------------------------------------------------
///  \file   LivermoreEMPhysics.h
///  \brief  Constructor of Livermore low-energy EM physics.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>    
///  \date     3 June 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __LIVERMORE_EM_PHYSICS__
#define __LIVERMORE_EM_PHYSICS__

#include <G4VPhysicsConstructor.hh>


namespace nexus {

  /// Constructor of the Livermore low-energy e.m. physics processes.
  /// This class object can be registered in a modular physics list.

  class LivermoreEMPhysics: public G4VPhysicsConstructor
  {
  public:
    /// Constructor
    LivermoreEMPhysics(const G4String& name="LivermoreEM");
    /// Destructor
    ~LivermoreEMPhysics();

  private:
    /// Construct all required particles (Geant4 mandatory method)
    void ConstructParticle();
    /// Construct all required physics processed (Geant4 mandatory method)
    void ConstructProcess();
  };

} // end namespace nexus

#endif
