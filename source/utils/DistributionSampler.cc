// ----------------------------------------------------------------------------
// nexus | DistributionSampler.cc
//
// This class provides the utilities to randomly sample from an input
// distribution
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "DistributionSampler.h"


namespace nexus {



  G4int DistributionSampler::GetRandBinIndex(G4RandGeneral *fRandomGeneral, std::vector<G4double> intensity){

    return round(fRandomGeneral->fire()*intensity.size());

  }

  G4double DistributionSampler::Sample(G4double sample, bool smear, G4double smearval){

    // Apply Gaussian smearing to smooth from bin-to-bin
    if (smear){
      return sample + G4RandGauss::shoot( 0., smearval);
    }
    // Return the sampled histogram without smearing the entries
    else
      return sample;

  }

  G4bool DistributionSampler::CheckOutOfBound(G4double min, G4double max, G4double val){

    // Out of bounds
    if (val < min || val > max)
      return true;
    // In suitable bounds -- pass
    else
      return false;

  }


} // end namespace nexus
