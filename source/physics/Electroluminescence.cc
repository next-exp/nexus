// ----------------------------------------------------------------------------
// nexus | Electroluminescence.cc
//
// This class describes the generation of the EL light.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Electroluminescence.h"

#include "IonizationElectron.h"
#include "BaseDriftField.h"

#include <G4MaterialPropertiesTable.hh>
#include <G4ParticleChange.hh>
#include <G4OpticalPhoton.hh>
#include <Randomize.hh>
#include <G4Poisson.hh>
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/PhysicalConstants.h>

using namespace nexus;
using namespace CLHEP;



Electroluminescence::Electroluminescence(const G4String& process_name,
					                               G4ProcessType type):
  G4VDiscreteProcess(process_name, type), theFastIntegralTable_(0),
  table_generation_(false), photons_per_point_(0)
{
  ParticleChange_ = new G4ParticleChange();
  pParticleChange = ParticleChange_;

  BuildThePhysicsTable();

   /// Messenger
  msg_ = new G4GenericMessenger(this, "/Physics/Electroluminescence/",
				"Control commands of the Electroluminescence physics process.");
  msg_->DeclareProperty("table_generation", table_generation_,
			"EL Table generation");
  msg_->DeclareProperty("photons_per_point", photons_per_point_,
			"Photon per point");

 }



Electroluminescence::~Electroluminescence()
{
  delete theFastIntegralTable_;
}



G4bool Electroluminescence::IsApplicable(const G4ParticleDefinition& pdef)
{
  return (pdef == *IonizationElectron::Definition());
}



G4VParticleChange*
Electroluminescence::PostStepDoIt(const G4Track& track, const G4Step& step)
{
  // Initialize particle change with current track values
  ParticleChange_->Initialize(track);

  // Get the current region and its associated drift field.
  // If no drift field is defined, kill the track and leave
  G4Region* region = track.GetVolume()->GetLogicalVolume()->GetRegion();
  BaseDriftField* field =
    dynamic_cast<BaseDriftField*>(region->GetUserInformation());
  if (!field) {
    ParticleChange_->ProposeTrackStatus(fStopAndKill);
    return G4VDiscreteProcess::PostStepDoIt(track, step);
  }

  // Get the light yield from the field
  const G4double yield = field->LightYield();
  G4double step_length = field->GetTotalDriftLength();

  if (yield <= 0.)
    return G4VDiscreteProcess::PostStepDoIt(track, step);

  // Generate a random number of photons around mean 'yield'
  G4double mean = yield * step_length;

  G4int num_photons;

  if (yield < 10.) { // Poissonian regime
    num_photons = G4int(G4Poisson(mean));
  }
  else {             // Gaussian regime
    G4double sigma = sqrt(mean);
    num_photons = G4int(G4RandGauss::shoot(mean, sigma) + 0.5);
  }

  if (table_generation_)
    num_photons = photons_per_point_;

  ParticleChange_->SetNumberOfSecondaries(num_photons);

  // Track secondaries first to avoid a memory bloat
  if ((num_photons > 0) && (track.GetTrackStatus() == fAlive))
    ParticleChange_->ProposeTrackStatus(fSuspend);


  //////////////////////////////////////////////////////////////////

  G4ThreeVector position = step.GetPreStepPoint()->GetPosition();
  G4double time = step.GetPreStepPoint()->GetGlobalTime();
  G4LorentzVector initial_position(position, time);

  G4ThreeVector position_end = step.GetPostStepPoint()->GetPosition();
  G4double time_end = step.GetPostStepPoint()->GetGlobalTime();
  G4LorentzVector final_position(position_end, time_end);

  // Energy is sampled from integral (like it is
  // done in G4Scintillation)
  G4Material* mat = step.GetPostStepPoint()->GetTouchable()->GetVolume()->GetLogicalVolume()->GetMaterial();
  G4MaterialPropertiesTable* mpt = mat->GetMaterialPropertiesTable();
  const G4MaterialPropertyVector* spectrum = mpt->GetProperty("ELSPECTRUM");

  if (!spectrum) return G4VDiscreteProcess::PostStepDoIt(track, step);

  G4PhysicsOrderedFreeVector* spectrum_integral =
    (G4PhysicsOrderedFreeVector*)(*theFastIntegralTable_)(mat->GetIndex());


  G4double sc_max = spectrum_integral->GetMaxValue();

  for (G4int i=0; i<num_photons; i++) {
    // Generate a random direction for the photon
    // (EL is supposed isotropic)
    G4double cos_theta = 1. - 2.*G4UniformRand();
    G4double sin_theta = sqrt((1.-cos_theta)*(1.+cos_theta));

    G4double phi = twopi * G4UniformRand();
    G4double sin_phi = sin(phi);
    G4double cos_phi = cos(phi);

    G4double px = sin_theta * cos_phi;
    G4double py = sin_theta * sin_phi;
    G4double pz = cos_theta;

    G4ThreeVector momentum(px, py, pz);

    // Determine photon polarization accordingly
    G4double sx = cos_theta * cos_phi;
    G4double sy = cos_theta * sin_phi;
    G4double sz = -sin_theta;

    G4ThreeVector polarization(sx, sy, sz);
    G4ThreeVector perp = momentum.cross(polarization);

    phi = twopi * G4UniformRand();
    sin_phi = sin(phi);
    cos_phi = cos(phi);

    polarization = cos_phi * polarization + sin_phi * perp;
    polarization = polarization.unit();

    // Generate a new photon and set properties
    G4DynamicParticle* photon =
      new G4DynamicParticle(G4OpticalPhoton::Definition(), momentum);

    photon->
      SetPolarization(polarization.x(), polarization.y(), polarization.z());

    // Determine photon energy
    G4double sc_value = G4UniformRand()*sc_max;
    G4double sampled_energy = spectrum_integral->GetEnergy(sc_value);
    photon->SetKineticEnergy(sampled_energy);

    G4LorentzVector xyzt =
      field->GeneratePointAlongDriftLine(initial_position, final_position);

    // Create the track
    G4Track* secondary = new G4Track(photon, xyzt.t(), xyzt.v());
    secondary->SetParentID(track.GetTrackID());
    ParticleChange_->AddSecondary(secondary);

  }

  return G4VDiscreteProcess::PostStepDoIt(track, step);
}



