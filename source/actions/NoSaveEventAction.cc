#include "NoSaveEventAction.h"

#include "PersistencyManager.h"

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>


namespace nexus {


  NoSaveEventAction::NoSaveEventAction(): 
    G4UserEventAction()
  {
  
  }
  
  
  
  NoSaveEventAction::~NoSaveEventAction()
  {
  }
  
  
  
  void NoSaveEventAction::BeginOfEventAction(const G4Event* /*event*/)
  {
    PersistencyManager* pm = dynamic_cast<PersistencyManager*>
     (G4VPersistencyManager::GetPersistencyManager());
    pm->StoreCurrentEvent(false);
  }

  void NoSaveEventAction::EndOfEventAction(const G4Event* event)
  {
  }
}
