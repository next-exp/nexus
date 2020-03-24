// ----------------------------------------------------------------------------
///  \file   AnalysisTrackingAction.h
///  \brief  General-purpose user tracking action.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     25 Mar 2013 
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef ANALYSIS_TRACKING_ACTION_H
#define ANALYSIS_TRACKING_ACTION_H

#include "BaseGeometry.h"
#include <G4UserTrackingAction.hh>

class G4Track;
class G4GenericMessenger;
class TH1F;
class TFile;


namespace nexus {

  // General-purpose user tracking action

  class AnalysisTrackingAction: public G4UserTrackingAction
  {
  public:
    /// Constructor
    AnalysisTrackingAction();
    /// Destructor
    virtual ~AnalysisTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

  private:
    G4int cer, scint;

    G4GenericMessenger* _msg;
    G4int file_no_;
    
    //TH1F* hScintEnergy;
    //TH1F* hCherEnergy;

    TH1F* hScintLambda;
    TH1F* hCherLambda;
    
    TFile* Times;
  };

}

#endif
