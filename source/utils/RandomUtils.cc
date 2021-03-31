// ----------------------------------------------------------------------------
// nexus | RandomUtils.cc
//
// This class defines commonly used functions to generate random quantities.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------
#include "RandomUtils.h"

#include <Randomize.hh>

#include "CLHEP/Units/SystemOfUnits.h"

namespace nexus {

  G4double RandomEnergy(G4double energy_max,G4double energy_min)
  {
    if (energy_max == energy_min){
      return energy_min;
    } else {
      return (G4UniformRand()*(energy_max - energy_min) + energy_min);
    }
  }


  G4ThreeVector Direction(G4double costheta_min, G4double costheta_max,
                          G4double phi_min, G4double phi_max)
  {
    G4double cosTheta  = (costheta_max - costheta_min) * G4UniformRand() + costheta_min;
    G4double sinTheta2 = 1. - cosTheta*cosTheta;
    if (sinTheta2 < 0.)  sinTheta2 = 0.;
    G4double sinTheta  = std::sqrt(sinTheta2);

    G4double phi = (phi_max - phi_min) * G4UniformRand() + phi_min;

    return G4ThreeVector (sinTheta*std::cos(phi),
                          sinTheta*std::sin(phi),
                          cosTheta).unit();
  }


}
