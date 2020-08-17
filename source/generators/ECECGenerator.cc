// ----------------------------------------------------------------------------
// nexus | ECECGenerator.h
//
// This class is the primary generator for events consisting in the decay
// of a double electron capture. The user must specify via configuration
// parameters the atomic number, mass number and energy level of the
// daughter isotope of interest.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------


#include "ECECGenerator.h"

#include "BaseGeometry.h"
#include "DetectorConstruction.h"

#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>

#include <G4Event.hh>
#include <G4AtomicShell.hh>
#include <G4AtomicShellEnumerator.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4DynamicParticle.hh>
#include <G4UAtomicDeexcitation.hh>

namespace nexus {

ECECGenerator::ECECGenerator():
  G4VPrimaryGenerator(),
  atomic_number_(-1),
  shell_name_("K"), // K, L1, L2, L3, M1, ..., M5
  region_("ACTIVE"),
  msg_ (nullptr),

  geom_ (nullptr),
  shell_(nullptr),
  atom_ (nullptr)
{
  msg_ = new G4GenericMessenger(this, "/Generator/ECEC/",
                                "Control commands of the ECEC primary generator.");

  G4GenericMessenger::Command& atomic_number_cmd =
    msg_->DeclareProperty("atomic_number", atomic_number_,
                          "Atomic number of the ion.");
  atomic_number_cmd.SetParameterName("atomic_number", false);
  atomic_number_cmd.SetRange("atomic_number > 0");

  msg_->DeclareProperty("shell", shell_name_,
                        "Shell from which the electron is captured.");

  msg_->DeclareProperty("region", region_,
                        "Region of the geometry where vertices will be generated.");

}


ECECGenerator::~ECECGenerator()
{
  delete msg_;
  delete shell_;
  delete atom_;
}


G4AtomicShellEnumerator ECECGenerator::GetShellID(G4String s)
{
       if (s == "K" ) return fKShell;
  else if (s == "L1") return fL1Shell;
  else if (s == "L2") return fL2Shell;
  else if (s == "L3") return fL3Shell;
  else if (s == "M1") return fM1Shell;
  else if (s == "M2") return fM2Shell;
  else if (s == "M3") return fM3Shell;
  else if (s == "M4") return fM4Shell;
  else if (s == "M5") return fM5Shell;
  else
    G4Exception("[ECECGenerator]", "GetShellID()", FatalException,
                "Cannot recognize shell.");

  return fKShell; // Doesn't mean anything. Just to avoid -Wreturn-type
}


void ECECGenerator::Initialize() {
  const DetectorConstruction* detconst =
    dynamic_cast<const DetectorConstruction*>
    (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  if (!detconst)
    G4Exception("[ECECGenerator]", "ECECGenerator()", FatalException,
                "Unable to load geometry.");

  geom_ = detconst->GetGeometry();

  atom_ = new G4UAtomicDeexcitation();
  atom_->SetFluo (true);
  atom_->SetAuger(true);
  atom_->SetCutForSecondaryPhotons(0);
  atom_->SetCutForAugerElectrons  (0);

  shell_ = atom_->GetAtomicShell(atomic_number_, GetShellID(shell_name_));

}


G4PrimaryParticle* ECECGenerator::GetPrimaryParticle(G4DynamicParticle* dparticle)
{
  G4ParticleDefinition* pdef = dparticle->GetDefinition();

  G4double mass   = pdef     ->GetPDGMass     ();
  G4double charge = pdef     ->GetPDGCharge   ();
  G4double px     = dparticle->GetMomentum    ().x();
  G4double py     = dparticle->GetMomentum    ().y();
  G4double pz     = dparticle->GetMomentum    ().z();
  G4double polx   = dparticle->GetPolarization().x();
  G4double poly   = dparticle->GetPolarization().y();
  G4double polz   = dparticle->GetPolarization().z();

  G4PrimaryParticle* particle = new G4PrimaryParticle(pdef, px, py, pz);
  particle->SetMass        (mass            );
  particle->SetCharge      (charge          );
  particle->SetPolarization(polx, poly, polz);

  return particle;
}


void ECECGenerator::GeneratePrimaryVertex(G4Event* event)
{
  if (!atom_) // First time only
    Initialize();

  // Generate an initial position for the ion using the geometry
  G4ThreeVector position = geom_->GenerateVertex(region_);

  // Ion generated at the start-of-event time
  G4double time = 0.;

  // All particles are generated from the same position
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);

  for (unsigned int iatom=0; iatom<2; ++iatom){ // 2 atoms with a single vacancy
    // Ask geant to generate the particles for us
    std::vector<G4DynamicParticle*> particles;
    atom_->GenerateParticles(&particles, shell_, atomic_number_,
                             0.01 * eV, 0.01 * eV); // Not 0: gives problems!!

    // Convert them to primary particles and add them to the vertex
    for (auto particle : particles)
      vertex->SetPrimary(GetPrimaryParticle(particle));
  }
  event->AddPrimaryVertex(vertex);
}

} // namespace nexus
