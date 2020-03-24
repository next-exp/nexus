// ----------------------------------------------------------------------------
///  \file   NexusPhysics.h
///  \brief  
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     28 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
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

    G4bool _noCompt; ///< Switch off Compton scattering
    G4bool _noCher;
    G4bool _noScint;

    G4GenericMessenger* _msg;
    WavelengthShifting* _wls;
  };

} // end namespace nexus

#endif
