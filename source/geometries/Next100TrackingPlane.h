// -----------------------------------------------------------------------------
//  nexus | Next100TrackingPlane.h
//
//  * Author: <justo.martin-albo@ific.uv.es>
//  * Creation date: 10 February 2020
// -----------------------------------------------------------------------------

#ifndef NEXT100_TRACKING_PLANE_H
#define NEXT100_TRACKING_PLANE_H

#include "BaseGeometry.h"


namespace nexus {

  class Next100TrackingPlane: public BaseGeometry
  {
  public:
    // Constructor
    // The mother volume in which all elements of the tracking plane
    // will be positioned must be provided at construction time.
    Next100TrackingPlane(G4LogicalVolume* mother_volume);
    // Destructor
    virtual ~Next100TrackingPlane();
    //
    virtual void Construct();
    //
    virtual G4ThreeVector GenerateVertex(const G4String&) const;

  private:
    // Default constructor hidden
    Next100TrackingPlane();

  private:
    G4double copper_plate_diameter_, copper_plate_thickness_;
    G4double distance_gate_tracking_plane_;
    G4double distance_board_board_;
    G4LogicalVolume* mother_volume_;
  };

} // namespace nexus

#endif
