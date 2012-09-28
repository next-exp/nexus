// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 30 June 2011
//
//  Copyright (c) 2011, 2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "DefaultPhysicsList.h"

#include "ConfigService.h"
#include "NexusPhysics.h"

#include <G4EmStandardPhysics_option3.hh>
#include <G4EmLivermorePhysics.hh>
#include <G4OpticalPhysics.hh>
#include <G4DecayPhysics.hh>
#include <G4StepLimiterBuilder.hh>


namespace nexus {
  
  
  DefaultPhysicsList::DefaultPhysicsList(): G4VModularPhysicsList()
  {
    // Get user configuration parameters for physics
    const ParamStore& cfg = ConfigService::Instance().Physics();

    // Register electromagnetic processes selected by user.
    // Default: use low-energy em extension
    G4int option = 1; 
    if (cfg.PeekIParam("low_energy_em")) 
      option = cfg.GetIParam("low_energy_em");

    if (option == 0) RegisterPhysics(new G4EmStandardPhysics_option3);
    else RegisterPhysics(new G4EmLivermorePhysics);
      
    // Register decay process
    RegisterPhysics(new G4DecayPhysics);

    // Register the step-limiting process
    RegisterPhysics(new G4StepLimiterBuilder);

    // Register optical processes (if selected by user)
    if (cfg.GetIParam("optical")) {
      G4OpticalPhysics* optical = new G4OpticalPhysics();
      optical->SetTrackSecondariesFirst(kScintillation,true);
      optical->SetScintillationYieldFactor(1.);
      optical->SetMaxNumPhotonsPerStep(300);
            
      RegisterPhysics(optical);
    }
    
    // if (cfg.GetIParam("nexus")) {
    //   NexusPhysics* nexus = new NexusPhysics();
    //   nexus->ActivateDriftAndElectroluminescence(true);
    //   // Register nexus physics processes
    //   RegisterPhysics(nexus);
    // }
  }



  DefaultPhysicsList::~DefaultPhysicsList()
  {
  }
  
  
  
  void DefaultPhysicsList::SetCuts()
  {
    // set cut values for gamma at first and for e- second and next for e+,
    // because some processes for e+/e- need cut values for gamma
    SetCutValue(defaultCutValue, "gamma");
    SetCutValue(defaultCutValue, "e-");
    SetCutValue(defaultCutValue, "e+");
  }
  
  
} // end namespace nexus
