#ifndef __BACK2BACK__
#define __BACK2BACK__

#include <G4VPrimaryGenerator.hh>

class G4Event;
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
  
  class Back2backGammas: public G4VPrimaryGenerator
  {
  public:
    //Constructor
    Back2backGammas();
    //Destructor
    ~Back2backGammas();

    void GeneratePrimaryVertex(G4Event* evt);

  private:

    G4GenericMessenger* _msg;
    const BaseGeometry* _geom;

    G4String _region;
  };

}// end namespace nexus
#endif
