// ----------------------------------------------------------------------------
// nexus | Next100TrackingPlane.h
//
// Tracking plane of the NEXT-100 geometry.
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

  class Next100SiPMBoard;
  class CylinderPointSampler;
  class BoxPointSampler;

  // Geometry of the tracking plane of the NEXT-100 detector

  class Next100TrackingPlane: public GeometryBase
  {
  public:
    // Constructor
    Next100TrackingPlane();
    // Destructor
    ~Next100TrackingPlane();
    //
    void SetMotherPhysicalVolume(G4VPhysicalVolume*);
    void SetELtoTPdistance(G4double);
    //
    void Construct() override;
    //
    G4ThreeVector GenerateVertex(const G4String&) const override;

    void PrintSiPMPosInGas() const;
    void GetSiPMPosInGas(std::vector<G4ThreeVector>& sipm_pos) const;

  private:
    void PlaceSiPMBoardColumns(G4int, G4double, G4double, G4int&, G4LogicalVolume*);

  private:
    const G4double copper_plate_diameter_, copper_plate_thickness_;
    const G4double distance_board_board_;

    G4double gate_tp_dist_;

    std::vector<G4ThreeVector> board_pos_;
    std::vector<G4ThreeVector> plug_pos_;

    G4bool visibility_;

    Next100SiPMBoard* sipm_board_geom_;

    CylinderPointSampler* copper_plate_gen_;
    BoxPointSampler* plug_gen_;

    G4VPhysicalVolume* mpv_; // Pointer to mother's physical volume

    G4GenericMessenger* msg_;
    // Geometry Navigator
    G4Navigator* geom_navigator_;
  };

  inline void Next100TrackingPlane::SetMotherPhysicalVolume(G4VPhysicalVolume* p)
  { mpv_ = p; }

  inline void Next100TrackingPlane::SetELtoTPdistance(G4double distance){
    gate_tp_dist_ = distance;
  }

} // namespace nexus

#endif
