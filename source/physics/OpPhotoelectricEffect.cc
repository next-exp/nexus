// ----------------------------------------------------------------------------
// nexus | OpPhotoelectricEffect.h
//
// Add photoelectric effect physics to optical photons hitting
// specific materials.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "OpPhotoelectricEffect.h"

#include "IonizationElectron.h"

#include <G4Material.hh>
#include <G4ParticleDefinition.hh>
#include <G4OpticalPhoton.hh>
#include <G4RandomDirection.hh>
#include <G4Poisson.hh>


namespace nexus {


  using namespace CLHEP;

  OpPhotoelectricEffect::OpPhotoelectricEffect(const G4String& process_name,
                                             G4ProcessType type):
    G4VDiscreteProcess(process_name, type), particle_change_(0)
  {
    // Create particle change object
    particle_change_ = new G4ParticleChange();
    pParticleChange = particle_change_;
  }



  OpPhotoelectricEffect::~OpPhotoelectricEffect()
  {
    delete particle_change_;
  }



  G4bool OpPhotoelectricEffect::IsApplicable(const G4ParticleDefinition& pdef)
  {
    return pdef == *G4OpticalPhoton::Definition();
  }

  G4VParticleChange*
  OpPhotoelectricEffect::PostStepDoIt(const G4Track& track, const G4Step& step)
  {
    // Initialize particle change with current track values
    particle_change_->Initialize(track);

    const G4Material* material = track.GetMaterial();

    G4MaterialPropertiesTable* mpt = material->GetMaterialPropertiesTable();
    if (!mpt)
      return G4VDiscreteProcess::PostStepDoIt(track, step);

    G4MaterialPropertyVector*
    probabilities = mpt->GetProperty("OP_PHOTOELECTRIC_PROBABILITY");

    if (!probabilities)
      return G4VDiscreteProcess::PostStepDoIt(track, step);

    G4double photon_energy = track.GetDynamicParticle()->GetTotalEnergy();
    G4int    n_ie          = G4Poisson(probabilities->Value(photon_energy));

    if (!n_ie)
      return G4VDiscreteProcess::PostStepDoIt(track, step);

    particle_change_->ProposeTrackStatus(fStopAndKill);
    particle_change_->SetNumberOfSecondaries(n_ie);

    for (G4int i=0; i < n_ie; i++) {
      G4ThreeVector momentum_direction = G4RandomDirection();
      G4double      kinetic_energy     = photon_energy/n_ie;

      G4DynamicParticle*
      ie = new G4DynamicParticle(IonizationElectron::Definition(),
                                 momentum_direction, kinetic_energy);


      G4StepPoint* post = step.GetPostStepPoint();
      G4Track* new_track = new G4Track(ie,
                                       post->GetGlobalTime(),
                                       post->GetPosition  ());

      new_track->SetTouchableHandle(post->GetTouchableHandle());
      new_track->SetParentID(track.GetTrackID());
      particle_change_->AddSecondary(new_track);
    }

    return particle_change_;
  }


  G4double OpPhotoelectricEffect::GetMeanFreePath(const G4Track&, G4double,
                                                 G4ForceCondition* condition)
  {
    *condition = StronglyForced;
    return DBL_MAX;
  }


  G4double OpPhotoelectricEffect::GetMeanLifeTime(const G4Track&,
                                                 G4ForceCondition* condition)
  {
    *condition = Forced;
    return DBL_MAX;
  }

} // end namespace nexus
