// ----------------------------------------------------------------------------
///  \file   DefaultTrackingAction.h
///  \brief  General-purpose user tracking action.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     25 Mar 2013 
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __DEFAULT_TRACKING_ACTION__
#define __DEFAULT_TRACKING_ACTION__

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
