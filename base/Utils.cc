// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>
//  Created:  7 May 2009
//
//  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "Utils.h"

#include "ConfigService.h"
#include "ParamStore.h"

#include <bhep/bhep_svc.h>
#include <bhep/particle.h>


namespace nexus {
  
  
  void Utils::OpenOutputDst(const G4String& filename)
  {
    if (!bhep::bhep_svc::instance()->get_writer_root().isOpen()) {
      bhep::bhep_svc::instance()->get_writer_root().open(filename, "RECREATE");

      // set dst info
      time_t time1; time(&time1);
      bhep::bhep_svc::instance()->get_dst_info().set_time(time1);
      bhep::bhep_svc::instance()->get_dst_info().set_type(bhep::MC);
      bhep::bhep_svc::instance()->get_dst_info().set_label("NEXUS_DST");

      // set run info
      bhep::bhep_svc::instance()->get_run_info().set_time(time1);
      bhep::bhep_svc::instance()->get_run_info().set_type(bhep::MC);
      bhep::bhep_svc::instance()->get_run_info().set_label("NEXUS");
    }
  }
  
  
  
  void Utils::CloseOutputDst()
  {
    ConfigService::Instance().Geometry().Save();
    ConfigService::Instance().Generation().Save();
    ConfigService::Instance().Physics().Save();
    ConfigService::Instance().Actions().Save();
    ConfigService::Instance().Job().Save();

    bhep::dst& ndst = bhep::bhep_svc::instance()->get_dst_info();
    bhep::run& nrun = bhep::bhep_svc::instance()->get_run_info();

    // add final parameters to dst info
    size_t nevt = bhep::bhep_svc::instance()->get_writer_root().max_events();
    ndst.set_Nevt(nevt);

    nrun.add_property("STATS_written_events", double(nevt));
    
    // save dst info
    bhep::bhep_svc::instance()->get_writer_root().write_dst_info(ndst);
    bhep::bhep_svc::instance()->get_dst_info().clear();

    // save run info
    bhep::bhep_svc::instance()->get_writer_root().write_run_info(nrun);
    bhep::bhep_svc::instance()->get_run_info().clear();

    // close file
    bhep::bhep_svc::instance()->get_writer_root().close();
  }
  
  

  bhep::particle& Utils::GetBParticle(G4int G4TrackID)
  {
    // retrieve bhep transient event
    bhep::event& bevt = bhep::bhep_svc::instance()->get_event();

    // get list of true particles in event
    vector<bhep::particle*> bparts = bevt.true_particles();
    vector<bhep::particle*>::iterator bp = bparts.begin();

    while (bp != bparts.end()) {
      G4int track_id = (*bp)->fetch_iproperty("G4TrackID");
      if (track_id == G4TrackID) {
	return **bp;
      }
      ++bp;
    }
    
    G4Exception("ERROR.- Non-existing particle in transient bhep event!");
  }










  G4ThreeVector Utils::DVto3Vec(const bhep::vdouble& v)
  {
    if (v.size() != 3)
      G4Exception("Size bigger than 3");
    
    G4double x = v[0];
    G4double y = v[1];
    G4double z = v[2];
    
    return G4ThreeVector(x,y,z);
  }




} // end namespace nexus
