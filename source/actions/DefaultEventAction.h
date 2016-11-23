// ----------------------------------------------------------------------------
///  \file   DefaultEventAction.h
///  \brief  A general-purpose user event action.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     15 Apr 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __DEFAULT_EVENT_ACTION__
#define __DEFAULT_EVENT_ACTION__

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
    G4GenericMessenger* _msg;
    G4int _nevt, _nupdate;
    G4double _energy_threshold;
    G4double _energy_max;
    G4bool _first_evt;
    G4int _start_id;
  };
  
} // namespace nexus

#endif
