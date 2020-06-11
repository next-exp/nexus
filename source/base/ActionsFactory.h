// ----------------------------------------------------------------------------
// nexus | ActionFactory.h
//
// This class instantiates the run, event, tracking, stepping and stacking
// actions that the user specifies via configuration parameters.
//
// The NEXT Collaboration
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
    G4GenericMessenger* msg_;

    G4String runact_name_; ///< Name of the user run action
    G4String evtact_name_; ///< Name of the user event action
    G4String trkact_name_; ///< Name of the user tracking action
    G4String stpact_name_; ///< Name of the user stepping action
    G4String stkact_name_; ///< Name of the user stacking action
  };

} // end namespace nexus

#endif
