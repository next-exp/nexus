// ----------------------------------------------------------------------------
///  \file   MikelTrackingAction.h
///  \brief  G4RadiactiveDecay check  user tracking action.
///
///  \author   <miquel.nebot@ific.uv.es>
///  \date     28 August 2013 
///  \version  
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __MIKEL_TRACKING_ACTION__
#define __MIKEL_TRACKING_ACTION__

#include <G4UserTrackingAction.hh>
#include <TH1.h>

class G4Track;


namespace nexus {

  // General-purpose user tracking action

  class MikelTrackingAction: public G4UserTrackingAction
  {
  public:
    /// Constructor
    MikelTrackingAction();
    /// Destructor
    virtual ~MikelTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

    TH1F* GammaEnergy;
  };

}

#endif  
