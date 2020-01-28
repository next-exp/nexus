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
#include <vector>


class G4GenericMessenger;
class G4TrajectoryContainer;
class G4HCofThisEvent;
class G4VHitsCollection;

namespace nexus {
  class HDF5Writer;
  class IonizationHit;
}

namespace nexus {

  struct event_info {
    G4int evt_id;
    G4double evt_energy;
    G4String evt_type;
  };


  /// TODO. CLASS DESCRIPTION

  class PersistencyManager: public G4VPersistencyManager
  {
  public:
    /// Create the singleton instance of the persistency manager
    static void Initialize(G4String historyFile_init, G4String historyFile_conf);

    /// Set whether to store or not the current event
    void StoreCurrentEvent(G4bool);
    void InteractingEvent(G4bool);

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

    void StoreTrajectories(G4TrajectoryContainer*);
    void StoreHits(G4HCofThisEvent*);
    void StoreIonizationHits(G4VHitsCollection*);
    void StorePmtHits(G4VHitsCollection*);

    void SaveConfigurationInfo(G4String history);


  private:
    G4GenericMessenger* _msg; ///< User configuration messenger

    G4String _historyFile_init;
    G4String _historyFile_conf;

    G4bool _ready;     ///< Is the PersistencyManager ready to go?
    G4bool _store_evt; ///< Should we store the current event?
    G4bool _interacting_evt; ///< Has the current event interacted in ACTIVE?

    G4String event_type_; ///< event type: bb0nu, bb2nu, background or not set

    G4int _saved_evts; ///< number of events to be saved
    G4int _interacting_evts; ///< number of events interacting in ACTIVE
    G4double _pmt_bin_size, _sipm_bin_size; ///< bin width of sensors

    G4int _nevt; ///< Event ID
    G4int _start_id; ///< ID for the first event in file
    G4bool _first_evt; ///< true only for the first event of the run

    HDF5Writer* _h5writer;  ///< Event writer to hdf5 file

    std::map<G4int, std::vector<G4int>* > _hit_map;
    std::vector<G4int> _sns_posvec;

    event_info event_info_;
  };


  // INLINE DEFINITIONS //////////////////////////////////////////////

  inline void PersistencyManager::StoreCurrentEvent(G4bool sce)
  { _store_evt = sce; }
  inline void PersistencyManager::InteractingEvent(G4bool ie)
  { _interacting_evt = ie; }
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
