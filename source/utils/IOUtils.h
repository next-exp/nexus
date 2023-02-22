// ----------------------------------------------------------------------------
// nexus | IOUtils.h
//
// Functions for helping with IO
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include <G4ThreeVector.hh>

#include <Randomize.hh>


#ifndef IOUTILS_H
#define IOUTILS_H

namespace nexus {

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
