// -----------------------------------------------------------------------------
// nexus | NextNewIcs.h
//
// Inner copper shielding of the NEXT-WHITE detector.
//
// The NEXT Collaboration
// -----------------------------------------------------------------------------

#ifndef NEXTNEW_ICS_H
#define NEXTNEW_ICS_H

#include "BaseGeometry.h"

#include <G4Navigator.hh>

class G4GenericMessenger;

namespace nexus {

  class CylinderPointSampler;

  class NextNewIcs: public BaseGeometry
  {
  public:
    /// Constructor
    NextNewIcs();

    /// Destructor
    ~NextNewIcs();

    /// Sets the Logical Volume where ICS will be placed
    void SetLogicalVolume(G4LogicalVolume* mother_logic);

    /// Sets external positions
    void SetNozzlesZPosition(const G4double lat_nozzle_z_pos, const G4double up_nozzle_z_pos);

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Builder
    void Construct();


  private:
    // Mother Logical Volume of the ICS
    G4LogicalVolume* mother_logic_;
    // Dimensions
    G4double body_inner_diam_, body_length_, body_thickness_, body_zpos_;
    G4double tracking_tread_diam_, tracking_tread_length_;

    // Dimensions coming from outside
    G4double  lat_nozzle_in_diam_, lat_nozzle_x_pos_, lat_nozzle_z_pos_;
    G4double  up_small_nozzle_in_diam_, up_big_nozzle_in_diam_, up_nozzle_y_pos_, up_nozzle_z_pos_;
    G4double  center_nozzle_z_pos_;
    
    // Visibility of the shielding
    G4bool visibility_;

    // Vertex generators
    CylinderPointSampler* body_gen_;
    CylinderPointSampler* tread_gen_;
    G4double body_perc_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

} // end namespace nexus

#endif
