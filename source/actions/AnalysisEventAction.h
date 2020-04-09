// ----------------------------------------------------------------------------
///  File   AnalysisEventAction.h
///  Description  An event action that counts scintillation photon.
///
///  Author:   paola.ferrario@dipc.org
///  Created:     6 Apr 2020
// ----------------------------------------------------------------------------

#ifndef ANALYSIS_EVENT_ACTION__
#define ANALYSIS_EVENT_ACTION__

#include <G4UserEventAction.hh>
#include <globals.hh>

class G4Event;
class G4GenericMessenger;
class TH1F;
class TFile;

namespace nexus {

  /// This class is a general-purpose event run action.

  class AnalysisEventAction: public G4UserEventAction
  {
  public:
    /// Constructor
    AnalysisEventAction();
    /// Destructor
    ~AnalysisEventAction();

    /// Hook at the beginning of the event loop
    void BeginOfEventAction(const G4Event*);
    /// Hook at the end of the event loop
    void EndOfEventAction(const G4Event*);

  private:
    G4GenericMessenger* _msg;
    G4int _nevt, _nupdate;
    G4double _energy_threshold;
    G4double _energy_max;
    G4String file_name_;
    G4int file_no_;

    TH1F* hNPhotons;

    TFile* Histo;
  };

} // namespace nexus

#endif
