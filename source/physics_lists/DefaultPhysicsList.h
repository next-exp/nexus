// ----------------------------------------------------------------------------
///  \file   DefaultPhysicsList.h
///  \brief  Default physics list for NEXUS.
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     30 June 2011
///  \version  $Id$
///
///  Copyright (c) 2011 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __DEFAULT_PHYSICS_LIST__
#define __DEFAULT_PHYSICS_LIST__

#include <G4VModularPhysicsList.hh>


namespace nexus {

  /// Default physics list for NEXUS
  
  class DefaultPhysicsList: public G4VModularPhysicsList  
  {
  public:
    /// Constructor
    DefaultPhysicsList();
    /// Destructor
    ~DefaultPhysicsList();
    /// Set cuts for secondary particle production
    void SetCuts();
  };

} // end namespace nexus

#endif
