// ----------------------------------------------------------------------------
// nexus | IonizationClustering.cc
//
// This class creates ionization electrons where energy is deposited.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "IonizationClustering.h"

#include "BaseDriftField.h"
#include "IonizationElectron.h"
#include "SegmentPointSampler.h"

#include <G4ParticleDefinition.hh>
#include <G4OpticalPhoton.hh>
#include <G4Poisson.hh>
#include <Randomize.hh>
#include <G4LorentzVector.hh>
#include <G4Gamma.hh>

#include "CLHEP/Units/SystemOfUnits.h"


namespace nexus {


  using namespace CLHEP;

  IonizationClustering::IonizationClustering(const G4String& process_name,
                                             G4ProcessType type):
    G4VRestDiscreteProcess(process_name, type), ParticleChange_(0), rnd_(0)
  {
    // Create particle change object
    ParticleChange_ = new G4ParticleChange();
    pParticleChange = ParticleChange_;

    // Create a segment point sample
    rnd_ = new SegmentPointSampler();
  }



  IonizationClustering::~IonizationClustering()
  {
    delete rnd_;
    delete ParticleChange_;
  }



  G4bool IonizationClustering::IsApplicable(const G4ParticleDefinition& pdef)
  {
    if (pdef == *G4OpticalPhoton::Definition() ||
        pdef == *IonizationElectron::Definition()) return false;

    else if ((pdef.GetPDGCharge() != 0.) ||
             (pdef == *G4Gamma::Definition())) return true;

    else return false;
  }



  G4VParticleChange*
  IonizationClustering::AtRestDoIt(const G4Track& track, const G4Step& step)
  {
    // The method simply calls the equivalent PostStepDoIt,
    // proceeding as in any other step.
    return IonizationClustering::PostStepDoIt(track, step);
  }



  G4VParticleChange*
  IonizationClustering::PostStepDoIt(const G4Track& track, const G4Step& step)
  {
    // Initialize particle change with current track values
    ParticleChange_->Initialize(track);

    //////////////////////////////////////////////////////////////////
    // Get energy deposited through ionization during the last step.
    // If no energy was deposited, we are done here.

    G4double energy_dep =
      step.GetTotalEnergyDeposit() - step.GetNonIonizingEnergyDeposit();

    if (energy_dep <= 0.)
      return G4VRestDiscreteProcess::PostStepDoIt(track, step);

    //////////////////////////////////////////////////////////////////
    // The clustering process makes sense only for those regions with
    // a drift field defined. Therefore, check whether the current region
    // has a drift field attached, and stop the process if that's not the case.

    G4Region* region = track.GetVolume()->GetLogicalVolume()->GetRegion();

    BaseDriftField* field =
      dynamic_cast<BaseDriftField*>(region->GetUserInformation());

    if (!field) return G4VRestDiscreteProcess::PostStepDoIt(track, step);

    //////////////////////////////////////////////////////////////////
    // Calculate the number of charges to be simulated generating a
    // a Gaussian random number with mean given by the 'empirical'
    // average energy needed to produce an ionization pair, W_i.
    // The fluctuations (sigma of the distribution) are in general
    // sub-Poissonian: \sigma^2 = F N, where F is the so-called Fano factor
    // and N is the average number of charges.

    // Fetch the W_i and F from the material properties table
    G4String mat_name = track.GetMaterial()->GetName();
    G4MaterialPropertiesTable* mpt =
      track.GetMaterial()->GetMaterialPropertiesTable();
    if (!mpt) {
      G4cout << "Material properties missing for material " << mat_name << G4endl;
      G4Exception("[IonizationClustering]", "PostStepDoIt()", FatalException,
		  "Material properties table not defined in material!");
      }

    G4double ioni_energy = mpt->GetConstProperty("IONIZATIONENERGY");
    G4double fano_factor = mpt->GetConstProperty("FANOFACTOR");

    G4double mean = energy_dep / ioni_energy;

    G4int num_charges = 0;

    if (mean > 10.) {
      G4double sigma = sqrt(mean*fano_factor);
      num_charges = G4int(G4RandGauss::shoot(mean, sigma) + 0.5);
    }
    else {
      num_charges = G4int(G4Poisson(mean));
    }

    ParticleChange_->SetNumberOfSecondaries(num_charges);

    // Track secondaries first
    if ((track.GetTrackStatus() == fAlive) && num_charges > 0)
      ParticleChange_->ProposeTrackStatus(fSuspend);

    //////////////////////////////////////////////////////////////////

    G4ThreeVector momentum_direction(0.,0.,1.);
    G4double kinetic_energy = 1.*eV;

    // Set pre and post points of the step in the random generator
    G4LorentzVector pre_point(step.GetPreStepPoint()->GetPosition(),
			                        step.GetPreStepPoint()->GetGlobalTime());
    G4LorentzVector post_point(step.GetPostStepPoint()->GetPosition(),
                  			       step.GetPostStepPoint()->GetGlobalTime());
    rnd_->SetPoints(pre_point, post_point);


    for (G4int i=0; i<num_charges; i++) {

      G4DynamicParticle* ionielectron =
        new G4DynamicParticle(IonizationElectron::Definition(),
          momentum_direction, kinetic_energy);

      // Calculate position and time. We distribute the ie- along
      // the step except for the depositions associated to gammas,
      // where we use the post-step point.
      G4LorentzVector point;
      if (track.GetDefinition() == G4Gamma::Definition()) point = post_point;
      else point = rnd_->Shoot();

      G4Track* aSecondaryTrack =
        new G4Track(ionielectron, point.t(), point.v());

      aSecondaryTrack->
        SetTouchableHandle(step.GetPreStepPoint()->GetTouchableHandle());

      ParticleChange_->AddSecondary(aSecondaryTrack);
    }

    return G4VRestDiscreteProcess::PostStepDoIt(track, step);
  }



  G4double IonizationClustering::GetMeanFreePath(const G4Track&,
    G4double, G4ForceCondition* condition)
  {
    *condition = StronglyForced;
    return DBL_MAX;
  }



  G4double IonizationClustering::GetMeanLifeTime(const G4Track&,
    G4ForceCondition* condition)
  {
    *condition = Forced;
    return DBL_MAX;
  }

} // end namespace nexus
