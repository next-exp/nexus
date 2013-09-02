// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 28 Oct 2009
//  
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Electroluminescence.h"

#include "IonizationElectron.h"
#include "BaseDriftField.h"

#include <G4MaterialPropertiesTable.hh>
#include <G4PhysicsOrderedFreeVector.hh>
#include <G4ParticleChange.hh>
#include <G4OpticalPhoton.hh>
#include <Randomize.hh>
#include <G4Poisson.hh>

using namespace nexus;



Electroluminescence::Electroluminescence(const G4String& process_name,
					                               G4ProcessType type):
  G4VDiscreteProcess(process_name, type),
  _theSlowIntegralTable(0), _theFastIntegralTable(0)
{
  _ParticleChange = new G4ParticleChange();
  pParticleChange = _ParticleChange;
    
  BuildThePhysicsTable();
}
  
  
  
Electroluminescence::~Electroluminescence()
{
  delete _theFastIntegralTable;
  delete _theSlowIntegralTable;
}
  
  
  
G4bool Electroluminescence::IsApplicable(const G4ParticleDefinition& pdef)
{
  return (pdef == *IonizationElectron::Definition());
}



G4VParticleChange*
Electroluminescence::PostStepDoIt(const G4Track& track, const G4Step& step)
{
  // Initialize particle change with current track values
  _ParticleChange->Initialize(track);
    
  // Get the current region and its associated drift field.
  // If no drift field is defined, kill the track and leave
  G4Region* region = track.GetVolume()->GetLogicalVolume()->GetRegion();
  BaseDriftField* field = 
    dynamic_cast<BaseDriftField*>(region->GetUserInformation());
  if (!field) {
    _ParticleChange->ProposeTrackStatus(fStopAndKill);
    return G4VDiscreteProcess::PostStepDoIt(track, step);
  }

  // Get the light yield from the field
  const G4double yield = field->LightYield();
  G4double step_length = step.GetStepLength();

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
    num_photons = G4int(G4RandGauss::shoot(yield, sigma) + 0.5);
  }
   
    //num_photons = 500000;
      
  _ParticleChange->SetNumberOfSecondaries(num_photons);

  // Track secondaries first to avoid a memory bloat
  if ((num_photons > 0) && (track.GetTrackStatus() == fAlive))
    _ParticleChange->ProposeTrackStatus(fSuspend);


  //////////////////////////////////////////////////////////////////

  G4ThreeVector position = step.GetPreStepPoint()->GetPosition();
  G4double time = step.GetPreStepPoint()->GetGlobalTime();
  G4LorentzVector initial_position(position, time);
    
  // Energy ia sampled from integral (like it is
  // done in G4Scintillation)
  G4Material* mat = step.GetPostStepPoint()->GetTouchable()->GetVolume()->GetLogicalVolume()->GetMaterial();
  G4MaterialPropertiesTable* mat_mpt = mat->GetMaterialPropertiesTable();
  const G4MaterialPropertyVector* Fast_Intensity = 
    mat_mpt->GetProperty("FASTCOMPONENT"); 
  const G4MaterialPropertyVector* Slow_Intensity =
    mat_mpt->GetProperty("SLOWCOMPONENT");
     
  if (!Fast_Intensity && !Slow_Intensity )
    return G4VDiscreteProcess::PostStepDoIt(track, step);

    G4int nscnt = 1;
    if (Fast_Intensity && Slow_Intensity) nscnt = 2;
   
    G4int mat_index = mat->GetIndex();
    G4int num = 0;

    for (G4int scnt = 1; scnt <= nscnt; scnt++) {

      G4double ScintillationTime = 0.*ns;
      G4PhysicsOrderedFreeVector* ScintillationIntegral = NULL;
      
      if (scnt == 1) { 

        if (nscnt == 1) {
          if(Fast_Intensity) {
            ScintillationTime = mat_mpt->GetConstProperty("FASTTIMECONSTANT");
            ScintillationIntegral = 
              (G4PhysicsOrderedFreeVector*)((*_theFastIntegralTable)(mat_index));
            }

          if(Slow_Intensity) {
            ScintillationTime = mat_mpt->GetConstProperty("SLOWTIMECONSTANT");
            ScintillationIntegral =
              (G4PhysicsOrderedFreeVector*)((*_theSlowIntegralTable)(mat_index));
          }
        } 
        else {
          G4double YieldRatio = mat_mpt->GetConstProperty("YIELDRATIO");
          num = G4int (std::min(YieldRatio,1.0) * num_photons);
          ScintillationTime = mat_mpt->GetConstProperty("FASTTIMECONSTANT");
          ScintillationIntegral =
            (G4PhysicsOrderedFreeVector*)((*_theFastIntegralTable)(mat_index));
        }
      } 
      else {
        num = num_photons - num;
        ScintillationTime = mat_mpt->GetConstProperty("SLOWTIMECONSTANT");
        ScintillationIntegral =
          (G4PhysicsOrderedFreeVector*)((*_theSlowIntegralTable)(mat_index));
      }

      if (!ScintillationIntegral) continue;
	
      // Max Scintillation Integral
 
      G4double sc_max = ScintillationIntegral->GetMaxValue();
      
      for (G4int i=0; i<num; i++) {
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
      
        photon->SetPolarization(polarization.x(), polarization.y(), polarization.z());

        // Determine photon energy
        G4double sc_value = G4UniformRand()*sc_max;
        G4double sampled_energy = 
        ScintillationIntegral->GetEnergy(sc_value);
        photon->SetKineticEnergy(sampled_energy);

        G4LorentzVector xyzt = 
          field->GeneratePointAlongDriftLine(initial_position);

        // Create the track
        G4Track* secondary = new G4Track(photon, xyzt.t(), xyzt.v());
        secondary->SetParentID(track.GetTrackID());
        //secondary->
          //SetTouchableHandle(step.GetPreStepPoint()->GetTouchableHandle());
        _ParticleChange->AddSecondary(secondary);
    }
  }
  
  return G4VDiscreteProcess::PostStepDoIt(track, step);
}
  
 

