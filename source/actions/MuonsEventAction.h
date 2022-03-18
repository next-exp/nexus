// ----------------------------------------------------------------------------
// nexus | MuonsEventAction.h
//
// This class is based on DefaultEventAction and modified to produce
// control histograms for muon generation.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef MUONS_EVENT_ACTION_H
#define MUONS_EVENT_ACTION_H

#include <G4UserEventAction.hh>
#include <G4AnalysisManager.hh>
#include <globals.hh>


class G4Event;
class G4GenericMessenger;

namespace nexus {
    
  /// This class is a general-purpose event run action.
  
  class MuonsEventAction: public G4UserEventAction
  {
  public:
    /// Constructor
    MuonsEventAction();
    /// Destructor
    ~MuonsEventAction();
    
    /// Hook at the beginning of the event loop
    void BeginOfEventAction(const G4Event*);
    /// Hook at the end of the event loop
    void EndOfEventAction(const G4Event*);

  private:
    G4GenericMessenger* msg_;
    G4int nevt_, nupdate_;
    G4double energy_threshold_;
    G4AnalysisManager* fG4AnalysisMan_; ///< Pointer to the Analysis Manager
    G4String stringHist_; ///< Output file name for histograms
    G4bool fOpen_; ///< Check if output file has been opened already
  };
  
} // namespace nexus

#endif
