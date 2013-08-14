// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 6 Apr 2009
//
//  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "EMStandardPhysicsList.h"

#include "EMStandardPhysics.h"
#include "OpticalPhysics.h"
#include "ConfigService.h"

#include <G4DecayPhysics.hh>


namespace nexus {
  
  
  EMStandardPhysicsList::EMStandardPhysicsList(): G4VModularPhysicsList()
  {
    // electromagnetic physics
    RegisterPhysics(new EMStandardPhysics);

    // particle decay
    RegisterPhysics(new G4DecayPhysics);

    // optical physics
    if (ConfigService::Instance().Physics().GetIParam("optical"))
      RegisterPhysics(new OpticalPhysics);
  }



  EMStandardPhysicsList::~EMStandardPhysicsList()
  {
  }
  
  
  
  void EMStandardPhysicsList::SetCuts()
  {
    // set cut values for gamma at first and for e- second and next for e+,
    // because some processes for e+/e- need cut values for gamma
    SetCutValue(defaultCutValue, "gamma");
    SetCutValue(defaultCutValue, "e-");
    SetCutValue(defaultCutValue, "e+");
  
    if (verboseLevel > 0) DumpCutValuesTable();
  }


} // end namespace nexus
