//
//  DefaultEventAction.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>
//     Created: 15 Apr 2009
//     Updated: 
//
//  Copyright (c) 2009 -- IFIC Neutrino Group 
//

#ifndef __DEFAULT_EVENT_ACTION__
#define __DEFAULT_EVENT_ACTION__

#include <G4UserEventAction.hh>

class G4Event;
class G4HCofThisEvent;
class EventManager2;


namespace nexus {

  class DefaultEventAction: public G4UserEventAction
  {
  public:
    /// Constructor
    DefaultEventAction();
    /// Destructor
    ~DefaultEventAction();
    
    ///
    void BeginOfEventAction(const G4Event*);
    ///
    void EndOfEventAction(const G4Event*);

  private:
    void Initialize();
    void ProcessHits(G4HCofThisEvent*);

  private:
    EventManager2* _evtMgr;
  };
  
} // namespace nexus

#endif
