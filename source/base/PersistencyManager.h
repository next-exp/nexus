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

#ifndef PERSISTENCY_MANAGER_H
#define PERSISTENCY_MANAGER_H

#include <G4VPersistencyManager.hh>
#include <map>

class G4GenericMessenger;
class G4TrajectoryContainer;
class G4HCofThisEvent;
class G4VHitsCollection;
namespace irene { class Event; }
namespace irene { class Particle; }
namespace irene { class Track; }
namespace irene { class RootWriter; }


namespace nexus {


  /// TODO. CLASS DESCRIPTION

  class PersistencyManager: public G4VPersistencyManager
  {
  public:
    /// Create the singleton instance of the persistency manager
    static void Initialize();

    /// Set whether to store or not the current event
    void StoreCurrentEvent(G4bool);

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

    void StoreTrajectories(G4TrajectoryContainer*, irene::Event*);
    void StoreHits(G4HCofThisEvent*, irene::Event*);
    void StoreIonizationHits(G4VHitsCollection*, irene::Event*);
    void StorePmtHits(G4VHitsCollection*, irene::Event*);


  private:
    G4GenericMessenger* _msg; ///< User configuration messenger

    G4bool _ready;     ///< Is the PersistencyManager ready to go?
    G4bool _store_evt; ///< Should we store the current event?

    irene::Event* _evt;         ///< Persistent irene event
    irene::RootWriter* _writer; ///< Event writer to ROOT file

    std::map<G4int, irene::Particle*> _iprtmap;
    std::map<G4int, irene::Track*> _itrkmap;
  };


  // INLINE DEFINITIONS //////////////////////////////////////////////

  inline void PersistencyManager::StoreCurrentEvent(G4bool sce)
  { _store_evt = sce; }
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