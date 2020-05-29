// ----------------------------------------------------------------------------
// nexus | ElecPositronPairGenerator.h
//
// This generator simulates an electron and a positron from the same vertex,
// with total kinetic energy settable by parameter.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef ELEC_POSITRON
#define ELEC_POSITRON

#include <G4VPrimaryGenerator.hh>

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;


namespace nexus {

  class BaseGeometry;

  class ElecPositronPairGenerator: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    ElecPositronPairGenerator();
    /// Destructor
    ~ElecPositronPairGenerator();

    /// This method is invoked at the beginning of the event. It sets
    /// a primary vertex (that is, a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:

    /// Generate a random kinetic energy with flat probability in
    //  the interval [energy_min, energy_max].
    G4double RandomEnergy(G4double emin, G4double emax) const;

  private:
    G4GenericMessenger* msg_;

    G4ParticleDefinition* particle_definition_;

    G4double energy_min_; ///< Minimum total kinetic energy
    G4double energy_max_; ///< Maximum total kinetic energy

    const BaseGeometry* geom_; ///< Pointer to the detector geometry

    G4String region_;

  };

} // end namespace nexus

#endif
