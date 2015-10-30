// ---------------------------------------------------------------------------- 
//  //
//  \brief Point Sampler on the surface of a rectangular used for muon generation.
//
//  Author: Neus Lopez March <neus.lopez@ific.uv.es>
//        
//  Created: 30 Jan 2015
//
//  Copyright (c) 2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#include "MuonsPointSampler.h"
#include <Randomize.hh>
#include <G4RunManager.hh>
#include "CLHEP/Units/PhysicalConstants.h"
#include <G4Box.hh>
#include <G4VSolid.hh>
#include <vector>


namespace nexus {

  using namespace CLHEP;
  
  MuonsPointSampler::MuonsPointSampler
  (G4double x, G4double yPoint, G4double z):
    _x(x),_yPoint(yPoint),_z(z)
  {
  }
  
  
  G4ThreeVector MuonsPointSampler::GenerateVertex()
  {
    
    // Get a random point in the surface of the muon plane
    G4ThreeVector point = GetXZPointInMuonsPlane();
    
    return point;
  }
  
  G4ThreeVector MuonsPointSampler::GetXZPointInMuonsPlane()
  {
    
    // y is fixed
    G4double x = -_x + G4UniformRand()*2*_x;
    G4double z = -_z + G4UniformRand()*2*_z;
    
    G4ThreeVector mypoint(x, _yPoint, z);
    
    // std::cout<<"Generating Muons in: "<<x<<" , "<<y<<" , "<<z<<std::endl;
    return mypoint;
  }
  
} // end namespace nexus
