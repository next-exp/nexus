// -----------------------------------------------------------------------------
// nexus | LSCHallA.h
//
// Class which builds a simplified version of the walls of LSC HallA.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef LSCHALLA_H
#define LSCHALLA_H

#include "BaseGeometry.h"

namespace nexus {class CylinderPointSampler2020;}

class G4LogicalVolume;
class G4GenericMessenger;


namespace nexus {

  class LSCHallA: public BaseGeometry {
  public:
    /// Constructor
    LSCHallA();

    /// Destructor
    ~LSCHallA();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Returns a point within a region projecting from a
    /// given point backwards along a line.
    G4ThreeVector ProjectToRegion(const G4String& region,
				  const G4ThreeVector& point,
				  const G4ThreeVector& dir) const;

    /// Builder
    void Construct();

    // Access to the dimensions of Hall A
    G4double GetLSCHallALength() const;
    G4double GetLSCHallARadius() const;
    G4double GetLSCHallAWallThickn() const;
    G4double GetLSCHallACastleZ() const;
    G4double GetLSCHallACastleY() const;

  private:

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Visibility of the walls
    G4bool visibility_;

    // Dimensions
    G4double lab_radius_, lab_length_, lab_wall_thickn_;

    // Distance from hall A centre to lead castle centre in z
    // and distance in y from cylinder centre and castle centre.
    G4double castle_centre_z_, castle_centre_y_;

    // Cylinder point samplers for vertexing
    CylinderPointSampler2020* hallA_vertex_gen_;
    CylinderPointSampler2020* hallA_outer_gen_;

  };

  inline G4double LSCHallA::GetLSCHallALength() const
  { return lab_length_; }

  inline G4double LSCHallA::GetLSCHallARadius() const
  { return lab_radius_; }

  inline G4double LSCHallA::GetLSCHallAWallThickn() const
  { return lab_wall_thickn_; }

  inline G4double LSCHallA::GetLSCHallACastleZ() const
  { return castle_centre_z_; }

  inline G4double LSCHallA::GetLSCHallACastleY() const
  { return castle_centre_y_; }
}

#endif
