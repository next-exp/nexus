// ----------------------------------------------------------------------------
// nexus | CsI.h
//

#ifndef CSI_H
#define CSI_H

#include "GeometryBase.h"
#include "CylinderPointSampler2020.h"

class G4GenericMessenger;

namespace nexus {

  class CsI: public GeometryBase {
  public:
    /// Constructor
    CsI();

    /// Destructor
    ~CsI();

    void Construct();
    G4ThreeVector GenerateVertex(const G4String& region) const;


  private:

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;
    CylinderPointSampler2020* inside_source_;

    // Dimension of the crystals
    G4double crystal_width_;
    G4double crystal_length_;


  };
}
#endif
