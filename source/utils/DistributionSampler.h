// ----------------------------------------------------------------------------
// nexus | DistributionSampler.h
//
// This class provides the utilities to randomly sample from an input
// distribution
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef DISTRIBUTION_SAMPLER
#define DISTRIBUTION_SAMPLER

#include <Randomize.hh>
#include <G4Types.hh>

namespace nexus {

  /// Generator of random points in a sphere
  class DistributionSampler
  {
  public:
    /// Constructor
    DistributionSampler();
    
    /// Destructor
    ~DistributionSampler();

    /// Get the random bin index of histogram distribution
    G4int GetRandBinIndex(G4RandGeneral *fRandomGeneral, std::vector<G4double> value);

    /// Get the value of the random sample
    G4double Sample(G4double sample, bool smear, G4double smearval);

    /// Check if the sampled value is out of bounds
    G4bool CheckOutOfBound(G4double min, G4double max, G4double val);

  private:


  };

} // namespace nexus

#endif
