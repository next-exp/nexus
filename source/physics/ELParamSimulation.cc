// -----------------------------------------------------------------------------
//  $Id$
//
//  Authors: <justo.martin-albo@ific.uv.es>, <paola.ferrario@ific.uv.es>
//  Created: 18 Oct 2011
//  
//  Copyright (c) 2011, 2012 NEXT Collaboration. All rights reserved.
// -----------------------------------------------------------------------------

#include "ELParamSimulation.h"

#include "ELLookupTable.h"
#include "IonizationElectron.h"

#include <G4RunManager.hh>

#include <TRandom2.h>
#include <TVector3.h>



namespace nexus {


  ELParamSimulation::ELParamSimulation(G4Region* region):
    G4VFastSimulationModel("ELParamSimulation", region),
    _table(0)
  {
    // check whether the region has an EL look-up table attached
    _table = dynamic_cast<ELLookupTable*>(region->GetUserInformation());
    // if (!_table) {
    //   G4String msg = "ERROR: no EL lookup table attached to the region!";
    //   G4Exception("[ELParamSimulation]", "ELParamSimulation()",
    // 		  FatalException, msg);
    // }

  }



  ELParamSimulation::~ELParamSimulation()
  {
  }



  G4bool ELParamSimulation::IsApplicable(const G4ParticleDefinition& pdef)
  {
    return (&pdef == IonizationElectron::Definition());
  }



  G4bool ELParamSimulation::ModelTrigger(const G4FastTrack& /*ftrack*/)
  {
    return true;
  }



  void ELParamSimulation::DoIt(const G4FastTrack& ftrack, G4FastStep& /*fstep*/)
  {
    // Get spatial coordinates
    G4ThreeVector position = ftrack.GetPrimaryTrack()->GetPosition();
    
    G4cout << "position = " << position << G4endl;


    //_table->GetSensorsMap()

    //if (_HCE.size() == 0) {

      G4HCofThisEvent* hce = 
	G4RunManager::GetRunManager()->GetCurrentEvent()->GetHCofThisEvent();
      
      //}

    
    G4cout << "No of collections: " << hce->GetNumberOfCollections() << G4endl;


    G4cout << "here" << G4endl;


    ///FIXME. Parameters to be put in some other place
    // G4int fNum_tbins = 5;
    // G4double fBinWidth = 200.;
    // G4int gain = 1170;

    // Get track coordinates
    // ...
    
    // Ask table for the right sensor map
    // ...

    // Fill PmtHits according to this map

  }
  
  
} // end namespace nexus
