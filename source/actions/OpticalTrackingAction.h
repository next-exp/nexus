// ----------------------------------------------------------------------------
///  \file   OpticalTrackingAction.h
///  \brief  Devoted to optical physics checks.
///
///  \author   <javier.munoz.vidal@gmail.com>
///  \date     Oct 2019 
///  \version  $Id$
///
///  Copyright (c) 2019 NEXT Collaboration. All rights reserved.
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
