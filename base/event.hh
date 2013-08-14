// NEXT simulation: event.hh
//
 
#ifndef event_h
#define event_h 1

#include "globals.hh"
#include "G4UserEventAction.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

#include <bhep/bhep_svc.h>
#include <bhep/engine.h>
#include <bhep/event.h>
#include <bhep/sreader.h>
#include <bhep/particle.h>

// #include <fstream>


class G4Event;

class event : public G4UserEventAction, public bhep::engine 
{
public:
  event(G4String ini_part_name, G4String dst_fname);
  ~event() {}
  
public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  //int clear_event(bhep::event& evt);

private:
  G4String dst_ext_;
  G4int store_all_;
  G4double ini_part_mass_;    

};
#endif

    
