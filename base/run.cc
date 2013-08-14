// NEXUS: run.cc
// Last modification: 5th June 2007
//

#include <nexus/run.h>

#include "G4UImanager.hh"
#include "G4VVisManager.hh"


namespace nexus {

  run::run(bhep::prlevel vl) 
  {
    __msg = bhep::messenger(vl);
    __msg.message("[nexus::run] class instance created.", bhep::DUMP);
  }


  void run::BeginOfRunAction(const G4Run* aRun)
  {
    __msg.message("[nexus::run] INFO.- Run start.", bhep::NORMAL);
    
    // Visualization
    if (G4VVisManager::GetConcreteInstance())
      {
	G4UImanager* UI = G4UImanager::GetUIpointer();
	UI->ApplyCommand("/vis/scene/notifyHandlers");
      }
  }


  void run::EndOfRunAction(const G4Run* aRun)
  {
    // Visualization
    if (G4VVisManager::GetConcreteInstance())
      G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");
  }

} // namespace nexus 
