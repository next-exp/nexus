// ----------------------------------------------------------------------------
///  \file   DefaultSteppingAction.h
///  \brief  General-purpose user stepping action.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     24 August 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef ANALYSIS_STEPPING_ACTION_H
#define ANALYSIS_STEPPING_ACTION_H

#include <G4UserSteppingAction.hh>
#include <globals.hh>
#include <map>

class G4Step;


namespace nexus {

  //  Stepping action to analyze the behaviour of optical photons

  class AnalysisSteppingAction: public G4UserSteppingAction
  {
  public:
    /// Constructor
    AnalysisSteppingAction();
    /// Destructor
    ~AnalysisSteppingAction();

    virtual void UserSteppingAction(const G4Step*);

  private:
    typedef std::map<G4String, int> detectorCounts;
    detectorCounts my_counts;
  };

} // namespace nexus

#endif
