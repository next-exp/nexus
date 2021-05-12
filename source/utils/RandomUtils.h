// ----------------------------------------------------------------------------
// nexus | RandomUtils.h
//
// Commonly used functions to generate random quantities.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include <G4ThreeVector.hh>


#ifndef RAND_U_H
#define RAND_U_H

namespace nexus {

  G4double UniformRandomInRange(G4double max_value, G4double min_value);

  G4ThreeVector RandomDirectionInRange(G4double costheta_min, G4double costheta_max,
                                       G4double phi_min, G4double phi_max);

}

#endif
