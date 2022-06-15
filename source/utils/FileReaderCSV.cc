// ----------------------------------------------------------------------------
// nexus | FileReaderCSV.cc
//
// This class provides the utilities to read in histogram information 
// from a CSV file
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "FileReaderCSV.h"

#include <G4ExceptionSeverity.hh>
#include <G4Exception.hh>


namespace nexus {

  FileReaderCSV::FileReaderCSV(){}

  FileReaderCSV::~FileReaderCSV(){}

  // --------

  // Input file format:
  // value,<intensity in bin>,<histogram x bin i centre>,<histogram x bin i width>
  void FileReaderCSV::LoadHistData1D(std::string filename, std::vector<double> &value,
                                    std::vector<double> &x, std::vector<double> &x_smear){
    
    // Open the file
    FileIn_.open(filename);

    // Check if file has opened properly
    if (!FileIn_.is_open()){
      G4Exception("[FileReaderCSV]", "LoadHistData1D()",
                FatalException, " could not read in the CSV file ");
    }

    // Read the Data from the file as strings
    std::string s_header, s_value, s_x;
    std::string s_x_smear;

    // Loop over the lines in the file and add the values to a vector
    while (FileIn_.peek()!=EOF) {

      if (s_header == "value"){

        std::getline(FileIn_, s_header, ',');

        std::getline(FileIn_, s_value, ',');
        std::getline(FileIn_, s_x, ',');
        std::getline(FileIn_, s_x_smear, '\n');

        value.push_back(stod(s_value));
        x.push_back(stod(s_x));
        x_smear.push_back(stod(s_x_smear));
      }
    }
  }

  // --------

  // Input file format:
  // value,<intensity in bin>,<histogram x bin i centre>,<histogram y bin j centre>,<histogram x bin i width>,<histogram y bin j width>
  void FileReaderCSV::LoadHistData2D(std::string filename, std::vector<double> &value,
                                     std::vector<double> &x, std::vector<double> &y,
                                     std::vector<double> &x_smear,
                                     std::vector<double> &y_smear){
    
    // Open the file
    FileIn_.open(filename);

    // Check if file has opened properly
    if (!FileIn_.is_open()){
      G4Exception("[FileReaderCSV]", "LoadHistData2D()",
                FatalException, " could not read in the CSV file ");
    }

    // Read the Data from the file as strings
    std::string s_header, s_value, s_x, s_y;
    std::string s_x_smear, s_y_smear;

    // Loop over the lines in the file and add the values to a vector
    while (FileIn_.peek()!=EOF) {

      if (s_header == "value"){
        std::getline(FileIn_, s_header, ',');

        std::getline(FileIn_, s_value, ',');
        std::getline(FileIn_, s_x, ',');
        std::getline(FileIn_, s_y, ',');
        std::getline(FileIn_, s_x_smear, ',');
        std::getline(FileIn_, s_y_smear, '\n');

        value.push_back(stod(s_value));
        x.push_back(stod(s_x));
        y.push_back(stod(s_y));
        x_smear.push_back(stod(s_x_smear));
        y_smear.push_back(stod(s_y_smear));
      }
    }
  }

  // --------

  // Input file format:
  // value,<intensity in bin>,<histogram x bin i centre>,<histogram y bin j centre>,<histogram z bin k centre>,<histogram x bin i width>,<histogram y bin j width>,<histogram z bin k width>
  void FileReaderCSV::LoadHistData3D(std::string filename, std::vector<double> &value,
                        std::vector<double> &x, std::vector<double> &y, std::vector<double> &z,
                        std::vector<double> &x_smear, std::vector<double> &y_smear, std::vector<double> &z_smear){
    
    // Open the file
    FileIn_.open(filename);

    // Check if file has opened properly
    if (!FileIn_.is_open()){
      G4Exception("[FileReaderCSV]", "LoadHistData3D()",
                FatalException, " could not read in the CSV file ");
    }

    // Read the Data from the file as strings
    std::string s_header, s_value, s_x, s_y, s_z;
    std::string s_x_smear, s_y_smear, s_z_smear;

    // Loop over the lines in the file and add the values to a vector
    while (FileIn_.peek()!=EOF) {

      if (s_header == "value"){
        std::getline(FileIn_, s_header, ',');

        std::getline(FileIn_, s_value, ',');
        std::getline(FileIn_, s_x, ',');
        std::getline(FileIn_, s_y, ',');
        std::getline(FileIn_, s_z, ',');
        std::getline(FileIn_, s_x_smear, ',');
        std::getline(FileIn_, s_y_smear, ',');
        std::getline(FileIn_, s_z_smear, '\n');

        value.push_back(stod(s_value));
        x.push_back(stod(s_x));
        y.push_back(stod(s_y));
        z.push_back(stod(s_z));
        x_smear.push_back(stod(s_x_smear));
        y_smear.push_back(stod(s_y_smear));
        z_smear.push_back(stod(s_z_smear));
      }
    }
  }

  // --------

} // end namespace nexus
