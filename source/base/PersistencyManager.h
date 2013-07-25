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

class G4GenericMessenger;
namespace irene { class Event; }
namespace irene { class RootWriter; }


namespace nexus {

  /// TODO. CLASS DESCRIPTION

  class PersistencyManager: public G4VPersistencyManager
  {
  public:
    /// Create the singleton instance of the persistency manager
    static void Initialize();

    /// 
    virtual G4bool Store(const G4Event*);
    virtual G4bool Store(const G4Run*);
    virtual G4bool Store(const G4VPhysicalVolume*);

    virtual G4bool Retrieve(G4Event*&);
    virtual G4bool Retrieve(G4Run*&);
    virtual G4bool Retrieve(G4VPhysicalVolume*&);

  public:
    void OpenFile(const G4String&);
    void CloseFile();


  private:
    PersistencyManager();
    ~PersistencyManager();
    PersistencyManager(const PersistencyManager&);

    void StoreTrajectories(const G4Event*, irene::Event*);


  private:
    G4GenericMessenger* _msg;
    G4bool _ready; ///< Is the PersistencyManager ready to go?
    irene::Event* _evt; ///< Persistent irene event
    irene::RootWriter* _writer; ///< Event writer to ROOT file
  };


  // INLINE DEFINITIONS //////////////////////////////////////////////

  inline G4bool PersistencyManager::Store(const G4VPhysicalVolume*)
  { return false; }
  inline G4bool PersistencyManager::Retrieve(G4Event*&) 
  { return false; }
  inline G4bool PersistencyManager::Retrieve(G4Run*&) 
  { return false; }
  inline G4bool PersistencyManager::Retrieve(G4VPhysicalVolume*&) 
  { return false; }

} // namespace nexus

#endif