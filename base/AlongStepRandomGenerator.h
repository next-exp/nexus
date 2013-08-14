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

#include <G4ThreeVector.hh>
#include <G4LorentzVector.hh>


namespace nexus {

  /// Random generator of 4D (space and time) points along a segment.

  class AlongStepRandomGenerator
  {
  public:
    /// Default constructor. Sets all coordinates to 0.
    AlongStepRandomGenerator();

    /// Constructor passing pre and post points of the segment
    AlongStepRandomGenerator(const G4LorentzVector&,
			     const G4LorentzVector&);

    /// Destructor
    ~AlongStepRandomGenerator();

    /// Set pre and post points of the segment
    void SetPoints(const G4LorentzVector&, const G4LorentzVector&);

    /// Generates a random point along the segment
    G4LorentzVector Shoot();
    
  private:
    G4ThreeVector _v0, _v1; ///< space coordinates
    G4double _t0, _t1;      ///< time coordinates
  };
  
} // end namespace nexus

#endif
