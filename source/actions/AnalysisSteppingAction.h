// ----------------------------------------------------------------------------
// nexus | AnalysisSteppingAction.h
//
// This class allows the user to print the total number of photons detected by
// all kinds of photosensors at the end of the run.
// It also shows examples of information that can be accessed at the stepping
// level, so it is useful for debugging.
//
// The  NEXT Collaboration
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
    detectorCounts my_counts_;
  };

} // namespace nexus

#endif
