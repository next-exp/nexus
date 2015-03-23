// ----------------------------------------------------------------------------
///  \file   ActionsFactory.h
///  \brief  
/// 
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     13 March 2013
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef ACTIONS_FACTORY_H
#define ACTIONS_FACTORY_H 

#include <G4String.hh>

class G4UserRunAction;
class G4UserEventAction;
class G4UserTrackingAction;
class G4UserSteppingAction;
class G4UserStackingAction;
class G4GenericMessenger;


namespace nexus {

  /// TODO. CLASS DESCRIPTION

  class ActionsFactory
  {
  public:
    ActionsFactory();
    ~ActionsFactory();

    G4UserRunAction* CreateRunAction() const;
    G4UserEventAction* CreateEventAction() const;
    G4UserTrackingAction* CreateTrackingAction() const;
    G4UserSteppingAction* CreateSteppingAction() const;
    G4UserStackingAction* CreateStackingAction() const;

  private:
    G4GenericMessenger* _msg;

    G4String _runact_name; ///< Name of the user run action
    G4String _evtact_name; ///< Name of the user event action
    G4String _trkact_name; ///< Name of the user tracking action
    G4String _stpact_name; ///< Name of the user stepping action
    G4String _stkact_name; ///< Name of the user stacking action
  };

} // end namespace nexus

#endif