void Electroluminescence::BuildThePhysicsTable()
{
  if (theFastIntegralTable_) return;

  const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();
  G4int numOfMaterials = G4Material::GetNumberOfMaterials();

  // create new physics table

  if(!theFastIntegralTable_)
    theFastIntegralTable_ = new G4PhysicsTable(numOfMaterials);

  for (G4int i=0 ; i<numOfMaterials; i++) {

  	G4PhysicsOrderedFreeVector* aPhysicsOrderedFreeVector =
  	  new G4PhysicsOrderedFreeVector();

  	// Retrieve vector of scintillation wavelength intensity for
  	// the material from the material's optical properties table.

  	G4Material* material = (*theMaterialTable)[i];

    G4MaterialPropertiesTable* mpt = material->GetMaterialPropertiesTable();

    if (mpt) {

  	  G4MaterialPropertyVector* theFastLightVector =
  	    mpt->GetProperty("ELSPECTRUM");

  	  if (theFastLightVector) {
        ComputeCumulativeDistribution(*theFastLightVector, *aPhysicsOrderedFreeVector);
		  }
  	}

  	// The scintillation integral(s) for a given material
  	// will be inserted in the table(s) according to the
  	// position of the material in the material table.

  	theFastIntegralTable_->insertAt(i,aPhysicsOrderedFreeVector);
  }
}



void Electroluminescence::ComputeCumulativeDistribution(
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



G4double Electroluminescence::GetMeanFreePath(const G4Track&, G4double,
                                              G4ForceCondition* condition)
{
  *condition = StronglyForced;
  return DBL_MAX;
}
