#ifndef FULLRING_
#define FULLRING_

#include "BaseGeometry.h"

class G4GenericMessenger;
namespace nexus {class PetaloTrap;
  class CylinderPointSampler;}

namespace nexus {
  class FullRing : public BaseGeometry {

  public:
    // Constructor
    FullRing();
    //Destructor
    ~FullRing();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    private:
    void Construct();
    void BuildDetector(); 
    void BuildPhantom();

    PetaloTrap* module_;
    
    G4LogicalVolume* lab_logic_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

    G4double det_thickness_, n_modules_, z_size_, ring_diameter_;

    double phantom_diam_; 
    double phantom_length_;

    CylinderPointSampler* cylindric_gen_;

  };
}
#endif
