// ----------------------------------------------------------------------------
///  \file   NexusPhysicsList.h
///  \brief  
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     6 Apr 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXUS_PHYSICS_LIST__
#define __NEXUS_PHYSICS_LIST__

#include <G4VModularPhysicsList.hh>


namespace nexus {

  class ParamStore;


  /// FIXME. Enum description.

  enum EMPhysics { STANDARD, LIVERMORE };


  /// FIXME. Class description.

  class NexusPhysicsList: public G4VModularPhysicsList  
  {
  public:
    /// Constructor
    NexusPhysicsList();
    /// Destructor
    ~NexusPhysicsList();
    /// Set cuts for secondar particle production
    void SetCuts();
  };

} // end namespace nexus

#endif
