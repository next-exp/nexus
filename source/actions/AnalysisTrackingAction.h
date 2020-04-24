// ----------------------------------------------------------------------------
///  \file   AnalysisTrackingAction.h
///
///  \author   <justo.martin-albo@ific.uv.es>, <paola.ferrario@dipc.org>
///  \date     25 Mar 2013
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

    G4GenericMessenger* msg_;
    G4String file_name_;
    G4int file_no_;

    //TH1F* hScintEnergy_;
    //TH1F* hCherEnergy_;

    TH1F* hCherLambda_;
    TH1F* hScintLambda_;
    TH1F* hScintTime;

    TFile* OptPhotons_;
  };

}

#endif
