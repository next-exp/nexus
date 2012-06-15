// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 15 Apr 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ---------------------------------------------------------------------------- 

#include "DefaultRunAction.h"

#include "ConfigService.h"
#include "BhepUtils.h"

#include <G4Run.hh>

#include <bhep/bhep_svc.h>


namespace nexus {
  
  
  void DefaultRunAction::BeginOfRunAction(const G4Run* run)
  {
    G4cout << "\n### Starting Run " << run->GetRunID() << ".  "
	   << "#######################################\n" << G4endl;
    
    // open output dst
    G4String odst = ConfigService::Instance().Job().GetSParam("output_dst");
    BhepUtils::OpenOutputDst(odst);
  }
  
  
  
  void DefaultRunAction::EndOfRunAction(const G4Run* run)
  {
    // save number of events processed in the run
    bhep::bhep_svc::instance()->get_run_info().
      add_property("STATS_run_events", run->GetNumberOfEvent());
    
    // close output dst
    BhepUtils::CloseOutputDst();
    
    G4cout << "\n### Run finished.  "
	   << "#########################################\n" << G4endl;
  }


} // end namespace nexus
