// ----------------------------------------------------------------------------
// nexus | MonolithicCsI.h
//

#ifndef MonolithicCsI_H
#define MonolithicCsI_H

#include "GeometryBase.h"
#include "CylinderPointSampler2020.h"
#include "BoxPointSampler.h"

class G4GenericMessenger;

namespace nexus {

  class MonolithicCsI: public GeometryBase {
  public:
    /// Constructor
    MonolithicCsI();

    /// Destructor
    ~MonolithicCsI();

    void Construct();
    G4ThreeVector GenerateVertex(const G4String& region) const;


  private:

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;
    BoxPointSampler* box_source_;

    // Dimension of the crystals
    G4double crystal_width_;
    G4double crystal_length_;


  };
}
#endif
