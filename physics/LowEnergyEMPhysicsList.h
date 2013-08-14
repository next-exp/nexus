// ----------------------------------------------------------------------------
///  \file   LowEnergyEMPhysicsList.h
///  \brief  Physics list for low-energy electromagnetic processes.
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     2 December 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __LOW_ENERGY_EM_PHYSICS_LIST__
#define __LOW_ENERGY_EM_PHYSICS_LIST__

#include <G4VModularPhysicsList.hh>


namespace nexus {

  class LowEnergyEMPhysicsList: public G4VModularPhysicsList
  {
  public:
    /// constructor
    LowEnergyEMPhysicsList();
    /// destructor
    ~LowEnergyEMPhysicsList() {}
    /// Set cuts for secondary particle production
    void SetCuts();
  };
  
} // end namespace nexus

#endif
