// ----------------------------------------------------------------------------
///  \file   PersistencyManager.h
///  \brief  
/// 
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     13 March 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __PERSISTENCY_MANAGER__
#define __PERSISTENCY_MANAGER__

#include <G4VPersistencyManager.hh>

namespace nexus {

  class PersistencyManager: public G4VPersistencyManager
  {
  public:
    static void Initialize();

    virtual G4bool Store(const G4Event*);
    virtual G4bool Store(const G4Run*);
    virtual G4bool Store(const G4VPhysicalVolume*);

    virtual G4bool Retrieve(G4Event*&) { return false; }
    virtual G4bool Retrieve(G4Run*&) { return false; }
    virtual G4bool Retrieve(G4VPhysicalVolume*&) { return false; }


  private:
    PersistencyManager();
    ~PersistencyManager();
    PersistencyManager(const PersistencyManager&);

  };

}

#endif