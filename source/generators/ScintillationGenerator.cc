// ----------------------------------------------------------------------------
// nexus | ScintillationGenerator.cc
//
// This class is the primary generator of a number of optical photons with
// energy following the scintillation spectrum of the material
// where the vertex is produced.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ScintillationGenerator.h"

#include "DetectorConstruction.h"
#include "GeometryBase.h"
#include "MaterialProperties.h"
#include "FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4ParticleDefinition.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <G4RandomDirection.hh>
#include <G4OpticalPhoton.hh>

#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(ScintillationGenerator, G4VPrimaryGenerator)


ScintillationGenerator::ScintillationGenerator() :
  G4VPrimaryGenerator(), msg_(0), geom_(0), nphotons_(1000000)
{
  msg_ = new G4GenericMessenger(this, "/Generator/ScintGenerator/",
    "Control commands of scintillation generator.");

  msg_->DeclareProperty("region", region_,
                        "Region of the geometry where the vertex will be generated.");

  msg_->DeclareProperty("nphotons", nphotons_, "Number of photons");

  geom_navigator_ =
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  DetectorConstruction* detconst =
    (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();
}

ScintillationGenerator::~ScintillationGenerator()
{
  delete msg_;
}

void ScintillationGenerator::GeneratePrimaryVertex(G4Event* event)
{
  G4ParticleDefinition* particle_definition = G4OpticalPhoton::Definition();
  // Generate an initial position for the particle using the geometry and set time to 0.
  G4ThreeVector position = geom_->GenerateVertex(region_);
  G4double time = 0.;

  // Energy is sampled from integral (like it is done in G4Scintillation)

  G4VPhysicalVolume* vol =
    geom_navigator_->LocateGlobalPointAndSetup(position, 0, false);
  G4Material* mat = vol->GetLogicalVolume()->GetMaterial();
  G4MaterialPropertiesTable* mpt = mat->GetMaterialPropertiesTable();

  if (!mpt) {
    G4Exception("[ScintillationGenerator]", "GeneratePrimaryVertex()",
                FatalException, "Material properties not defined for this material!");
  }
  // Using fast or slow component here is irrelevant, since we're not using time
  // and they're are the same in energy.
  G4MaterialPropertyVector* spectrum =
    mpt->GetProperty("SCINTILLATIONCOMPONENT1");

  if (!spectrum) {
    G4Exception("[ScintillationGenerator]", "GeneratePrimaryVertex()",
                FatalException, "Fast time decay constant not defined for this material!");
  }

  G4PhysicsOrderedFreeVector* spectrum_integral =
    new G4PhysicsOrderedFreeVector();
  ComputeCumulativeDistribution(*spectrum, *spectrum_integral);
  G4double sc_max = spectrum_integral->GetMaxValue();

  // Create a new vertex
  G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);

  for ( G4int i = 0; i<nphotons_; i++)
    {
      // Generate random direction by default
      G4ThreeVector _momentum_direction = G4RandomDirection();
      // Determine photon energy
      G4double sc_value = G4UniformRand()*sc_max;
      G4double pmod = spectrum_integral->GetEnergy(sc_value);
      G4double px = pmod * _momentum_direction.x();
      G4double py = pmod * _momentum_direction.y();
      G4double pz = pmod * _momentum_direction.z();

      // Create the new primary particle and set it some properties
      G4PrimaryParticle* particle =
        new G4PrimaryParticle(particle_definition, px, py, pz);

      G4ThreeVector polarization = G4RandomDirection();
      particle->SetPolarization(polarization);

      // Add particle to the vertex and this to the event
      vertex->SetPrimary(particle);
    }
  event->AddPrimaryVertex(vertex);
}

void ScintillationGenerator::ComputeCumulativeDistribution(
  const G4PhysicsOrderedFreeVector& pdf, G4PhysicsOrderedFreeVector& cdf)
{
  G4double sum = 0.;
  cdf.InsertValues(pdf.Energy(0), sum);

  for (unsigned int i=1; i<pdf.GetVectorLength(); ++i) {
    G4double area =
      0.5 * (pdf.Energy(i) - pdf.Energy(i-1)) * (pdf[i] + pdf[i-1]);
    sum = sum + area;
    cdf.InsertValues(pdf.Energy(i), sum);
  }
}
