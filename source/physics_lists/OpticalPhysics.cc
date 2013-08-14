// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martín-Albo <jmalbos@ific.uv.es>
//  Created: 27 Jan 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "OpticalPhysics.h"

#include <G4OpticalPhoton.hh>
#include <G4ProcessManager.hh>
#include <G4OpAbsorption.hh>
#include <G4OpRayleigh.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4OpWLS.hh>
#include <G4ParticleDefinition.hh>
#include <G4Cerenkov.hh>
#include <G4Scintillation.hh>


namespace nexus {


  OpticalPhysics::OpticalPhysics(const G4String& name): 
    G4VPhysicsConstructor(name)
  {
  }
  
  
  
  OpticalPhysics::~OpticalPhysics()
  {
  }
  
  
  
  void OpticalPhysics::ConstructParticle()
  {
    G4OpticalPhoton::OpticalPhotonDefinition();
  }
  
  
  
  void OpticalPhysics::ConstructProcess()
  {
    // Add processes to the optical photon
    
    G4ProcessManager* pmanager = 0;
    pmanager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

    G4OpAbsorption* absorp = new G4OpAbsorption();
    pmanager->AddDiscreteProcess(absorp);

    G4OpRayleigh* rayleigh = new G4OpRayleigh();
    pmanager->AddDiscreteProcess(rayleigh);

    G4OpWLS* wls = new G4OpWLS();
    pmanager->AddDiscreteProcess(wls);

    G4OpBoundaryProcess* boundary = new G4OpBoundaryProcess();
    boundary->SetModel(unified);
    pmanager->AddDiscreteProcess(boundary);

    
    // Add scintillation and Cerenkov to charged particles

    G4Scintillation* scint = new G4Scintillation();
    scint->SetScintillationYieldFactor(1.);
    scint->SetTrackSecondariesFirst(true);
    //scint->SetScintillationExcitationRatio();

    G4Cerenkov* cerenkov = new G4Cerenkov();
    cerenkov->SetMaxNumPhotonsPerStep(300);
    cerenkov->SetTrackSecondariesFirst(true);
    //cerenkov->SetMaxBetaChangePerStep();

    theParticleIterator->reset();
    while ((*theParticleIterator)()) {
      
      G4ParticleDefinition* particle = theParticleIterator->value();
      pmanager = particle->GetProcessManager();

      if (cerenkov->IsApplicable(*particle)) {
	pmanager->AddProcess(cerenkov);
	pmanager->SetProcessOrdering(cerenkov, idxPostStep);
      }

      if (scint->IsApplicable(*particle)) {
	pmanager->AddProcess(scint);
	pmanager->SetProcessOrderingToLast(scint, idxAtRest);
	pmanager->SetProcessOrderingToLast(scint, idxPostStep);
      }
    }
  }
  
} // end namespace nexus
