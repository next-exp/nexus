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
#include <vector>

class G4GenericMessenger;
class G4TrajectoryContainer;
class G4HCofThisEvent;
class G4VHitsCollection;

namespace nexus { class HDF5Writer; }

namespace nexus {


  /// TODO. CLASS DESCRIPTION

  class PersistencyManager: public G4VPersistencyManager
  {
  public:
    /// Create the singleton instance of the persistency manager
    static void Initialize(G4String init_macro, std::vector<G4String>& macros,
                           std::vector<G4String>& delayed_macros);

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
    PersistencyManager(G4String init_macro, std::vector<G4String>& macros,
                           std::vector<G4String>& delayed_macros);
    ~PersistencyManager();
    PersistencyManager(const PersistencyManager&);

    void StoreTrajectories(G4TrajectoryContainer*);
    void StoreHits(G4HCofThisEvent*);
    void StoreIonizationHits(G4VHitsCollection*);
    void StorePmtHits(G4VHitsCollection*);

    void SaveConfigurationInfo(G4String history);


  private:
    G4GenericMessenger* msg_; ///< User configuration messenger

    G4String init_macro_;
    std::vector<G4String> macros_;
    std::vector<G4String> delayed_macros_;
    std::vector<G4String> secondary_macros_;

    G4bool _ready;     ///< Is the PersistencyManager ready to go?
    G4bool _store_evt; ///< Should we store the current event?
    G4bool _interacting_evt; ///< Has the current event interacted in ACTIVE?

    G4String event_type_; ///< event type: bb0nu, bb2nu, background or not set

    std::vector<G4int> _sns_posvec;

    G4int _saved_evts; ///< number of events to be saved
    G4int _interacting_evts; ///< number of events interacting in ACTIVE
    G4double _pmt_bin_size, _sipm_bin_size; ///< bin width of sensors

    G4int _nevt; ///< Event ID
    G4int _start_id; ///< ID for the first event in file
    G4bool _first_evt; ///< true only for the first event of the run

    G4int _thr_charge;
    G4double _tof_time;
    G4bool _sns_only;
    G4bool _save_tot_charge;
    HDF5Writer* _h5writer;  ///< Event writer to hdf5 file

    G4double _bin_size, _tof_bin_size;

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
