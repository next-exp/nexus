// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : F. Monrabal <francesc.monrabal@ific.uv.es>
//           J. Martin-Albo <jmalbos@ific.uv.es>
//  Created: 6 Apr 2009
//
//  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "OpticalPhysicsList.h"

#include "OpticalPhysics.h"
#include "EMStandardPhysics.h"
#include "LowEnergyEMPhysics.h"

#include "ConfigService.h"

#include <G4UnitsTable.hh>


namespace nexus {

  
  OpticalPhysicsList::OpticalPhysicsList(): G4VModularPhysicsList()
  {
    RegisterPhysics(new OpticalPhysics);
  }


  
  void OpticalPhysicsList::SetCuts()
  {
    // set cut values for gamma at first and for e- second and next for e+,
    // because some processes for e+/e- need cut values for gamma
    SetCutValue(defaultCutValue, "gamma");
    SetCutValue(defaultCutValue, "e-");
    SetCutValue(defaultCutValue, "e+");
    
    if (verboseLevel>0) DumpCutValuesTable();
  }


} // end namespace nexus
