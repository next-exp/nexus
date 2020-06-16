// ----------------------------------------------------------------------------
// nexus | SaveAllEventAction.h
//
// This class is based on DefaultEventAction and modified to store
// all the events, no matter how much energy is deposited.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SAVEALL_EVENT_ACTION_H
#define SAVEALL_EVENT_ACTION_H

#include <G4UserEventAction.hh>
#include <globals.hh>

class G4Event;
class G4GenericMessenger;

namespace nexus {

  class SaveAllEventAction: public G4UserEventAction
  {
  public:
    /// Constructor
    SaveAllEventAction();
    /// Destructor
    ~SaveAllEventAction();

    /// Hook at the beginning of the event loop
    void BeginOfEventAction(const G4Event*);
    /// Hook at the end of the event loop
    void EndOfEventAction(const G4Event*);

  private:

    G4int nevt_, nupdate_;

  };

} // namespace nexus

#endif
