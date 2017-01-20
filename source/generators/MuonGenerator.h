// ----------------------------------------------------------------------------
///  \file   MuonGenerator.h
///  \brief  Point Sampler for muons generation.
///  
///  \author   Neus Lopez March <neus.lopez@ific.uv.es>    
///  \date     30 Jan 2014
/// 
///
///  Copyright (c) 2015 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __MUON_GENERATOR__
#define __MUON_GENERATOR__

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;


namespace nexus {

  class BaseGeometry;
  
  class MuonGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    MuonGenerator();
    /// Destructor
    ~MuonGenerator();
    
    /// This method is invoked at the beginning of the event. It sets 
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:

    /// Generate a random kinetic energy with flat probability in 
    //  the interval [energy_min, energy_max].
    G4double RandomEnergy() const;
    G4double GetPhi();
    G4double GetTheta();    

  private:
    G4GenericMessenger* _msg;
  
    G4ParticleDefinition* _particle_definition;

    G4double _energy_min; ///< Minimum kinetic energy 
    G4double _energy_max; ///< Maximum kinetic energy

    G4String _region;

    const BaseGeometry* _geom; ///< Pointer to the detector geometry

    G4double _momentum_X;
    G4double _momentum_Y;
    G4double _momentum_Z;
       
  };

} // end namespace nexus

#endif
