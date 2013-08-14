// -----------------------------------------------------------------------------
///  \file   ELTablesTrackingAction.h
///  \brief  
///
///  \author   J. Martin-Albo <jmalbos@ific.uv.es>
///  \date     21 June 2010
///  \version  $Id$
///
///  Copyright (c) 2010, 2011 NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef __EL_TABLES_TRACKING_ACTION__
#define __EL_TABLES_TRACKING_ACTION__

#include <G4UserTrackingAction.hh>


namespace nexus {

  class ELTablesTrackingAction: public G4UserTrackingAction
  {
  public:
    /// Constructor
    ELTablesTrackingAction();
    /// Destructor
    ~ELTablesTrackingAction();
    
    /// Hook at the beginning of the tracking loop
    void PreUserTrackingAction(const G4Track*);
    /// Hook at the end of the tracking loop
    void PostUserTrackingAction(const G4Track*);
  };

  inline ELTablesTrackingAction::ELTablesTrackingAction() {}
  inline ELTablesTrackingAction::~ELTablesTrackingAction() {}
  
} // namespace nexus

#endif
