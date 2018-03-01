//---------------------------------------------------------------------------
//   \file   MuonsEventAction.cc
///  \brief  Based in DefaultEventAction and modified to produce control histos for muons
///  
///  \author   Neus Lopez March <neus.lopez@ific.uv.es>    
///  \date     30 Jan 2014
/// 
///
///  Copyright (c) 2015 NEXT Collaboration
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

#include "CLHEP/Units/SystemOfUnits.h"
#include "AddUserInfoToPV.h"

using namespace CLHEP;
namespace nexus {


  MuonsEventAction::MuonsEventAction(): 
    G4UserEventAction(), _nevt(0), _nupdate(10), _energy_threshold(0.), _stringHist("")
  {
    _msg = new G4GenericMessenger(this, "/Actions/MuonsEventAction/");
    _msg->DeclareProperty("stringHist", _stringHist, "");
    G4GenericMessenger::Command& thresh_cmd =
       _msg->DeclareProperty("energy_threshold", _energy_threshold, "");
    thresh_cmd.SetUnitCategory("Energy");
    thresh_cmd.SetParameterName("energy_threshold", true);
    thresh_cmd.SetRange("energy_threshold>0.");

    //Muons Control plots

    hist = new TH1D ("Edepo","Energy_deposited",100,-1.0,3.4);
    hist2 = new TH1D ("Theta","Theta generated",100,-pi,pi);
    hist3 = new TH1D ("Phi","Phi generated",100,0.,twopi);
    tree = new TTree("Tree nexus","Flat tree with some nexus info");  
    tree->Branch("tree_theta", &tree_theta, "tree_theta/D");
    tree->Branch("tree_phi", &tree_phi, "tree_phi/D");
  }
  
  
  
  MuonsEventAction::~MuonsEventAction()
  {
    //added for muons
    const char * suf = _stringHist.c_str();
    TFile *file = new TFile(suf,"RECREATE","Muons");
    hist->Write();
    hist2->Write();
    hist3->Write();
    tree->Write();
    file->Close();

  }
    
  void MuonsEventAction::BeginOfEventAction(const G4Event* /*event*/)
  {
    // Print out event number info
    if ((_nevt % _nupdate) == 0) {
      G4cout << " >> Event no. " << _nevt << G4endl;
      if (_nevt == (10 * _nupdate)) _nupdate *= 10;
    }
  }


  void MuonsEventAction::EndOfEventAction(const G4Event* event)
  {
    _nevt++;

    // Determine whether total energy deposit in ionization sensitive 
    // detectors is above threshold
    if (_energy_threshold >= 0.) {

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
      hist->Fill(edep);
 
      PersistencyManager* pm = dynamic_cast<PersistencyManager*>
        (G4VPersistencyManager::GetPersistencyManager());
 
      if (edep > _energy_threshold) pm->StoreCurrentEvent(true);
      else pm->StoreCurrentEvent(false);

    }

    //Retrieving muons generation information
    G4PrimaryVertex* my_vertex = event->GetPrimaryVertex();
    G4VUserPrimaryVertexInformation *getinfo2 = my_vertex->GetUserInformation();
    AddUserInfoToPV *my_getinfo2 = dynamic_cast<AddUserInfoToPV*>(getinfo2);
    
    Double_t my_theta = my_getinfo2->GetTheta();
    Double_t my_phi = my_getinfo2->GetPhi();
    //    std::cout<<"get the info back in MuonsEventAction: "<<my_getinfo2->GetTheta()<<std::endl;
    hist2->Fill(my_theta);
    hist3->Fill(my_phi);
    tree_theta = my_theta;
    tree_phi = my_phi;
    tree->Fill();


  }


} // end namespace nexus
