#ifndef CAL_SOURCE_H
#define CAL_SOURCE_H

#include "BaseGeometry.h"

class G4GenericMessenger;

namespace nexus {

  class CalibrationSource: public BaseGeometry {
  public:
    /// Constructor
    CalibrationSource();

    /// Destructor
    ~CalibrationSource();

    void Construct();
   
    G4double GetSourceDiameter(); 
    G4double GetSourceThickness();
    
    G4double GetCapsuleDiameter();
    G4double GetCapsuleThickness();

    G4double GetSourceZpos();

  private:

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg; 
    
    // Dimension of the Na22 source itself
    G4double _source_diam;
    G4double _source_thick;

    // Dimension of the whole support
    G4double _capsule_diam;
    G4double _capsule_thick;

    // Position of source inside its capsule
    G4double _source_z_pos;

    // Radioactive source being used
    G4String _source;
    

  };
}
#endif
