// ----------------------------------------------------------------------------
// nexus | NextDemoTrackingPlane.h
//
// Tracking plane of the Demo++ geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_TRACKING_PLANE_H
#define NEXT100_TRACKING_PLANE_H

#include "GeometryBase.h"
#include <G4ThreeVector.hh>
#include <vector>

class G4VPhysicalVolume;
class G4GenericMessenger;
class G4Navigator;

namespace nexus {

  class NextDemoSiPMBoard;
  class BoxPointSamplerLegacy;

  // Geometry of the tracking plane of the Demo++ detector
  class NextDemoTrackingPlane: public GeometryBase
  {
  public:
    // Constructor
    NextDemoTrackingPlane();

    // Destructor
    ~NextDemoTrackingPlane();

    void SetConfig(G4String config);

    void SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys);

    void Construct() override;

    G4ThreeVector GenerateVertex(const G4String&) const override;

  private:
    void GenerateBoardPositions(G4double board_posz);

  private:
    G4bool verbosity_;
    G4bool visibility_;

    // Configuration
    G4String config_;

    const G4double plate_side_, plate_thickn_, plate_hole_side_;

    G4int              num_boards_;
    NextDemoSiPMBoard* sipm_board_;
    G4ThreeVector      board_size_;
    std::vector<G4ThreeVector> board_pos_;

    BoxPointSamplerLegacy*    plate_gen_;

    G4VPhysicalVolume*  mother_phys_;

    G4GenericMessenger* msg_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;
  };

  inline void NextDemoTrackingPlane::SetConfig(G4String config)
  { config_ = config; }

  inline void NextDemoTrackingPlane::SetMotherPhysicalVolume(G4VPhysicalVolume* mother_phys)
    { mother_phys_ = mother_phys; }

} // namespace nexus

#endif
