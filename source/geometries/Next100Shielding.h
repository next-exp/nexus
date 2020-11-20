// ----------------------------------------------------------------------------
// nexus | Next100Shielding.h
//
// Lead castle placed at the LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_SHIELDING_H
#define NEXT100_SHIELDING_H

#include "BaseGeometry.h"

#include <G4Navigator.hh>

class G4GenericMessenger;



namespace nexus {

  class BoxPointSampler;

  class Next100Shielding: public BaseGeometry
  {
  public:
    /// Constructor
    Next100Shielding();

    /// Destructor
    ~Next100Shielding();

    // Returns the inner air logical volume to place the vessel into it
    G4LogicalVolume* GetAirLogicalVolume() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Returns a point within a region projecting from a
    /// given point backwards along a line.
    G4ThreeVector ProjectToRegion(const G4String& region,
				  const G4ThreeVector& point,
				  const G4ThreeVector& dir) const;


    /// Builder
    void Construct();

    /// Retrieve dimensions
    G4ThreeVector GetDimensions() const;


  private:

    // Dimensions
    G4double lead_x_, lead_y_, lead_z_;
    G4double shield_x_, shield_y_, shield_z_;
    G4double  beam_base_thickness_, lateral_z_separation_, roof_z_separation_ , front_x_separation_;
    G4double lead_thickness_, steel_thickness_;

    // Visibility of the shielding
    G4bool visibility_;

    // Vertex generators
    BoxPointSampler* lead_gen_;
    BoxPointSampler* external_gen_;
    BoxPointSampler* steel_gen_;
    BoxPointSampler* inner_air_gen_;
    BoxPointSampler* lat_roof_gen_;
    BoxPointSampler* front_roof_gen_;
    BoxPointSampler* struct_x_gen_;
    BoxPointSampler* struct_z_gen_;
    BoxPointSampler* lat_beam_gen_;
    BoxPointSampler* front_beam_gen_;

    G4double perc_roof_vol_;
    G4double perc_front_roof_vol_;
    G4double perc_top_struct_vol_;
    G4double perc_struc_x_vol_;


    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Air logical volume
    G4LogicalVolume* air_box_logic_;

  };

} // end namespace nexus

#endif
