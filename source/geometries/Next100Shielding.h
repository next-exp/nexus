// ----------------------------------------------------------------------------
// nexus | Next100Shielding.h
//
// Lead castle placed at the LSC.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_SHIELDING_H
#define NEXT100_SHIELDING_H

#include "GeometryBase.h"

#include <G4Navigator.hh>

class G4GenericMessenger;



namespace nexus {

  class BoxPointSampler;

  class Next100Shielding: public GeometryBase
  {
  public:
    /// Constructor
    Next100Shielding();

    /// Destructor
    ~Next100Shielding();

    // Returns the inner air logical volume to place the vessel into it
    G4LogicalVolume* GetAirLogicalVolume() const;

    // Returns the Air Box global position
    G4ThreeVector GetAirDisplacement() const;

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    G4double GetHeight() const;

    /// Builder
    void Construct();

    /// Retrieve dimensions
    G4ThreeVector GetDimensions() const;


  private:

    // Dimensions
    G4double lead_x_, lead_y_, lead_z_;
    G4double shield_x_, shield_y_, shield_z_;
    G4double beam_thickness_1, beam_thickness_2;
    G4double lateral_z_separation_, roof_z_separation_ , front_x_separation_;
    G4double lead_thickness_, steel_thickness_;
    G4double pedestal_x_, pedestal_top_x_;
    G4double support_beam_dist_, support_front_dist_;
    G4double pedestal_lateral_beam_thickness_;
    G4double pedestal_front_beam_thickness_;
    G4double pedestal_roof_thickness_;
    G4double pedestal_lateral_length_;
    G4double bubble_seal_thickness_;
    G4double edpm_seal_thickness_;

    G4bool visibility_;
    G4bool verbosity_;

    // Vertex generators
    BoxPointSampler* lead_gen_;
    BoxPointSampler* steel_gen_;
    BoxPointSampler* inner_air_gen_;
    BoxPointSampler* external_gen_;
    BoxPointSampler* lat_roof_gen_;
    BoxPointSampler* front_roof_gen_;
    BoxPointSampler* struct_x_gen_;
    BoxPointSampler* struct_z_gen_;
    BoxPointSampler* lat_beam_gen_;
    BoxPointSampler* front_beam_gen_;

    BoxPointSampler* ped_support_bottom_gen_;
    BoxPointSampler* ped_support_top_gen_;
    BoxPointSampler* ped_front_gen_;
    BoxPointSampler* ped_lateral_gen_;
    BoxPointSampler* ped_roof_lat_gen_;
    BoxPointSampler* ped_roof_front_gen_;

    BoxPointSampler* bubble_seal_front_gen_;
    BoxPointSampler* bubble_seal_lateral_gen_;
    BoxPointSampler* edpm_seal_front_gen_;
    BoxPointSampler* edpm_seal_lateral_gen_;

    G4double perc_roof_vol_;
    G4double perc_front_roof_vol_;
    G4double perc_top_struct_vol_;
    G4double perc_struc_x_vol_;
    G4double perc_ped_bottom_vol_;
    G4double perc_ped_top_vol_;
    G4double perc_ped_front_vol_;
    G4double perc_ped_lateral_vol_;
    G4double perc_ped_roof_vol_;

    G4double perc_bubble_front_vol_;
    G4double perc_buble_lateral_vol_;
    G4double perc_edpm_front_vol_;
    G4double perc_edpm_lateral_vol_;


    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Air logical volume
    G4LogicalVolume* air_box_logic_;

  };

  inline G4double Next100Shielding::GetHeight() const
  { return lead_y_; }

} // end namespace nexus

#endif
