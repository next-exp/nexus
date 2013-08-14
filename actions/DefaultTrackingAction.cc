//
//  DefaultTrackingAction.cc
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>
//     Created: 27 Apr 2009
//     Updated: 
//
//  Copyright (c) 2009 -- IFIC Neutrino Group 
//

#include "DefaultTrackingAction.h"

#include <globals.hh>
#include <G4Track.hh>

#include <bhep/event.h>
#include <bhep/bhep_svc.h>


namespace nexus {
  
  void DefaultTrackingAction::PreUserTrackingAction(const G4Track* track)
  {
    G4int track_id         = track->GetTrackID();
    G4int parent_id        = track->GetParentID();
    G4String particle_name = track->GetDefinition()->GetParticleName();
    G4String volume        = track->GetVolume()->GetName();

    bhep::event& bevt = bhep::bhep_svc::instance().get_event();

    bhep::particle* particle = new bhep::particle(bhep::TRUTH , particle_name);
   
    if (parent_id == 0) {
      particle->set_primary(true);
      //particle->add_property("CreatorProcess", "none");
    }
    else {
      particle->set_primary(false);
      //bhep::particle& mother_part = get_particle(mother_id);
      //part->set_mother(mother_part);
      //mother_part.add_daughter(*part);
      //part->add_property("CreatorProcess", aTrack->GetCreatorProcess()->GetProcessName());
    }

    particle->add_property("G4ID", bhep::to_string( track_id ));
    particle->add_property("IniVol", volume); 
    particle->add_property("OutActive",   "0"); 
    particle->add_property("EnergyDep",   "0.0");
  
    // Adding the particle to bhep::event
    bevt.add_true_particle(particle);
  }
  
  
  
  void DefaultTrackingAction::PostUserTrackingAction(const G4Track* track)
  {

  }



  bhep::particle& DefaultTrackingAction::GetBParticle(G4int G4ID)
  {
    bhep::event& evt = bhep::bhep_svc::instance().get_event();
    vector<bhep::particle*> bparticles;

    if (evt.filter(bhep::TRUTH, "G4ID", bhep::to_string(G4ID), bparticles)) {
      return *bparticles[0];
    }
    else {
      G4cerr << "ERROR.- Non-existing particle in transient bhep event!" << G4endl;
      exit(1);
    }
  }


} // end namespace nexus
