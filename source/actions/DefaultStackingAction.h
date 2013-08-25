// ----------------------------------------------------------------------------
///  \file   DefaultStackingAction.h
///  \brief  General-purpose user stacking action.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     25 Aug 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
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