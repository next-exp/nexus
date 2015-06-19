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

namespace gate { class Event; }
namespace gate { class MCParticle; }
namespace gate { class MCTrack; }
namespace gate { class RootWriter; }
namespace gate { class Run; }

namespace nexus { class HDF5Writer; }

namespace nexus {


  /// TODO. CLASS DESCRIPTION

  class PersistencyManager: public G4VPersistencyManager
  {
  public:
    /// Create the singleton instance of the persistency manager
    static void Initialize(G4String historyFile_init, G4String historyFile_conf);

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
    void OpenFile(G4String);
    void CloseFile();


  private:
    PersistencyManager(G4String historyFile_init, G4String historyFile_conf);
    ~PersistencyManager();
    PersistencyManager(const PersistencyManager&);

    void StoreTrajectories(G4TrajectoryContainer*, gate::Event*);
    void StoreHits(G4HCofThisEvent*, gate::Event*);
    void StoreIonizationHits(G4VHitsCollection*, gate::Event*);
    void StorePmtHits(G4VHitsCollection*, gate::Event*);

    void SaveConfigurationInfo(G4String history, gate::Run& grun);


  private:
    G4GenericMessenger* _msg; ///< User configuration messenger

    G4String _historyFile_init;
    G4String _historyFile_conf;

    G4bool _ready;     ///< Is the PersistencyManager ready to go?
    G4bool _store_evt; ///< Should we store the current event?

    G4String event_type_; ///< event type: bb0nu, bb2nu, background or not set

    // gate::Event* _evt;         ///< Persistent gate event
    gate::RootWriter* _writer; ///< Event writer to ROOT file

    std::map<G4int, gate::MCParticle*> _iprtmap;
    std::map<G4int, gate::MCTrack*> _itrkmap;

    G4int _saved_evts; ///< number of events to be saved

    G4int _nevt; ///< Event ID
    G4int _start_id; ///< ID for the first event in file
    G4bool _first_evt; ///< true only for the first event of the run

    G4bool _hdf5dump; ///< if true write to hdf5 file

    HDF5Writer* _h5writer;  ///< Event writer to hdf5 file

    std::pair<G4int, G4double> _event_info;
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
