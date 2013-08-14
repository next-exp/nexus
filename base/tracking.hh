// NEXT simulation: tracking.hh
//

#ifndef tracking_h
#define tracking_h 1

#include "globals.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "G4Step.hh"

#include <bhep/bhep_svc.h>
#include <bhep/event.h>
#include <bhep/particle.h>
#include <bhep/track.h>
#include <bhep/material.h>
#include <bhep/engine.h>


bhep::particle& get_bhep_part(G4int G4_id);


class tracking : public G4UserTrackingAction , public bhep::engine
{
private:
  G4String ini_part_;

public:
  tracking() {};
  ~tracking() {};
   
  virtual void PreUserTrackingAction(const G4Track*);
  virtual void PostUserTrackingAction(const G4Track*);

};
#endif
