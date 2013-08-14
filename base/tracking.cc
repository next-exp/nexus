// NEXT simulation: tracking.cc
//

#include <next/tracking.hh>



bhep::particle& get_bhep_part(G4int G4_id) 
{
  bhep::event& ievt = bhep::bhep_svc::instance().get_event();
  vector <bhep::particle *> list_part;
  
  if (ievt.filter(bhep::TRUTH, "G4_id", bhep::to_string(G4_id), list_part)) {
    // There is only a particle per G4_id
    return *list_part[0] ;
  }
  else {
    G4cout << "\n*** TRACKING ERROR: Trying to retrieve a G4 particle inexistent in bhep\n";
    exit(1);
  }
}


void tracking::PreUserTrackingAction(const G4Track* aTrack)
{
  G4int track_id  = aTrack->GetTrackID();
  G4int mother_id = aTrack->GetParentID();
  G4String part_name = aTrack->GetDefinition()->GetParticleName();
  G4String volume = aTrack->GetVolume()->GetName();
  
  // Getting the bhep event
  bhep::event& ievt = bhep::bhep_svc::instance().get_event();
  
  // Creating the new particle
  bhep::particle* ipart = new bhep::particle(bhep::TRUTH, part_name);
  
  // Adding properties to the particle
  //
  ipart->add_property("G4_id", bhep::to_string(track_id));
  m_.message("* PreUserTrackingAction:: Setting its G4_id to", track_id, bhep::VVERBOSE);
  
  if (mother_id == 0) {
    // Setting particle as primary
    ipart->set_primary(true);
    m_.message("* PreUserTrackingAction:: Setting particle as primary", bhep::VERBOSE);
  }
  else {
    // Setting particle as secondary
    ipart->set_primary(false);
    m_.message("* PreUserTrackingAction:: Setting particle as secondary", bhep::VERBOSE);
    
    // Setting mother - daughter relation
    bhep::particle& mother_part = get_bhep_part(mother_id);
    ipart->set_mother(mother_part);
    mother_part.add_daughter(*ipart);
    m_.message("     Setting its mother G4_id to", mother_id, bhep::VVERBOSE);
  }
  
  // Setting the volume where it was generated
  ipart->add_property("IniVol", volume);
  m_.message("* PreUserTrackingAction:: Setting its Initial Volume to", volume, bhep::VVERBOSE);
  
  if (mother_id == 0) {
    // Adding flag properties for initial particles
    ipart->add_property("OutActive", "0");
    
    // Adding Edep properties for initial particles
    ipart->add_property("EnergyDep", "0.0");
    ipart->add_property("TrackLength", "0.0");    
  }
  
  // Adding the particle to bhep::event
  ievt.add_true_particle(ipart);
  m_.message("* PreUserTrackingAction:: Adding new particle", part_name, bhep::VERBOSE);
}


void tracking::PostUserTrackingAction(const G4Track* aTrack)
{
  G4int track_id = aTrack->GetTrackID();
  
  // Getting the right BHEP particle
  bhep::particle& ipart = get_bhep_part(track_id);
  
  // Adding production position and tri-momentum
  //
  G4ThreeVector ini_pos = aTrack->GetVertexPosition();
  G4ThreeVector ini_p_direction = aTrack->GetVertexMomentumDirection();
  G4double      ini_ekin = aTrack->GetVertexKineticEnergy();
  G4double      mass = ipart.mass();
  G4double      ini_p_tot = sqrt ( sqr(mass+ini_ekin) - sqr(mass) );  // from  (m²+ekin²)² = E² = m²+p²
  G4ThreeVector ini_p = ini_p_tot * ini_p_direction;
  ipart.set_p(ini_p/MeV);
  ipart.set_vertex(ini_pos.x()/mm, ini_pos.y()/mm, ini_pos.z()/mm);
  
  // Adding decay position and tri-momentum
  //
  G4String decay_vol = aTrack->GetVolume()->GetName();
  G4ThreeVector fin_pos = aTrack->GetPosition();
  G4ThreeVector fin_p = aTrack->GetMomentum();
  ipart.set_decay_p(fin_p.x()/MeV, fin_p.y()/MeV, fin_p.z()/MeV);
  ipart.set_decay_vertex(fin_pos.x()/mm, fin_pos.y()/mm, fin_pos.z()/mm);
  ipart.add_property("DecVol", decay_vol);
  
  m_.message("\n************************************************************", bhep::DETAILED);
  m_.message("*  Initial E tot (MeV) : ", ipart.e()/MeV, bhep::DETAILED);
  m_.message("*  Initial Ekin (MeV)  : ", ini_ekin/MeV, bhep::VERBOSE);
  m_.message("*  Initial p tot (MeV) : ", ini_p_tot/MeV, bhep::VERBOSE);
  m_.message("*  Initial p (MeV)     : ", ini_p/MeV, bhep::DETAILED);
  m_.message("*  Initial p direction : ", ini_p_direction, bhep::VERBOSE);
  m_.message("*  Vertex position (mm): ", ini_pos/mm, bhep::DETAILED); 
  m_.message("*  Final p (MeV)       : ", fin_p/MeV, bhep::DETAILED);
  m_.message("*  Decay position (mm) : ", fin_pos/mm, bhep::DETAILED);
  m_.message("*  Decay Volume        : ", decay_vol, bhep::VERBOSE);
  m_.message("************************************************************\n", bhep::DETAILED);
}
