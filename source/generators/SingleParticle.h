// ----------------------------------------------------------------------------
// nexus | SingleParticle.h
//
// This class is the primary generator for events consisting of
// a single particle. The user must specify via configuration
// parameters the particle type, a kinetic energy interval and, optionally,
// a momentum direction.
// Particle energy is generated with flat random probability
// between E_min and E_max.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SINGLE_PARTICLE_H
#define SINGLE_PARTICLE_H

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
    G4GenericMessenger* msg_;
  
    G4ParticleDefinition* particle_definition_;

    G4double energy_min_; ///< Minimum kinetic energy 
    G4double energy_max_; ///< Maximum kinetic energy

    const BaseGeometry* geom_; ///< Pointer to the detector geometry

    G4String region_;

    G4double momentum_X_;
    G4double momentum_Y_;
    G4double momentum_Z_;

    G4double costheta_min_;
    G4double costheta_max_;
    G4double phi_min_;
    G4double phi_max_;

    
  };

} // end namespace nexus

#endif
