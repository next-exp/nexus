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

#include "fstream"
#include "string"
#include "vector"


namespace nexus {

  /// Sampler of random uniform points in a box-shaped volume

  class FileReaderCSV {
  public:
    /// Constructor
    FileReaderCSV();

    /// Destructor
    ~FileReaderCSV();

    /// Read in the 1d histogram stored in a csv file
    void LoadHistData1D(std::string filename, std::vector<double> &value,
                        std::vector<double> &x,
                        std::vector<double> &x_smear);
    
    /// Read in the 2d histogram stored in a csv file
    void LoadHistData2D(std::string filename, std::vector<double> &value,
                        std::vector<double> &x, std::vector<double> &y,
                        std::vector<double> &x_smear,
                        std::vector<double> &y_smear);
    
    /// Read in the 3d histogram stored in a csv file
    void LoadHistData3D(std::string filename, std::vector<double> &value,
                        std::vector<double> &x, std::vector<double> &y, std::vector<double> &z,
                        std::vector<double> &x_smear, std::vector<double> &y_smear, std::vector<double> &z_smear);

  private:

  // File pointer
  std::ifstream FileIn_; // Input CSV
    
  };

} // namespace nexus

#endif
