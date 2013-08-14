//
//  GenbbInterface.cc
//
//     Author : J Martin-Albo <jmalbos@ific.uv.es>    
//     Created: 27 Mar 2009
//     Updated:    Mar 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#include "GenbbInterface.h"
#include "InputManager.h"
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4PrimaryVertex.hh>
#include <G4Event.hh>
#include <bhep/gstore.h>


namespace nexus {


  GenbbInterface::GenbbInterface():
    _input(InputManager::Instance().GetGenerationInput())
  {
    Initialize();
  }
  
  
  
  void GenbbInterface::Initialize()
  {
  }
  
  
  
  void GenbbInterface::GeneratePrimaryVertex(G4Event* evt)
  {
  }
  

} // end namespace nexus


