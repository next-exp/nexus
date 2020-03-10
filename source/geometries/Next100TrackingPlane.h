// -----------------------------------------------------------------------------
//  nexus | Next100TrackingPlane.h
//
//  * Author: <justo.martin-albo@ific.uv.es>
//  * Creation date: 10 February 2020
// -----------------------------------------------------------------------------

#ifndef NEXT100_TRACKING_PLANE_H
#define NEXT100_TRACKING_PLANE_H

#include "BaseGeometry.h"
#include <G4ThreeVector.hh>
#include <vector>

class G4VPhysicalVolume;
class G4GenericMessenger;

namespace nexus {

  class Next100SiPMBoard;
  class CylinderPointSampler2020;

  // Geometry of the tracking plane of the NEXT-100 detector

  class Next100TrackingPlane: public BaseGeometry
  {
  public:
    // Constructor
    Next100TrackingPlane(G4double origin_z_coord=0.);
    // Destructor
    ~Next100TrackingPlane();
    //
    void SetMotherPhysicalVolume(G4VPhysicalVolume*);
    //
    void Construct();
    //
    G4ThreeVector GenerateVertex(const G4String&) const;

  private:
    const G4double z0_; // Z position of origin of coordinates
    const G4double copper_plate_diameter_, copper_plate_thickness_;
    const G4double distance_board_board_;

    std::vector<G4ThreeVector> board_pos_;

    G4bool visibility_;

    Next100SiPMBoard* sipm_board_geom_;

    CylinderPointSampler2020* copper_plate_gen_;

    G4VPhysicalVolume* mpv_; // Pointer to mother's physical volume

    G4GenericMessenger* msg_;
  };

  inline void Next100TrackingPlane::SetMotherPhysicalVolume(G4VPhysicalVolume* p)
  { mpv_ = p; }

} // namespace nexus

#endif
