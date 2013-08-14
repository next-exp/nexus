// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  F. Monrabal <franmon4@ific.uv.es>
//  Created: 27 Jan 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "EMStandardPhysics.h"

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

  
  EMStandardPhysics::EMStandardPhysics(G4int verbosity, const G4String& name): 
    G4VPhysicsConstructor(name), _verbose(verbosity)
  {
    G4LossTableManager::Instance();
  }



  EMStandardPhysics::~EMStandardPhysics()
  {
  }
  
  
  
  void EMStandardPhysics::ConstructParticle()
  {
    // gamma
    G4Gamma::Gamma();
    
    //leptons
    G4Electron::Electron();
    G4Positron::Positron();
    G4MuonPlus::MuonPlus();
    G4MuonMinus::MuonMinus();
    
    // mesons
    G4PionPlus::PionPlusDefinition();
    G4PionMinus::PionMinusDefinition();
    G4KaonPlus::KaonPlusDefinition();
    G4KaonMinus::KaonMinusDefinition();
    
    // barions
    G4Proton::Proton();
    G4AntiProton::AntiProton();
    
    // ions
    G4Deuteron::Deuteron();
    G4Triton::Triton();
    G4He3::He3();
    G4Alpha::Alpha();
    G4GenericIon::GenericIonDefinition();
  }
  
  
  
  void EMStandardPhysics::ConstructProcess()
  {
    theParticleIterator->reset();
    while ((*theParticleIterator)()) {

      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String name = particle->GetParticleName();

      if (_verbose > 1) {
	G4cout << "### " << GetPhysicsName() << " instantiates for " 
	       << name << G4endl;
      }
      
      if (name == "gamma") {
 	pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
	pmanager->AddDiscreteProcess(new G4ComptonScattering);
	pmanager->AddDiscreteProcess(new G4GammaConversion);
      }
      else if (name == "e-") {
	pmanager->AddProcess(new G4eMultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
	pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
      }
      else if (name == "e+") {
	pmanager->AddProcess(new G4eMultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
	pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
	pmanager->AddProcess(new G4eplusAnnihilation,   0,-1, 4);
      }
      else if (name == "mu+" || name == "mu-") {
	pmanager->AddProcess(new G4MuMultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4MuIonisation,        -1, 2, 2);
	pmanager->AddProcess(new G4MuBremsstrahlung,    -1, 3, 3);
	pmanager->AddProcess(new G4MuPairProduction,    -1, 4, 4);
      }
      else if (name == "alpha" || name == "He3" ||
	       name == "GenericIon") {
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);
      }
      else if (name == "pi+" || name == "pi-" ||
	       name == "kaon+" || name == "kaon-" ||
	       name == "proton" ) {
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4hIonisation,         -1, 2, 2);
	pmanager->AddProcess(new G4hBremsstrahlung,     -1,-3, 3);
	pmanager->AddProcess(new G4hPairProduction,     -1,-4, 4);
      }
      else if (name == "anti_omega-" ||
	       name == "anti_proton" ||
	       name == "anti_sigma+" ||
	       name == "anti_sigma-" ||
	       name == "anti_xi-" ||
	       name == "deuteron" ||
	       name == "kaon+" ||
	       name == "kaon-" ||
	       name == "omega-" ||
	       name == "pi+" ||
	       name == "pi-" ||
	       name == "proton" ||
	       name == "sigma+" ||
	       name == "sigma-" ||
	       name == "tau+" ||
	       name == "tau-" ||
	       name == "triton" ||
	       name == "xi-" ) {
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4hIonisation,         -1, 2, 2);
      }
    } 
    
    G4EmProcessOptions opt;
    opt.SetVerbose(_verbose);
  }
  

} // end namespace nexus
