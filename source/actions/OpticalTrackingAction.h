// ----------------------------------------------------------------------------
// nexus | OpticalTrackingAction.h
//
// This class saves the trajectories of optical photons, in addition to the
// particles saved by the default tracking action. Its purpose is to store
// optical photon information in the output file.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef OPTICAL_TRACKING_ACTION_H
#define OPTICAL_TRACKING_ACTION_H

#include <G4UserTrackingAction.hh>

class G4Track;


namespace nexus {

  // Optical-checking user tracking action

  class OpticalTrackingAction: public G4UserTrackingAction
  {
  public:
    /// Constructor
    OpticalTrackingAction();
    /// Destructor
    virtual ~OpticalTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);
  };

}

#endif
