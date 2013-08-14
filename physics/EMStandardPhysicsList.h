// ----------------------------------------------------------------------------
///  \file   EMStandardPhysicsList.h
///  \brief  Physics list with the standard electromagnetic processes.
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     6 Apr 2009
///  \version  $Id$
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __EM_STANDARD_PHYSICS_LIST__
#define __EM_STANDARD_PHYSICS_LIST__

#include <G4VModularPhysicsList.hh>


namespace nexus {

  class EMStandardPhysicsList: public G4VModularPhysicsList  
  {
  public:
    /// Constructor
    EMStandardPhysicsList();
    /// Destructor
    ~EMStandardPhysicsList();
    /// Set cuts for secondar particle production
    void SetCuts();
  };

} // end namespace nexus

#endif
