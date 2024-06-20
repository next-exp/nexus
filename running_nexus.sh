#!/bin/bash

# Geant4 Path, edit G4Install path to where the main geant4 code folder your downloaded
export G4INSTALL=/home/investigator/Documents/simulation_software//geant4-v11.0.2/install;
export PATH=$G4INSTALL/bin:$PATH;
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$G4INSTALL/lib;
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$G4INSTALL/lib;

echo geant4;

cd $G4INSTALL/bin;
source geant4.sh;
cd -;

echo g4finished;

export QT_PATH=/home/investigator/Qt/5.15.2/gcc_64;
export PATH=$QT_PATH/bin:$PATH;
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$QT_PATH/lib;
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$QT_PATH/lib;

echo QT;

# Path to GSL
export GSL_PATH=/usr/local;
export PATH=$GSL_PATH/bin:$PATH;
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GSL_PATH/lib;
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$GSL_PATH/lib;

echo gsl;

# Path to HDF5
export HDF5_PATH=/usr/local/hdf5;  # Set the installation path of HDF5
export HDF5_LIB=/usr/local/hdf5/lib;  # Set the path to HDF5 libraries
export HDF5_INC=/usr/local/hdf5/include;  # Set the path to HDF5 headers

echo hdf5;

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HDF5_LIB;
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$HDF5_LIB;

echo sourcing;

# Source the default nexus setup file to set the nexusdir environmental variable
source /home/$USER/Documents/simulation_software/nexus_original/scripts/nexus_setup.sh;

echo sourced;

# Add the nexus exe to the path
export PATH=$PATH:/home/$USER/Documents/simulation_software/nexus_original/bin;

#cmake compilation
cmake_compile () {
cmake --build build --target install
cd build;
cmake ..;
cmake --build . -j 32;
cd ..;
}

echo "Setup Nexus is complete!"
echo "To compile run cmake_compile"


