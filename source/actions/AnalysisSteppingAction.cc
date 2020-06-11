// ----------------------------------------------------------------------------
// nexus | AnalysisSteppingAction.cc
//
// This class allows the user to print the total number of photons detected by
// all kinds of photosensors at the end of the run.
// It produces histograms with photon velocities and wavelengths.
// It also shows examples of information that can be accessed at the stepping
// level, so it is useful for debugging.
//
// The  NEXT Collaboration
// ----------------------------------------------------------------------------

#include "AnalysisSteppingAction.h"

#include <G4Step.hh>
#include <G4SteppingManager.hh>
#include <G4ProcessManager.hh>
#include <G4OpticalPhoton.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4Electron.hh>
#include <G4GenericMessenger.hh>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"

#include <iostream>
#include <sstream>

using namespace nexus;
using namespace CLHEP;


AnalysisSteppingAction::AnalysisSteppingAction(): G4UserSteppingAction(), file_no_(0)
{
  msg_ = new G4GenericMessenger(this, "/Actions/AnalysisSteppingAction/");
  msg_->DeclareProperty("file_number", file_no_, "");

  detected = 0;
  not_det = 0;

  times.clear();
  wavelengths.clear();


  hVelocity = new TH1F("PhVelocity", "Velocity of detected photons", 1000, 0, 0.4);
  hVelocity->GetXaxis()->SetTitle("velocity (ps)");
  hVE = new TH2F("PhLambdaV", "Wavelength vs velocity of detected photons", 1000., 0, 0.4, 1000, 0, 1500.);
  hVE->GetXaxis()->SetTitle("velocity (ps)");
  hVE->GetYaxis()->SetTitle("wavelength (nm)");

  hCherLambdaDet =
    new TH1F("CherLambdaDet", "Detection wavelength", 1000, 0, 1500.);
  hCherLambdaDet->GetXaxis()->SetTitle("wavelength (nm)");

  hTV = new TH2F("PhVelTime", "Velocity vs arrival time of detected photons", 1000, 0, 1000, 1000., 0, 0.4);
  hTV->GetYaxis()->SetTitle("velocity (mm/ps)");
  hTV->GetXaxis()->SetTitle("time (ps)");
}



AnalysisSteppingAction::~AnalysisSteppingAction()
{


  G4cout << "Detected photons = " << detected << G4endl;
  G4cout << "Non detected photons = " << not_det << G4endl;


   double first = 1000.*second ;

   for (unsigned int i=0; i< times.size(); ++i) {
     if (times[i] < first) {
       first = times[i];
     }
   }
   //G4cout << first/picosecond << G4endl;
   //std::vector<double> times_sub;

   for (unsigned int i=0; i< times.size(); ++i) {
     //    times_sub.push_back(times[i] - first);
     hTV->Fill((times[i] - first)/picosecond, wavelengths[i]/mm*picosecond);
   }

   std::ostringstream file_number;
   file_number << file_no_;
   G4String filename = "PhotonVelocitiesCherLXe."+file_number.str()+".root";

   TFile* histo_file = new TFile(filename,"recreate");
   hVelocity->Write();
   hVE->Write();
   hCherLambdaDet->Write();
   hTV->Write();
   histo_file->Close();

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

  //  Check whether the track is an optical photon
  if (pdef != G4OpticalPhoton::Definition()) return;


  G4StepPoint* point1 = step->GetPreStepPoint();
  G4StepPoint* point2 = step->GetPostStepPoint();
  G4TouchableHandle touch1 = point1->GetTouchableHandle();
  G4TouchableHandle touch2 = point2->GetTouchableHandle();
  G4String vol1name = touch1->GetVolume()->GetName();
  G4String vol2name = touch2->GetVolume()->GetName();
  //G4Track* track = step->GetTrack();

  //G4String proc_name = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  //G4int copy_no = step->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);

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

    // if boundary->GetStatus() == 2 in SiPMpet refraction takes place
    if (boundary->GetStatus() == Detection) {
	detected = detected + 1;
	double distance =
	  std::pow(point2->GetPosition().getX() - point1->GetPosition().getX(), 2) +
	  std::pow(point2->GetPosition().getY() - point1->GetPosition().getY(), 2)  + std::pow(point2->GetPosition().getZ() - point1->GetPosition().getZ(), 2) ;
	distance = std::sqrt(distance);
	G4double lambda = h_Planck*c_light/step->GetTrack()->GetKineticEnergy()/nanometer;
	hVelocity->Fill(distance/ step->GetDeltaTime()/mm*picosecond);
	hVE->Fill(distance/ step->GetDeltaTime()/mm*picosecond, lambda);
	hCherLambdaDet->Fill(h_Planck*c_light/step->GetTrack()->GetKineticEnergy()/nanometer);
	times.push_back(step->GetDeltaTime());
	wavelengths.push_back(distance/ step->GetDeltaTime());

	G4String detector_name = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetName();
	//G4cout << "##### Sensitive Volume: " << detector_name << G4endl;

	detectorCounts::iterator it = my_counts.find(detector_name);
	if (it != my_counts.end()) my_counts[it->first] += 1;
	else my_counts[detector_name] = 1;
      } else {
	not_det = not_det + 1;
      }

      //	G4cout << "check: " << velocity << ", " << track_velocity << G4endl;


  }

  return;
}
