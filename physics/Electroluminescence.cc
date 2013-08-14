// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 28 Oct 2009
//  
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "Electroluminescence.h"

#include "AlongStepRandomGenerator.h"
#include "IonizationDrift.h"

#include "G4ios.hh"
#include "G4EmProcessSubType.hh"
#include <G4Poisson.hh>
#include <G4OpticalPhoton.hh>


using namespace std;


namespace nexus {
  
  Electroluminescence::Electroluminescence(const G4String& process_name,
					   G4ProcessType type):
    G4VRestDiscreteProcess(process_name, type),
    _secondaries_first(false), _drift(0)
  {
    Initialize();
  }

  
  
  Electroluminescence::~Electroluminescence() 
  {
  }
  
  
  
  void Electroluminescence::Initialize()
  {
    _drift = new IonizationDrift(1. * mm/microsecond,
     				 1000. * micrometer/sqrt(cm),
				 300.  * micrometer/sqrt(cm),
				 0.,
				 22. * eV,
				 300. * mm);

    _rnd = new AlongStepRandomGenerator();
  }
  
  
  
  /// Proceed as in any other step (this method simply calls the
  /// equivalent PostStepDoIt).
  G4VParticleChange*
  Electroluminescence::AtRestDoIt(const G4Track& track, const G4Step& step)
  {
    return Electroluminescence::PostStepDoIt(track, step);
  }
  
  
  
  /// This method is called for each tracking step of a charged particle
  /// in a scintillator. A Poisson/Gauss-distributed number of photons is 
  /// generated according to the scintillation yield formula, distributed 
  /// evenly along the track segment and uniformly into 4pi
  G4VParticleChange*
  Electroluminescence::PostStepDoIt(const G4Track& track, const G4Step& step)
  {
    pParticleChange->Initialize(track);

//     G4MaterialPropertiesTable* mpt = 
//       track.GetMaterial()->GetMaterialPropertiesTable();

//     G4double number = mpt->GetConstProperty("DRIFTVELOCITY");
//     G4cout << "drift_velocity = " << number << G4endl;

    _drift->SetStep(step);
    G4int num_el = _drift->NumberOfCarriers();
    G4int num_ph = num_el * 1000;

    //for (size_t i=0; i<num_el; i++) {
      
    G4LorentzVector xyzt0 = _drift->GenerateOne();

    G4LorentzVector xyzt1(xyzt0);

    G4double timegap = (0.5*cm)/(9.*mm/microsecond);
    xyzt1.setT(xyzt1.t()+timegap);
    xyzt1.setZ(xyzt0.z()+0.5*cm);      

    _rnd->SetPoints(xyzt0, xyzt1);

    pParticleChange->SetNumberOfSecondaries(num_ph);

    if (_secondaries_first) {
      if (track.GetTrackStatus() == fAlive)
	pParticleChange->ProposeTrackStatus(fSuspend);
    }
    
    
    for (size_t j=0; j<num_ph; j++) {
	
      // Generate random photon direction
	
      G4double cos_theta = 1. - 2.*G4UniformRand();
      G4double sin_theta = sqrt((1.-cos_theta)*(1.+cos_theta));
	
      G4double phi = twopi*G4UniformRand();
      G4double sin_phi = sin(phi);
      G4double cos_phi = cos(phi);

      G4double px = sin_theta * cos_phi;
      G4double py = sin_theta * sin_phi;
      G4double pz = cos_theta;

      G4ThreeVector momentum(px, py, pz);

      // Determine polarization of new photon 

      G4double sx = cos_theta * cos_phi;
      G4double sy = cos_theta * sin_phi; 
      G4double sz = -sin_theta;

      G4ThreeVector polarization(sx, sy, sz);

      G4ThreeVector perpendicular = momentum.cross(polarization);

      phi = twopi*G4UniformRand();
      sin_phi = sin(phi);
      cos_phi = cos(phi);

      polarization = cos_phi * polarization + sin_phi * perpendicular;
      polarization = polarization.unit();

      // Generate a new photon:
      G4DynamicParticle* photon =
	new G4DynamicParticle(G4OpticalPhoton::Definition(), momentum);

      photon->SetPolarization(polarization.x(),
			      polarization.y(),
			      polarization.z());
	
      G4double sampledEnergy = 6. * eV;

      photon->SetKineticEnergy(sampledEnergy);
	
      G4LorentzVector point = _rnd->Shoot();

      //G4double aSecondaryTime = t0 + deltaTime;
      G4double time = point.t();

      G4ThreeVector position = point.v();
      // 	  x0 + rand * step.GetDeltaPosition();
	

      G4Track* secondary = new G4Track(photon, time, position);
      //secondary->SetTouchableHandle(step.GetPreStepPoint()->GetTouchableHandle());
      //aSecondaryTrack->SetTouchableHandle((G4VTouchable*)0);
	
      secondary->SetParentID(track.GetTrackID());

      pParticleChange->AddSecondary(secondary);
    }
    return G4VRestDiscreteProcess::PostStepDoIt(track, step);
  }

  
  
  G4double Electroluminescence::GetMeanFreePath(const G4Track&,
						G4double,
						G4ForceCondition* condition)
  {
    *condition = StronglyForced;
    return DBL_MAX;
  }

  
  
  G4double Electroluminescence::GetMeanLifeTime(const G4Track&,
						G4ForceCondition* condition)
  {
    *condition = Forced;
    return DBL_MAX;
  }



  G4int Electroluminescence::LightYield(G4double pressure, 
					G4double electric_field)
  {
    G4double a = 140. / kilovolt;
    G4double b = 116. / (cm * bar);
    G4double length = 1. * cm;

    G4double yield = (a * electric_field - b * pressure) * length;

    return G4int(yield);
  }

  
} // end namespace nexus
