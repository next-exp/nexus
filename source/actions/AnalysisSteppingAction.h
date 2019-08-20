// ----------------------------------------------------------------------------
///  \file   AnalysisSteppingAction.h
///
///
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef ANALYSIS_STEPPING_ACTION_H
#define ANALYSIS_STEPPING_ACTION_H

#include <G4UserSteppingAction.hh>
#include <G4GenericMessenger.hh>
#include <globals.hh>

#include <vector>

class G4Step;
class G4GenericMessenger;
class TH1F;
class TH2F;

namespace nexus {

  class AnalysisSteppingAction: public G4UserSteppingAction
  {
  public:
    /// Constructor
    AnalysisSteppingAction();
    /// Destructor
    ~AnalysisSteppingAction();

    virtual void UserSteppingAction(const G4Step*);

  private:

    G4int detected;
    G4int not_det;

    std::vector<double> times;
    std::vector<double> wavelengths;

    TH1F* hVelocity;
    TH2F* hVE;
    TH2F* hTV;
    TH1F* hCherLambdaDet;

    G4GenericMessenger* _msg;
    G4int file_no_;

    typedef std::map<G4String, int> detectorCounts;
    detectorCounts my_counts;
  };

} // namespace nexus

#endif
