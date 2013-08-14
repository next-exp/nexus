// -----------------------------------------------------------------------------
///  \file   DefaultTrackingAction.h
///  \brief  General-purpose user tracking action.
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     27 Apr 2009
///  \version  $Id$ 
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef __DEFAULT_TRACKING_ACTION__
#define __DEFAULT_TRACKING_ACTION__

#include <G4UserTrackingAction.hh>


namespace nexus {

  /// This class is a general-purpose user tracking action.

  class DefaultTrackingAction: public G4UserTrackingAction
  {
  public:
    /// Constructor
    DefaultTrackingAction();
    /// Destructor
    ~DefaultTrackingAction();
    
    /// Hook at the beginning of the tracking loop
    void PreUserTrackingAction(const G4Track*);
    /// Hook at the end of the tracking loop
    void PostUserTrackingAction(const G4Track*);
  };

  inline DefaultTrackingAction::DefaultTrackingAction() {}
  inline DefaultTrackingAction::~DefaultTrackingAction() {}
  
} // namespace nexus

#endif
