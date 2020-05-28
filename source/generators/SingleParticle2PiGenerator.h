// ----------------------------------------------------------------------------
// nexus | SingleParticle2PiGenerator.h
//
// This class is the primary generator for events consisting of
// a single particle, generated in the direction towards the detector,
// from the NEXT-DEMO sideport. The user must specify via configuration
// parameters the particle type, a kinetic energy interval.
// Particle energy is generated with flat random probability
// between E_min and E_max.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef SINGLE_PARTICLE_2PI_GEN__
#define SINGLE_PARTICLE_2PI_GEN__

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;


namespace nexus {

  class BaseGeometry;

  class SingleParticle2PiGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    SingleParticle2PiGenerator();
    /// Destructor
    ~SingleParticle2PiGenerator();

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
  };

} // end namespace nexus

#endif
