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
//#include "DriftTrackInfo.h"
#include "BaseDriftField.h"
#include "UniformElectricDriftField.h"
#include "G4Scintillation.hh"
#include "XenonGasProperties.h"
#include "G4Material.hh" 
#include "G4MaterialPropertiesTable.hh"
#include "G4PhysicsOrderedFreeVector.hh"

#include <G4TransportationManager.hh>
#include <G4OpticalPhoton.hh>
#include <Randomize.hh>
#include <G4Poisson.hh>



namespace nexus {

  
  Electroluminescence::Electroluminescence(const G4String& process_name,
					   G4ProcessType type):
    G4VDiscreteProcess(process_name, type), _secondaries_first(true)

  {
    pParticleChange = &_ParticleChange;
    
    _theSlowIntegralTable = NULL;
    _theFastIntegralTable = NULL;

    BuildThePhysicsTable();

  }
  
  
  
  Electroluminescence::~Electroluminescence()
  {
  }
  
  
  
  G4bool Electroluminescence::IsApplicable(const G4ParticleDefinition& pdef)
  {
    return (pdef == *IonizationElectron::Definition());
  }



  G4VParticleChange*
  Electroluminescence::PostStepDoIt(const G4Track& track, const G4Step& step)
  {
    G4cout << "EL: PostStepDoIt" << G4endl;
    // Initialize particle change with current track values
    _ParticleChange.Initialize(track);
    
    // Get the current region and its associated drift field
    G4Region* region = track.GetVolume()->GetLogicalVolume()->GetRegion();
    // BaseDriftField* field =
    //   dynamic_cast<BaseDriftField*>(region->GetUserInformation());
    UniformElectricDriftField* field =
      dynamic_cast<UniformElectricDriftField*>(region->GetUserInformation());
 
    // If no drift field is defined (i.e., the dynamic_cast returned a null
    // pointer), kill the track and finish the process.
    if (!field) {
      _ParticleChange.ProposeTrackStatus(fStopAndKill);
      return G4VDiscreteProcess::PostStepDoIt(track, step);
    }
    
    G4double step_length = step.GetStepLength();
    
    //G4double yield = field->LightYield();
    G4double yield = field->GetLightYield();
 
    G4cout << "Yield: " << yield << G4endl;
    if (yield == 0){
      return G4VDiscreteProcess::PostStepDoIt(track, step);
    }

    //G4cout << "yield of photons: " << yield / (1/cm) << G4endl;
    

    // Get the average light yield for the last step from the 
    // drift track info
    // DriftTrackInfo* drift_info = 
    //   dynamic_cast<DriftTrackInfo*>(track.GetUserInformation());
    
    // if (!drift_info) {
    //   G4cout << "[Electroluminescence] WARNING: "
    // 	     << "No DriftTrackInfo attached to the ionization electron. "
    // 	     << "Therefore, EL light emission cannot be simulated.\n" 
    // 	     << G4endl;
    //   return G4VDiscreteProcess::PostStepDoIt(track, step);
    // }

    // G4double yield = drift_info->GetELLightYield();
    
    // Generate a random number of photons around mean 'yield'

    yield = yield * step_length;
    G4int num_photons;
        
    if (yield < 10.) { // Poissonian regime
      num_photons = G4int(G4Poisson(yield));
    }
    else {             // Gaussian regime            
      G4double sigma = sqrt(yield);
      num_photons = G4int(G4RandGauss::shoot(yield, sigma) + 0.5);
    }
    //_sum = _sum + num_photons;
   
    num_photons = 500000;
      
    G4cout << "Numb of EL photons: " << num_photons << G4endl;
    //   num_photons = 10;
    
    _ParticleChange.SetNumberOfSecondaries(num_photons);
    
    if (_secondaries_first) {
      if (track.GetTrackStatus() == fAlive)
	_ParticleChange.ProposeTrackStatus(fSuspend);
    }

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
	  if(Fast_Intensity){
	    ScintillationTime = mat_mpt->GetConstProperty("FASTTIMECONSTANT");
	    ScintillationIntegral =
	      (G4PhysicsOrderedFreeVector*)((*_theFastIntegralTable)(mat_index));
	  }
	  if(Slow_Intensity){
	    ScintillationTime = mat_mpt->GetConstProperty("SLOWTIMECONSTANT");
	    ScintillationIntegral =
	      (G4PhysicsOrderedFreeVector*)((*_theSlowIntegralTable)(mat_index));
	  }
	} else {
	  G4double YieldRatio = mat_mpt->GetConstProperty("YIELDRATIO");
	  num = G4int (std::min(YieldRatio,1.0) * num_photons);
	  ScintillationTime = mat_mpt->GetConstProperty("FASTTIMECONSTANT");
	  ScintillationIntegral =
	    (G4PhysicsOrderedFreeVector*)((*_theFastIntegralTable)(mat_index));
	}
      } else {
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
      
	photon->SetPolarization(polarization.x(), 
				polarization.y(), 
				polarization.z());

	// Determine photon energy
	G4double sc_value = G4UniformRand()*sc_max;
	G4double sampled_energy = 
	ScintillationIntegral->GetEnergy(sc_value);
	photon->SetKineticEnergy(sampled_energy);
	
	//     G4double kinetic_energy = 6. * eV;
	//   photon->SetKineticEnergy(kinetic_energy);
	
	// Generate a random position (and related time) along the
	// electron drift line
	
// 	if (cfg.PeekIParam("photonsperpoint")) {
// 	  //  const ParamStore& cfg = ConfigService::Instance().Generation();
// 	  G4double drift_dist = cfg.GetDParam("drift_distance");
// 	  G4double transv_diff = cfg.GetDParam("transv_diffusion");
// 	  G4double long_diff = cfg.GetDParam("longit_diffusion");
// 	  G4double sigmaxy = transv_diff*sqrt(drift_dist);
// 	  G4double sigmaz = long_diff*sqrt(drift_dist);
// 	  G4double x = G4RandGauss::shoot(position.getX(), sigmaxy);
// 	  G4double y = G4RandGauss::shoot(position.getY(), sigmaxy);
// 	  //	  G4double z = G4RandGauss::shoot(position.getZ(), sigmaz);	  
// 	  initial_position.setX(x);
// 	  initial_position.setY(y);
// 	}

	G4LorentzVector xyzt = 
	  field->GeneratePointAlongDriftLine(initial_position);
	
	// Create the track
	G4Track* secondary = new G4Track(photon, xyzt.t(), xyzt.v());
	secondary->SetParentID(track.GetTrackID());
	//secondary->
	//SetTouchableHandle(step.GetPreStepPoint()->GetTouchableHandle());
      _ParticleChange.AddSecondary(secondary);
      }
    }
    
    return G4VDiscreteProcess::PostStepDoIt(track, step);
  }
  
 
  void Electroluminescence::BuildThePhysicsTable()
  {
    if (_theFastIntegralTable && _theSlowIntegralTable) return;

    const G4MaterialTable* theMaterialTable = 
      G4Material::GetMaterialTable();
    G4int numOfMaterials = G4Material::GetNumberOfMaterials();

    // create new physics table
	
    if(!_theFastIntegralTable) _theFastIntegralTable = new G4PhysicsTable(numOfMaterials);
    if(!_theSlowIntegralTable) _theSlowIntegralTable = new G4PhysicsTable(numOfMaterials);

    // loop for materials

    for (G4int i=0 ; i < numOfMaterials; i++)
      {
  	G4PhysicsOrderedFreeVector* aPhysicsOrderedFreeVector =
  	  new G4PhysicsOrderedFreeVector();
  	G4PhysicsOrderedFreeVector* bPhysicsOrderedFreeVector =
  	  new G4PhysicsOrderedFreeVector();

  	// Retrieve vector of scintillation wavelength intensity for
  	// the material from the material's optical properties table.

  	G4Material* aMaterial = (*theMaterialTable)[i];

  	G4MaterialPropertiesTable* aMaterialPropertiesTable =
  	  aMaterial->GetMaterialPropertiesTable();

  	if (aMaterialPropertiesTable) {

  	  G4MaterialPropertyVector* theFastLightVector = 
  	    aMaterialPropertiesTable->GetProperty("FASTCOMPONENT");

  	  if (theFastLightVector) {
		
  	    // Retrieve the first intensity point in vector
  	    // of (photon energy, intensity) pairs 

  	    // theFastLightVector->ResetIterator();
  	    // ++(*theFastLightVector);
	    // advance to 1st entry 

  	    // G4double currentIN = theFastLightVector->
  	    //   GetProperty();

	    G4double currentIN = (*theFastLightVector)[0];

  	    if (currentIN >= 0.0) {

  	      // Create first (photon energy, Scintillation 
  	      // Integral) pair  

  	      // G4double currentPM = theFastLightVector->
  	      // 	GetPhotonEnergy();
	      G4double currentPM = theFastLightVector->Energy(0);
  
  	      G4double currentCII = 0.0;

  	      aPhysicsOrderedFreeVector->
  		InsertValues(currentPM , currentCII);

  	      // Set previous values to current ones prior to loop

  	      G4double prevPM  = currentPM;
  	      G4double prevCII = currentCII;
  	      G4double prevIN  = currentIN;

  	      // loop over all (photon energy, intensity)
  	      // pairs stored for this material  

  	      // while(++(*theFastLightVector))
  	      // 	{
  	      // 	  currentPM = theFastLightVector->
  	      // 	    GetPhotonEnergy();

  	      // 	  currentIN=theFastLightVector->	
  	      // 	    GetProperty();

  	      // 	  currentCII = 0.5 * (prevIN + currentIN);

  	      // 	  currentCII = prevCII +
  	      // 	    (currentPM - prevPM) * currentCII;

  	      // 	  aPhysicsOrderedFreeVector->
  	      // 	    InsertValues(currentPM, currentCII);

  	      // 	  prevPM  = currentPM;
  	      // 	  prevCII = currentCII;
  	      // 	  prevIN  = currentIN;
  	      // 	}
	      for (size_t ii = 1;
		   ii < theFastLightVector->GetVectorLength();
		   ++ii)
		{
		  currentPM = theFastLightVector->Energy(ii);
		  currentIN = (*theFastLightVector)[ii];
		  
		  currentCII = 0.5 * (prevIN + currentIN);
		  
		  currentCII = prevCII +
		    (currentPM - prevPM) * currentCII;
		  
		  aPhysicsOrderedFreeVector->
		    InsertValues(currentPM, currentCII);
		  
		  prevPM  = currentPM;
		  prevCII = currentCII;
		  prevIN  = currentIN;
		}
	      
  	    }
  	  }

  	  G4MaterialPropertyVector* theSlowLightVector =
  	    aMaterialPropertiesTable->GetProperty("SLOWCOMPONENT");

  	  if (theSlowLightVector) {

  	    // Retrieve the first intensity point in vector
  	    // of (photon energy, intensity) pairs

  	    // theSlowLightVector->ResetIterator();
  	    // ++(*theSlowLightVector);  // advance to 1st entry

  	    // G4double currentIN = theSlowLightVector->
  	    //   GetProperty();

	    G4double currentIN = (*theSlowLightVector)[0];

  	    if (currentIN >= 0.0) {

  	      // Create first (photon energy, Scintillation
  	      // Integra)l pair

  	      // G4double currentPM = theSlowLightVector->
  	      // 	GetPhotonEnergy();
	      G4double currentPM = theSlowLightVector->Energy(0);

  	      G4double currentCII = 0.0;

  	      bPhysicsOrderedFreeVector->
  		InsertValues(currentPM , currentCII);

  	      // Set previous values to current ones prior to loop

  	      G4double prevPM  = currentPM;
  	      G4double prevCII = currentCII;
  	      G4double prevIN  = currentIN;

  	      // loop over all (photon energy, intensity)
  	      // pairs stored for this material

  	      // while(++(*theSlowLightVector))
  	      // 	{
  	      // 	  currentPM = theSlowLightVector->
  	      // 	    GetPhotonEnergy();

  	      // 	  currentIN=theSlowLightVector->
  	      // 	    GetProperty();

  	      // 	  currentCII = 0.5 * (prevIN + currentIN);

  	      // 	  currentCII = prevCII +
  	      // 	    (currentPM - prevPM) * currentCII;

  	      // 	  bPhysicsOrderedFreeVector->
  	      // 	    InsertValues(currentPM, currentCII);

  	      // 	  prevPM  = currentPM;
  	      // 	  prevCII = currentCII;
  	      // 	  prevIN  = currentIN;
  	      // 	}
	      for (size_t ii = 1;
		   ii < theSlowLightVector->GetVectorLength();
		   ++ii)
		{
		  currentPM = theSlowLightVector->Energy(ii);
		  currentIN = (*theSlowLightVector)[ii];
 
		  currentCII = 0.5 * (prevIN + currentIN);
 
		  currentCII = prevCII +
		    (currentPM - prevPM) * currentCII;
 
		  bPhysicsOrderedFreeVector->
		    InsertValues(currentPM, currentCII);
 
		  prevPM  = currentPM;
		  prevCII = currentCII;
		  prevIN  = currentIN;
		}

  	    }
  	  }
  	}

  	// The scintillation integral(s) for a given material
  	// will be inserted in the table(s) according to the
  	// position of the material in the material table.

  	_theFastIntegralTable->insertAt(i,aPhysicsOrderedFreeVector);
        _theSlowIntegralTable->insertAt(i,bPhysicsOrderedFreeVector);

      }
  }
 
  
  G4double Electroluminescence::GetMeanFreePath
  (const G4Track&, G4double, G4ForceCondition* condition)
  {
    *condition = StronglyForced;
    return DBL_MAX;
  }
  

} // end namespace nexus
