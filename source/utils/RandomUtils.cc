// ----------------------------------------------------------------------------
// nexus | RandomUtils.cc
//
// Commonly used functions to generate random quantities.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------
#include "RandomUtils.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace nexus {

  G4double UniformRandomInRange(G4double max_value, G4double min_value)
  {
    if (max_value == min_value){
      return min_value;
    } else {
      return (G4UniformRand()*(max_value - min_value) + min_value);
    }
  }


  G4ThreeVector RandomDirectionInRange(G4double costheta_min, G4double costheta_max,
                                       G4double phi_min, G4double phi_max)
  {
    G4double cosTheta  = UniformRandomInRange(costheta_min, costheta_max);
    G4double sinTheta2 = 1. - cosTheta*cosTheta;
    if (sinTheta2 < 0.)  sinTheta2 = 0.;
    G4double sinTheta  = std::sqrt(sinTheta2);

    G4double phi = UniformRandomInRange(phi_min, phi_max);

    return G4ThreeVector (sinTheta*std::cos(phi),
                          sinTheta*std::sin(phi),
                          cosTheta).unit();
  }

  G4int GetRandBinIndex(G4RandGeneral *fRandomGeneral, std::vector<G4double> intensity){

    return round(fRandomGeneral->fire()*intensity.size());

  }

  G4double Sample(G4double sample, G4bool smear, G4double smearval){

    // Apply Gaussian smearing to smooth from bin-to-bin
    if (smear){
      return sample + G4RandGauss::shoot( 0., smearval);
    }
    // Return the sampled histogram without smearing the entries
    else
      return sample;

  }

  G4bool CheckOutOfBound(G4double min, G4double max, G4double val){

    // Out of bounds
    if (val < min || val > max)
      return true;
    // In suitable bounds -- pass
    else
      return false;

  }

  G4bool CheckOutOfBoundMin(G4double min, G4double val){

    // Out of bounds
    if (val < min)
      return true;
    // In suitable bounds -- pass
    else
      return false;

  }

  G4bool CheckOutOfBoundMax(G4double max, G4double val){

    // Out of bounds
    if (val > max)
      return true;
    // In suitable bounds -- pass
    else
      return false;

  }


}
