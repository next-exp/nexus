// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 27 Jan 2010
//
//  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "NexusPhysics.h"

#include "IonizationElectron.h"
#include "IonizationClustering.h"
#include "IonizationDrift.h"
#include "Electroluminescence.h"
#include "WavelengthShifting.h"

#include <G4GenericMessenger.hh>
#include <G4OpticalPhoton.hh>
#include <G4ProcessManager.hh>
#include <G4ProcessTable.hh>
#include <G4StepLimiter.hh>
#include <G4FastSimulationManagerProcess.hh>
#include <G4PhysicsConstructorFactory.hh>


namespace nexus {

  /// Macro that allows the use of this physics constructor 
  /// with the generic physics list
  G4_DECLARE_PHYSCONSTR_FACTORY(NexusPhysics);



  NexusPhysics::NexusPhysics(): 
    G4VPhysicsConstructor("NexusPhysics"), 
    _clustering(true), _drift(true), _electroluminescence(true)
  {
    _msg = new G4GenericMessenger(this, "/PhysicsList/Nexus/",
      "Control commands of the nexus physics list.");

    _msg->DeclareProperty("clustering", _clustering,
      "Switch on/off the ionization clustering");

    _msg->DeclareProperty("drift", _drift,
      "Switch on/off the ionization drift.");

    _msg->DeclareProperty("electroluminescence", _electroluminescence,
      "Switch on/off the electroluminescence.");
  }
  
  
  
  NexusPhysics::~NexusPhysics()
  {
    delete _msg;
  }
  
  
  
  void NexusPhysics::ConstructParticle()
  {
    IonizationElectron::Definition();
    G4OpticalPhoton::Definition();
    //G4OpticalPhoton::OpticalPhotonDefinition();
  }
  
  
  
  void NexusPhysics::ConstructProcess()
  {
    G4ProcessManager* pmanager = 0;
    
    // Add our own wavelength shifting process for the optical photon
    pmanager = G4OpticalPhoton::Definition()->GetProcessManager();
    if (!pmanager) {
      G4Exception("ConstructProcess()", "[NexusPhysics]", FatalException, 
        "G4OpticalPhoton without a process manager.");
    }
    WavelengthShifting* wls = new WavelengthShifting();
    pmanager->AddDiscreteProcess(wls);

    pmanager = IonizationElectron::Definition()->GetProcessManager();
    if (!pmanager) {
      G4Exception("ConstructProcess()", "[NexusPhysics]", FatalException, 
        "Ionization electron without a process manager.");
    }

    // Add drift and electroluminescence to the process table of the ie-

    if (_drift) {
      // First, we remove the standard transportation from the
      // process table of the ionization electron
      G4VProcess* transportation = G4ProcessTable::GetProcessTable()->
        FindProcess("Transportation", IonizationElectron::Definition());
      pmanager->RemoveProcess(transportation);

      IonizationDrift* drift = new IonizationDrift();
      pmanager->AddContinuousProcess(drift);
      pmanager->AddDiscreteProcess(drift);
    }

    if (_electroluminescence) {
      Electroluminescence* el = new Electroluminescence();
      pmanager->AddDiscreteProcess(el);
    }


    // Add clustering to all pertinent particles
    
    if (_clustering) {

      IonizationClustering* clust = new IonizationClustering();

      theParticleIterator->reset();
      while ((*theParticleIterator)()) {
        G4ParticleDefinition* particle = theParticleIterator->value();
        pmanager = particle->GetProcessManager();

        if (clust->IsApplicable(*particle)) {
          pmanager->AddDiscreteProcess(clust);
          pmanager->AddRestProcess(clust);
        }
      }
    }

  }



} // end namespace nexus
