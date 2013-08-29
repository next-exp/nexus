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

  /// FIXME. Class description
  
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
    G4bool _clustering;          ///< Switch on/of the ionization clustering
    G4bool _drift;               ///< Switch on/of the ionization drift
    G4bool _electroluminescence; ///< Switch on/off the electroluminescence

    G4GenericMessenger* _msg;
  };

} // end namespace nexus

#endif
