// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  <justo.martin-albo@ific.uv.es>
//  Created: 27 Jan 2010
//
//  Copyright (c) 2010-2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "NexusPhysics.h"
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
    G4VPhysicsConstructor("NexusPhysics"), risetime_(false),
    _noCompt(false), _noCher(false), _noScint(false)
  {
    _msg = new G4GenericMessenger(this, "/PhysicsList/Nexus/",
      "Control commands of the nexus physics list.");

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
    G4OpticalPhoton::Definition();
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
