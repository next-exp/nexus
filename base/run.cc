// NEXT simulation: run.cc
// 

#include <next/run.hh>



run::run(G4String dst_fname) {

  dst_fname_ = dst_fname;
  
  // Getting the dst type from its name extension
  size_t i2 = dst_fname.find_last_of(".")+1;
  size_t il = dst_fname.size()-1;
  dst_ext_  = dst_fname.substr(i2,il);
}


void run::BeginOfRunAction(const G4Run* aRun)
{
  G4cout <<  G4endl << "#########################################################" << G4endl;
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  G4cout << "#########################################################" << G4endl << G4endl;
  
  if (G4VVisManager::GetConcreteInstance())
    {
      G4UImanager* UI = G4UImanager::GetUIpointer();
      UI->ApplyCommand("/vis/scene/notifyHandlers");
    } 
}


void run::EndOfRunAction(const G4Run* aRun)
{
  G4cout << G4endl << "#########################################################" << G4endl;
  G4cout << "### Run " << aRun->GetRunID() << " end." << G4endl;
  G4cout << "#########################################################" << G4endl << G4endl;
  
  if (G4VVisManager::GetConcreteInstance()) {
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");
  }
  
  // Dumping useful information stored through "bhep"
  //
  // create a generic store
  bhep::gstore store;
  
  // and the reader for CHECK variables
  bhep::sreader reader(store);
  reader.file("next.par");
  reader.info_level(bhep::NORMAL);
  reader.group("CHECK");
  reader.read();
  
  G4int print_parts_ = store.fetch_istore("print_parts");
  
  // If it is set
  if (print_parts_>=1) {
    
    G4cout << G4endl << G4endl << "#########################################################" << G4endl;
    G4cout << "### START READING EVENTS FROM   " << dst_fname_ << G4endl;
    G4cout << "#########################################################" << G4endl << G4endl;
    
    G4int num_events = 0;
    
    // for 'hdf5' dst
    if (dst_ext_ == "h5") {
      bhep::bhep_svc::instance().get_writer_hdf5().close();
      bhep::reader_hdf5& rh = bhep::bhep_svc::instance().get_reader_hdf5();
      rh.open(dst_fname_);
      size_t ievent = 1;
      bool goodkey = true;
      while(goodkey) {
	try
	  {
	    bhep::event& new_event = rh.read_event(ievent);
	    G4cout << G4endl << G4endl << "***************************************************************";
	    G4cout << G4endl << "Reading event number = " << ievent << endl;
	    G4cout << "***************************************************************" << G4endl << G4endl;
	    G4cout << new_event << G4endl;
	    ievent++;
	  }
	catch(bhep::bad_key)
	  {
	    cout << "warning, ievent = " << ievent << " does not exist" << endl;
	    goodkey = false;
	  }
      }
    }
    // for 'txt' dst
    else if (dst_ext_ == "txt") {
      bhep::bhep_svc::instance().get_writer_txt().close();
      bhep::reader_txt& rt = bhep::bhep_svc::instance().get_reader_txt();
      rt.open(dst_fname_);
      size_t max_events = rt.max_events();
      for(size_t ievent=0; ievent<max_events; ievent++) {
	num_events++;
	bhep::event& new_event = rt.read_next();
	G4cout << G4endl << G4endl << "***************************************************************";
	G4cout << G4endl << "Reading event number = " << num_events << endl;
	G4cout << "***************************************************************" << G4endl << G4endl;
	G4cout << new_event << G4endl;
      }
    }
    // for 'gz' dst
    else if (dst_ext_ == "gz") {
      bhep::bhep_svc::instance().get_writer_gz().close();
      bhep::reader_gz& rz = bhep::bhep_svc::instance().get_reader_gz();
      rz.open(dst_fname_);
      size_t max_events = rz.max_events();
      for(size_t ievent=0; ievent<max_events; ievent++) {
	num_events++;
	bhep::event& new_event = rz.read_next();
	G4cout << G4endl << G4endl << "***************************************************************";
	G4cout << G4endl << "Reading event number = " << num_events << endl;
	G4cout << "***************************************************************" << G4endl << G4endl;
	G4cout << new_event << G4endl;
      }
    }
    
  }
}
