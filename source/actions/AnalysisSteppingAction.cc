// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>    
//  Created: 24 August 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "AnalysisSteppingAction.h"

#include <G4Step.hh>
#include <G4SteppingManager.hh>
#include <G4ProcessManager.hh>
#include <G4OpticalPhoton.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4VPhysicalVolume.hh>

using namespace nexus;



AnalysisSteppingAction::AnalysisSteppingAction(): G4UserSteppingAction()
{
}



AnalysisSteppingAction::~AnalysisSteppingAction()
{
  G4double total_counts = 0;
  detectorCounts::iterator it = my_counts.begin();
  while (it != my_counts.end()) {
    G4cout << "Detector " << it->first << ": " << it->second << " counts" << G4endl;
    total_counts += it->second;
    it ++;
  }
  G4cout << "TOTAL COUNTS: " << total_counts << G4endl;
}



void AnalysisSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4ParticleDefinition* pdef = step->GetTrack()->GetDefinition();
 
  //Check whether the track is an optical photon
  if (pdef != G4OpticalPhoton::Definition()) return;
  
  /*
  // example of information one can access about optical photons
  
  G4Track* track = step->GetTrack();
  G4int pid = track->GetParentID();
  G4int tid = track->GetTrackID();
  G4StepPoint* point1 = step->GetPreStepPoint();
  G4StepPoint* point2 = step->GetPostStepPoint();
  G4TouchableHandle touch1 = point1->GetTouchableHandle();
  G4TouchableHandle touch2 = point2->GetTouchableHandle();
  G4String vol1name = touch1->GetVolume()->GetName();
  G4String vol2name = touch2->GetVolume()->GetName();

  G4String proc_name = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  G4int copy_no = step->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);
  */

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
    if (boundary->GetStatus() == Detection ){
      G4String detector_name = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName();
      //G4cout << "##### Sensitive Volume: " << detector_name << G4endl;

      detectorCounts::iterator it = my_counts.find(detector_name);
      if (it != my_counts.end()) my_counts[it->first] += 1;
      else my_counts[detector_name] = 1;
    }
  }

  return;
}
