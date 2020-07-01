// ----------------------------------------------------------------------------
// nexus | WavelengthShifting.cc
//
// This is a class with a nexus-defined process for wavelength shifting.
// It differs from G4OpWls, because the mean free path is not calculated
// from a wls_absorption length property but through a probability property,
// thus independent from the material thickness.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "WavelengthShifting.h"

#include <G4OpticalPhoton.hh>
#include <Randomize.hh>
#include <G4WLSTimeGeneratorProfileExponential.hh>

#include "CLHEP/Units/PhysicalConstants.h"

#include <TH1F.h>
#include <TFile.h>

namespace nexus {

  using namespace CLHEP;

  WavelengthShifting::WavelengthShifting(const G4String& name, G4ProcessType type): 
    G4VDiscreteProcess(name, type), wlsIntegralTable_(0)
  {
    ParticleChange_ = new G4ParticleChange();
    pParticleChange = ParticleChange_;
   
    WLSTimeGeneratorProfile_ = 
      new G4WLSTimeGeneratorProfileExponential("WLSTimeGeneratorProfileExponential");

    BuildThePhysicsTable();

    // hWLSTime = new TH1F("WLSTime", "WLS photon production time", 50000, 0., 50000);
    // hWLSTime->GetXaxis()->SetTitle("time (ps)");
  }

  WavelengthShifting::~WavelengthShifting()
  {
    delete ParticleChange_;
    if (wlsIntegralTable_ != 0) {
      wlsIntegralTable_->clearAndDestroy();
      delete wlsIntegralTable_;
    }
    delete WLSTimeGeneratorProfile_;

    // histo_file = new TFile("HistoFile.root","recreate");
    // hWLSTime->Write();
    // histo_file->Close();
  }

  G4bool WavelengthShifting::IsApplicable(const G4ParticleDefinition& aParticleType)
  {
    return ( &aParticleType == G4OpticalPhoton::Definition() );
  }
  
  G4VParticleChange* WavelengthShifting::PostStepDoIt(const G4Track& track,const G4Step& step)
  {
    ParticleChange_->Initialize(track);     
    ParticleChange_->ProposeTrackStatus(fStopAndKill);

    const G4Material* material = track.GetMaterial();   
 
    G4StepPoint* pPostStepPoint = step.GetPostStepPoint();
    
   G4MaterialPropertiesTable* aMaterialPropertiesTable =
     material->GetMaterialPropertiesTable();
   if (!aMaterialPropertiesTable)
     return G4VDiscreteProcess::PostStepDoIt(track, step);
 
   G4MaterialPropertyVector* WLS_Conversion_Efficiency= 
     aMaterialPropertiesTable->GetProperty("WLSCONVEFFICIENCY"); 
    
   if (!WLS_Conversion_Efficiency) {
     return G4VDiscreteProcess::PostStepDoIt(track, step);
   }

   const G4DynamicParticle* particle = track.GetDynamicParticle();
   
   G4double thePhotonEnergy = particle->GetTotalEnergy();
   G4double conversion_efficiency = 
     WLS_Conversion_Efficiency->Value(thePhotonEnergy);
   
   G4double rndm = G4UniformRand();
   if (rndm > conversion_efficiency) {
     return G4VDiscreteProcess::PostStepDoIt(track, step);
   }	 
   ParticleChange_->SetNumberOfSecondaries(1);

   G4int materialIndex = material->GetIndex();
   G4PhysicsOrderedFreeVector* WLSIntegral  =
     (G4PhysicsOrderedFreeVector*)((*wlsIntegralTable_)(materialIndex));
    
   // Sample the energy randomly
   G4double wls_max = WLSIntegral->GetMaxValue();
   G4double wls_value = G4UniformRand()*wls_max;
   G4double sampledEnergy = WLSIntegral->GetEnergy(wls_value);
   
   // Generate random photon direction 
   G4double costheta = 1. - 2.*G4UniformRand();
   G4double sintheta = std::sqrt((1.-costheta)*(1.+costheta));
   
   G4double phi = twopi*G4UniformRand();
   G4double sinphi = std::sin(phi);
   G4double cosphi = std::cos(phi);
   
   G4double px = sintheta*cosphi;
   G4double py = sintheta*sinphi;
   G4double pz = costheta;
    
   // Create photon momentum direction vector     
   G4ParticleMomentum photonMomentum(px, py, pz);
     
   // Determine polarization of new photon
   G4double sx = costheta*cosphi;
   G4double sy = costheta*sinphi;
   G4double sz = -sintheta;
   
   G4ThreeVector photonPolarization(sx, sy, sz);  
   G4ThreeVector perp = photonMomentum.cross(photonPolarization);
   
   phi = twopi*G4UniformRand();
   sinphi = std::sin(phi);
   cosphi = std::cos(phi);   
   photonPolarization = cosphi * photonPolarization + sinphi * perp;  
   photonPolarization = photonPolarization.unit();
    
   // Generate a new photon  
   G4DynamicParticle* aWLSPhoton =
     new G4DynamicParticle(G4OpticalPhoton::OpticalPhoton(),
			   photonMomentum);
   aWLSPhoton->SetPolarization
     (photonPolarization.x(),
      photonPolarization.y(),
      photonPolarization.z());
     
   aWLSPhoton->SetKineticEnergy(sampledEnergy);
     
    // Generate new G4Track object and give position of WLS optical photon
   G4double WLSTime = aMaterialPropertiesTable->GetConstProperty("WLSTIMECONSTANT");
   G4double TimeDelay = WLSTimeGeneratorProfile_->GenerateTime(WLSTime);
   G4double aSecondaryTime = (pPostStepPoint->GetGlobalTime()) + TimeDelay; 
   G4ThreeVector aSecondaryPosition = pPostStepPoint->GetPosition();
 
   G4Track* aSecondaryTrack = 
     new G4Track(aWLSPhoton,aSecondaryTime,aSecondaryPosition);  
   aSecondaryTrack->SetTouchableHandle(track.GetTouchableHandle());   
   aSecondaryTrack->SetParentID(track.GetTrackID());
   ParticleChange_->AddSecondary(aSecondaryTrack);

   //hWLSTime->Fill(TimeDelay/picosecond);
     
   return G4VDiscreteProcess::PostStepDoIt(track, step);
   
  }

