// ----------------------------------------------------------------------------
//  $Id$
//
//  Author:  J. Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 9 Mar 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PrimaryGeneration.h"

#include <G4VPrimaryGenerator.hh>
#include <G4Event.hh>


namespace nexus {
  
  
  void PrimaryGeneration::GeneratePrimaries(G4Event* event)
  {
    if (_generator == 0)
      G4Exception("", "", FatalException, "");
      //G4Exception("[DetectorConstruction] ERROR: Primary generator not set!");

    _generator->GeneratePrimaryVertex(event);
  }


} // namespace nexus
