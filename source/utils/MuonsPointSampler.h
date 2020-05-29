// ----------------------------------------------------------------------------
// nexus | MuonsPointSampler.h
//
// This class is a sampler of random uniform points in a rectangular surface,
// with fixed y coordinate.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef MUONS_POINT_SAMPLER
#define MUONS_POINT_SAMPLER

#include <G4ThreeVector.hh>
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
    G4double x_, yPoint_,z_;

  };

  // inline methods ..................................................

  inline MuonsPointSampler::~MuonsPointSampler() { }

} // namespace nexus

#endif
