// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 2 December 2009
//
//  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "LowEnergyEMPhysicsList.h"

#include "LowEnergyEMPhysics.h"
#include "OpticalPhysics.h"
#include "ConfigService.h"

#include <G4DecayPhysics.hh>


namespace nexus {


  LowEnergyEMPhysicsList::LowEnergyEMPhysicsList(): G4VModularPhysicsList() 
  {
    // electromagnetic physics
    RegisterPhysics(new LowEnergyEMPhysics);

    // particle decay
    RegisterPhysics(new G4DecayPhysics);

    // optical physics
    if (ConfigService::Instance().Physics().GetIParam("optical"))
      RegisterPhysics(new OpticalPhysics);
  }


  
  void LowEnergyEMPhysicsList::SetCuts() 
  {
    //special for low energy physics
    G4double lowlimit = 1.*keV;  
    G4ProductionCutsTable::GetProductionCutsTable()->
      SetEnergyRange(lowlimit, 100.*GeV);
    
    // set cut values for gamma at first and for e- second and next for e+,
    // because some processes for e+/e- need cut values for gamma 
    SetCutValue(5.*mm, "gamma");
    SetCutValue(5.*mm, "e-");
    SetCutValue(5.*mm, "e+");
  }
  

} // end namespace nexus
