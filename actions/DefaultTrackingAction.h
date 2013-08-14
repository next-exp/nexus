//
//  DefaultTrackingAction.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>
//     Created: 27 Apr 2009
//     Updated: 
//
//  Copyright (c) 2009 -- IFIC Neutrino Group 
//

#ifndef __DEFAULT_TRACKING_ACTION__
#define __DEFAULT_TRACKING_ACTION__

#include <G4UserTrackingAction.hh>
#include <globals.hh>

class G4Track;
namespace bhep { class particle; }


namespace nexus {

  class DefaultTrackingAction: public G4UserTrackingAction
  {
  public:
    /// Constructor
    DefaultTrackingAction() {}
    /// Destructor
    ~DefaultTrackingAction() {}
    
    ///
    void PreUserTrackingAction(const G4Track*);
    ///
    void PostUserTrackingAction(const G4Track*);
    ///
    static bhep::particle& GetBParticle(G4int);
  };
  
} // namespace nexus

#endif
