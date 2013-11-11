// ----------------------------------------------------------------------------
///  \file   SegmentPointSampler.h
///  \brief  Sampler of random points (3D or 4D) along a segment.
///
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     29 Oct 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2012 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#ifndef __SEGMENT_POINT_SAMPLER__
#define __SEGMENT_POINT_SAMPLER__

#include <G4LorentzVector.hh>
#include <G4ThreeVector.hh>
#include <Randomize.hh>


namespace nexus {

  /// Sampler of random points with uniform probability along a segment.
  /// Points can be 3D (only spatial dimensions) or 4D (space-time).

  class SegmentPointSampler
  {
  public:
    /// Constructor providing pre and post points of the segment
    SegmentPointSampler(const G4ThreeVector& pre,
			const G4ThreeVector& post);

    /// Constructor providing pre and post 4D (space, time) points of 
    /// the segment
    SegmentPointSampler(const G4LorentzVector& pre,
			const G4LorentzVector& post);
    
    /// Default constructor. Sets all coordinates to 0.
    SegmentPointSampler();
    
    /// Destructor
    ~SegmentPointSampler();

    /// Sets pre and post points of the segment
    void SetPoints(const G4ThreeVector&, const G4ThreeVector&);
    
    /// Sets pre and post 4D points of the segment
    void SetPoints(const G4LorentzVector&, const G4LorentzVector&);
    
    /// Returns the pre 4D-point
    const G4LorentzVector& GetPrePoint() const;
    /// Returns the post 4D-point
    const G4LorentzVector& GetPostPoint() const;

    /// Generates a random 4D point along the segment
    G4LorentzVector Shoot() const;
    
  private:
    G4LorentzVector _pre, _post;
  };
  
  // INLINE METHODS ////////////////////////////////////////////////////////////
  
  inline SegmentPointSampler::SegmentPointSampler() {}
  
  inline SegmentPointSampler::SegmentPointSampler
  (const G4LorentzVector& pre, const G4LorentzVector& post):
    _pre(pre), _post(post) {}
  
  inline SegmentPointSampler::SegmentPointSampler
  (const G4ThreeVector& pre, const G4ThreeVector& post)
  { _pre.setVect(pre); _post.setVect(post); }

  inline SegmentPointSampler::~SegmentPointSampler() {}

  inline void SegmentPointSampler::SetPoints
  (const G4LorentzVector& pre, const G4LorentzVector& post)
  { _pre = pre; _post = post; }

  inline const G4LorentzVector& SegmentPointSampler::GetPrePoint() const
  { return _pre; }

  inline const G4LorentzVector& SegmentPointSampler::GetPostPoint() const
  { return _post; }

  inline G4LorentzVector SegmentPointSampler::Shoot() const
  { G4double rnd = G4UniformRand(); 
    G4double time = _pre.t() + rnd * (_post.t() - _pre.t());
    G4ThreeVector position = _pre.v() + rnd * (_post.v() - _pre.v());
    return G4LorentzVector(position,time); }

} // end namespace nexus

#endif
