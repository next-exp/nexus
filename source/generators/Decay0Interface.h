// ----------------------------------------------------------------------------
// nexus | Decay0Interface.h
//
// This class is the primary generator of a Xe-136 double beta decay,
// interfacing the DECAY0 c++ code, translated from the original
// FORTRAN package, with nexus.
// The possibility of reading a previously generated ascii file with the
// electron momenta is also allowed.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef DECAY0_INTERFACE_H
#define DECAY0_INTERFACE_H

#include <G4VPrimaryGenerator.hh>
#include <fstream>

class G4GenericMessenger;
class G4Event;
class G4PrimaryParticle;
class decay0;

namespace nexus {

  class GeometryBase;


  /// This primary generator sets the G4Event objects according to the
  /// information read from an ascii file produced by the Decay0
  /// Monte-Carlo event generator.

  class Decay0Interface : public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    Decay0Interface();
    /// Destructor
    ~Decay0Interface();

    /// Read an event from file and generate primary particles
    /// and primary vertices accordingly
    void GeneratePrimaryVertex(G4Event*);

  private:
    /// Open the Decay0 input file selected by the user
    void OpenInputFile(G4String);
    /// Parse information in the file header
    void ProcessHeader();

    /// Return the PDG code equivalent to a given GEANT3 particle code
    G4int G3toPDG(const G4int);

  private:
    G4GenericMessenger* msg_;

    G4String decay_file_;

    std::ifstream file_; ///< ASCII file produced by Decay0
    G4String region_; ///< region of generation of vertices in geometry

    G4bool opened_;

    int myEventCounter_;

    decay0 *decay0_;
    int Xe136DecayMode_; // See method printDecayModeList  Default is 1
    int Ba136FinalState_; // labeled by energy level, in keV .
                          // Valid list: 0, 819,  1551, 1579, 2080, 2129, 2141, 2223, 2315, 2400)
			  // default is 0 (ground state)

    double energyThreshold_;

    std::ofstream fOutDebug_; // for debugging...
    const GeometryBase* geom_;


    inline void SetEnergyThreshold(double e) { energyThreshold_ = e;}
    inline void SetXe136DecayMode(int dcm) { Xe136DecayMode_ = dcm;}
    inline void SetBa136FinalState(int fs) { Ba136FinalState_ = fs;}
  };


} //end namespace nexus




#endif
