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

#ifndef SEGMENT_POINT_SAMPLER
#define SEGMENT_POINT_SAMPLER

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
    G4LorentzVector pre_, post_;
  };
  
  // INLINE METHODS ////////////////////////////////////////////////////////////
  
  inline SegmentPointSampler::SegmentPointSampler() {}
  
  inline SegmentPointSampler::SegmentPointSampler
  (const G4LorentzVector& pre, const G4LorentzVector& post):
    pre_(pre), post_(post) {}
  
  inline SegmentPointSampler::SegmentPointSampler
  (const G4ThreeVector& pre, const G4ThreeVector& post)
  { pre_.setVect(pre); post_.setVect(post); }

  inline SegmentPointSampler::~SegmentPointSampler() {}

  inline void SegmentPointSampler::SetPoints
  (const G4LorentzVector& pre, const G4LorentzVector& post)
  { pre_ = pre; post_ = post; }

  inline const G4LorentzVector& SegmentPointSampler::GetPrePoint() const
  { return pre_; }

  inline const G4LorentzVector& SegmentPointSampler::GetPostPoint() const
  { return post_; }

  inline G4LorentzVector SegmentPointSampler::Shoot() const
  { G4double rnd = G4UniformRand(); 
    G4double time = pre_.t() + rnd * (post_.t() - pre_.t());
    G4ThreeVector position = pre_.v() + rnd * (post_.v() - pre_.v());
    return G4LorentzVector(position,time); }

} // end namespace nexus

#endif
