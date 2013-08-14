// ----------------------------------------------------------------------------
///  \file   LowEnergyEMPhysics.h
///  \brief  Low-energy electromagnetic physics processes.
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     28 Jan 2010
///  \version  $Id$
///
///  Copyright (c) 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __LOW_ENERGY_EM_PHYSICS__
#define __LOW_ENERGY_EM_PHYSICS__

#include <G4VPhysicsConstructor.hh>
#include <globals.hh>

namespace nexus {

  /// This class provides construction of the low-energy electromagnetic
  /// physics processes.

  class LowEnergyEMPhysics: public G4VPhysicsConstructor
  {
  public:
    /// Constructor
    LowEnergyEMPhysics(G4int verbosity=0, 
		       const G4String& name="LowEnergyEM");
    /// Destructor
    ~LowEnergyEMPhysics();

  private:
    /// Construct all required particles (Geant4 mandatory method)
    virtual void ConstructParticle();
    /// Construct all required physics processed (Geant4 mandatory method)
    void ConstructProcess();
    
  private:
    G4int verbose;
  };

} // end namespace nexus

#endif
