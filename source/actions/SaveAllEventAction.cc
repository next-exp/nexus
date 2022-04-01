// ----------------------------------------------------------------------------
// nexus | SaveAllEventAction.cc
//
// This class is based on DefaultEventAction and modified to store
// all the events, no matter how much energy is deposited.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "SaveAllEventAction.h"
#include "FactoryBase.h"
#include "PersistencyManager.h"
#include "Trajectory.h"

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>


namespace nexus {

REGISTER_CLASS(SaveAllEventAction, G4UserEventAction)

  SaveAllEventAction::SaveAllEventAction():
    G4UserEventAction(), nevt_(0), nupdate_(10)
  {

  }



  SaveAllEventAction::~SaveAllEventAction()
  {
  }



  void SaveAllEventAction::BeginOfEventAction(const G4Event* /*event*/)
  {
    // Print out event number info
    if ((nevt_ % nupdate_) == 0) {
      G4cout << " >> Event no. " << nevt_ << G4endl;
      if (nevt_ == (10 * nupdate_)) nupdate_ *= 10;
    }
  }



  void SaveAllEventAction::EndOfEventAction(const G4Event* event)
  {
    nevt_++;

    G4double edep = 0.;

    // draw tracks in visual mode
    G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
    if (tc) {
      for (size_t i=0; i<tc->size(); i++) {
        Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
        edep += trj->GetEnergyDeposit();
        if (G4VVisManager::GetConcreteInstance()) {
          trj->DrawTrajectory();
        }
      }
    }

    PersistencyManager* pm = dynamic_cast<PersistencyManager*>
      (G4VPersistencyManager::GetPersistencyManager());

    // Save the number of events that have interacted
    if (!event->IsAborted() && edep>0) {
      pm->InteractingEvent(true);
    } else {
      pm->InteractingEvent(false);
    }

  }


} // end namespace nexus
