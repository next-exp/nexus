// ----------------------------------------------------------------------------
// nexus | RandomUtils.h
//
// Commonly used functions to generate random quantities.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include <G4ThreeVector.hh>

#include <Randomize.hh>


#ifndef RAND_U_H
#define RAND_U_H

namespace nexus {

    G4double UniformRandomInRange(G4double max_value, G4double min_value);

    G4ThreeVector RandomDirectionInRange(G4double costheta_min, G4double costheta_max,
                                       G4double phi_min, G4double phi_max);

    /// Get the random bin index of histogram distribution
    G4int GetRandBinIndex(G4RandGeneral *fRandomGeneral, std::vector<G4double> value);

    /// Get the value of the random sample
    G4double Sample(G4double sample, G4bool smear, G4double smearval);

    /// Check if the sampled value is out of bounds
    G4bool CheckOutOfBound(G4double min, G4double max, G4double val);
    
    /// Check if the sampled value is out of bounds, min check only
    G4bool CheckOutOfBoundMin(G4double min, G4double val);
    
    /// Check if the sampled value is out of bounds, max check only
    G4bool CheckOutOfBoundMax(G4double max, G4double val);

  enum vtx_region {VOLUME, INSIDE, INNER_SURF, OUTER_SURF, CENTER};


}

#endif
