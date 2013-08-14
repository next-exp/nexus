// NEXUS: event_trigger.cc 
//    -- Last modification: June 4th 2007, jmalbos
//

#include <nexus/event_trigger.h>
#include <bhep/EventManager2.h>


namespace nexus {

  
  event_trigger::event_trigger( EventManager2& evt_mgr, bhep::prlevel vl )
  {
    __evt_mgr = &evt_mgr;
    msg = bhep::messenger(vl);
    msg.message("nexus::event_trigger instance created.", bhep::VERBOSE);
  }


  void event_trigger::BeginOfEventAction(const G4Event* evt)
  {
    G4int event_id = evt->GetEventID();    
    msg.message("\n###### Starting event number", event_id,"######", bhep::VERBOSE);

    // Event properties
    bhep::event& bevt = bhep::bhep_svc::instance().get_event();
    bevt.add_property("G4EventNum", bhep::to_string(event_id)); // G4 event number
    bevt.add_property("TriggerStatus", "0");
    bevt.add_property("ContainsHits", "0");
  }

  
  void event_trigger::EndOfEventAction(const G4Event* evt)
  {
    static G4int num_events = 0;
    G4int event_id = evt->GetEventID();

    //  Visualization  /////////////////////////////////////////////////
    //
    G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
    G4int n_trajectories = 0;
  
    if (trajectoryContainer) 
      n_trajectories = trajectoryContainer->entries();
  
    if (G4VVisManager::GetConcreteInstance()) {
      for (G4int i=0; i<n_trajectories; i++) {
	G4Trajectory* trj = (G4Trajectory*)((*(evt->GetTrajectoryContainer()))[i]);
	trj->DrawTrajectory();
      }
    }
    ////////////////////////////////////////////////////////////////////
        
    // Writing the event to the dst file
    bhep::event& bevt = bhep::bhep_svc::instance().get_event();
    if( bevt.fetch_property("TriggerStatus")=="0" &&
	bevt.fetch_property("ContainsHits")=="1")
      __evt_mgr->write(bevt);
    
    // bhep transient event is re-cleared
    bevt.clear();

    // Verbosing
    msg.message("###### Event number", event_id, "finished ######\n\n", bhep::VERBOSE);
    num_events ++;
    if (!(num_events % 500))
      msg.message("*** ", num_events, " processed events.", bhep::NORMAL);
  }
  
} // namespace nexus
