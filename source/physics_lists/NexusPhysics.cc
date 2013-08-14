// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martín-Albo
//  Created: 27 Jan 2010
//
//  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "NexusPhysics.h"

#include "IonizationCluster.h"
#include "IonizationElectron.h"
#include "IonizationClustering.h"
#include "IonizationDrift.h"
#include "Electroluminescence.h"

#include <G4ProcessManager.hh>
#include <G4ProcessTable.hh>


namespace nexus {

  
  NexusPhysics::NexusPhysics(const G4String& name): 
    G4VPhysicsConstructor(name)
  {
  }
  
  
  
  NexusPhysics::~NexusPhysics()
  {
  }
  
  
  
  void NexusPhysics::ConstructParticle()
  {
    IonizationCluster::Definition();
    IonizationElectron::Definition();
  }
  
  
  
  void NexusPhysics::ConstructProcess()
  {
    IonizationClustering* clustering = new IonizationClustering();

    IonizationDrift* drift = new IonizationDrift();
    drift->ShootChargesAfterDrift(true);

    theParticleIterator->reset();
    while ((*theParticleIterator)()) {

      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      if (particle == IonizationCluster::Definition() ||
	  particle == IonizationElectron::Definition()) {

	G4VProcess* transp = G4ProcessTable::GetProcessTable()->
	  FindProcess("Transportation", particle);
	pmanager->RemoveProcess(transp);

	pmanager->AddProcess(drift);
	pmanager->SetProcessOrderingToFirst(drift, idxAlongStep);
	pmanager->SetProcessOrderingToFirst(drift, idxPostStep);

	if (particle == IonizationElectron::Definition()) {
	  Electroluminescence* el = new Electroluminescence();
	  pmanager->AddDiscreteProcess(el);
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
