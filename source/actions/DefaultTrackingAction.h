// ----------------------------------------------------------------------------
// nexus | DefaultTrackingAction.h
//
// This class is the default tracking action of the NEXT simulation.
// It stores in memory the trajectories of all particles, except optical photons
// and ionization electrons, with the relevant tracking information that will be
// saved to the output file.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef DEFAULT_TRACKING_ACTION_H
#define DEFAULT_TRACKING_ACTION_H

#include <G4UserTrackingAction.hh>

class G4Track;


namespace nexus {

  // General-purpose user tracking action

  class DefaultTrackingAction: public G4UserTrackingAction
  {
  public:
    /// Constructor
    DefaultTrackingAction();
    /// Destructor
    virtual ~DefaultTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);
  };

}

#endif
