// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 24 August 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "DefaultSteppingAction.h"

#include <G4Step.hh>
#include <G4SteppingManager.hh>
#include <G4OpticalPhoton.hh>
#include <G4OpBoundaryProcess.hh>

using namespace nexus;



DefaultSteppingAction::DefaultSteppingAction(): G4UserSteppingAction()
{
}



DefaultSteppingAction::~DefaultSteppingAction()
{
}



void DefaultSteppingAction::UserSteppingAction(const G4Step* /*step*/)
{
  /*
  G4ParticleDefinition* pdef = step->GetTrack()->GetDefinition();
  //G4Track* track = step->GetTrack();
  //  Check whether the track is an optical photon   
  if (pdef != G4OpticalPhoton::Definition()) return;

  G4StepPoint* point2 = step->GetPostStepPoint();
  G4TouchableHandle touch2 = point2->GetTouchableHandle();
  G4String vol2name = touch2->GetVolume()->GetName();

  // Retrieve the pointer to the optical boundary process.
  // We do this only once per run defining our local pointer as static.
  static G4OpBoundaryProcess* boundary = 0;
  
  if (!boundary) { // the pointer is not defined yet
    // Get the list of processes defined for the optical photon
    // and loop through it to find the optical boundary process.
    G4ProcessVector* pv = pdef->GetProcessManager()->GetProcessList();
    for (G4int i=0; i<pv->size(); i++) {
      if ((*pv)[i]->GetProcessName() == "OpBoundary") {
	boundary = (G4OpBoundaryProcess*) (*pv)[i];
	break;
      }
    }
  }

  if (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
    G4cout << "Boundary status: " << boundary->GetStatus() 
	   << " in volume "<< vol2name << G4endl;
    // if (vol2name == "PHOTODIODES") {
    if (boundary->GetStatus() == Detection) {
      G4cout << "Detected" << G4endl;
    // 	  detected = detected + 1;	    
    // 	} else {
    // 	not_det = not_det + 1;
    }
    // }

  }
*/  

  return;
}
