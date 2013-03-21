// ----------------------------------------------------------------------------
///  \file   SingleParticle.h
///  \brief  Primary generator for single-particle events.
///  
///  \author   J Martin-Albo <jmalbos@ific.uv.es>    
///  \date     27 Mar 2009
///  \version  $Id$
///
///  Copyright (c) 2009, 2010 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __SINGLE_PARTICLE__
#define __SINGLE_PARTICLE__

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;


namespace nexus {

  class BaseGeometry;


  /// Primary generator (concrete class of G4VPrimaryGenerator) for events 
  /// consisting of a single particle. The user must specify via configuration
  /// parameters the particle type, a kinetic energy interval (a random
  /// value with random .
  /// Particle energy is generated with flat random probability
  /// between E_min and E_max.
  
  class SingleParticle: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    SingleParticle();
    /// Destructor
    ~SingleParticle();
    
    /// This method is invoked at the beginning of the event. It sets 
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:

    void SetParticleDefinition(G4String);

    /// Generate a random kinetic energy with flat probability in 
    //  the interval [energy_min, energy_max].
    G4double RandomEnergy() const;
    
  private:
    G4GenericMessenger* _msg;
  
    G4ParticleDefinition* _particle_definition;

    G4double _energy_min; ///< Minimum kinetic energy 
    G4double _energy_max; ///< Maximum kinetic energy

    G4String _region;

    const BaseGeometry* _geom; ///< Pointer to the detector geometry
  };

} // end namespace nexus

#endif
