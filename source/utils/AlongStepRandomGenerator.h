// ----------------------------------------------------------------------------
///  \file   AlongStepRandomGenerator.h
///  \brief  Generator of random points along a segment.
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     29 Oct 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __ALONG_STEP_RANDOM_GENERATOR__
#define __ALONG_STEP_RANDOM_GENERATOR__

#include <G4LorentzVector.hh>
#include <G4ThreeVector.hh>
#include <Randomize.hh>

namespace nexus {

  /// Random generator of 4D (space and time) points along a segment.

  class AlongStepRandomGenerator
  {
  public:
    /// Constructor passing pre and post points of the segment
    AlongStepRandomGenerator(const G4LorentzVector& pre,
			     const G4LorentzVector& post);
    
    /// Default constructor. Sets all coordinates to 0.
    AlongStepRandomGenerator();
    
    /// Destructor
    ~AlongStepRandomGenerator();
    
    /// Set pre and post points of the segment
    void SetPoints(const G4LorentzVector&, const G4LorentzVector&);
    
    const G4LorentzVector& GetPrePoint() const;
    const G4LorentzVector& GetPostPoint() const;

    /// Generates a random point along the segment
    G4LorentzVector Shoot();
    
  private:
    G4LorentzVector _pre, _post;
  };
  
  // inline methods ..................................................
  
  inline AlongStepRandomGenerator::AlongStepRandomGenerator() {}
  
  inline AlongStepRandomGenerator::AlongStepRandomGenerator
  (const G4LorentzVector& pre, const G4LorentzVector& post):
    _pre(pre), _post(post) {}
  
  inline AlongStepRandomGenerator::~AlongStepRandomGenerator() {}

  inline void AlongStepRandomGenerator::SetPoints
  (const G4LorentzVector& pre, const G4LorentzVector& post)
  { _pre = pre; _post = post; }

  inline const G4LorentzVector& AlongStepRandomGenerator::GetPrePoint() const
  { return _pre; }

  inline const G4LorentzVector& AlongStepRandomGenerator::GetPostPoint() const
  { return _post; }

  inline G4LorentzVector AlongStepRandomGenerator::Shoot()
  { G4double rnd = G4UniformRand(); 
    G4double time = _pre.t() + rnd * (_post.t() - _pre.t());
    G4ThreeVector position = _pre.v() + rnd * (_post.v() - _pre.v());
    return G4LorentzVector(position,time); }

} // end namespace nexus

#endif
