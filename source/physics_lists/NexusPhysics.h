// ----------------------------------------------------------------------------
///  \file   NexusPhysics.h
///  \brief  
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     28 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010, 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXUS_PHYSICS__
#define __NEXUS_PHYSICS__

#include <G4VPhysicsConstructor.hh>


namespace nexus {

  /// FIXME. Class description
  
  class NexusPhysics: public G4VPhysicsConstructor
  {
  public:
    /// Constructor
    NexusPhysics(const G4String& name="Nexus");
    /// Destructor
    ~NexusPhysics();

    void TrackSecondariesFirst(G4bool);

  private:
    /// Construct all required particles (Geant4 mandatory method)
    virtual void ConstructParticle();
    /// Construct all required physics processes (Geant4 mandatory method)
    virtual void ConstructProcess();

  private:

    G4bool _track_sec_first;
  };

  // inline methods ..............................

  inline void NexusPhysics::TrackSecondariesFirst(G4bool tsf) 
  { _track_sec_first = tsf; }


} // end namespace nexus

#endif
