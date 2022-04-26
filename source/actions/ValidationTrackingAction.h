// ----------------------------------------------------------------------------
// nexus | ValidationTrackingAction.h
//
// This class is based on DefaultTrackinAction.
// In addition, it creates and saves ROOT histograms with the energy
// of the gammas produced in the simulation. Its purpose is to produce
// histograms to be compared across different versions of GEANT4,
// before changing version.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef VALIDATION_TRACKING_ACTION_H
#define VALIDATION_TRACKING_ACTION_H

#include <G4UserTrackingAction.hh>
#include <G4AnalysisManager.hh>

class G4Track;

namespace nexus {

  // General-purpose user tracking action

  class ValidationTrackingAction: public G4UserTrackingAction
  {
  public:
    /// Constructor
    ValidationTrackingAction();
    /// Destructor
    virtual ~ValidationTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

  private:
    G4AnalysisManager* fG4AnalysisMan_; ///< Pointer to the Analysis Manager
  };

}

#endif
