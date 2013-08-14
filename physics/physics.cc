// NEXUS: physics.cc
//    -- Last modification: July 7 2007, jmalbos
//

#include <nexus/physics.h>
#include <nexus/geometry.h>

#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4UnitsTable.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4MultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"

#include "G4UserSpecialCuts.hh"
#include "G4StepLimiter.hh"

#include "G4Decay.hh"

#include "G4ProductionCuts.hh"
#include "G4RegionStore.hh"



namespace nexus {


  physics::physics( const bhep::gstore& physics_store, bhep::prlevel vl ):  G4VUserPhysicsList()
  {
    __physics_store = new bhep::gstore(physics_store);
    
    msg = bhep::messenger(vl);
    msg.message( "[nexus::physics] Class instance created.", bhep::DUMP );
    
    initialize();
  }
  
  
  physics::~physics()
  {
    delete __physics_store;
    msg.message( "[nexus::physics] Class instance deleted.", bhep::DUMP );
  }


  void physics::initialize()
  {
    msg.message("\n", bhep::NORMAL);
    msg.message("-------------------------------------------------", bhep::NORMAL);
    msg.message("  INITIALIZING PHYSICS                           ", bhep::NORMAL);
    msg.message("-------------------------------------------------", bhep::NORMAL);
    
    SetVerboseLevel(0);
    
    msg.message("  Reading input parameters.", bhep::VERBOSE);
    read_params();

    msg.message("-------------------------------------------------", bhep::VERBOSE);
  }


  void physics::read_params()
  {
    __energy_loss     = __physics_store->fetch_istore("energy_loss");
    __mult_scattering = __physics_store->fetch_istore("mult_scattering");
    __bulk_cut        = __physics_store->fetch_dstore("bulk_cut") * mm;
    
    //defaultCutValue = 1.0e-3*mm;  
    
    msg.message("     + Electrons Energy Losses:", __energy_loss, bhep::VERBOSE);
    msg.message("     + Electrons Multiple Scatering:", __mult_scattering, bhep::VERBOSE);
    msg.message("     + Bulk Region cut set to:", __bulk_cut/mm, "mm", bhep::VERBOSE);
    //msg.message("     + Default Cut Length:", G4BestUnit(defaultCutValue, "Length"), bhep::VERBOSE);
  }
  

  void physics::ConstructParticle()
  {
    // bosons
    ConstructBosons();

    // leptons
    ConstructLeptons();

    // mesons
    G4MesonConstructor mCtor;
    mCtor.ConstructParticle();

    // baryons
    G4BaryonConstructor bCtor;
    bCtor.ConstructParticle();

    // ions
    G4IonConstructor iCtor;
    iCtor.ConstructParticle();
  }


  void physics::ConstructBosons()
  {
    // pseudo-particles
    G4Geantino::GeantinoDefinition();
    G4ChargedGeantino::ChargedGeantinoDefinition();

    // gamma
    G4Gamma::GammaDefinition();
  }


  void physics::ConstructLeptons()
  {
    // leptons
    // e+/-
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    // mu+/-
    G4MuonPlus::MuonPlusDefinition();
    G4MuonMinus::MuonMinusDefinition();
    // nu_e
    G4NeutrinoE::NeutrinoEDefinition();
    G4AntiNeutrinoE::AntiNeutrinoEDefinition();
    // nu_mu
    G4NeutrinoMu::NeutrinoMuDefinition();
    G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
  }


  void physics::ConstructMesons()
  {
    // mesons
    // light mesons
    G4PionPlus::PionPlusDefinition();
    G4PionMinus::PionMinusDefinition();
    G4PionZero::PionZeroDefinition();
    G4Eta::EtaDefinition();
    G4EtaPrime::EtaPrimeDefinition();
    G4KaonPlus::KaonPlusDefinition();
    G4KaonMinus::KaonMinusDefinition();
    G4KaonZero::KaonZeroDefinition();
    G4AntiKaonZero::AntiKaonZeroDefinition();
    G4KaonZeroLong::KaonZeroLongDefinition();
    G4KaonZeroShort::KaonZeroShortDefinition();
  }


  void physics::ConstructBaryons()
  {
    //  barions
    G4Proton::ProtonDefinition();
    G4AntiProton::AntiProtonDefinition();

    G4Neutron::NeutronDefinition();
    G4AntiNeutron::AntiNeutronDefinition();
  }


  void physics::ConstructProcess()
  {
    AddTransportation();
    ConstructEM();
    ConstructGeneral();
  }


