// ----------------------------------------------------------------------------
///  \file   OpticalPhysicsList.h
///  \brief  Physics list for optical processes.
///
///  \author   <francesc.monrabal@ific.uv.es>
///            <justo.martin-albo@ific.uv.es>
///  \date     6 Apr 2009
///  \version  $Id$ 
///
///  Copyright (c) 2009 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __OPTICAL_PHYSICS_LIST__
#define __OPTICAL_PHYSICS_LIST__

#include <G4VModularPhysicsList.hh>


namespace nexus {

  // Physics list with all optical photon processes and standard EM

  class OpticalPhysicsList: public G4VModularPhysicsList
  {
  public:
    /// Constructor
    OpticalPhysicsList();
    /// Destructor
    ~OpticalPhysicsList() {}

    /// Set production cuts for secondary particles
    void SetCuts();
    void SetParameters();

  private:
    G4String _physics_list;
  };

} // end namespace nexus

#endif