  void WavelengthShifting::BuildThePhysicsTable()
  {
    if (wlsIntegralTable_) return;
  
    const G4MaterialTable* theMaterialTable = 
      G4Material::GetMaterialTable();
    G4int numOfMaterials = G4Material::GetNumberOfMaterials();
  
    // create new physics table   
    if(!wlsIntegralTable_)
      wlsIntegralTable_ = new G4PhysicsTable(numOfMaterials);
   
    // loop for materials
  
    for (G4int i=0 ; i < numOfMaterials; i++) {
      G4PhysicsOrderedFreeVector* aPhysicsOrderedFreeVector =
	new G4PhysicsOrderedFreeVector();
       
      // Retrieve vector of WLS wavelength intensity for
      // the material from the material's optical properties table.      
      G4Material* aMaterial = (*theMaterialTable)[i];
     
      G4MaterialPropertiesTable* aMaterialPropertiesTable =
	aMaterial->GetMaterialPropertiesTable();
     
      if (aMaterialPropertiesTable) { 
	G4MaterialPropertyVector* theWLSVector = 
	  aMaterialPropertiesTable->GetProperty("WLSCOMPONENT");
	if (theWLSVector) {	 
	  ComputeCumulativeDistribution(*theWLSVector, *aPhysicsOrderedFreeVector);
	}
      }
      // The WLS integral for a given material
      // will be inserted in the table according to the
      // position of the material in the material table.    
      wlsIntegralTable_->insertAt(i,aPhysicsOrderedFreeVector);
    }
  }

  G4double WavelengthShifting::GetMeanFreePath(const G4Track& track, G4double, G4ForceCondition* /*condition*/)
  { 
    G4double AttenuationLength = DBL_MAX;
  
     const G4Material* material = track.GetMaterial();
     G4MaterialPropertiesTable* aMaterialPropertiesTable = material->GetMaterialPropertiesTable();
     if (aMaterialPropertiesTable) {
       G4MaterialPropertyVector* WLS_Conversion_Efficiency= 
	 aMaterialPropertiesTable->GetProperty("WLSCONVEFFICIENCY"); 
       if (WLS_Conversion_Efficiency) {
	 const G4DynamicParticle* particle = track.GetDynamicParticle();
   
	 G4double thePhotonEnergy = particle->GetTotalEnergy();
	 G4double conversion_efficiency = 
	   WLS_Conversion_Efficiency->Value(thePhotonEnergy);
	 
	 // If the photon has zero conversion efficiency, it must not enter the process at all.
	 if (conversion_efficiency == 0.) {
	   return AttenuationLength;
	 }	 
	 // ParticleChange_->SetNumberOfSecondaries(1);
	 AttenuationLength = DBL_MIN;
       }
     }
     
     return AttenuationLength;
  }

  void WavelengthShifting::ComputeCumulativeDistribution(const G4MaterialPropertyVector& pdf, 
							 G4PhysicsOrderedFreeVector& cdf)
  {
    if (pdf[0] >= 0.0) { 
      // The first entry of the cumulative distribution function is zero.  
      cdf.InsertValues(pdf.Energy(0), 0.0);

      G4double area_sum = 0.;       
      // loop over all (photon energy, intensity) pairs stored for this material
      // and add the areas calculated with the trapezoidal rule
      for (size_t j = 1; j < pdf.GetVectorLength(); j++) {
	area_sum = cdf[j-1] + (pdf[j-1] + pdf[j]) * (pdf.Energy(j) - pdf.Energy(j-1)) * 0.5 ;
	cdf.InsertValues(pdf.Energy(j), area_sum);     
      }
    } 
  }

}
