// ----------------------------------------------------------------------------
///  \file   ELSimEventAction.h
///  \brief  A general-purpose user event action.
///
///  \author   P. Ferrario <paolafer@ific.uv.es>
///  \date     23 Jan 2015
///  \version  $Id:$
///
///  Copyright (c) 2009-2015 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __ELSIM_EVENT_ACTION__
#define __ELSIM_EVENT_ACTION__

#include <G4UserEventAction.hh>
#include <globals.hh>

class G4Event;
class G4GenericMessenger;

namespace nexus {
    
  /// This class is a general-purpose event action, where all events are stored, not only
  /// those that deposit some energy in the active volume. It is used mainly for EL table production
  
  class ELSimEventAction: public G4UserEventAction
  {
  public:
    /// Constructor
    ELSimEventAction();
    /// Destructor
    ~ELSimEventAction();
    
    /// Hook at the beginning of the event loop
    void BeginOfEventAction(const G4Event*);
    /// Hook at the end of the event loop
    void EndOfEventAction(const G4Event*);

  private:
   
    G4int _nevt, _nupdate;

  };
  
} // namespace nexus

#endif
