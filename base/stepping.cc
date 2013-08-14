// NEXT simulation: stepping.cc
// Implementation of simulation stepping
// 

#include <next/stepping.hh>
#include <next/tracking.hh>
#include <next/run.hh>


stepping::stepping(G4String ini_part)
{
  m_.message("In Stepping Constructor", bhep::VERBOSE);
  
  ini_part_ = ini_part;
}


stepping::~stepping()
{
  m_.message("In Stepping Destructor", bhep::VERBOSE);
}


void stepping::UserSteppingAction(const G4Step* aStep)
{ 
  //const G4VTouchable* post_step_touch = aStep->GetPostStepPoint()->GetTouchable();
  
  G4Track* track       = fpSteppingManager->GetTrack();
  G4int    track_id    = track->GetTrackID();
  G4int    mother_id   = track->GetParentID();
  G4int    step_id     = track->GetCurrentStepNumber();
  G4String part_name   = track->GetDefinition()->GetParticleName();
  //G4double part_charge = track->GetDynamicParticle()->GetCharge();
  G4String volume      = track->GetVolume()->GetName();
  
  // Getting the right BHEP particle
  bhep::particle& ipart = get_bhep_part(track_id);
  
  
  if (mother_id == 0) {
    
    if (volume == "ACTIVE") {

      static G4double energy_dep = 0.;
      static G4int prev_step_id = 0;
      
      
      // HACK: Only known test to reset when changing of particle
      // (because it is possible to have two different particles consecutively
      // with the same track_id) 
      if (step_id < prev_step_id)
	energy_dep = 0.0;
      
      prev_step_id = step_id;
      
      G4double energy_step = aStep->GetTotalEnergyDeposit();
      energy_dep += energy_step;
      
      // Creating and storing tracking hits
      //
      // Creating BHEP new hit
      bhep::hit* hit = new bhep::hit("active");
      // Getting and storing the position
      G4ThreeVector step_pos = track->GetPosition();
      hit->set_point( bhep::Point3D( step_pos.x(), step_pos.y(), step_pos.z() ) );
      // Associating the hit to the particle
      hit->set_mother_particle(ipart);
      hit->add_property( "EnergyDep", bhep::to_string(energy_step) );
      ipart.add_hit("active", hit);
      m_.message("* UserSteppingAction:: Adding new tracking hit: ", *hit, bhep::VERBOSE);
      
      // If the particle leaves the ACTIVE TPC volume or decays, the properties are stored
      G4String next_volume = track->GetNextVolume()->GetName();
      G4double ekin = track->GetKineticEnergy();

      if ( (ekin == 0.) || (next_volume != "ACTIVE")) {
	
	G4double track_length = track->GetTrackLength();
	
	// Adding the track length within the ACTIVE volume of the TPC 
	ipart.change_property("TrackLength", bhep::to_string(track_length));
	m_.message("* UserSteppingAction:: Modifying prop. TrackLength to",
		   track_length/cm, "cm", bhep::VERBOSE);
	// Adding the energy deposited in the chamber only for initial particles	
	ipart.change_property("EnergyDep", bhep::to_string(energy_dep));
	m_.message("* UserSteppingAction:: Modifying prop. EnergyDep to",
		   energy_dep/MeV, "MeV", bhep::VERBOSE);
      }
      
      // Setting the flag fist time the particle leaves the ACTIVE volume
      if ((next_volume != "ACTIVE") && (ipart.fetch_property("OutActive") == "0")) {
	// Setting the flag
	ipart.change_property("OutActive", "1");
	m_.message("* UserSteppingAction:: Setting prop. OutActive to 1", bhep::VERBOSE);
      }            
    }
  }
}

