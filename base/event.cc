// NEXT simulation: event.cc
//

#include <next/event.hh>


event::event(G4String ini_part_name, G4String dst_fname)
{
  G4ParticleTable *part_table = G4ParticleTable::GetParticleTable();
  ini_part_mass_ = part_table->FindParticle(ini_part_name)->GetPDGMass();
    
  // Getting the dst type from its name extension
  size_t i2 = dst_fname.find_last_of(".")+1;
  size_t il = dst_fname.size()-1;
  dst_ext_  = dst_fname.substr(i2,il);
  
  // Setting the dst file name to the appropiate writer
  if (dst_ext_=="h5") 
    bhep::bhep_svc::instance().get_writer_hdf5().open(dst_fname);
  else if (dst_ext_=="gz") 
    bhep::bhep_svc::instance().get_writer_gz().open(dst_fname);
  else if (dst_ext_=="txt") 
    bhep::bhep_svc::instance().get_writer_txt().open(dst_fname);
  else {
    G4cout << "\n\nERROR: non-valid extension of dst file!\n\n";
    exit(1);
  }
}
  
/*
int event::clear_event(bhep::event& evt){
  
  // Getting the particles
  vector <bhep::particle *> list_part = evt.true_particles();
  
  int num_del_parts = 0;
  
  // Identifying meaning-less particles
  for (unsigned int i=0; i<list_part.size(); i++) {
    G4String Ini_Vol  = list_part[i]->fetch_property("Ini_Vol");
    int num_cal_hits  = list_part[i]->hits("cal").size();
    int num_trk_hits  = list_part[i]->hits("trk").size();
    int num_daughters = list_part[i]->daughters().size();
    G4String G4_id    = list_part[i]->fetch_property("G4_id");
      
    // Deleting if the particle was generated in the Calorimeter AND
    // It did not produce any extra "CAL" or "TRK" hit AND 
    // It has no daughter particles (This last option could be discarded).
    if ( (Ini_Vol == "CAL_BLK") && (num_daughters == 0) &&
	 (num_cal_hits == 1) && (num_trk_hits == 0) ) {
      m_.message("      Deleting particle with G4_id:", G4_id, bhep::VVERBOSE);
      evt.delete_particle(list_part[i], bhep::TRUTH);
      
      num_del_parts +=1;
    }
    
    return num_del_parts;
  }
}
*/

void event::BeginOfEventAction(const G4Event* evt)
{
  G4int event_id = evt->GetEventID();    
    
  m_.message("\n###### Starting event number", event_id,"######", bhep::NORMAL);

  bhep::event& ievt = bhep::bhep_svc::instance().get_event();

  // Adding the G4 event number
  ievt.add_property("G4_evt_num", bhep::to_string(event_id));

  // Adding the initial particles mass
  ievt.add_property("Ini_Part_Mass", bhep::to_string(ini_part_mass_));
  m_.message("* BeginOfEventAction:: Setting evt prop. Ini_Part_Mass to",
	     ini_part_mass_/MeV, "MeV", bhep::VERBOSE);
}

 
void event::EndOfEventAction(const G4Event* evt)
{
  static G4int num_events = 0;

  G4int event_id = evt->GetEventID();

  // get number of stored trajectories
  //
  G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
  G4int n_trajectories = 0;
  
  if (trajectoryContainer) 
    n_trajectories = trajectoryContainer->entries();
  
  m_.message("\n\n***** Trajectories stored in this event:", n_trajectories, "*****\n",
	     bhep::VERBOSE);

  // extract the trajectories and draw them
  //
  if (G4VVisManager::GetConcreteInstance()) {
    for (G4int i=0; i<n_trajectories; i++) {
      G4Trajectory* trj = (G4Trajectory*)((*(evt->GetTrajectoryContainer()))[i]);
      trj->DrawTrajectory();
    }
  }

  // Dump bhep events to "dst" file
  //
  bhep::event& ievt = bhep::bhep_svc::instance().get_event();
  
  /*
  // Clearing the event if applies
  if (!store_all_) {
              m_.message("*** Clearing event number", event_id, bhep::DETAILED);
              int num_del_parts = clear_event(ievt);
              m_.message("*** Number of deleted particles:", num_del_parts, "\n\n", bhep::VERBOSE);
          }
  */
  
  // Writing the event to the dst file
  //
  // dst_writer_->write(ievt, event_id);
  if (dst_ext_=="h5") 
    bhep::bhep_svc::instance().get_writer_hdf5().write(ievt, event_id+1);
  else if (dst_ext_=="gz") 
    bhep::bhep_svc::instance().get_writer_gz().write(ievt, event_id+1);
  else if (dst_ext_=="txt") 
    bhep::bhep_svc::instance().get_writer_txt().write(ievt, event_id+1);

  // bhep event is re-cleared
  //
  ievt.clear();

  // Verbosing
  m_.message("###### Event number", event_id, "finished ######\n\n", bhep::DETAILED);
  num_events ++;
  if (!(num_events % 500))
    m_.message("*** ", num_events, " processed events.", bhep::CONCISE);
}


