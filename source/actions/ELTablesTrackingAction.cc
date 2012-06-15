// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>
//  Created: 21 June 2010
//
//  Copyright (c) 2010, 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "ELTablesTrackingAction.h"

#include "IonizationElectron.h"
#include "DriftTrackInfo.h"

#include <G4TrackingManager.hh>


namespace nexus {

  
  void ELTablesTrackingAction::PreUserTrackingAction(const G4Track* track)
  {
    if (track->GetDefinition() == IonizationElectron::Definition())
      fpTrackingManager->SetUserTrackInformation(new DriftTrackInfo());
  }
  
  
  
  void ELTablesTrackingAction::PostUserTrackingAction(const G4Track* track)
  {
  }


} // end namespace nexus
