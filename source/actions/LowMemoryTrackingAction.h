// ----------------------------------------------------------------------------
// nexus | LowMemoryTrackingAction.h
//
// This class is a tracking action of the NEXT simulation.
// It stores in memory the trajectories of particles -- with the exception of
// optical photons, ionization electrons, and electrons starting in
// Rock or water -- with the relevant tracking information that will be
// saved to the output file. First version useful for TonneScale with no
// Cerenkov studies (June 2020).
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef LOW_MEMORY_TRACKING_ACTION_H
#define LOW_MEMORY_TRACKING_ACTION_H

#include <G4UserTrackingAction.hh>

class G4Track;


namespace nexus {

  // Low memory usage user tracking action

  class LowMemoryTrackingAction: public G4UserTrackingAction
  {
  public:
    /// Constructor
    LowMemoryTrackingAction();
    /// Destructor
    virtual ~LowMemoryTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);
  };

}

#endif
