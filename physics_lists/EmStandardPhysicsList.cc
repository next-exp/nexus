//
//  EmStandardPhysicsList.h
//
//     Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 6 Apr 2009
//     Updated: 
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#include "EmStandardPhysicsList.h"

#include <G4ProcessManager.hh>
#include <G4ParticleTypes.hh>

#include <G4ComptonScattering.hh>
#include <G4GammaConversion.hh>
#include <G4PhotoElectricEffect.hh>

#include <G4eMultipleScattering.hh>
#include <G4hMultipleScattering.hh>

#include <G4eIonisation.hh>
#include <G4eBremsstrahlung.hh>
#include <G4eplusAnnihilation.hh>

#include <G4MuIonisation.hh>
#include <G4MuBremsstrahlung.hh>
#include <G4MuPairProduction.hh>

#include <G4hIonisation.hh>
#include <G4hBremsstrahlung.hh>
#include <G4hPairProduction.hh>

#include <G4ionIonisation.hh>

#include <G4Decay.hh>


namespace nexus {


  EmStandardPhysicsList::EmStandardPhysicsList():G4VUserPhysicsList()
  {
    SetVerboseLevel(1);
  }
  
  
  
  void EmStandardPhysicsList::ConstructParticle()
  {
    ConstructBosons();
    ConstructLeptons();
    ConstructMesons();
    ConstructBaryons();
  }
  
  
  
  void EmStandardPhysicsList::ConstructBosons()
  {
    // pseudo-particles
    G4Geantino::GeantinoDefinition();
    G4ChargedGeantino::ChargedGeantinoDefinition();

    // gamma
    G4Gamma::GammaDefinition();

    // optical photon
    //G4OpticalPhoton::OpticalPhotonDefinition();
  }



  void EmStandardPhysicsList::ConstructLeptons()
  {
    // leptons
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    G4MuonPlus::MuonPlusDefinition();
    G4MuonMinus::MuonMinusDefinition();
    
    G4NeutrinoE::NeutrinoEDefinition();
    G4AntiNeutrinoE::AntiNeutrinoEDefinition();
    G4NeutrinoMu::NeutrinoMuDefinition();
    G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
  }



  void EmStandardPhysicsList::ConstructMesons()
  {
    //  mesons
    G4PionPlus::PionPlusDefinition();
    G4PionMinus::PionMinusDefinition();
    G4PionZero::PionZeroDefinition();
    G4Eta::EtaDefinition();
    G4EtaPrime::EtaPrimeDefinition();
    G4KaonPlus::KaonPlusDefinition();
    G4KaonMinus::KaonMinusDefinition();
    G4KaonZero::KaonZeroDefinition();
    G4AntiKaonZero::AntiKaonZeroDefinition();
    G4KaonZeroLong::KaonZeroLongDefinition();
    G4KaonZeroShort::KaonZeroShortDefinition();
  }
  
  
  
  void EmStandardPhysicsList::ConstructBaryons()
  {
    //  barions
    G4Proton::ProtonDefinition();
    G4AntiProton::AntiProtonDefinition();
    G4Neutron::NeutronDefinition();
    G4AntiNeutron::AntiNeutronDefinition();
  }
  
  
  
  void EmStandardPhysicsList::ConstructProcess()
  {
    AddTransportation();
    ConstructEM();
    ConstructDecay();
  }
  
  

  void EmStandardPhysicsList::ConstructEM()
  {
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String particle_name = particle->GetParticleName();
    
      if (particle_name == "gamma") {
	// gamma         
	pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
	pmanager->AddDiscreteProcess(new G4ComptonScattering);
	pmanager->AddDiscreteProcess(new G4GammaConversion);
      
      } else if (particle_name == "e-") {
	//electron
	pmanager->AddProcess(new G4eMultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
	pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);      

      } else if (particle_name == "e+") {
	//positron
	pmanager->AddProcess(new G4eMultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
	pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
	pmanager->AddProcess(new G4eplusAnnihilation,   0,-1, 4);
    
      } else if (particle_name == "mu+" || 
		 particle_name == "mu-") {
	//muon  
	pmanager->AddProcess(new G4hMultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4MuIonisation,       -1, 2, 2);
	pmanager->AddProcess(new G4MuBremsstrahlung,   -1, 3, 3);
	pmanager->AddProcess(new G4MuPairProduction,   -1, 4, 4);
             
      } else if (particle_name == "proton" ||
		 particle_name == "pi-" ||
		 particle_name == "pi+") {
	//proton  
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4hIonisation,         -1, 2, 2);
	pmanager->AddProcess(new G4hBremsstrahlung,     -1, 3, 3);
	pmanager->AddProcess(new G4hPairProduction,     -1, 4, 4);       
     
      } else if (particle_name == "alpha" || 
		 particle_name == "He3" || 
		 particle_name == "GenericIon" ) {
	//Ions 
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);
      
      } else if ((!particle->IsShortLived()) &&
		 (particle->GetPDGCharge() != 0.0) && 
		 (particle->GetParticleName() != "chargedgeantino")) {
	//all others charged particles except geantino
	pmanager->AddProcess(new G4hMultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4hIonisation,        -1, 2, 2);        
      }     
    }
  }



  void EmStandardPhysicsList::ConstructDecay()
  {
    // Add Decay Process
    G4Decay* theDecayProcess = new G4Decay();
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      if (theDecayProcess->IsApplicable(*particle)) { 
	pmanager ->AddProcess(theDecayProcess);
	// set ordering for PostStepDoIt and AtRestDoIt
	pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
	pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
      }
    }
  }



  void EmStandardPhysicsList::SetCuts()
  {
    if (verboseLevel >0){
      G4cout << "EmStandardPhysicsList::SetCuts:";
      G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
    }
  
    // set cut values for gamma at first and for e- second and next for e+,
    // because some processes for e+/e- need cut values for gamma
    //
    SetCutValue(defaultCutValue, "gamma");
    SetCutValue(defaultCutValue, "e-");
    SetCutValue(defaultCutValue, "e+");
  
    if (verboseLevel>0) DumpCutValuesTable();
  }


} // end namespace nexus
