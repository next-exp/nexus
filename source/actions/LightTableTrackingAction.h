// ----------------------------------------------------------------------------
// nexus | LightTableTrackingAction.h
//
// This class is the tracking action to be used when runing simulations
// to create the LightTables.
// It stores in memory just the trajectory of the first photon of each event
// as its initial vertex is shared by all of them.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef LIGHTTABLE_TRACKING_ACTION_H
#define LIGHTTABLE_TRACKING_ACTION_H

#include <G4UserTrackingAction.hh>

class G4Track;


namespace nexus {

  // General-purpose user tracking action

  class LightTableTrackingAction: public G4UserTrackingAction
  {
  public:
    /// Constructor
    LightTableTrackingAction();
    /// Destructor
    virtual ~LightTableTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);
  };

}

#endif
