// ----------------------------------------------------------------------------
///  \file   Decay0Interface.h (originally GenbbInterface.h)
///  \brief  Primary generator interface for Decay0 output files.
///  
///  \author   <justo.martin-albo@ific.uv.es>
///  \date     27 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009-2013 NEXT Collaboration. All rights reserved.
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

  class BaseGeometry;
  

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
    G4GenericMessenger* _msg;

    std::ifstream _file; ///< ASCII file produced by Decay0
    G4String _region; ///< region of generation of vertices in geometry

    G4bool _opened;
    
    int _myEventCounter;
    
    decay0 *_decay0;
    int _Xe136DecayMode; // See method printDecayModeList  Default is 1
    int _Ba136FinalState; // labeled by energy level, in keV . 
                          // Valid list: 0, 819,  1551, 1579, 2080, 2129, 2141, 2223, 2315, 2400) 
			  // default is 0 (ground state)  
    
    std::ofstream _fOutDebug; // for debugging... 
    const BaseGeometry* _geom;
    
    
    inline void SetXe136DecayMode(int dcm) { _Xe136DecayMode = dcm;}
    inline void SetBa136FinalState(int fs) { _Ba136FinalState = fs;}
  };

  
} //end namespace nexus  




#endif
