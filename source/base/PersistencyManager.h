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
class TFile;
class TTree;
namespace irene { class Event; }


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

  private:
    void OpenFile(const G4String&);

  public:
    void CloseFile();


  private:
    PersistencyManager();
    ~PersistencyManager();
    PersistencyManager(const PersistencyManager&);

  private:
    G4GenericMessenger* _msg;

    G4bool _ready; ///< Is the PersistencyManager ready to go?

    TFile* _file;       ///< Output ROOT file
    TTree* _evttree;    ///< Event tree
    irene::Event* _evt; ///< Persistent irene event
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