// ----------------------------------------------------------------------------
///  \file   FastSimEventAction.h
///  \brief  A general-purpose user event action.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     10 September 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef FAST_SIM_EVENT_ACTION_H
#define FAST_SIM_EVENT_ACTION_H

#include <G4UserEventAction.hh>
#include <globals.hh>

class G4Event;


namespace nexus {
    
  /// This class is a general-purpose event run action.
  
  class FastSimEventAction: public G4UserEventAction
  {
  public:
    /// Constructor
    FastSimEventAction();
    /// Destructor
    ~FastSimEventAction();
    
    /// Hook at the beginning of the event loop
    void BeginOfEventAction(const G4Event*);
    /// Hook at the end of the event loop
    void EndOfEventAction(const G4Event*);

  private:
    G4int _nevt, _nupdate;
  };
  
} // namespace nexus

#endif