  void physics::ConstructEM()
  {
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      G4String particleName = particle->GetParticleName();
     
      // gamma         
      if (particleName == "gamma") {
	pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
	pmanager->AddDiscreteProcess(new G4ComptonScattering);
	pmanager->AddDiscreteProcess(new G4GammaConversion);
      }

      // electron
      else if (particleName == "e-") {
	if (__mult_scattering) {
	  G4MultipleScattering * emuls = new G4MultipleScattering();
	  // Setting the FacRange to 0.005 instead of default value 0.2
	  //emuls->SetFacrange(0.005);
	  pmanager->AddProcess(emuls,                   -1, 1, 1);
	}
	if (__energy_loss) {
	  pmanager->AddProcess(new G4eIonisation,       -1, 2, 2);
	  pmanager->AddProcess(new G4eBremsstrahlung,   -1, 3, 3);      
	  pmanager->AddProcess(new G4StepLimiter,       -1,-1, 4);
	  //pmanager->AddProcess(new G4UserSpecialCuts,   -1,-1, 4);
	}
      } 

      // positron
      else if (particleName == "e+") {
	if (__mult_scattering) {
	  G4MultipleScattering * pmuls = new G4MultipleScattering();
	  // Setting the FacRange to 0.005 instead of default value 0.2
	  //pmuls->SetFacrange(0.005);
	  pmanager->AddProcess(pmuls,                   -1, 1, 1);
	}
	if (__energy_loss) {
	  pmanager->AddProcess(new G4eIonisation,       -1, 2, 2);
	  pmanager->AddProcess(new G4eBremsstrahlung,   -1, 3, 3);
	  pmanager->AddProcess(new G4eplusAnnihilation,  0,-1, 4);
	  pmanager->AddProcess(new G4StepLimiter,       -1,-1, 5);
	  //pmanager->AddProcess(new G4UserSpecialCuts,   -1,-1, 5);
	}
      }

      // ions
      else if( particleName == "alpha" || particleName == "GenericIon" ) {
	pmanager->AddProcess(new G4MultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4ionIonisation,     -1, 2, 2);
	pmanager->AddProcess(new G4StepLimiter,       -1,-1, 3);
	//pmanager->AddProcess(new G4UserSpecialCuts,   -1,-1, 3);
      }

      // muons
      else if( particleName == "mu+" || particleName == "mu-" ) {
	pmanager->AddProcess(new G4MultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4MuIonisation,      -1, 2, 2);
	pmanager->AddProcess(new G4MuBremsstrahlung,  -1, 3, 3);
	pmanager->AddProcess(new G4MuPairProduction,  -1, 4, 4);       
	//pmanager->AddProcess(new G4StepLimiter,       -1,-1, 5);
	//pmanager->AddProcess(new G4UserSpecialCuts,   -1,-1, 5);
     
      }

      // all others charged particles except geantino
      else if ((!particle->IsShortLived())       &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {
	pmanager->AddProcess(new G4MultipleScattering,-1, 1, 1);
	pmanager->AddProcess(new G4hIonisation,       -1, 2, 2);
	//pmanager->AddProcess(new G4StepLimiter,       -1,-1, 3);
	//pmanager->AddProcess(new G4UserSpecialCuts,   -1,-1, 3);      
      }
    }
  }

  
  void physics::ConstructGeneral()
  {
    // Add Decay Process
    G4Decay* theDecayProcess = new G4Decay();
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      if (theDecayProcess->IsApplicable(*particle)) { 
	pmanager ->AddProcess(theDecayProcess);
	// set ordering for PostStepDoIt and AtRestDoIt
	pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
	pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
      }
    }
  }


  void physics::SetCuts()
  {
    // default production thresholds
    SetCutsWithDefault();
    
    // production thresholds for detector regions
    //
    G4ProductionCuts* cuts = new G4ProductionCuts;
    cuts->SetProductionCut( __bulk_cut, G4ProductionCuts::GetIndex("e-") );
    cuts->SetProductionCut( __bulk_cut, G4ProductionCuts::GetIndex("e+") );
    cuts->SetProductionCut( __bulk_cut, G4ProductionCuts::GetIndex("gamma") );
    //cuts->SetProductionCut( 0.1*mm, G4ProductionCuts::GetIndex("gamma") );
    
    G4String region_name = "BulkRegion";
    G4Region *region = G4RegionStore::GetInstance()->GetRegion(region_name);
    region->SetProductionCuts(cuts);
    
    // Verbosing
    if (msg.level() > bhep::NORMAL) DumpCutValuesTable();
  }

} // namespace nexus
