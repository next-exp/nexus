// -----------------------------------------------------------------------------
// nexus | NextDemo.h
//
// Geometry of the NEXT-DEMO++ detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXT_DEMO_H
#define NEXT_DEMO_H

#include "GeometryBase.h"

class G4GenericMessenger;

namespace nexus {

  class NextDemoVessel;
  class NextDemoInnerElements;


  class NextDemo: public GeometryBase
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
    G4double specific_vertex_X_, specific_vertex_Y_, specific_vertex_Z_;
    NextDemoVessel* vessel_geom_;
    NextDemoInnerElements* inner_geom_;
    G4GenericMessenger* msg_;
  };

} // end namespace nexus

#endif
