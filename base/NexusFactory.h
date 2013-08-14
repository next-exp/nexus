//
//  NexusFactory.h
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 15 Apr 2009
//     Updated: 30 Apr 2009  
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

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
  
  class NexusFactory
  {
  public:
    /// Constructor
    NexusFactory() {}
    /// Destructor
    ~NexusFactory() {}

    PrimaryGeneration* CreatePrimaryGeneration(const G4String&);
    
    DetectorConstruction* CreateDetectorConstruction(const G4String&);

    G4VUserPhysicsList* CreatePhysicsList(const G4String&);

    G4UserRunAction* CreateRunAction(const G4String&);

    G4UserEventAction* CreateEventAction(const G4String&);

    G4UserTrackingAction* CreateTrackingAction(const G4String&);
    
    G4UserSteppingAction* CreateSteppingAction(const G4String&);
  };

} // namespace nexus

#endif
