// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 27 Jan 2010
//
//  Copyright (c) 2010-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "NexusPhysics.h"

#include "IonizationElectron.h"
#include "IonizationClustering.h"
#include "IonizationDrift.h"
#include "Electroluminescence.h"

#include <G4ProcessManager.hh>
#include <G4ProcessTable.hh>
#include <G4StepLimiter.hh>
#include <G4FastSimulationManagerProcess.hh>


namespace nexus {

  
  NexusPhysics::NexusPhysics(const G4String& name): G4VPhysicsConstructor(name)
  {
  }
  
  
  
  NexusPhysics::~NexusPhysics()
  {
  }
  
  
  
  void NexusPhysics::ConstructParticle()
  {
    IonizationElectron::Definition();
  }
  
  
  
  void NexusPhysics::ConstructProcess()
  {
    IonizationClustering* clustering = new IonizationClustering();
    
    IonizationDrift* drift = new IonizationDrift();
    
    G4StepLimiter* steplimit = new G4StepLimiter();

    theParticleIterator->reset();
    while ((*theParticleIterator)()) {

      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      // if (particle->GetPDGCharge() != 0.) {
      // 	pmanager->AddDiscreteProcess(steplimit);
      // }
      
      if (particle == IonizationElectron::Definition()) {
	
	G4VProcess* transp = G4ProcessTable::GetProcessTable()->
	  FindProcess("Transportation", particle);
	pmanager->RemoveProcess(transp);

	pmanager->AddProcess(drift);
	pmanager->SetProcessOrderingToFirst(drift, idxAlongStep);
	pmanager->SetProcessOrderingToFirst(drift, idxPostStep);

	if (particle == IonizationElectron::Definition()) {
	  Electroluminescence* el = new Electroluminescence();
	  pmanager->AddDiscreteProcess(el);
	  //pmanager->AddProcess(new G4FastSimulationManagerProcess(), -1, 0, 0);
	}
      }
      else if (clustering->IsApplicable(*particle)) {
	pmanager->AddProcess(clustering);
	pmanager->SetProcessOrderingToLast(clustering, idxAtRest);
	pmanager->SetProcessOrderingToLast(clustering, idxPostStep);
      }
    }
  }
  
  
} // end namespace nexus
