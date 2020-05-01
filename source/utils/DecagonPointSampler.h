// ----------------------------------------------------------------------------
///  \file   DecagonPointSampler.h  /// Decagon for polygon N=10  // Ruty //
///  \file   HexagonPointSampler.h
///  \brief  Generator of vertices in a cylindric geometry.
///
///  \author  Justo Martín-Albo <jmalbos@ific.uv.es> 
///           Francesc Monrabal <francesc.monrabal@ific.uv.es>
///  \date    7 May 2010
///  \version $Id$
//
///  Copyright (c) 2010 NEXT Collaboration
//
//  Updated based on "HexagonPointSampler.h"  ==>>>  "DecagonPointSampler.h"
//  for the new DEMO++  ACTIVE  having 10 walls polygon instead of the former 6 walls.
//  Draft by: Ruth Weiss Babai <ruty.wb@gmail.com>
//  Date:     10 Apr 2020
//  ************   Need to be chacked   ************
// ----------------------------------------------------------------------------

#ifndef __DECAGON_POINT_SAMPLER__
#define __DECAGON_POINT_SAMPLER__
//#ifndef __HEXAGON_POINT_SAMPLER__
//#define __HEXAGON_POINT_SAMPLER__

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>

#include <vector>

namespace nexus {

  enum DecagonRegion { INSIDE10, PLANE10 };

  
  /// FIXME.

  //class HexagonPointSampler
  class DecagonPointSampler
  {
  public:
    
    /// Constructor
    //HexagonPointSampler(G4double apothem, G4double length, G4double thickness,
    DecagonPointSampler(G4double apothem, G4double length, G4double thickness,
			G4ThreeVector origin=G4ThreeVector(0.,0.,0.),
			G4RotationMatrix* rotation=0);
    
    /// Destructor
    //~HexagonPointSampler();
    ~DecagonPointSampler();

    /// Returns vertex within a given region of the chamber
    //G4ThreeVector GenerateVertex(HexagonRegion);
    G4ThreeVector GenerateVertex(DecagonRegion);

    /// Calculates the position of Decagonal (hexagonal) cells of a given pitch
    /// and stores them in a vector (notice that the vector will be 
    /// cleared before filling it)
    void TesselateWithFixedPitch(G4double pitch, 
				 std::vector<G4ThreeVector>& vpos);
    
    
  private:
    /// Calculates the position of cells in the honeycomb (?) and stores
    /// them in a vector
    void PlaceCells(std::vector<G4ThreeVector>&, G4int, G4double);

    /// Fills a vector with the positions of all points needed
    /// to create a look-up table
    void TriangleWalker(G4double, G4double, G4double);

    G4ThreeVector RandomPointInTriangle();

    G4double RandomRadius(G4double inner, G4double outer);
    G4double RandomPhi();
    G4double RandomLength(G4double origin, G4double max_length);
    G4ThreeVector RotateAndTranslate(const G4ThreeVector&);

  private:
    /// Default constructor is hidden
    //HexagonPointSampler();
    DecagonPointSampler();

  private:

    G4double _length, _radius, _apothem; ///< internal dimensions
    G4double _thickness; 
    
    G4ThreeVector _origin;
    G4RotationMatrix* _rotation;

    G4double _binning;
    std::vector<G4ThreeVector> _table_vertices;

    G4int _number_events;
  };

  // inline methods ..................................................

  //inline HexagonPointSampler::~HexagonPointSampler() {}
  inline DecagonPointSampler::~DecagonPointSampler() {}
  
} // namespace nexus

#endif
