// NEXUS: stepping_trigger.cc
//    -- Last modification: 
// 

#include <nexus/stepping_trigger.h>
#include <nexus/tracking.h>
#include <nexus/run.h>


namespace nexus {

  stepping_trigger::stepping_trigger( bhep::prlevel vl )
  {
    msg = bhep::messenger(vl);
  }

  
  // return particle corresponding to a given G4 index
  bhep::particle& stepping_trigger::get_particle( G4int G4_id )
  {
    bhep::event& evt = bhep::bhep_svc::instance().get_event();
    vector<bhep::particle *> particles;
    
    if( evt.filter( bhep::TRUTH, "G4Id", bhep::to_string(G4_id), particles) ) 
      return *particles[0];
    else {
      std::ostringstream err_msg;
      err_msg << "\ERROR.- stepping_trigger: Trying to retrieve a non-existing particle from transient event"
	      << std::endl;
      throw std::runtime_error(err_msg.str());
    }
  }

  
  void stepping_trigger::UserSteppingAction( const G4Step* aStep )
  { 
    G4Track* track = fpSteppingManager->GetTrack();
    bhep::event& bevt = bhep::bhep_svc::instance().get_event();

    //Check if event has already been discarded by the trigger
    if( bevt.fetch_property("TriggerStatus")=="1" )
      track->SetTrackStatus( fKillTrackAndSecondaries );

    G4int    track_id     = track->GetTrackID();
    G4int    step_id      = track->GetCurrentStepNumber();
    G4String part_name    = track->GetDefinition()->GetParticleName();
    G4double part_charge  = track->GetDynamicParticle()->GetCharge();
    G4String volume       = track->GetVolume()->GetName();
    G4ThreeVector ini_pos = track->GetVertexPosition();
    G4double track_time   = track->GetLocalTime() * ns;
    //G4int  mother_id    = track->GetParentID();

    bhep::particle& part = get_particle(track_id);
    
    static G4double energy_dep_ACTIVE;
    static G4double time;
    static G4String previous_volume;

    // Resetting tracking variables if particle's 1st step
    if( step_id==1 ) {
      energy_dep_ACTIVE=0.;
      time = track_time;
      previous_volume = volume;
    }

    
    ////////////////////////////////////////////////////////
    ///  ACTIVE  ///////////////////////////////////////////
    
    if( volume=="ACTIVE" ) {

      if( part_charge!=0 ) {
	
	if( previous_volume!="ACTIVE" ) {
	  bevt.change_property("TriggerStatus", "1");
	  track->SetTrackStatus( fKillTrackAndSecondaries );
	}
	
	G4double delta_time = aStep->GetDeltaTime();
	time += delta_time;
	
	G4double energy_step = aStep->GetTotalEnergyDeposit();
	energy_dep_ACTIVE += energy_step;

	G4double step_length = aStep->GetStepLength()*mm;

	// Storing hits in the dst
	//
	bhep::hit* hit = new bhep::hit("bulk");

	hit->set_mother_particle(part);

	G4ThreeVector step_pos = track->GetPosition();
	hit->set_point( bhep::Point3D( step_pos.x(), step_pos.y(), step_pos.z() ) );
		
	hit->add_property( "EnergyDep", bhep::to_string(energy_step) );
	hit->add_property( "Time", bhep::to_string(time) );
	hit->add_property( "StepLength", bhep::to_string(step_length) );
	
	part.add_hit("bulk", hit);
	
	bevt.change_property("ContainsHits", "1"); // Alguna particula tiene hits!
	
	G4String next_volume = track->GetNextVolume()->GetName();
	G4double kin_energy = track->GetKineticEnergy();
	
	if( kin_energy==0. || next_volume!="ACTIVE" )
	  part.change_property("EnergyDep", bhep::to_string(energy_dep_ACTIVE));
      }
    }

    if( volume!="ACTIVE" && part.fetch_property("OutActive")=="0" )
      part.change_property("OutActive", "1");
    
  }
  
} // namespace nexus
