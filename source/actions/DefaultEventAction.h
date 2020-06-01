// ----------------------------------------------------------------------------
// nexus | DefaultEventAction.h
//
// This is the default event action of the NEXT simulations. Only events with
// deposited energy larger than 0 are saved in the nexus output file.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef DEFAULT_EVENT_ACTION_H
#define DEFAULT_EVENT_ACTION_H

#include <G4UserEventAction.hh>
#include <globals.hh>

class G4Event;
class G4GenericMessenger;

namespace nexus {

  /// This class is a general-purpose event run action.

  class DefaultEventAction: public G4UserEventAction
  {
  public:
    /// Constructor
    DefaultEventAction();
    /// Destructor
    ~DefaultEventAction();

    /// Hook at the beginning of the event loop
    void BeginOfEventAction(const G4Event*);
    /// Hook at the end of the event loop
    void EndOfEventAction(const G4Event*);

  private:
    G4GenericMessenger* msg_;
    G4int nevt_, nupdate_;
    G4double energy_threshold_;
    G4double energy_max_;
  };

} // namespace nexus

#endif
