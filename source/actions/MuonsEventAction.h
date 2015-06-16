//---------------------------------------------------------------------------
//   \file   MuonsEventAction.h
///  \brief  Based in DefaultEventAction and modified to produce control histos for muons
///  
///  \author   Neus Lopez March <neus.lopez@ific.uv.es>    
///  \date     30 Jan 2014
/// 
///
///  Copyright (c) 2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __MUONS_EVENT_ACTION__
#define __MUONS_EVENT_ACTION__

#include <G4UserEventAction.hh>
#include <globals.hh>


#include <TTree.h>
#include <TH1.h>


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
    G4GenericMessenger* _msg;
    G4int _nevt, _nupdate;
    G4double _energy_threshold;
    //
    TH1D *hist;
    TH1D *hist2;
    TH1D *hist3;
    TTree *tree;
    Double_t tree_phi;
    Double_t tree_theta;
    G4String _stringHist;
  };
  
} // namespace nexus

#endif
