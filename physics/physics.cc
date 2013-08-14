// NEXT simulation: physics.cc
//

#include <next/physics.hh>
#include <next/geometry.hh>


physics::physics():  G4VUserPhysicsList()
{
    G4cout << G4endl << "*************** SETTING THE PHYSICS *****************";

    // create a generic store
    bhep::gstore store;
    // and the reader
    bhep::sreader reader(store);
    reader.file("next.par");
    reader.info_level(bhep::NORMAL);
    reader.group("PHYSICS");
    reader.read();

    e_loss_ = store.fetch_istore("energy_loss");
    G4cout << G4endl << "Electrons Energy Loss set to: " <<  e_loss_ << G4endl;

    mult_scat_ = store.fetch_istore("mult_scattering");
    G4cout << "Electrons Multiple Scatering set to: " << mult_scat_ << G4endl;

    bulk_cut_ = store.fetch_dstore("bulk_cut") * mm;
    G4cout << "Bulk Region cut set to: " << bulk_cut_/mm << " mm" << G4endl;
    
    defaultCutValue = 1.0e-3*mm;  
    G4cout << "Default Cut Length : " << G4BestUnit(defaultCutValue, "Length");

    SetVerboseLevel(0);

    G4cout << G4endl <<  "***************** END OF PHYSICS ********************"
           << G4endl << G4endl;
}


physics::~physics()
{}


void physics::ConstructParticle()
{
    // In this method, static member functions should be called
    // for all particles which you want to use.
    // This ensures that objects of these particle types will be
    // created in the program. 

    //  bosons
    ConstructBosons();

    //  leptons
    ConstructLeptons();

    //  mesons
    G4MesonConstructor mCtor;
    mCtor.ConstructParticle();

    //  baryons
    G4BaryonConstructor bCtor;
    bCtor.ConstructParticle();

    //  ions
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
    //  e+/-
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
    //  mesons
    //    light mesons
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
            if (mult_scat_) {
                G4MultipleScattering * emuls = new G4MultipleScattering();
                // Setting the FacRange to 0.005 instead of default value 0.2
                emuls->SetFacrange(0.005);
                pmanager->AddProcess(emuls,                   -1, 1, 1);
            }
            if (e_loss_) {
                pmanager->AddProcess(new G4eIonisation,       -1, 2, 2);
                pmanager->AddProcess(new G4eBremsstrahlung,   -1, 3, 3);      
                pmanager->AddProcess(new G4StepLimiter,       -1,-1, 4);
		//pmanager->AddProcess(new G4UserSpecialCuts,   -1,-1, 4);
            }
        } 

        // positron
        else if (particleName == "e+") {
            if (mult_scat_) {
                G4MultipleScattering * pmuls = new G4MultipleScattering();
                // Setting the FacRange to 0.005 instead of default value 0.2
                pmuls->SetFacrange(0.005);
                pmanager->AddProcess(pmuls,                   -1, 1, 1);
            }
            if (e_loss_) {
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


#include "G4Decay.hh"

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


#include "G4ProductionCuts.hh"
#include "G4RegionStore.hh"

void physics::SetCuts()
{
    // G4VUserPhysicsList::SetCutsWithDefault method sets 
    // the default cut value for all particle types 
    //
    SetCutsWithDefault();
    
    // Defining cuts for the different regions
    //
    
    // TPC cuts
    G4ProductionCuts* bulk_reg_cuts = new G4ProductionCuts;
    bulk_reg_cuts->SetProductionCut(bulk_cut_);
    G4Region* bulk_reg = G4RegionStore::GetInstance()->GetRegion("Bulk_region");
    bulk_reg->SetProductionCuts(bulk_reg_cuts);

    // Verbosing
    if (m_.level() > bhep::NORMAL) DumpCutValuesTable();
}

