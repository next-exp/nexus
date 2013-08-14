// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 3 June 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "LivermoreEMPhysics.h"

#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4MuonPlus.hh>
#include <G4MuonMinus.hh>
#include <G4PionPlus.hh>
#include <G4PionMinus.hh>
#include <G4KaonPlus.hh>
#include <G4KaonMinus.hh>
#include <G4Proton.hh>
#include <G4AntiProton.hh>
#include <G4Neutron.hh>
#include <G4AntiNeutron.hh>
#include <G4Alpha.hh>
#include <G4Deuteron.hh>
#include <G4Triton.hh>
#include <G4He3.hh>
#include <G4GenericIon.hh>

#include <G4ProcessManager.hh>
#include <G4RayleighScattering.hh>
#include <G4LivermoreRayleighModel.hh>
#include <G4PhotoElectricEffect.hh>
#include <G4LivermorePhotoElectricModel.hh>
#include <G4ComptonScattering.hh>
#include <G4LivermoreComptonModel.hh>
#include <G4GammaConversion.hh>
#include <G4LivermoreGammaConversionModel.hh>
#include <G4eMultipleScattering.hh>
#include <G4UrbanMscModel93.hh>
#include <G4eIonisation.hh>
#include <G4LivermoreIonisationModel.hh>
#include <G4UniversalFluctuation.hh>
#include <G4eBremsstrahlung.hh>
#include <G4LivermoreBremsstrahlungModel.hh>
#include <G4eplusAnnihilation.hh>
#include <G4StepLimiter.hh>
#include <G4MuMultipleScattering.hh>
#include <G4MuIonisation.hh>
#include <G4MuBremsstrahlung.hh>
#include <G4MuPairProduction.hh>
#include <G4CoulombScattering.hh>
#include <G4MuonMinusCaptureAtRest.hh>
#include <G4hMultipleScattering.hh>
#include <G4hIonisation.hh>
#include <G4hBremsstrahlung.hh>
#include <G4hPairProduction.hh>
#include <G4ionIonisation.hh>
#include <G4NuclearStopping.hh>
#include <G4IonParametrisedLossModel.hh>


namespace nexus {
  
  
  LivermoreEMPhysics::LivermoreEMPhysics(const G4String& name): 
    G4VPhysicsConstructor(name)
  {
    SetVerboseLevel(0);
  }
  
  
  
  LivermoreEMPhysics::~LivermoreEMPhysics()
  {
  }
  
  
  
  void LivermoreEMPhysics::ConstructParticle()
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
    G4Neutron::NeutronDefinition();
    G4AntiNeutron::AntiNeutronDefinition();

    G4Alpha::Alpha();
    G4Deuteron::Deuteron();
    G4Triton::Triton();
    G4He3::He3();
    G4GenericIon::GenericIonDefinition();
  }
  
  
  
  void LivermoreEMPhysics::ConstructProcess()
  {
    G4double cut = 1.*keV;
    

    theParticleIterator->reset();
    
    while ((*theParticleIterator)()) {
      
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      if (particle == G4Gamma::Definition()) { 

	G4PhotoElectricEffect* photoel = new G4PhotoElectricEffect();
	G4LivermorePhotoElectricModel* photoel_livermore =
	  new G4LivermorePhotoElectricModel();
	photoel_livermore->ActivateAuger(true);
	photoel_livermore->SetCutForLowEnSecPhotons(cut);
	photoel_livermore->SetCutForLowEnSecElectrons(cut);
	photoel->AddEmModel(0, photoel_livermore);
	pmanager->AddDiscreteProcess(photoel);

	G4ComptonScattering* compton = new G4ComptonScattering();
	compton->AddEmModel(0, new G4LivermoreComptonModel());
	pmanager->AddDiscreteProcess(compton);

	G4RayleighScattering* rayleigh = new G4RayleighScattering();
	rayleigh->AddEmModel(0, new G4LivermoreRayleighModel());
	pmanager->AddDiscreteProcess(rayleigh);

	G4GammaConversion* conv = new G4GammaConversion();
	conv->SetModel(new G4LivermoreGammaConversionModel());
	pmanager->AddDiscreteProcess(conv);
      } 
      
      else if (particle == G4Electron::Definition()) {
	
	G4eMultipleScattering* msc = new G4eMultipleScattering();
	msc->AddEmModel(0, new G4UrbanMscModel93());
	msc->SetStepLimitType(fUseDistanceToBoundary);
	pmanager->AddProcess(msc, -1, 1, 1);
	
	G4eIonisation* eioni = new G4eIonisation();
	G4LivermoreIonisationModel* eioni_livermore = 
	  new G4LivermoreIonisationModel();
	eioni_livermore->SetDeexcitationFlag(true);
	eioni_livermore->ActivateAuger(true);
	eioni->AddEmModel(0, eioni_livermore, new G4UniversalFluctuation());
	eioni->SetStepFunction(0.2, 100*um);
	pmanager->AddProcess(eioni, -1, 2, 2);

	G4eBremsstrahlung* ebremss = new G4eBremsstrahlung();
	ebremss->AddEmModel(0, new G4LivermoreBremsstrahlungModel());
	pmanager->AddProcess(ebremss, -1,-3, 3);

	pmanager->AddProcess(new G4StepLimiter(), -1,-1, 4);
      }

      else if (particle == G4Positron::Definition()) { 

	G4eMultipleScattering* msc = new G4eMultipleScattering();
	msc->AddEmModel(0, new G4UrbanMscModel93());
	msc->SetStepLimitType(fUseDistanceToBoundary);
	pmanager->AddProcess(msc, -1, 1, 1);

	G4eIonisation * eioni = new G4eIonisation();
	eioni->SetStepFunction(0.2, 50*um);
	pmanager->AddProcess(eioni, -1, 2, 2);

	pmanager->AddProcess(new G4eBremsstrahlung(), -1, -1, 3);
	pmanager->AddProcess(new G4eplusAnnihilation(), 0, -1, 4);
      }
      
      else if (particle == G4MuonPlus::Definition() || 
	       particle == G4MuonMinus::Definition()) { 
	
	pmanager->AddProcess(new G4MuMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4MuIonisation(),       -1, 2, 2);
	pmanager->AddProcess(new G4MuBremsstrahlung(),   -1,-1, 3);
	pmanager->AddProcess(new G4MuPairProduction(),   -1,-1, 4);
	pmanager->AddDiscreteProcess(new G4CoulombScattering());
      }

      else if (particle == G4Proton::Definition() || 
	       particle == G4PionPlus::Definition() || 
	       particle == G4PionMinus::Definition()) { 
	
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4hIonisation,         -1, 2, 2);
	pmanager->AddProcess(new G4hBremsstrahlung,     -1,-3, 3);
	pmanager->AddProcess(new G4hPairProduction,     -1,-4, 4);
      }

      else if (particle == G4Alpha::Definition() || 
	       particle == G4Triton::Definition() ||
	       particle == G4Deuteron::Definition() || 
	       particle == G4Triton::Definition()) {
	
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);
	pmanager->AddProcess(new G4NuclearStopping(),   -1, 3,-1);
      }

      else if (particle == G4GenericIon::Definition()) { 
	
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	
	G4ionIonisation* ionisation = new G4ionIonisation();
	ionisation->SetEmModel(new G4IonParametrisedLossModel());
	pmanager->AddProcess(ionisation, -1, 2, 2);

	pmanager->AddProcess(new G4NuclearStopping(), -1, 3, -1);
      }
    }
  }
    
  
  
} // end namespace nexus
