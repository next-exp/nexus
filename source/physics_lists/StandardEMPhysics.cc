// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  F. Monrabal <franmon4@ific.uv.es>
//  Created: 27 Jan 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "StandardEMPhysics.h"

#include "IonizationClustering.h"
#include "IonizationCluster.h"
#include "IonizationDrift.h"
#include "IonizationElectron.h"

#include <G4ProcessTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4ProcessManager.hh>
#include <G4LossTableManager.hh>
#include <G4EmProcessOptions.hh>

#include <G4ParticleTypes.hh>

#include <G4ComptonScattering.hh>
#include <G4GammaConversion.hh>
#include <G4PhotoElectricEffect.hh>

#include <G4eMultipleScattering.hh>
#include <G4eIonisation.hh>
#include <G4eBremsstrahlung.hh>
#include <G4eplusAnnihilation.hh>
#include <G4StepLimiter.hh>

#include <G4MuMultipleScattering.hh>
#include <G4MuIonisation.hh>
#include <G4MuBremsstrahlung.hh>
#include <G4MuPairProduction.hh>

#include <G4hMultipleScattering.hh>
#include <G4hIonisation.hh>
#include <G4hBremsstrahlung.hh>
#include <G4hPairProduction.hh>

#include <G4ionIonisation.hh>


namespace nexus {

  
  StandardEMPhysics::StandardEMPhysics(const G4String& name): 
    G4VPhysicsConstructor(name)
  {
    G4LossTableManager::Instance();
  }



  StandardEMPhysics::~StandardEMPhysics()
  {
  }
  
  
  
  void StandardEMPhysics::ConstructParticle()
  {
    G4Gamma::Gamma();
    
    G4Electron::Electron();
    G4Positron::Positron();
    G4MuonPlus::MuonPlus();
    G4MuonMinus::MuonMinus();
    
    G4PionPlus::PionPlusDefinition();
    G4PionMinus::PionMinusDefinition();
    G4KaonPlus::KaonPlusDefinition();
    G4KaonMinus::KaonMinusDefinition();
    
    G4Proton::Proton();
    G4AntiProton::AntiProton();
    
    G4Deuteron::Deuteron();
    G4Triton::Triton();
    G4He3::He3();
    G4Alpha::Alpha();
    G4GenericIon::GenericIonDefinition();
  }
  
  
  
  void StandardEMPhysics::ConstructProcess()
  {
    theParticleIterator->reset();
    while ((*theParticleIterator)()) {

      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String name = particle->GetParticleName();
      
      if (particle == G4Gamma::Definition()) { 
 	pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
	pmanager->AddDiscreteProcess(new G4ComptonScattering);
	pmanager->AddDiscreteProcess(new G4GammaConversion);
      }
      else if (particle == G4Electron::Definition()) {
	pmanager->AddProcess(new G4eMultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
	pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
	pmanager->AddProcess(new G4StepLimiter,        -1,-1, 4);
      }
      else if (particle == G4Positron::Definition()) {
	pmanager->AddProcess(new G4eMultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
	pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
	pmanager->AddProcess(new G4eplusAnnihilation,   0,-1, 4);
      }
      else if (particle == G4MuonPlus::Definition() || 
	       particle == G4MuonMinus::Definition()) {
	pmanager->AddProcess(new G4MuMultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4MuIonisation,        -1, 2, 2);
	pmanager->AddProcess(new G4MuBremsstrahlung,    -1, 3, 3);
	pmanager->AddProcess(new G4MuPairProduction,    -1, 4, 4);
      }
      else if (particle == G4Alpha::Definition() ||
	       particle == G4Triton::Definition() ||
	       particle == G4GenericIon::Definition()) {
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);
      }
      else if (particle == G4PionPlus::Definition() ||
	       particle == G4PionMinus::Definition() ||
	       particle == G4KaonPlus::Definition() ||
	       particle == G4KaonMinus::Definition() ||
	       particle == G4Proton::Definition()) {
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4hIonisation,         -1, 2, 2);
	pmanager->AddProcess(new G4hBremsstrahlung,     -1,-3, 3);
	pmanager->AddProcess(new G4hPairProduction,     -1,-4, 4);
      }
    } 
  }
  

} // end namespace nexus
