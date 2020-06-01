#ifndef __NOSAVE_EVENT_ACTION__
#define __NOSAVE_EVENT_ACTION__

#include <G4UserEventAction.hh>
#include <globals.hh>

class G4Event;
class G4GenericMessenger;

namespace nexus {
    
  /// This class is a general-purpose event action, where all events are stored, not only
  /// those that deposit some energy in the active volume. It is used mainly for EL table production
  
  class NoSaveEventAction: public G4UserEventAction
  {
  public:
    /// Constructor
    NoSaveEventAction();
    /// Destructor
    ~NoSaveEventAction();
    
    /// Hook at the beginning of the event loop
    void BeginOfEventAction(const G4Event*);
    /// Hook at the end of the event loop
    void EndOfEventAction(const G4Event*);

  };
  
} // namespace nexus

#endif
