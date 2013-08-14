// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 6 Apr 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NexusPhysicsList.h"

#include "ConfigService.h"
#include "LivermoreEMPhysics.h"
#include "StandardEMPhysics.h"
#include "NexusPhysics.h"
#include "OpticalPhysics.h"

#include <G4DecayPhysics.hh>
#include <G4RegionStore.hh>
#include <G4Region.hh>
#include <G4ProcessTable.hh>
#include <G4ParticleDefinition.hh>


namespace nexus {
  
  
  NexusPhysicsList::NexusPhysicsList(): 
    G4VModularPhysicsList()
					
  {
    const ParamStore& cfg = ConfigService::Instance().Physics();
    
    // register electromagnetic physics chosen by the user
    // (the default is livermore)
    G4int option = LIVERMORE;
    if (cfg.PeekIParam("em_physics")) 
      option = cfg.GetIParam("em_physics");
    
    switch (option) {
    case STANDARD:
      RegisterPhysics(new StandardEMPhysics);
      break;
    default:
      RegisterPhysics(new LivermoreEMPhysics);
    }
    
    // register decay process
    RegisterPhysics(new G4DecayPhysics);
    
    // register nexus physics processes
    RegisterPhysics(new NexusPhysics);

    if (cfg.GetIParam("optical"))
      RegisterPhysics(new OpticalPhysics);
  }



  NexusPhysicsList::~NexusPhysicsList()
  {
  }
  
  
  
  void NexusPhysicsList::SetCuts()
  {
    // set cut values for gamma at first and for e- second and next for e+,
    // because some processes for e+/e- need cut values for gamma
    SetCutValue(defaultCutValue, "gamma");
    SetCutValue(defaultCutValue, "e-");
    SetCutValue(defaultCutValue, "e+");
  
    if (verboseLevel > 0) DumpCutValuesTable();
  }
  
  
} // end namespace nexus
