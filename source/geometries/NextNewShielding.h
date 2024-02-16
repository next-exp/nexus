// ----------------------------------------------------------------------------
// nexus | NextNewShielding.h
//
// Lead castle placed at the LSC to be used with the NEW geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEW_SHIELDING_H
#define NEW_SHIELDING_H

#include "GeometryBase.h"

#include <G4Navigator.hh>

class G4GenericMessenger;



namespace nexus {

  class BoxPointSamplerLegacy;

  class NextNewShielding: public GeometryBase
  {
  public:
    /// Constructor
    NextNewShielding();

    /// Destructor
    ~NextNewShielding();

    // Returns the inner air logical volume to place the vessel into it
    G4LogicalVolume* GetAirLogicalVolume() const;

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
    G4double  beam_base_thickness_, lateral_z_separation_, roof_z_separation_ , front_x_separation_;
    G4double lead_thickness_, steel_thickness_;

    // Visibility of the shielding
    G4bool visibility_;

    // Vertex generators
    BoxPointSamplerLegacy* lead_gen_;
    BoxPointSamplerLegacy* external_gen_;
    BoxPointSamplerLegacy* steel_gen_;
    BoxPointSamplerLegacy* inner_air_gen_;
    BoxPointSamplerLegacy* lat_roof_gen_;
    BoxPointSamplerLegacy* front_roof_gen_;
    BoxPointSamplerLegacy* struct_x_gen_;
    BoxPointSamplerLegacy* struct_z_gen_;
    BoxPointSamplerLegacy* lat_beam_gen_;
    BoxPointSamplerLegacy* front_beam_gen_;

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

  inline G4double NextNewShielding::GetHeight() const
  { return lead_y_; }

} // end namespace nexus

#endif
