// ----------------------------------------------------------------------------
// nexus | LambertianGenerator.h
//
// This class is a generator for particle events generated in a given cone
// following a Lambertian distribution. The user must specify via configuration
// parameters the particle type, a kinetic energy interval, momentum direction
// and phi and theta limits.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef LAMBERTIAN_GENERATOR_H
#define LAMBERTIAN_GENERATOR_H

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;


namespace nexus {

  class GeometryBase;

  class LambertianGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    LambertianGenerator();
    /// Destructor
    ~LambertianGenerator();

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

    const GeometryBase* geom_; ///< Pointer to the detector geometry

    G4String region_;

    G4ThreeVector momentum_;

    G4double costheta_min_;
    G4double costheta_max_;
    G4double phi_min_;
    G4double phi_max_;


  };

} // end namespace nexus

#endif
