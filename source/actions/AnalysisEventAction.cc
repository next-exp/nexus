// ----------------------------------------------------------------------------
// nexus | AnalysisEventAction.cc
//
// This class is based on DefaultEventAction and modified to produce
// a histogram of the number of scintillation photons event by event.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "AnalysisEventAction.h"
#include "Trajectory.h"
#include "PersistencyManager.h"
#include "IonizationHit.h"

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>
#include <G4GenericMessenger.hh>
#include <G4HCofThisEvent.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>
#include <globals.hh>
#include <G4OpticalPhoton.hh>

#include "TH1F.h"
#include "TFile.h"


namespace nexus {


  AnalysisEventAction::AnalysisEventAction(): G4UserEventAction(),
                                              nevt_(0), nupdate_(10),
                                              energy_threshold_(0.),
                                              energy_max_(DBL_MAX),
                                              file_name_("OpticalEvent"),
                                              file_no_(0)
  {
    msg_ = new G4GenericMessenger(this, "/Actions/AnalysisEventAction/");

    G4GenericMessenger::Command& thresh_cmd =
       msg_->DeclareProperty("energy_threshold", energy_threshold_,
                             "Minimum deposited energy to save the event to file.");
    thresh_cmd.SetParameterName("energy_threshold", true);
    thresh_cmd.SetUnitCategory("Energy");
    thresh_cmd.SetRange("energy_threshold>0.");

    G4GenericMessenger::Command& max_energy_cmd =
      msg_->DeclareProperty("max_energy", energy_max_,
                            "Maximum deposited energy to save the event to file.");
    max_energy_cmd.SetParameterName("max_energy", true);
    max_energy_cmd.SetUnitCategory("Energy");
    max_energy_cmd.SetRange("max_energy>0.");

    msg_->DeclareProperty("file_name", file_name_, "");
    msg_->DeclareProperty("file_number", file_no_, "");

    hNPhotons = new TH1F("NPhotons", "NPhotons", 5000, 0, 70000.);
    hNPhotons->GetXaxis()->SetTitle("Number of optical photons");
  }



  AnalysisEventAction::~AnalysisEventAction()
  {
    std::ostringstream file_number;
    file_number << file_no_;
    G4String filename = file_name_+"."+file_number.str()+".root";
    Histo = new TFile(filename, "recreate");
    hNPhotons->Write();
    Histo->Close();
  }



  void AnalysisEventAction::BeginOfEventAction(const G4Event* /*event*/)
  {
    // Print out event number info
    if ((nevt_ % nupdate_) == 0) {
      G4cout << " >> Event no. " << nevt_  << G4endl;
      if (nevt_  == (10 * nupdate_)) nupdate_ *= 10;
    }
  }



  void AnalysisEventAction::EndOfEventAction(const G4Event* event)
  {
    nevt_++;

    // Determine whether total energy deposit in ionization sensitive
    // detectors is above threshold
    if (energy_threshold_ >= 0.) {

      // Get the trajectories stored for this event and loop through them
      // to calculate the total energy deposit

      G4double edep = 0.;
      G4int n_opt_photons = 0;

      G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
      if (tc) {
        for (unsigned int i=0; i<tc->size(); ++i) {
          Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
          edep += trj->GetEnergyDeposit();
          // Draw tracks in visual mode
          if (G4VVisManager::GetConcreteInstance()) trj->DrawTrajectory();

          if (trj->GetParticleDefinition() == G4OpticalPhoton::Definition()) {
            n_opt_photons += 1;
          }
        }
      }

      PersistencyManager* pm = dynamic_cast<PersistencyManager*>
        (G4VPersistencyManager::GetPersistencyManager());

      // if (edep > _energy_threshold) pm->StoreCurrentEvent(true);
      // else pm->StoreCurrentEvent(false);
      if (!event->IsAborted() && edep>0) {
	pm->InteractingEvent(true);
      } else {
	pm->InteractingEvent(false);
      }
      if (!event->IsAborted() && edep > energy_threshold_ && edep < energy_max_) {
	pm->StoreCurrentEvent(true);
        hNPhotons->Fill(n_opt_photons);
      } else {
	pm->StoreCurrentEvent(false);
      }

    }
  }


} // end namespace nexus
