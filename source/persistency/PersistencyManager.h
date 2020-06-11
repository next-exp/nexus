// ----------------------------------------------------------------------------
// nexus | PersistencyManager.h
//
// This class writes all the relevant information of the simulation
// to an ouput file.
//
// The NEXT Collaboration
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

    G4bool ready_;     ///< Is the PersistencyManager ready to go?
    G4bool store_evt_; ///< Should we store the current event?
    G4bool interacting_evt_; ///< Has the current event interacted in ACTIVE?

    G4String event_type_; ///< event type: bb0nu, bb2nu, background or not set

    std::vector<G4int> sns_posvec_;

    G4int saved_evts_; ///< number of events to be saved
    G4int interacting_evts_; ///< number of events interacting in ACTIVE
    G4double pmt_bin_size_, sipm_bin_size_; ///< bin width of sensors

    G4int nevt_; ///< Event ID
    G4int start_id_; ///< ID for the first event in file
    G4bool first_evt_; ///< true only for the first event of the run

    G4int thr_charge_;
    G4double tof_time_;
    G4bool sns_only_;
    G4bool save_tot_charge_;
    HDF5Writer* h5writer_;  ///< Event writer to hdf5 file

    G4double bin_size_, tof_bin_size_;

  };


  // INLINE DEFINITIONS //////////////////////////////////////////////

  inline void PersistencyManager::StoreCurrentEvent(G4bool sce)
  { store_evt_ = sce; }
  inline void PersistencyManager::InteractingEvent(G4bool ie)
  { interacting_evt_ = ie; }
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
