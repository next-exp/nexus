// ----------------------------------------------------------------------------
// nexus | RandomUtils.h
//
// Commonly used generic functions.
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

    /// Read in the 1d histogram stored in a csv file
    void LoadHistData1D(std::string filename, std::vector<G4double> &value,
                        std::vector<G4double> &x,
                        std::vector<G4double> &x_smear);
    
    /// Read in the 2d histogram stored in a csv file
    void LoadHistData2D(std::string filename, std::vector<G4double> &value,
                        std::vector<G4double> &x, std::vector<G4double> &y,
                        std::vector<G4double> &x_smear,
                        std::vector<G4double> &y_smear);
    
    /// Read in the 3d histogram stored in a csv file
    void LoadHistData3D(std::string filename, std::vector<G4double> &value,
                        std::vector<G4double> &x, std::vector<G4double> &y, std::vector<G4double> &z,
                        std::vector<G4double> &x_smear, std::vector<G4double> &y_smear, std::vector<G4double> &z_smear);


    // Check if a loaded variable from a csv is in the desired range
    // Header name is the string in the file that identifies the bins you want to check. e.g. energy, azimuth, zenith
    void CheckVarBounds(std::string filename, G4double var_min, G4double var_max, std::string HeaderName);


}

#endif
