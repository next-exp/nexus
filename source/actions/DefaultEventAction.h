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

    // At some point this should converge with the calculations done in EndOfEventAction
    void AddToEventEnergy(G4double e) { energy_evt_ += e; }
    bool IsDepositedEnergyInRange() {
      return (energy_evt_ >= energy_min_) &&
             (energy_evt_ <  energy_max_);
    }

  private:
    G4GenericMessenger* msg_;
    G4int nevt_, nupdate_;
    G4double energy_min_;
    G4double energy_max_;
    G4double energy_evt_;
  };

} // namespace nexus

#endif
