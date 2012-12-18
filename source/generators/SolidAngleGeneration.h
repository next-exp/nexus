#ifndef __SOLIDANGLE_GENERATOR__
#define __SOLIDANGLE_GENERATOR__

#include <G4VPrimaryGenerator.hh>

class G4Event;
class G4ParticleDefinition;


namespace nexus {

  class BaseGeometry;


  /// Primary generator (concrete class of G4VPrimaryGenerator) for events 
  /// consisting of a single particle. The user must specify via configuration
  /// parameters the particle type, a kinetic energy interval.
  /// Particle energy is generated with flat random probability
  /// between E_min and E_max.
  /// In addition, the user must specify the fraction of solid angle
  /// in which the particle will be generated. This is done through the
  /// variable theta and phi (sferical coordinates), 
  /// setting a minimum and maximum value for both of
  /// them.
  
  class SolidAngleGeneration: public G4VPrimaryGenerator
  {
  public:
    /// Constructor
    SolidAngleGeneration();

    /// Destructor
    ~SolidAngleGeneration() {}
    
    /// This method is invoked at the beginning of the event. It sets 
    /// a primary vertex (that is, a particle in a given position and time)
    /// in the event.
    void GeneratePrimaryVertex(G4Event*);

  private:
    /// Read and set user's configuration parameters
    void ReadConfigParams();

    /// Generate a random kinetic energy with flat probability in 
    //  the interval [energy_min, energy_max].
    G4double RandomEnergy() const;
    
  private:
    
    G4int _charge; ///< Particle charge
    G4double _mass; ///< Particle rest mass

    G4double _energy_min; ///< Minimum possible kinetic energy 
    G4double _energy_max; ///< Maximum possible kinetic energy

    G4double _costheta_min; ///< Minimum possibile value for cos(theta)
    G4double _costheta_max; ///< Maximum possibile value for cos(theta)
    G4double _phi_min; ///< Minimum possibile value for phi
    G4double _phi_max; ///< Maximum possibile value for phi

    G4String _region;
    G4ParticleDefinition* _particle_definition;
    const BaseGeometry* _geom;
  };

} // end namespace nexus

#endif
