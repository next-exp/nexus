// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 10 Jan 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "LowEnergyEMPhysics.h"

#include <G4ParticleDefinition.hh>
#include <G4ProcessManager.hh>
#include <G4LossTableManager.hh>
#include <G4EmProcessOptions.hh>

#include <G4ParticleTypes.hh>

#include <G4LowEnergyRayleigh.hh>
#include <G4LowEnergyPhotoElectric.hh>
#include <G4LowEnergyCompton.hh>
#include <G4LowEnergyGammaConversion.hh> 

#include <G4LowEnergyIonisation.hh>
#include <G4LowEnergyBremsstrahlung.hh>

#include <G4MultipleScattering.hh>
#include <G4eMultipleScattering.hh>
#include <G4hMultipleScattering.hh>

#include <G4eIonisation.hh>
#include <G4eBremsstrahlung.hh>
#include <G4eplusAnnihilation.hh>

#include <G4MuMultipleScattering.hh>
#include <G4MuIonisation.hh>
#include <G4MuBremsstrahlung.hh>
#include <G4MuPairProduction.hh>
#include <G4MuonMinusCaptureAtRest.hh>

#include <G4hIonisation.hh>
#include <G4hBremsstrahlung.hh>
#include <G4hPairProduction.hh>

#include <G4ionIonisation.hh>

#include <G4StepLimiter.hh>



namespace nexus {

  
  LowEnergyEMPhysics::LowEnergyEMPhysics(G4int verbosity, const G4String& name): 
    G4VPhysicsConstructor(name), verbose(verbosity)
  {
    G4LossTableManager::Instance();
  }
  
  
  
  LowEnergyEMPhysics::~LowEnergyEMPhysics()
  {
  }
  
  
  
  void LowEnergyEMPhysics::ConstructParticle()
  {
    G4Gamma::Gamma();
    
    // leptons
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
  
  
  
  void LowEnergyEMPhysics::ConstructProcess()
  {
    G4LowEnergyPhotoElectric*  lowePhot = new G4LowEnergyPhotoElectric();
    G4LowEnergyIonisation*     loweIon  = new G4LowEnergyIonisation();
    G4LowEnergyBremsstrahlung* loweBrem = new G4LowEnergyBremsstrahlung();
    
    // fluorescence apply specific cut for fluorescence from photons, electrons
    // and bremsstrahlung photons
    G4double fluorcut = 1.*keV;
    lowePhot->SetCutForLowEnSecPhotons(fluorcut);
    loweIon->SetCutForLowEnSecPhotons(fluorcut);
    loweBrem->SetCutForLowEnSecPhotons(fluorcut);
    
    
    theParticleIterator->reset();
    while ((*theParticleIterator)()) {
      
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
      G4String particleType = particle->GetParticleType();
      G4double charge = particle->GetPDGCharge();
      
      // process ordering: AddProcess(name, at rest, along step, post step)
      // -1 = not implemented, then ordering
      
      if (particleName == "gamma") {
	pmanager->AddDiscreteProcess(new G4LowEnergyRayleigh());
	pmanager->AddDiscreteProcess(lowePhot);
	pmanager->AddDiscreteProcess(new G4LowEnergyCompton());
	pmanager->AddDiscreteProcess(new G4LowEnergyGammaConversion());
      } 
      else if (particleName == "e-") {
	pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(loweIon,                   -1, 2, 2);
	pmanager->AddProcess(loweBrem,                  -1,-1, 3);
	pmanager->AddProcess(new G4StepLimiter,         -1,-1, 4);
      } 
      else if (particleName == "e+") {
	pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(loweIon,                   -1, 2, 2);
	pmanager->AddProcess(loweBrem,                  -1,-1, 3);
	pmanager->AddProcess(new G4eplusAnnihilation(),  0,-1, 4);      
      } 
      else if (particleName == "mu+" || particleName == "mu-") {
	pmanager->AddProcess(new G4MuMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4MuIonisation(),       -1, 2, 2);
	pmanager->AddProcess(new G4MuBremsstrahlung(),   -1,-1, 3);
	pmanager->AddProcess(new G4MuPairProduction(),   -1,-1, 4);
	if (particleName == "mu-")
	  pmanager->AddProcess(new G4MuonMinusCaptureAtRest(), 0,-1,-1);

      } 
      else if (particleName == "proton" ||
	       particleName == "pi-"    ||
	       particleName == "pi+") {
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4hIonisation,         -1, 2, 2);
	pmanager->AddProcess(new G4hBremsstrahlung,     -1,-3, 3);
	pmanager->AddProcess(new G4hPairProduction,     -1,-4, 4);
      }
      else if (particleName == "alpha"      ||
	       particleName == "deuteron"   ||
	       particleName == "triton"     ||
	       particleName == "He3"        ||
	       particleName == "GenericIon" || 
	       (particleType == "nucleus" && charge != 0)) {
	pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);
      } 
      else if ((!particle->IsShortLived()) &&
	       (charge != 0.0) && 
	       (particleName != "chargedgeantino")) {
	pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
	pmanager->AddProcess(new G4hIonisation,        -1, 2, 2);
      }
    }
    
    G4EmProcessOptions opt;
    opt.SetVerbose(verbose);
  }
  
  
} // end namespace nexus
