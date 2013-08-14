/* ====================================================================
 *                NEXT MonteCarlo Simulation (Geant4)
 * ====================================================================
 * 
 * Copyright (C) 2007 J. Muñoz, J. Martín-Albo, J.J. Gómez-Cadenas
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */


#include <next/geometry.hh>            // Geometry definition
#include <next/physics.hh>             // Physical proceses to consider
#include <next/generator.hh>           // Initial perticles generator
#include <next/run.hh>                 // User actions per run basis
#include <next/event.hh>               // User actions per event basis
#include <next/tracking.hh>            // User actions per track basis
//#include <next/stacking.hh>          // User actions related with tracks stacking
#include <next/stepping.hh>            // User actions per step basis
#include <next/stepping_verbose.hh>    // Information dump per step basis

#include "G4RunManager.hh"  // Program manager
#include "G4UImanager.hh"   // User Interface manager
#include "G4UIterminal.hh"  // User Interface manager through the terminal

#include <bhep/bhep_svc.h>
#include <bhep/sreader.h>

#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"      // For terminal tcsh use
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif



int main(int argc,char** argv) {

  G4String mode_;

  if(argc==1) {
    // Automatic mode
    mode_ = "auto";
  }
  else if (argc==2) {
    G4String option_ = argv[1];
    if ((option_ == "vis") || (option_ == "visual")) {
      // Visual mode
      mode_ = "vis";
    }
    else {
      G4cout << "ERROR: " << option_ << " is not a valid running option!!" << G4endl;
      exit(1);
    }
  }
  else {
    G4cout << "usage: ./xnext <option>" << endl;
    exit(1);
  }
  
  
  G4cout << G4endl << "***** Reading parameters from next.par file ..." 
	 << G4endl << G4endl;

  // create a generic store
  bhep::gstore store;

  // and the reader for GLOBAL variables
  bhep::sreader reader(store);
  reader.file("next.par");
  reader.info_level(bhep::NORMAL);
  reader.group("GLOBAL");
  reader.read();

  G4cout << "**********************************************************" << G4endl;

  G4String sim_verbosity_ = store.fetch_sstore("simulation_verbosity");
  G4cout << "***** Simulation verbosity set to: " << sim_verbosity_ << G4endl;

  G4String G4Tracking_verbosity_ = store.fetch_sstore("G4Tracking_verbosity");
  G4cout << "***** G4 Tracking verbosity set to: " << G4Tracking_verbosity_ << G4endl;

  G4String odst_fname_ = store.fetch_sstore("odst_fname");
  G4cout << "***** Output DST file name:" << odst_fname_ << G4endl;

  G4String idata_type_ = store.fetch_sstore("idata_type");
  G4cout << "***** Input data origin:" << idata_type_ << G4endl;
  
  G4String idst_fname_ = store.fetch_sstore("idst_fname");

  G4double min_E_  = store.fetch_dstore("min_E");
  G4double max_E_  = store.fetch_dstore("max_E");
  G4String part_name_ = store.fetch_sstore("part_name");
  
  G4int num_events_ = store.fetch_istore("num_events");
  
  if (idata_type_ == "shoot") {
    G4cout << "***** Generator data will follow a flat random distribution in: " << G4endl;
    G4cout << "*****           Ekin  (Mev): (" << min_E_ << " , " << max_E_ << ")" << G4endl;
    G4cout << "*****           Theta (deg): (0 , 90)" <<  G4endl;
    G4cout << "*****           Phi   (deg): (0 , 180)" <<  G4endl;
    G4cout << "*****           Vertex: bulk" << G4endl;
    G4cout << "***** Initial events: " << num_events_ << " " << part_name_ << G4endl;
  }
  else if (idata_type_ == "idst") {
    G4cout << "***** Generator data source read from:" << idst_fname_ << G4endl;
    G4cout << "***** Initial events: " << num_events_ << G4endl;
  }
  else {
    G4cout << "***** Unknown idst_type option!!" << G4endl;
    exit(1);
  }
  
  G4String geom_fname_ = store.fetch_sstore("geom_fname");
  G4cout << "***** Reading geometry parameters from: " << geom_fname_ << G4endl;

  G4cout << "**********************************************************" << G4endl << G4endl;

  

  // The bhep service is generated. This server must be instantiated
  // wherever you want to access all the info.
  bhep::bhep_svc bsvc;  


  // my Verbose output class
  G4VSteppingVerbose* verbosity = new stepping_verbose;
  G4VSteppingVerbose::SetInstance(verbosity);


  // Run manager
  G4RunManager * runManager = new G4RunManager;

  
  // UserInitialization classes (mandatory) 
  // geometry
  geometry* geom = new geometry(geom_fname_);
  geom->set_info_level(sim_verbosity_);
  runManager->SetUserInitialization(geom);

  // physics
  physics* phys = new physics;
  phys->set_info_level(sim_verbosity_);
  runManager->SetUserInitialization(phys);
  
#ifdef G4VIS_USE
  // Visualization, if you choose to have it!
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif


  // UserAction classes
  // generator
  generator* my_gen;
  
  if (idata_type_ == "shoot")
    my_gen = new generator( min_E_, max_E_, part_name_, geom_fname_);
  else 
    my_gen = new generator( idst_fname_);
  
  my_gen->set_info_level(sim_verbosity_);
  runManager->SetUserAction(my_gen);
  
  // run
  run* my_run = new run(odst_fname_);
  runManager->SetUserAction(my_run);  

  //event
  event* my_evt = new event(part_name_, odst_fname_);
  my_evt->set_info_level(sim_verbosity_);
  runManager->SetUserAction(my_evt);

  // track
  tracking* my_tracking = new tracking();
  my_tracking->set_info_level(sim_verbosity_);
  runManager->SetUserAction(my_tracking);

  // step
  stepping* my_stepping = new stepping(part_name_);
  my_stepping->set_info_level(sim_verbosity_);
  runManager->SetUserAction(my_stepping);

  // stack
  // stacking* my_stacking = new stacking(part_name_);
  // my_stacking->set_info_level(sim_verbosity_);
  // runManager->SetUserAction(my_stacking);

  
  //Initialize G4 kernel
  runManager->Initialize();

  //get the pointer to the User Interface manager
  //in order for the user to issue commands to the program 
  G4UImanager * UI = G4UImanager::GetUIpointer();  



  // Automatic mode
  if (mode_ == "auto") {
    
#ifdef G4VIS_USE
    visManager->SetVerboseLevel("quiet");
#endif
    
    G4String command = "/control/shell cp currentEvent.rndm currentRun.rndm";
    UI->ApplyCommand(command);
    command = "/random/setSavingFlag true";
    UI->ApplyCommand(command);
    command = "/random/resetEngineFrom currentRun.rndm";
    UI->ApplyCommand(command);
    
    command = "/tracking/verbose ";
    UI->ApplyCommand(command + G4Tracking_verbosity_);
    
    command = "/run/beamOn ";
    UI->ApplyCommand(command+bhep::to_string(num_events_));
  }
  // Visual mode
  else if (mode_ == "vis") {  
          
    // G4UIterminal is a (dumb) terminal.
    G4UIsession *session = 0;
    
#ifdef G4UI_USE_TCSH
    session = new G4UIterminal(new G4UItcsh);      
#else
    session = new G4UIterminal();
#endif     
    
    UI->ApplyCommand("/control/execute vis.mac");    
    session->SessionStart();
    delete session;
  }
  
  
#ifdef G4VIS_USE
  delete visManager;
#endif
  
  delete runManager;
  delete verbosity;
  
  return 0;
}

// End of main.cc
