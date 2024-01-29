// ----------------------------------------------------------------------------
// nexus | MuonsEventAction.cc
//
// This class is based on DefaultEventAction and modified to produce
// control histograms for muon generation.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "MuonsEventAction.h"
#include "Trajectory.h"
#include "PersistencyManager.h"
#include "IonizationHit.h"
#include "FactoryBase.h"

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>
#include <G4GenericMessenger.hh>
#include <globals.hh>


#include "CLHEP/Units/SystemOfUnits.h"
#include "AddUserInfoToPV.h"

using namespace CLHEP;
namespace nexus {

REGISTER_CLASS(MuonsEventAction, G4UserEventAction)

  MuonsEventAction::MuonsEventAction():
    G4UserEventAction(), nevt_(0), nupdate_(10), energy_threshold_(0.), stringHist_(""), fOpen_(false)
  {
    msg_ = new G4GenericMessenger(this, "/Actions/MuonsEventAction/");
    msg_->DeclareProperty("stringHist", stringHist_, "");

    G4GenericMessenger::Command& thresh_cmd =
       msg_->DeclareProperty("energy_threshold", energy_threshold_,
                             "Minimum deposited energy to save the event to file.");
    thresh_cmd.SetParameterName("energy_threshold", true);
    thresh_cmd.SetUnitCategory("Energy");
    thresh_cmd.SetRange("energy_threshold>0.");

    // Get analysis manager
    fG4AnalysisMan_ = G4AnalysisManager::Instance();
    
    // Create histogram(s) for muons
    fG4AnalysisMan_->CreateH1("Edepo", "Energy_deposited", 100, -1.0, 3.4);
    fG4AnalysisMan_->CreateH1("Zenith", "Zenith generated", 100, 0., pi);
    fG4AnalysisMan_->CreateH1("Azimuth", "Azimuth generated", 100, 0., twopi);
    
    // Create Ntuple branches for muons
    fG4AnalysisMan_->CreateNtuple("Tree nexus", "Flat tree of muon zenith and azimuth");
    fG4AnalysisMan_->CreateNtupleDColumn("tree_zenith");
    fG4AnalysisMan_->CreateNtupleDColumn("tree_azimuth");
    fG4AnalysisMan_->FinishNtuple();

  }



  MuonsEventAction::~MuonsEventAction()
  {
    // Write histogram to root file
    fG4AnalysisMan_->Write();
    fG4AnalysisMan_->CloseFile();

  }

  void MuonsEventAction::BeginOfEventAction(const G4Event* /*event*/)
  {
    // Print out event number info
    if ((nevt_ % nupdate_) == 0) {
      G4cout << " >> Event no. " << nevt_  << G4endl;
      if (nevt_  == (10 * nupdate_)) nupdate_ *= 10;
    }

    // Get analysis manager instance
    fG4AnalysisMan_ = G4AnalysisManager::Instance();

    // Open output file
    if (!fOpen_) {
      fG4AnalysisMan_->OpenFile(stringHist_);
      fOpen_ = true;
    }

  }


  void MuonsEventAction::EndOfEventAction(const G4Event* event)
  {
    nevt_++;

    // Determine whether total energy deposit in ionization sensitive
    // detectors is above threshold
    if (energy_threshold_ >= 0.) {

      // Get the trajectories stored for this event and loop through them
      // to calculate the total energy deposit

      G4double edep = 0.;

      G4TrajectoryContainer* tc = event->GetTrajectoryContainer();
      if (tc) {
        for (unsigned int i=0; i<tc->size(); ++i) {
          Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
          edep += trj->GetEnergyDeposit();
          // Draw tracks in visual mode
          if (G4VVisManager::GetConcreteInstance()) trj->DrawTrajectory();
        }
      }
      // Control plot for energy
      fG4AnalysisMan_->FillH1(0, edep);

      PersistencyManager* pm = dynamic_cast<PersistencyManager*>
        (G4VPersistencyManager::GetPersistencyManager());

      if (edep > energy_threshold_) pm->StoreCurrentEvent(true);
      else pm->StoreCurrentEvent(false);

    }

    // Retrieving muon generation information
    G4PrimaryVertex* my_vertex = event->GetPrimaryVertex();
    G4VUserPrimaryVertexInformation* getinfo2 = my_vertex->GetUserInformation();
    AddUserInfoToPV* my_getinfo2 = dynamic_cast<AddUserInfoToPV*>(getinfo2);

    G4double my_zenith = my_getinfo2->GetZenith();
    G4double my_azimuth = my_getinfo2->GetAzimuth();

    fG4AnalysisMan_->FillH1(1, my_zenith);
    fG4AnalysisMan_->FillH1(2, my_azimuth);

    fG4AnalysisMan_->FillNtupleDColumn(0, my_zenith);
    fG4AnalysisMan_->FillNtupleDColumn(1, my_azimuth);
    fG4AnalysisMan_->AddNtupleRow();

  }


} // end namespace nexus
