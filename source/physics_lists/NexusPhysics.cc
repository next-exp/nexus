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
#include <G4PhysicsConstructorFactory.hh>




namespace nexus {

  G4_DECLARE_PHYSCONSTR_FACTORY(NexusPhysics);

  NexusPhysics::NexusPhysics(): 
    G4VPhysicsConstructor("NexusPhysics"),
    _drift_el(true)
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
    G4ProcessManager* pmanager = 0;
    
    // Add nexus-defined processes to the ionization electron
    if (_drift_el) {
      pmanager = IonizationElectron::Definition()->GetProcessManager();
      if (!pmanager) {
    	G4Exception("[NexusPhysics]", "ConstructProcess()", FatalException,
		    " ERROR: ionization electron without a process manager");
	return;
      }

      // First, we remove the transportation from the ie- process table
      G4VProcess* transp = G4ProcessTable::GetProcessTable()->
     	FindProcess("Transportation", IonizationElectron::Definition());
      pmanager->RemoveProcess(transp);

      // Second, add drift and electroluminescence
      IonizationDrift* drift = new IonizationDrift();
      pmanager->AddContinuousProcess(drift);
      pmanager->AddDiscreteProcess(drift);

      Electroluminescence* el = new Electroluminescence();
      pmanager->AddDiscreteProcess(el);

      //pmanager->AddProcess(new G4FastSimulationManagerProcess(), -1, 0, 0);
    }
    
    IonizationClustering* clustering = 0;
    if (_drift_el) clustering = new IonizationClustering();
    
    // Loop through all registered particles
    theParticleIterator->reset();
    while ((*theParticleIterator)()) {

      G4ParticleDefinition* particle = theParticleIterator->value();
      pmanager = particle->GetProcessManager();

      if (clustering) {
	if (clustering->IsApplicable(*particle)) {
	  pmanager->AddRestProcess(clustering);
	  pmanager->AddDiscreteProcess(clustering);
	}
      }
    }
  }
  
  
} // end namespace nexus
