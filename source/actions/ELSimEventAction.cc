// ----------------------------------------------------------------------------
// nexus | ELSimEventAction.cc
//
// This class is based on DefaultEventAction and modified to store
// all the events, no matter how much energy is deposited.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ELSimEventAction.h"

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>


namespace nexus {


  ELSimEventAction::ELSimEventAction(): 
    G4UserEventAction(), nevt_(0), nupdate_(10)
  {
  
  }
  
  
  
  ELSimEventAction::~ELSimEventAction()
  {
  }
  
  
  
  void ELSimEventAction::BeginOfEventAction(const G4Event* /*event*/)
  {
    // Print out event number info
    if ((nevt_ % nupdate_) == 0) {
      G4cout << " >> Event no. " << nevt_ << G4endl;
      if (nevt_ == (10 * nupdate_)) nupdate_ *= 10;
    }
  }



  void ELSimEventAction::EndOfEventAction(const G4Event* event)
  {
    nevt_++;

    
    // draw tracks in visual mode
    if (G4VVisManager::GetConcreteInstance()) {
      G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
      if (tc) {
        for (size_t i=0; i<tc->size(); i++) {
          G4VTrajectory* trj = (*tc)[i];
          trj->DrawTrajectory();
        }
      }
    }
  }


} // end namespace nexus