void Electroluminescence::BuildThePhysicsTable()
{
  if (_theFastIntegralTable && _theSlowIntegralTable) return;

  const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();
  G4int numOfMaterials = G4Material::GetNumberOfMaterials();

  // create new physics table
	
  if(!_theFastIntegralTable) 
    _theFastIntegralTable = new G4PhysicsTable(numOfMaterials);
  if(!_theSlowIntegralTable) 
    _theSlowIntegralTable = new G4PhysicsTable(numOfMaterials);


  for (G4int i=0 ; i<numOfMaterials; i++) {

  	G4PhysicsOrderedFreeVector* aPhysicsOrderedFreeVector =
  	  new G4PhysicsOrderedFreeVector();
  	G4PhysicsOrderedFreeVector* bPhysicsOrderedFreeVector =
  	  new G4PhysicsOrderedFreeVector();

  	// Retrieve vector of scintillation wavelength intensity for
  	// the material from the material's optical properties table.

  	G4Material* material = (*theMaterialTable)[i];

    G4MaterialPropertiesTable* mpt = material->GetMaterialPropertiesTable();

    if (mpt) {

  	  G4MaterialPropertyVector* theFastLightVector = 
  	    mpt->GetProperty("FASTCOMPONENT");

  	  if (theFastLightVector) {
        ComputeCumulativeDistribution(*theFastLightVector, *aPhysicsOrderedFreeVector);
		  }

  	  G4MaterialPropertyVector* theSlowLightVector =
  	    mpt->GetProperty("SLOWCOMPONENT");

  	  if (theSlowLightVector) {
        ComputeCumulativeDistribution(*theSlowLightVector, *bPhysicsOrderedFreeVector);
  	  }
  	}

  	// The scintillation integral(s) for a given material
  	// will be inserted in the table(s) according to the
  	// position of the material in the material table.

  	_theFastIntegralTable->insertAt(i,aPhysicsOrderedFreeVector);
    _theSlowIntegralTable->insertAt(i,bPhysicsOrderedFreeVector);
  }
}



void Electroluminescence::ComputeCumulativeDistribution(
  const G4PhysicsOrderedFreeVector& pdf, G4PhysicsOrderedFreeVector& cdf)
{
  G4double sum = 0.;
  cdf.InsertValues(pdf.Energy(0), sum);

  for (G4int i=1; i<pdf.GetVectorLength(); ++i) {
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
