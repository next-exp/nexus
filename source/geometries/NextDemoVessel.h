// -----------------------------------------------------------------------------
// nexus | NextDemoVessel.h
//
// Geometry of the pressure vessel of the NEXT-DEMO++ detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_DEMO_VESSEL_H
#define NEXT_DEMO_VESSEL_H

#include "BaseGeometry.h"

namespace nexus {

  class NextDemoVessel: public BaseGeometry
  {
  public:
    NextDemoVessel();
    ~NextDemoVessel();
    void Construct() override;
    G4ThreeVector GenerateVertex(const G4String& region) const override;

  private:
    const G4double vessel_diam_;
    const G4double vessel_length_;
    const G4double vessel_thickn_;
  };

} // end namespace nexus

#endif
