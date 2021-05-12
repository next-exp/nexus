// ----------------------------------------------------------------------------
// nexus | RandomUtils.cc
//
// Commonly used functions to generate random quantities.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------
#include "RandomUtils.h"

#include <Randomize.hh>

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


}
