//
//  PrimaryGeneration.h
//
//     Author:  J Martin-Albo <jmalbos@ific.uv.es>    
//     Created:  9 Mar 2009
//     Updated: 27 Mar 2009
//
//  Copyright (c) 2009 -- NEXT Collaboration
// 

#include "PrimaryGeneration.h"
#include "InputManager.h"
#include "SingleParticle.h"
#include "GenbbInterface.h"
#include <G4VPrimaryGenerator.hh>
#include <G4Event.hh>
#include <bhep/gstore.h>


namespace nexus {
  
  
  PrimaryGeneration::PrimaryGeneration():_generator(0)
  {
  }
  
  
  
  PrimaryGeneration::~PrimaryGeneration()
  {
    delete _generator;
  }
  
  
  
  void PrimaryGeneration::SetGenerator(G4VPrimaryGenerator* g)
  {
    _generator = g;
  }
  
  
  
  void PrimaryGeneration::GeneratePrimaries(G4Event* evt)
  {
    _generator->GeneratePrimaryVertex(evt);
  }


} // namespace nexus
