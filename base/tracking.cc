// NEXUS: tracking.cc
//    -- Last modification: April 1 2008, jmalbos
//

#include <nexus/tracking.h>


namespace nexus {

  tracking::tracking( bhep::prlevel vl)
  {
    msg = bhep::messenger(vl);
  }
  

  bhep::particle& tracking::get_particle( G4int G4_id ) 
  {
    bhep::event& evt = bhep::bhep_svc::instance().get_event();
    vector <bhep::particle *> particles;
    
    if( evt.filter( bhep::TRUTH, "G4Id", bhep::to_string(G4_id), particles ) )
      return *particles[0];
    else {
      std::ostringstream err_msg;
      err_msg << "\ERROR.- tracking: Trying to retrieve a non-existing particle from transient event."
	      << std::endl;
      throw std::runtime_error(err_msg.str());
    }
  }

  
  void tracking::PreUserTrackingAction(const G4Track* aTrack)
  {
    G4int track_id       = aTrack->GetTrackID();
    G4int mother_id      = aTrack->GetParentID();
    G4String part_name   = aTrack->GetDefinition()->GetParticleName();
    G4String volume      = aTrack->GetVolume()->GetName();
  
    // Getting the transient event using bhep service 
    bhep::event& evt = bhep::bhep_svc::instance().get_event();

    // Creating a new MC particle 
    bhep::particle* part = new bhep::particle( bhep::TRUTH, part_name );
  
    // Particle family hierarchy
    if (mother_id == 0) {
      part->set_primary(true);  // Primary
      part->add_property("CreatorProcess", "none");
    }
    else {
      part->set_primary(false); // Secondary
      bhep::particle& mother_part = get_particle(mother_id);
      part->set_mother(mother_part);
      mother_part.add_daughter(*part);
      part->add_property("CreatorProcess", aTrack->GetCreatorProcess()->GetProcessName());
    }
    
    // Particle common properties 
    //
    part->add_property("G4Id", bhep::to_string( track_id ));
    part->add_property("IniVol", volume); 
    part->add_property("OutActive",   "0"); 
    part->add_property("EnergyDep",   "0.0");
  
    // Adding the particle to bhep::event
    evt.add_true_particle(part);
  }


  void tracking::PostUserTrackingAction(const G4Track* aTrack)
  {
    // Getting the particle from the transient event
    G4int track_id = aTrack->GetTrackID();
    bhep::particle& part = get_particle(track_id);
  
    // Kinematics at production vertex
    //
    G4ThreeVector vtx_pos         = aTrack->GetVertexPosition();
    G4ThreeVector vtx_p_direction = aTrack->GetVertexMomentumDirection();
    G4double      vtx_kin_energy  = aTrack->GetVertexKineticEnergy();
    G4double      mass            = part.mass();
    G4double      vtx_p_tot       = sqrt( sqr(mass+vtx_kin_energy)-sqr(mass) ); // E² = m²+p² = (m²+ekin²)² 
    G4ThreeVector vtx_p           = vtx_p_tot * vtx_p_direction;

    part.set_p( vtx_p/MeV );
    part.set_vertex( vtx_pos.x()/mm, vtx_pos.y()/mm, vtx_pos.z()/mm );
  
    // Kinematics at decay vertex
    //
    G4String      decay_vol    = aTrack->GetVolume()->GetName();
    G4ThreeVector decay_pos    = aTrack->GetPosition();
    G4ThreeVector decay_p      = aTrack->GetMomentum();
    G4double      track_length = aTrack->GetTrackLength();

    part.set_decay_p( decay_p.x()/MeV, decay_p.y()/MeV, decay_p.z()/MeV);
    part.set_decay_vertex( decay_pos.x()/mm, decay_pos.y()/mm, decay_pos.z()/mm);
    part.add_property("DecVol", decay_vol);
    part.add_property("TrackLength", bhep::to_string(track_length));
  
    /*
    msg.message("---------------------------------------------------------------", bhep::DETAILED);
    msg.message(" - Initial E tot (MeV) :", part.e()/MeV, bhep::DETAILED);
    msg.message(" - Initial Ekin (MeV)  :", vtx_kin_energy/MeV, bhep::VERBOSE);
    msg.message(" - Initial p tot (MeV) :", vtx_p_tot/MeV, bhep::VERBOSE);
    msg.message(" - Initial p (MeV)     :", vtx_p/MeV, bhep::DETAILED);
    msg.message(" - Initial p direction :", vtx_p_direction, bhep::VERBOSE);
    msg.message(" - Vertex position (mm):", vtx_pos/mm, bhep::DETAILED); 
    msg.message(" - Final p (MeV)       :", decay_p/MeV, bhep::DETAILED);
    msg.message(" - Decay position (mm) :", decay_pos/mm, bhep::DETAILED);
    msg.message(" - Decay Volume        :", decay_vol, bhep::VERBOSE);
    msg.message("---------------------------------------------------------------", bhep::DETAILED);
    */
  }

} // namespace nexus
