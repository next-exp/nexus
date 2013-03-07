// ----------------------------------------------------------------------------
///  \file   NexusFactory.h
///  \brief  Factory to create instances of G4 user initialization classes.
///
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     15 Apr 2009
///  \version  $Id$  
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __NEXUS_FACTORY__
#define __NEXUS_FACTORY__

#include <G4RunManager.hh>
//#include <globals.hh>

class G4GenericMessenger;
class G4VUserPhysicsList;
class G4UserRunAction;
class G4UserEventAction;
class G4UserTrackingAction;
class G4UserSteppingAction;


namespace nexus {

  class DetectorConstruction;
  class PrimaryGeneration;


  /// Class factory to create instances of the Geant4 user initialization
  /// classes specified through the G4 UI manager. 
  /// The class is implemented as a (Meyer's) singleton, and thus a single
  /// instance (with a lifespan equal to that of the job) is offered to the
  /// user. 
  /// Notice that this class does not take responsibility for deleting the
  /// allocated object (this is done actually by the G4RunManager).

  class NexusFactory: public G4RunManager
  {
  public:
    /// Constructor
    NexusFactory();
    /// Destructor
    ~NexusFactory();

    /// Create an instance of the detector initialization class
    DetectorConstruction* CreateDetectorConstruction();

    /// Create an instance of a physics list
    G4VUserPhysicsList* CreatePhysicsList();

    /// Create an instance of the primary generation initialization class
    PrimaryGeneration* CreatePrimaryGeneration();
    
    /// Create an instance of a user action
    G4UserRunAction* CreateRunAction();
    
    /// Create an instance of a event action
    G4UserEventAction* CreateEventAction();
    
    /// Create an instance of a tracking action
    G4UserTrackingAction* CreateTrackingAction();
    
    /// Create an instance of a stepping action
    G4UserSteppingAction* CreateSteppingAction();

  private:


  private:
    G4GenericMessenger* _msg;

    G4String _geometry_name;
    G4String _physics_list_name;
    G4String _generator_name;
    G4String _run_action_name;
    G4String _event_action_name;
    G4String _tracking_action_name;
    G4String _stepping_action_name;
  };
  
} // namespace nexus

#endif
