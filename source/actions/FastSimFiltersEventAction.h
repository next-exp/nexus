// ----------------------------------------------------------------------------
///  \file   FastSimFiltersEventAction.h
///  \brief  
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     9 Apr 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration 
// ----------------------------------------------------------------------------

#ifndef __FAST_SIM_FILTERS_EVENT_ACTION__
#define __FAST_SIM_FILTERS_EVENT_ACTION__

#include <G4UserEventAction.hh>
#include <globals.hh>

class G4Event;


namespace nexus {

  class FastSimFiltersEventAction: public G4UserEventAction
  {
  public:
    // constructor, destructor
    FastSimFiltersEventAction();
    ~FastSimFiltersEventAction();
    
    // hooks
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);

  private:
    G4bool GammaFilter(const G4Event*);

  private:

    G4int _evt_no;
    G4int _gamma_filter;
    G4int _hit_filter;

    G4int _job_events;
    G4int _proc_events;

    G4double _gamma_min_energy;
    G4double _min_Edep;
    
    G4bool _filtered;
  };

} // namespace nexus

#endif
