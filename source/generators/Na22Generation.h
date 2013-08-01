#ifndef __NA22GENERATION__
#define __NA22GENERATION__

#include <G4VPrimaryGenerator.hh>

class G4Event;
class G4ParticleDefinition;
class G4GenericMessenger;

namespace nexus {

  class BaseGeometry;

  /// Primary generator (concrete class of G4VPrimaryGenerator) 
  /// for events Na22-like consisting of two particles in coincidence (back-to-back)
  /// coming from e+e- annihilation and one disexcitation gamma. 
  /// The user must specify via configuration parameters the particle type, 
  /// a kinetic energy interval and the direction of generation for both the 
  /// disexcitation gamma and the annihilation pair.
  /// Each particle energy is generated with flat random probability
  /// between E_min and E_max.
  /// If a direction of generation is not specified, the user should choose a fraction 
  /// of solid angle inside whom a direction will be randomly generation. See nexus_example5.config.
  
  class Na22Generation: public G4VPrimaryGenerator
  {
  public:
    //Constructor
    Na22Generation();
    //Destructor
    ~Na22Generation();

    void GeneratePrimaryVertex(G4Event* evt);

  private:

    G4GenericMessenger* _msg;
    
    G4double _energy_min; ///< Minimum possible kinetic energy for annihilation particle
    G4double _energy_max; ///< Maximum possible kinetic energy for annihilation particle
    G4double _energy_min_dis; ///< Minimum possible kinetic energy for disexcitation gamma
    G4double _energy_max_dis; ///< Maximum possible kinetic energy for disexcitation gamma
    
    G4String _solid_angle;
    G4String _solid_angle_dis;

    G4bool _direction_is_random;
    G4bool _direction_dis_is_random;
    G4ThreeVector _momentum_direction; 
    G4ThreeVector _momentum_direction_dis; 

    G4String _region;
    G4ParticleDefinition* _particle_dis_definition;
    G4ParticleDefinition* _particle_definition;

    const BaseGeometry* _geom;
  };

}// end namespace nexus
#endif
