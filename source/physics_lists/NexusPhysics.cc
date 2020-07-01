// ----------------------------------------------------------------------------
// nexus | NexusPhysics.cc
//
// This class registers any new physics process defined in nexus.
//
// The NEXT Collaboration
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
    G4VPhysicsConstructor("NexusPhysics"), risetime_(false), noCompt_(false)
  {
    msg_ = new G4GenericMessenger(this, "/PhysicsList/Nexus/",
      "Control commands of the nexus physics list.");

    msg_->DeclareProperty("scintRiseTime", risetime_,
      "True if LYSO is used");

    msg_->DeclareProperty("offCompt", noCompt_,
			  "Switch off Compton Scattering.");
  }



  NexusPhysics::~NexusPhysics()
  {
    delete msg_;
    delete wls_;
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
    wls_ = new WavelengthShifting();
    pmanager->AddDiscreteProcess(wls_);

    // Add rise time to scintillation
    if (risetime_) {
      pmanager  = G4Electron::Definition()->GetProcessManager();
      G4Scintillation*  theScintillationProcess =
	(G4Scintillation*)G4ProcessTable::GetProcessTable()->
	FindProcess("Scintillation", G4Electron::Definition());
      theScintillationProcess->SetFiniteRiseTime(true);
    }


    if (noCompt_) {
      pmanager  = G4Gamma::Definition()->GetProcessManager();
       G4VProcess* cs = G4ProcessTable::GetProcessTable()->
        FindProcess("compt", G4Gamma::Definition());
       pmanager->RemoveProcess(cs);
    }

  }



} // end namespace nexus
