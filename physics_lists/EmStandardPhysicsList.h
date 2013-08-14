//
//  EmStandardPhysicsList.h
//
//     Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 6 Apr 2009
//     Updated: 
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#ifndef __EM_STANDARD_PHYSICS_LIST__
#define __EM_STANDARD_PHYSICS_LIST__

#include <G4VUserPhysicsList.hh>


namespace nexus {

  class EmStandardPhysicsList: public G4VUserPhysicsList
  {
  public:
    EmStandardPhysicsList();
    ~EmStandardPhysicsList() {}

  protected:
    /// Construct all required particles (Geant4 mandatory method)
    void ConstructParticle();
    /// Construct all required physics processed (Geant4 mandatory method)
    void ConstructProcess();

    void SetCuts();

  protected:
    void ConstructBosons();
    void ConstructLeptons();
    void ConstructMesons();
    void ConstructBaryons();

    void ConstructDecay();
    void ConstructEM();
  };

} // end namespace nexus

#endif
