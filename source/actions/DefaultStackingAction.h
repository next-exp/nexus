// ----------------------------------------------------------------------------
// nexus | DefaultStackingAction.h
//
// This class is an example of how to implement a stacking action, if needed.
// At the moment, it is not used in the NEXT simulations.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef DEFAULT_STACKING_ACTION_H
#define DEFAULT_STACKING_ACTION_H

#include <G4UserStackingAction.hh>


namespace nexus {

  // General-purpose user stacking action

  class DefaultStackingAction: public G4UserStackingAction
  {
  public:
    /// Constructor
    DefaultStackingAction();
    /// Destructor
    ~DefaultStackingAction();

    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
    virtual void NewStage();
    virtual void PrepareNewEvent();
  };

} // end namespace nexus

#endif
