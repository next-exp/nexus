// ----------------------------------------------------------------------------
// nexus | LightTableTrackingAction.h
//
// Tracking action to be used for the generation of light (look-up) tables.
// It creates a trajectory for the first optical photon of each event, so that
// the initial vertex (shared by all photons in an event) gets registered.
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
