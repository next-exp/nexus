// NEXT simulation: stepping.hh
//

#ifndef stepping_h
#define stepping_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4VTouchable.hh"

#include <bhep/bhep_svc.h>
#include <bhep/engine.h>
#include <bhep/point.h>
#include <bhep/event.h>
#include <bhep/particle.h>
#include <bhep/hit.h>
#include <bhep/utilities.h>
#include <bhep/sreader.h>

#include <fstream>
#include <sstream>
#include <vector>



class stepping : public G4UserSteppingAction, public bhep::engine
{

private:
  G4String ini_part_;

public:
  stepping(G4String ini_part);
  ~stepping();

public:
  // implement interface
  void UserSteppingAction(const G4Step*);
  
};

#endif
