// ----------------------------------------------------------------------------
// nexus | RandomUtils.cc
//
// Commonly used generic functions.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------
#include "RandomUtils.h"

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

  G4int GetRandBinIndex(G4RandGeneral *fRandomGeneral, std::vector<G4double> intensity){

    return round(fRandomGeneral->fire()*intensity.size());

  }

  G4double Sample(G4double sample, G4bool smear, G4double smearval){

    // Apply Gaussian smearing to smooth from bin-to-bin
    if (smear){
      return sample + G4RandGauss::shoot( 0., smearval);
    }
    // Return the sampled histogram without smearing the entries
    else
      return sample;

  }

  G4bool CheckOutOfBound(G4double min, G4double max, G4double val){

    // Out of bounds
    if (val < min || val > max)
      return true;
    // In suitable bounds -- pass
    else
      return false;

  }

  G4bool CheckOutOfBoundMin(G4double min, G4double val){

    // Out of bounds
    if (val < min)
      return true;
    // In suitable bounds -- pass
    else
      return false;

  }

  G4bool CheckOutOfBoundMax(G4double max, G4double val){

    // Out of bounds
    if (val > max)
      return true;
    // In suitable bounds -- pass
    else
      return false;

  }

  // --------

  // Input file format:
  // value,<intensity in bin>,<histogram x bin i centre>,<histogram x bin i width>
  void LoadHistData1D(std::string filename, std::vector<G4double> &value,
                                    std::vector<G4double> &x, std::vector<G4double> &x_smear){
    
    // Open the file
    std::ifstream FileIn_(filename);

    // Check if file has opened properly
    if (!FileIn_.is_open()){
      G4Exception("[RandomUtils]", "LoadHistData1D()",
                FatalException, " could not read in the CSV file ");
    }

    // Read the Data from the file as strings
    std::string s_header, s_value, s_x;
    std::string s_x_smear;

    // Loop over the lines in the file and add the values to a vector
    while (FileIn_.peek()!=EOF) {

      std::getline(FileIn_, s_header, ',');

      if (s_header == "value"){

        std::getline(FileIn_, s_value, ',');
        std::getline(FileIn_, s_x, ',');
        std::getline(FileIn_, s_x_smear, '\n');

        value.push_back(stod(s_value));
        x.push_back(stod(s_x));
        x_smear.push_back(stod(s_x_smear));
      }

    } // END While

    FileIn_.close();
  
  } // END LoadHistData1D

  // --------

  // Input file format:
  // value,<intensity in bin>,<histogram x bin i centre>,<histogram y bin j centre>,<histogram x bin i width>,<histogram y bin j width>
  void LoadHistData2D(std::string filename, std::vector<G4double> &value,
                                     std::vector<G4double> &x, std::vector<G4double> &y,
                                     std::vector<G4double> &x_smear,
                                     std::vector<G4double> &y_smear){
    
    // Open the file
    std::ifstream FileIn_(filename);
    
    // Check if file has opened properly
    if (!FileIn_.is_open()){
      G4Exception("[RandomUtils]", "LoadHistData2D()",
                FatalException, " could not read in the CSV file ");
    }

    // Read the Data from the file as strings
    std::string s_header, s_value, s_x, s_y;
    std::string s_x_smear, s_y_smear;

    // Loop over the lines in the file and add the values to a vector
    while (FileIn_.peek()!=EOF) {

      std::getline(FileIn_, s_header, ',');
      
      if (s_header == "value"){

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
    
    } // END While

    FileIn_.close();
  
  } // END LoadHistData2D
  

  // --------

  // Input file format:
  // value,<intensity in bin>,<histogram x bin i centre>,<histogram y bin j centre>,<histogram z bin k centre>,<histogram x bin i width>,<histogram y bin j width>,<histogram z bin k width>
  void LoadHistData3D(std::string filename, std::vector<G4double> &value,
                        std::vector<G4double> &x, std::vector<G4double> &y, std::vector<G4double> &z,
                        std::vector<G4double> &x_smear, std::vector<G4double> &y_smear, std::vector<G4double> &z_smear){
    
    std::ifstream FileIn_(filename);

    // Check if file has opened properly
    if (!FileIn_.is_open()){
      G4Exception("[RandomUtils]", "LoadHistData3D()",
                FatalException, " could not read in the CSV file ");
    }

    // Read the Data from the file as strings
    std::string s_header, s_value, s_x, s_y, s_z;
    std::string s_x_smear, s_y_smear, s_z_smear;

    // Loop over the lines in the file and add the values to a vector
    while (FileIn_.peek()!=EOF) {

      std::getline(FileIn_, s_header, ',');

      if (s_header == "value"){
      
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

    } // END While

    FileIn_.close();
  
  }  // END LoadHistData3D

  // --------

  void CheckVarBounds(std::string filename, G4double var_min, G4double var_max, std::string HeaderName){
    
    std::ifstream FileIn_(filename);

    // Max and min energies in sampled file
    G4double file_VarMin = 1.0e20;
    G4double file_VarMax = 0.;

    // Check if file has opened properly
    if (!FileIn_.is_open()){
      G4Exception("[RandomUtils]", "CheckVarBounds()",
                FatalException, " could not read in the CSV file ");
    }

    // Read the Data from the file as strings
    std::string s_line, s_var;

    // Loop over the lines in the file
    while (std::getline(FileIn_, s_line)) {

      std::stringstream s(s_line);

      G4bool match = false; 

      // Loop over the columns in the file
      while (std::getline(s, s_var, ',' )) {

          // Check if the column matches the header
          if (s_var == HeaderName){
            match = true;
            continue;
          }

          // Read the value
          if (match){

            G4double var = stod(s_var);
        
            // Get the max and min values from the file
            if (var < file_VarMin) file_VarMin = var;
            if (var > file_VarMax) file_VarMax = var;

            match = false;

          }

      }

    } // END While

     // Check if the specified variable range has been set to a suitable value
    if ((var_min < file_VarMin || var_max > file_VarMax )){
      std::cout << "The minimum " << HeaderName <<" value allowed is: " << file_VarMin << ", your input config min value is: " << var_min << std::endl;
      std::cout << "The maximum " << HeaderName <<" value allowed is: " << file_VarMax << ", your input config max value is: " << var_max << std::endl;
      G4Exception("[RandomUtils]", "CheckVarBounds()",
                FatalException, " Specified range for sampling is outside permitted range or the min/max of the variable has not been set");
    }

    FileIn_.close();
  
  }  // END CheckVarBounds


  // -------


}
