#ifndef PETBOX_
#define PETBOX_

#include "BaseGeometry.h"

class G4GenericMessenger;
class G4LogicalVolume;

namespace nexus {
}

namespace nexus {
  class PetBox : public BaseGeometry {

  public:
    // Constructor
    PetBox();
    //Destructor
    ~PetBox();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    private:
    void Construct();
    void BuildBox();
    void BuildSensors();

    G4LogicalVolume* lab_logic_;
    G4LogicalVolume* LXe_logic_;

    G4bool visibility_;

    G4double source_pos_x_, source_pos_y_, source_pos_z_;
    G4double box_size_, box_thickness_;

    /// Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };
}
#endif
