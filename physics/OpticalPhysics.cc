// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  F. Monrabal <franmon4@ific.uv.es>
//  Created: 6 Apr 2009
//
//  Copyright (c) 2009,2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "OpticalPhysics.h"

#include <G4LossTableManager.hh>
#include <G4ParticleTypes.hh>
#include <G4ProcessManager.hh>
#include <G4ParticleDefinition.hh>

#include <G4Cerenkov.hh>
#include <G4Scintillation.hh>
#include <Electroluminescence.h>
#include <G4OpAbsorption.hh>
#include <G4OpRayleigh.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4OpWLS.hh>


namespace nexus {


  OpticalPhysics::OpticalPhysics(G4int verbosity, const G4String& name): 
    G4VPhysicsConstructor(name), verbose(verbosity)
  {
    G4LossTableManager::Instance();
  }
  
  
  
  OpticalPhysics::~OpticalPhysics()
  {
  }
  

  
  void OpticalPhysics::ConstructParticle()
  {
    // bosons
    G4Gamma::Gamma();
    G4OpticalPhoton::OpticalPhotonDefinition();
    
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
  
  
  
  void OpticalPhysics::ConstructProcess()
  {
    G4Cerenkov* cerenkov = new G4Cerenkov("Cerenkov");
    cerenkov->SetMaxNumPhotonsPerStep(300);
    cerenkov->SetTrackSecondariesFirst(true);

    G4Scintillation* scintillation = new G4Scintillation("Scintillation");
    scintillation->SetScintillationYieldFactor(1.);
    scintillation->SetTrackSecondariesFirst(true);

    Electroluminescence* el = new Electroluminescence("EL");
    el->SetTrackSecondariesFirst(true);

    G4OpBoundaryProcess* boundary = new G4OpBoundaryProcess();
    G4OpticalSurfaceModel themodel = unified;
    boundary->SetModel(themodel);

    G4OpAbsorption* absorption = new G4OpAbsorption();
    G4OpRayleigh* rayleigh = new G4OpRayleigh();
    G4OpWLS* WLS = new G4OpWLS();

    theParticleIterator->reset();
    while ((*theParticleIterator)()) {

      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String name = particle->GetParticleName();

      if (cerenkov->IsApplicable(*particle)) {
	pmanager->AddProcess(cerenkov);
	pmanager->SetProcessOrdering(cerenkov, idxPostStep);
      }

      if (scintillation->IsApplicable(*particle)) {
 	pmanager->AddProcess(scintillation);
 	pmanager->SetProcessOrderingToLast(scintillation, idxAtRest);
 	pmanager->SetProcessOrderingToLast(scintillation, idxPostStep);
      }
      
      if (el->IsApplicable(*particle)) {
	pmanager->AddProcess(el);
	pmanager->SetProcessOrderingToLast(el, idxAtRest);
	pmanager->SetProcessOrderingToLast(el, idxPostStep);
      }

      if (name == "opticalphoton") {
	pmanager->AddDiscreteProcess(absorption);
	pmanager->AddDiscreteProcess(rayleigh);
	pmanager->AddDiscreteProcess(boundary);
	pmanager->AddDiscreteProcess(WLS);
      }
    }
  }
  
  
} // end namespace nexus
