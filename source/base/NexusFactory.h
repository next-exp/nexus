// ----------------------------------------------------------------------------
///  \file   NexusFactory.h
///  \brief  Factory to create instances of G4 user initialization classes.
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     15 Apr 2009
///  \version  $Id$  
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXUS_FACTORY__
#define __NEXUS_FACTORY__

#include <globals.hh>

class G4VUserPhysicsList;
class G4UserRunAction;
class G4UserEventAction;
class G4UserTrackingAction;
class G4UserSteppingAction;


namespace nexus {

  class DetectorConstruction;
  class PrimaryGeneration;
  

  /// Class factory to create instances of the Geant4 user initialization
  /// classes specified by their name. Notice that this class does not
  /// take responsibility for deleting the allocated object (this is done
  /// actually by the G4RunManager).

  class NexusFactory
  {
  public:
    /// Constructor
    NexusFactory() {}
    /// Destructor
    ~NexusFactory() {}

  public:
    /// Create an instance of the detector initialization class
    DetectorConstruction* CreateDetectorConstruction(const G4String& name);

    /// Create an instance of a physics list
    G4VUserPhysicsList* CreatePhysicsList(const G4String& name);

    /// Create an instance of the primary generation initialization class
    PrimaryGeneration* CreatePrimaryGeneration(const G4String& name);
    
    /// Create an instance of a user action
    G4UserRunAction* CreateRunAction(const G4String& name);
    
    /// Create an instance of a event action
    G4UserEventAction* CreateEventAction(const G4String& name);
    
    /// Create an instance of a tracking action
    G4UserTrackingAction* CreateTrackingAction(const G4String& name);
    
    /// Create an instance of a stepping action
    G4UserSteppingAction* CreateSteppingAction(const G4String& name);
  };

} // namespace nexus

#endif
