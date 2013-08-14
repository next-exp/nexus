/*   
 *  NEXUS: main application
 *     - Created on: Feb 2007
 *     - Last modification: April 21 2008, JMALBOS
 * 
 *  ---
 *  Copyright (C) 2007 - J. Martin-Albo, J. Muñoz
 * 
 *  This file is part of NEXUS.
 *
 *  NEXUS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEXUS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEXUS; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *  ---
 */


#include <nexus/manager.h>
#include <nexus/geometry.h>        
#include <nexus/physics.h>           
#include <nexus/generator.h>         
#include <nexus/run.h>               
#include <nexus/event.h>
#include <nexus/event_trigger.h>
#include <nexus/tracking.h>          
#include <nexus/stepping.h>
#include <nexus/stepping_trigger.h>
#include <nexus/stepping_verbose.h>

#include <bhep/bhep_svc.h>

#include "G4RunManager.hh"  // Program manager
#include "G4UImanager.hh"   // User Interface manager
#include "G4UIterminal.hh"  // User Interface manager through the terminal

#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"      // For terminal tcsh use
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif


int main( int argc, char** argv ) 
{
  int error_code=EXIT_SUCCESS;

  try {
    
    std::string params_filename = "../scripts/nexus.par";
    bool        debug           = false;
    bool        visual_mode     = false;
          
    int iarg=1;
    while( iarg<argc ) {
    
      std::string arg=argv[iarg];
      
      if( arg[0] == '-' ) {
	
	if( arg == "-v" || arg == "-visual" || arg == "-vis" ) visual_mode = true;
	
	if( arg == "-d" || arg == "-debug" ) debug=true;
	
	if( arg == "-p" || arg == "-params" ) {
	  iarg++;
	  if (iarg==argc)
	    throw std::runtime_error("Missing simulation parameters filename!");
	  params_filename=argv[iarg];
	}
      }
      iarg++;
    }
    

    /// a nexus manager
    nexus::manager* nexusManager = new nexus::manager( params_filename );
    nexusManager->initialize();
    

    /// Geant4 verbose stepping
    G4VSteppingVerbose* Verbosity = new nexus::stepping_verbose();
    G4VSteppingVerbose::SetInstance( Verbosity );


    /// a Geant4 run manager
    G4RunManager * runManager = new G4RunManager;


    /// UserInitialization classes
    ///
    nexus::geometry* Geometry = new nexus::geometry( nexusManager->get_geom_store(),
						     nexusManager->get_physics_store(),
						     nexusManager->get_verbosity() );
    nexus::physics* Physics   = new nexus::physics(  nexusManager->get_physics_store(), 
						     nexusManager->get_verbosity() );
    runManager->SetUserInitialization( Geometry );
    runManager->SetUserInitialization( Physics );


    /// UserAction classes
    ///
    nexus::generator* Generator = new nexus::generator( nexusManager->get_generator_store(),
							nexusManager->get_geom_store(),
							nexusManager->get_event_mgr(),
							nexusManager->get_verbosity() );
    Generator->set_active_dimensions( Geometry->get_active_dimensions() );
    Generator->set_ivessel_dimensions( Geometry->get_ivessel_dimensions() );
    Generator->set_ovessel_dimensions( Geometry->get_ovessel_dimensions() );
    Generator->set_buffer_dimensions( Geometry->get_buffer_dimensions() );
    runManager->SetUserAction( Generator );

    nexus::run* Run = new nexus::run();
    runManager->SetUserAction( Run );  

    nexus::tracking* Tracking = new nexus::tracking();
    runManager->SetUserAction( Tracking );

    if( nexusManager->get_trigger()==1 ) {
      nexus::event_trigger* Event = new nexus::event_trigger( nexusManager->get_event_mgr(), 
							      nexusManager->get_verbosity() );
      runManager->SetUserAction( Event );
      
      nexus::stepping_trigger* Stepping = new nexus::stepping_trigger();
      runManager->SetUserAction( Stepping );  
    }
    else {
      nexus::event* Event       = new nexus::event( nexusManager->get_event_mgr(), 
						    nexusManager->get_verbosity() );
      runManager->SetUserAction( Event );

      nexus::stepping* Stepping = new nexus::stepping();
      runManager->SetUserAction( Stepping );  
    }
    
    
    /// Visualization
#ifdef G4VIS_USE
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
#endif


    /// Initialize G4 kernel
    runManager->Initialize();


    // Geant4 User Interface manager
    G4UImanager* UI = G4UImanager::GetUIpointer();  

    G4String G4Tracking_verbosity_ = nexusManager->get_tracking_verbosity();
    G4int num_events_ = nexusManager->get_num_events();

      
    ////////////////////////////////////////////////////////////

    if( visual_mode ) {  
    
      // G4UIterminal is a (dumb) terminal.
      G4UIsession *session = 0;
    
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);      
#else
      session = new G4UIterminal();
#endif     
    
      UI->ApplyCommand("/control/execute ../scripts/vis.mac");    
      session->SessionStart();
      delete session;
    }
  
    else {
    
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
  
    ////////////////////////////////////////////////////////////
  
#ifdef G4VIS_USE
    delete visManager;
#endif

    delete nexusManager;
    delete runManager;
    delete Verbosity;
  
  }
  catch( std::exception& x ) {
    std::cout << "\n[nexus] ERROR: " << x.what() << std::endl;
    error_code=EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "\n[nexus] ERROR: " << "unexpected error!" << std::endl; 
    error_code=EXIT_FAILURE;
  }
  
  return error_code;  

} // End of nexus.cc
