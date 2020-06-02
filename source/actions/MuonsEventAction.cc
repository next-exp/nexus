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

#include <G4Event.hh>
#include <G4VVisManager.hh>
#include <G4Trajectory.hh>
#include <G4GenericMessenger.hh>
#include <G4HCofThisEvent.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>
#include <globals.hh>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

#include "CLHEP/Units/SystemOfUnits.h"
#include "AddUserInfoToPV.h"

using namespace CLHEP;
namespace nexus {


  MuonsEventAction::MuonsEventAction(): 
    G4UserEventAction(), nevt_(0), nupdate_(10), energy_threshold_(0.), stringHist_("")
  {
    msg_ = new G4GenericMessenger(this, "/Actions/MuonsEventAction/");
    msg_->DeclareProperty("stringHist", stringHist_, "");
    
    G4GenericMessenger::Command& thresh_cmd =
       msg_->DeclareProperty("energy_threshold", energy_threshold_,
                             "Minimum deposited energy to save the event to file.");
    thresh_cmd.SetParameterName("energy_threshold", true);
    thresh_cmd.SetUnitCategory("Energy");
    thresh_cmd.SetRange("energy_threshold>0.");

    // Muons Control plots
    hist1_ = new TH1D ("Edepo","Energy_deposited",100,-1.0,3.4);
    hist2_ = new TH1D ("Theta","Theta generated",100,-pi,pi);
    hist3_ = new TH1D ("Phi","Phi generated",100,0.,twopi);
    tree_  = new TTree("Tree nexus","Flat tree with some nexus info");  
    tree_->Branch("tree_theta", &tree_theta_, "tree_theta/D");
    tree_->Branch("tree_phi", &tree_phi_, "tree_phi/D");
  }
  
  
  
  MuonsEventAction::~MuonsEventAction()
  {
    //added for muons
    const char * suf = stringHist_.c_str();
    TFile *file = new TFile(suf,"RECREATE","Muons");
    hist1_->Write();
    hist2_->Write();
    hist3_->Write();
    tree_->Write();
    file->Close();

  }
    
  void MuonsEventAction::BeginOfEventAction(const G4Event* /*event*/)
  {
   // Print out event number info
    if ((nevt_ % nupdate_) == 0) {
      G4cout << " >> Event no. " << nevt_  << G4endl;
      if (nevt_  == (10 * nupdate_)) nupdate_ *= 10;
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
      //control plot for energy
      hist1_->Fill(edep);
 
      PersistencyManager* pm = dynamic_cast<PersistencyManager*>
        (G4VPersistencyManager::GetPersistencyManager());
 
      if (edep > energy_threshold_) pm->StoreCurrentEvent(true);
      else pm->StoreCurrentEvent(false);

    }

    //Retrieving muons generation information
    G4PrimaryVertex* my_vertex = event->GetPrimaryVertex();
    G4VUserPrimaryVertexInformation *getinfo2 = my_vertex->GetUserInformation();
    AddUserInfoToPV *my_getinfo2 = dynamic_cast<AddUserInfoToPV*>(getinfo2);
    
    Double_t my_theta = my_getinfo2->GetTheta();
    Double_t my_phi = my_getinfo2->GetPhi();
    //    std::cout<<"get the info back in MuonsEventAction: "<<my_getinfo2->GetTheta()<<std::endl;
    hist2_->Fill(my_theta);
    hist3_->Fill(my_phi);
    tree_theta_ = my_theta;
    tree_phi_ = my_phi;
    tree_->Fill();


  }


} // end namespace nexus
