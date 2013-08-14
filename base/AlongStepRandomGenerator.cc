// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : J Martin-Albo <jmalbos@ific.uv.es>    
//  Created: 29 Oct 2009
//
//  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "AlongStepRandomGenerator.h"

#include <Randomize.hh>


namespace nexus {

  
  AlongStepRandomGenerator::AlongStepRandomGenerator(): _t0(0), _t1(0)
  {
  }



  AlongStepRandomGenerator::AlongStepRandomGenerator
  (const G4LorentzVector& pre, const G4LorentzVector& post):
    _t0(pre.t()), _v0(pre.v()), _t1(post.t()), _v1(post.v()) 
  {
  }
  


  AlongStepRandomGenerator::~AlongStepRandomGenerator()
  {
  }

  
  
  void AlongStepRandomGenerator::SetPoints
  (const G4LorentzVector& pre, const G4LorentzVector& post)
  {
    _t0 = pre.t();
    _v0 = pre.v();
    _t1 = post.t();
    _v1 = post.v();
  }
  


  G4LorentzVector AlongStepRandomGenerator::Shoot()
  {
    G4double rnd = G4UniformRand();
    G4double time = _t0 + rnd * (_t1 - _t0);
    G4ThreeVector position = _v0 + rnd * (_v1 - _v0);
    G4LorentzVector point(position, time);
    return point;
  }
  
  
} // end namespace nexus
