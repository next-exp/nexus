// ----------------------------------------------------------------------------
// nexus | NexusPhysics.cc
//
// This class registers any new physics process defined in nexus.
//
// The NEXT Collaboration
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
    clustering_(true), drift_(true), electroluminescence_(true)
  {
    msg_ = new G4GenericMessenger(this, "/PhysicsList/Nexus/",
      "Control commands of the nexus physics list.");

    msg_->DeclareProperty("clustering", clustering_,
      "Switch on/off the ionization clustering");

    msg_->DeclareProperty("drift", drift_,
      "Switch on/off the ionization drift.");

    msg_->DeclareProperty("electroluminescence", electroluminescence_,
      "Switch on/off the electroluminescence.");
  }



  NexusPhysics::~NexusPhysics()
  {
    delete msg_;
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
      G4Exception("[NexusPhysics]", "ConstructProcess()", FatalException,
        "G4OpticalPhoton without a process manager.");
    }
    WavelengthShifting* wls = new WavelengthShifting();
    pmanager->AddDiscreteProcess(wls);

    pmanager = IonizationElectron::Definition()->GetProcessManager();
    if (!pmanager) {
      G4Exception("[NexusPhysics]", "ConstructProcess()", FatalException,
        "Ionization electron without a process manager.");
    }

    // Add drift and electroluminescence to the process table of the ie-

    if (drift_) {
      // First, we remove the standard transportation from the
      // process table of the ionization electron
      G4VProcess* transportation = G4ProcessTable::GetProcessTable()->
        FindProcess("Transportation", IonizationElectron::Definition());
      pmanager->RemoveProcess(transportation);

      IonizationDrift* drift = new IonizationDrift();
      pmanager->AddContinuousProcess(drift);
      pmanager->AddDiscreteProcess(drift);
    }

    if (electroluminescence_) {
      Electroluminescence* el = new Electroluminescence();
      pmanager->AddDiscreteProcess(el);
    }


    // Add clustering to all pertinent particles

    if (clustering_) {

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

  }



} // end namespace nexus
