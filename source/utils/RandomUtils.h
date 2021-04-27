// ----------------------------------------------------------------------------
// nexus | RandomUtils.h
//
// This class defines commonly used functions to generate random quantities.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include <G4ThreeVector.hh>


#ifndef RAND_U_H
#define RAND_U_H

namespace nexus {

  G4double RandomEnergy(G4double energy_max,G4double energy_min);

  G4ThreeVector Direction(G4double costheta_min, G4double costheta_max,
                          G4double phi_min, G4double phi_max);

}

#endif
