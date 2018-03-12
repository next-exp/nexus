#ifndef FULLRING_
#define FULLRING_

#include "BaseGeometry.h"

class G4GenericMessenger;
class G4LogicalVolume;
namespace nexus {
  class PetaloTrap;
  class CylinderPointSampler;
  class PetKDBFixedPitch;}

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
    void BuildCryostat();
    void BuildDetector(); 
    void BuildPhantom();
    void BuildOneModule(); 

    PetaloTrap* module_;
    PetKDBFixedPitch* kdb_;
    
    G4LogicalVolume* lab_logic_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_; 

    G4double det_thickness_, n_modules_, r_dim_;
    G4double internal_diam_, external_diam_;
    G4double cryo_width_, cryo_thickn_;
    G4int n_cells_;

    double phantom_diam_; 
    double phantom_length_;

    CylinderPointSampler* cylindric_gen_;

    G4LogicalVolume* LXe_logic_;

  };
}
#endif
