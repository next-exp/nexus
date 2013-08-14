// NEXUS: stepping_verbose.cc
//    -- Last modification: 5th June 2007, jmalbos
//

#include <nexus/stepping_verbose.h>

#include "G4SteppingManager.hh"
#include "G4UnitsTable.hh"


namespace nexus {

  void stepping_verbose::StepInfo()
  {
    CopyState();
  
    G4int prec = G4cout.precision(3);

    if( verboseLevel >= 1 ) {
      
      if( verboseLevel >= 4 ) 
	VerboseTrack();
      
      if( verboseLevel >= 3 ) {
	G4cout << G4endl;    
	G4cout << std::setw( 5) << "In stepping verbose:: StepInfo " << G4endl
	       << std::setw( 6) << "Step num"
	       << std::setw( 6) << "X"          << "    "
	       << std::setw( 6) << "Y"          << "    "
	       << std::setw( 6) << "Z"          << "    "
	       << std::setw( 9) << "KineE"      << " "
	       << std::setw( 9) << "dEStep"     << " "  
	       << std::setw(10) << "StepLeng"     
	       << std::setw(10) << "TrakLeng"  << "  "
	       << std::setw(10) << "Volume"    << "  "
	       << std::setw(10) << "Material"
	       << std::setw(10) << "Process"   << G4endl;	          
      }

      G4cout << std::setw(5) << fTrack->GetCurrentStepNumber() << " "
	     << std::setw(6) << G4BestUnit(fTrack->GetPosition().x(),"Length")
	     << std::setw(6) << G4BestUnit(fTrack->GetPosition().y(),"Length")
	     << std::setw(6) << G4BestUnit(fTrack->GetPosition().z(),"Length")
	     << std::setw(6) << G4BestUnit(fTrack->GetKineticEnergy(),"Energy")
	     << std::setw(6) << G4BestUnit(fStep->GetTotalEnergyDeposit(),"Energy")
	     << std::setw(6) << G4BestUnit(fStep->GetStepLength(),"Length")
	     << std::setw(6) << G4BestUnit(fTrack->GetTrackLength(),"Length")
	     << "  ";

      if( fTrack->GetNextVolume() != 0 )
	G4cout << std::setw(10) << fTrack->GetVolume()->GetName();
      else
	G4cout << std::setw(10) << "OutOfWorld";
      
      G4cout << std::setw(10) << fTrack->GetMaterial()->GetName();

      if(fStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL) {
	G4cout << "  " 
	       << std::setw(10) 
	       << fStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
      } 
      else {
	G4cout << "   UserLimit";
      }
      
      G4cout << G4endl;
      
      if( verboseLevel == 2 ) {
	G4int tN2ndariesTot = fN2ndariesAtRestDoIt +
	  fN2ndariesAlongStepDoIt +
	  fN2ndariesPostStepDoIt;
	if(tN2ndariesTot>0) {
	  G4cout << G4endl 
		 << "    :----- List of 2ndaries - "
		 << "#SpawnInStep=" << std::setw(3) << tN2ndariesTot << "  "
		 << "(Rest="  << std::setw(2) << fN2ndariesAtRestDoIt
		 << ",Along=" << std::setw(2) << fN2ndariesAlongStepDoIt
		 << ",Post="  << std::setw(2) << fN2ndariesPostStepDoIt
		 << "), "
		 << "#SpawnTotal=" << std::setw(3) << (*fSecondary).size()
		 << " ---------------"
		 << G4endl;

	  for(size_t lp1=(*fSecondary).size()-tN2ndariesTot; 
	      lp1<(*fSecondary).size(); lp1++){
	    G4cout << "    : "
		   << std::setw(6)
		   << G4BestUnit((*fSecondary)[lp1]->GetPosition().x(),"Length")
		   << std::setw(6)
		   << G4BestUnit((*fSecondary)[lp1]->GetPosition().y(),"Length")
		   << std::setw(6)
		   << G4BestUnit((*fSecondary)[lp1]->GetPosition().z(),"Length")
		   << std::setw(6)
		   << G4BestUnit((*fSecondary)[lp1]->GetKineticEnergy(),"Energy")
		   << std::setw(10)
		   << (*fSecondary)[lp1]->GetDefinition()->GetParticleName()
		   << G4endl;
	  }
              
	  G4cout << "    :-----------------------------"
		 << "----------------------------------"
		 << "-- EndOf2ndaries Info ---------------"
		 << G4endl;
	}
      }
    
    }
    G4cout.precision(prec);
  }


  void stepping_verbose::TrackingStarted()
  {
    CopyState();

    G4int prec = G4cout.precision(3);
    if( verboseLevel > 0 ){

      G4cout << std::setw( 5) << "In Stepping Verbose: Tracking Started:" << G4endl << " Step "
	     << std::setw( 6) << "X"          << "    "
	     << std::setw( 6) << "Y"          << "    "  
	     << std::setw( 6) << "Z"          << "    "
	     << std::setw( 9) << "KineE"      << " "
	     << std::setw( 9) << "dEStep"     << " "  
	     << std::setw(10) << "StepLeng"  
	     << std::setw(10) << "TrakLeng"   << "  "
	     << std::setw(10) << "Volume"     << "  "
	     << std::setw(10) << "Material"
	     << std::setw(10) << "Process"    << G4endl;	     

      G4cout << std::setw(5) << fTrack->GetCurrentStepNumber() << " "
	     << std::setw(6) << G4BestUnit(fTrack->GetPosition().x(),"Length")
	     << std::setw(6) << G4BestUnit(fTrack->GetPosition().y(),"Length")
	     << std::setw(6) << G4BestUnit(fTrack->GetPosition().z(),"Length")
	     << std::setw(6) << G4BestUnit(fTrack->GetKineticEnergy(),"Energy")
	     << std::setw(6) << G4BestUnit(fStep->GetTotalEnergyDeposit(),"Energy")
	     << std::setw(6) << G4BestUnit(fStep->GetStepLength(),"Length")
	     << std::setw(6) << G4BestUnit(fTrack->GetTrackLength(),"Length")
	     << "  ";

      if(fTrack->GetNextVolume()){
	G4cout << std::setw(10) << fTrack->GetVolume()->GetName();
      } else {
	G4cout << std::setw(10) << "OutOfWorld";
      }
      G4cout << std::setw(10) << "  XXXXXX";
      G4cout << "    initStep" << G4endl;
    }
    G4cout.precision(prec);
  }

} // namespace nexus
