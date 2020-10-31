// -----------------------------------------------------------------------------
// nexus | NextDemo.h
//
// Geometry of the NEXT-DEMO++ detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_DEMO_H
#define NEXT_DEMO_H

#include "BaseGeometry.h"

namespace nexus {

  class NextDemoVessel;
  class NextDemoInnerElements;


  class NextDemo: public BaseGeometry
  {
  public:
    NextDemo();
    ~NextDemo();
    void Construct() override;
    G4ThreeVector GenerateVertex(const G4String& region) const override;

  private:
    void ConstructLab();

  private:
    const G4double lab_size_;

    NextDemoVessel* vessel_geom_;
    NextDemoInnerElements* inner_geom_;
  };

} // end namespace nexus

#endif
