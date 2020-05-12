// ----------------------------------------------------------------------------
//
//  Author : paola.ferrario@dipc.org
//  Created: 6 Apr 2020
//
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
                                              _nevt(0), _nupdate(10),
                                              _energy_threshold(0.),
                                              _energy_max(DBL_MAX),
                                              file_name_("OpticalEvent"),
                                              file_no_(0)
  {
    _msg = new G4GenericMessenger(this, "/Actions/AnalysisEventAction/");

    G4GenericMessenger::Command& thresh_cmd =
       _msg->DeclareProperty("energy_threshold", _energy_threshold,
                             "Minimum deposited energy to save the event to file.");
    thresh_cmd.SetParameterName("energy_threshold", true);
    thresh_cmd.SetUnitCategory("Energy");
    thresh_cmd.SetRange("energy_threshold>0.");

    G4GenericMessenger::Command& max_energy_cmd =
      _msg->DeclareProperty("max_energy", _energy_max,
                            "Maximum deposited energy to save the event to file.");
    max_energy_cmd.SetParameterName("max_energy", true);
    max_energy_cmd.SetUnitCategory("Energy");
    max_energy_cmd.SetRange("max_energy>0.");

    _msg->DeclareProperty("file_name", file_name_, "");
    _msg->DeclareProperty("file_number", file_no_, "");

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
    if ((_nevt % _nupdate) == 0) {
      G4cout << " >> Event no. " << _nevt  << G4endl;
      if (_nevt  == (10 * _nupdate)) _nupdate *= 10;
    }
  }



  void AnalysisEventAction::EndOfEventAction(const G4Event* event)
  {
    _nevt++;

    // Determine whether total energy deposit in ionization sensitive
    // detectors is above threshold
    if (_energy_threshold >= 0.) {

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
      if (!event->IsAborted() && edep > _energy_threshold && edep < _energy_max) {
	pm->StoreCurrentEvent(true);
        hNPhotons->Fill(n_opt_photons);
      } else {
	pm->StoreCurrentEvent(false);
      }

    }
  }


} // end namespace nexus
