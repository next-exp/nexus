// ----------------------------------------------------------------------------
// nexus | NoSaveEventAction.h
//
// This event action saves no event information to file
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NOSAVE_EVENT_ACTION_H
#define NOSAVE_EVENT_ACTION_H

#include <G4UserEventAction.hh>
#include <globals.hh>

class G4Event;

namespace nexus {
    
  /// This class is an Event Action which forces nexus
  /// Not to store any event information
  
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
