// ----------------------------------------------------------------------------
///  \file   MuonsPointSampler.h
///  \brief  Generator of vertices in a cylindric geometry.
///
///  Author: Neus Lopez March <neus.lopez@ific.uv.es>
//        
//  Created: 30 Jan 2015
//
//  Copyright (c) 2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __MUONS_POINT_SAMPLER__
#define __MUONS_POINT_SAMPLER__

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <G4Box.hh>
#include <vector>

namespace nexus {

  class MuonsPointSampler
  {
  public:
    
    /// Constructor
    MuonsPointSampler(G4double x, G4double yPoint, G4double z);
     
    /// Destructor
    ~MuonsPointSampler();
    
    /// Returns vertex in the Muons plane

    G4ThreeVector GenerateVertex();
   
  private:
    /// Default constructor is hidden
    MuonsPointSampler();
    
  private:
    
    G4ThreeVector GetXZPointInMuonsPlane();
    G4double _x, _yPoint,_z;
   
  };

  // inline methods ..................................................

  inline MuonsPointSampler::~MuonsPointSampler() { }
  
} // namespace nexus

#endif
