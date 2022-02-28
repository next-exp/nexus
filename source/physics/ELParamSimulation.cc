// ----------------------------------------------------------------------------
// nexus | ELParamSimulation.cc
//
// This class (WIP) implements a parametrized simulation .
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ELParamSimulation.h"

#include "ELLookupTable.h"
#include "IonizationElectron.h"

#include <G4RunManager.hh>


namespace nexus {


  ELParamSimulation::ELParamSimulation(G4Region* region):
    G4VFastSimulationModel("ELParamSimulation", region),
    table_(0)
  {
    // check whether the region has an EL look-up table attached
    table_ = dynamic_cast<ELLookupTable*>(region->GetUserInformation());
    // if (!table_) {
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


    //table_->GetSensorsMap()

    //if (HCE_.size() == 0) {

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

    // Fill SensorHits according to this map

  }


} // end namespace nexus
