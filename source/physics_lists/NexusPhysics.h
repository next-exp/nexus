// ----------------------------------------------------------------------------
///  \file   NexusPhysics.h
///  \brief  
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     28 May 2010
///  \version  $Id$
///
///  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
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
    NexusPhysics();
    /// Destructor
    ~NexusPhysics();

    /// Construct all required particles (Geant4 mandatory method)
    virtual void ConstructParticle();
    /// Construct all required physics processes (Geant4 mandatory method)
    virtual void ConstructProcess();

    void ActivateDriftAndElectroluminescence(G4bool);

    void TrackSecondariesFirst(G4bool);
    
  private:
    G4bool _drift_el; ///< switch on or off the drift+el processes
    G4bool _track_sec_first;
  };

  // INLINE METHODS //////////////////////////////////////////////////

  inline void NexusPhysics::ActivateDriftAndElectroluminescence(G4bool b)
  { _drift_el = b; }

  inline void NexusPhysics::TrackSecondariesFirst(G4bool tsf) 
  { _track_sec_first = tsf; }


} // end namespace nexus

#endif
