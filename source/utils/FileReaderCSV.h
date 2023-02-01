// ----------------------------------------------------------------------------
// nexus | FileReaderCSV.h
//
// This class provides the utilities to read in histogram information 
// from a CSV file
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef FILE_READER_CSV_H
#define FILE_READER_CSV_H

#include <G4Types.hh>

#include "fstream"
#include "string"
#include "vector"

namespace nexus {

  /// Class for reading in CSV files that correspond to a binned histogram

  class FileReaderCSV {
  public:
    /// Constructor
    FileReaderCSV();

    /// Destructor
    ~FileReaderCSV();

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

  private:
    
  };

} // namespace nexus

#endif
