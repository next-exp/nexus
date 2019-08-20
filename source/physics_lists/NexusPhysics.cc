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

#include <G4Scintillation.hh>
#include <G4GenericMessenger.hh>
#include <G4OpticalPhoton.hh>
#include <G4Gamma.hh>
#include <G4Electron.hh>
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
    _clustering(true), _drift(true), _electroluminescence(true),
    risetime_(false),
    _noCompt(false),  _noCher(false), _noScint(false)
  {
    _msg = new G4GenericMessenger(this, "/PhysicsList/Nexus/",
      "Control commands of the nexus physics list.");

    _msg->DeclareProperty("clustering", _clustering,
      "Switch on/off the ionization clustering");

    _msg->DeclareProperty("drift", _drift,
      "Switch on/off the ionization drift.");

    _msg->DeclareProperty("electroluminescence", _electroluminescence,
      "Switch on/off the electroluminescence.");

    _msg->DeclareProperty("scintRiseTime", risetime_,
      "True if LYSO is used");

    _msg->DeclareProperty("offCompt", _noCompt,
			  "Switch off Compton Scattering.");

    _msg->DeclareProperty("offCherenkov", _noCher,
			  "Switch off Cherenkov.");
    _msg->DeclareProperty("offScintillation", _noScint,
			  "Switch off Scintillation.");
  }



  NexusPhysics::~NexusPhysics()
  {
    delete _msg;
    delete _wls;
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
    //WavelengthShifting* wls = new WavelengthShifting();
    _wls = new WavelengthShifting();
    pmanager->AddDiscreteProcess(_wls);

    // Add rise time to scintillation
    if (risetime_) {
      pmanager  = G4Electron::Definition()->GetProcessManager();
      G4Scintillation*  theScintillationProcess = 
	(G4Scintillation*)G4ProcessTable::GetProcessTable()->
	FindProcess("Scintillation", G4Electron::Definition());
      theScintillationProcess->SetFiniteRiseTime(true);
    }
    
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

      auto aParticleIterator = GetParticleIterator();
      aParticleIterator->reset();
      while ((*aParticleIterator)()) {
        G4ParticleDefinition* particle = aParticleIterator->value();
        pmanager = particle->GetProcessManager();

        if (clust->IsApplicable(*particle)) {
          pmanager->AddDiscreteProcess(clust);
          pmanager->AddRestProcess(clust);
        }
      }
    }

    if (_noCompt) {
      pmanager  = G4Gamma::Definition()->GetProcessManager();
       G4VProcess* cs = G4ProcessTable::GetProcessTable()->
        FindProcess("compt", G4Gamma::Definition());
       pmanager->RemoveProcess(cs);
    }
    if (_noCher) {
       pmanager  = G4Electron::Definition()->GetProcessManager();
       G4VProcess* cs = G4ProcessTable::GetProcessTable()->
        FindProcess("Cerenkov", G4Electron::Definition());
       pmanager->RemoveProcess(cs);
       pmanager  = G4Gamma::Definition()->GetProcessManager();
       cs = G4ProcessTable::GetProcessTable()->
	 FindProcess("Cerenkov", G4Gamma::Definition());
       pmanager->RemoveProcess(cs);
    }
    if (_noScint) {
       pmanager  = G4Electron::Definition()->GetProcessManager();
       G4VProcess* cs = G4ProcessTable::GetProcessTable()->
        FindProcess("Scintillation", G4Electron::Definition());
       pmanager->RemoveProcess(cs);
        pmanager  = G4Gamma::Definition()->GetProcessManager();
       cs = G4ProcessTable::GetProcessTable()->
	 FindProcess("Scintillation", G4Gamma::Definition());
       pmanager->RemoveProcess(cs);
       // pmanager  = G4Alpha::Definition()->GetProcessManager();
       //   cs = G4ProcessTable::GetProcessTable()->
       // 	 FindProcess("Scintillation", G4Alpha::Definition());
       // pmanager->RemoveProcess(cs);
    }

  }



} // end namespace nexus
