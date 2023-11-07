// ----------------------------------------------------------------------------
// nexus | Next100Vessel.h
//
// Vessel of the NEXT-100 geometry.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXT100_VESSEL_H
#define NEXT100_VESSEL_H

#include "GeometryBase.h"

#include <G4Navigator.hh>


class G4GenericMessenger;
class G4VPhysicalVolume;

namespace nexus {

  class CylinderPointSampler2020;
  class SpherePointSampler;

  class Next100Vessel: public GeometryBase
  {
  public:
    /// Constructor
    Next100Vessel(G4double ics_ep_lip_width);

    /// Destructor
    ~Next100Vessel();

    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Returns the logical and physical volume of the inner object
    G4LogicalVolume* GetInternalLogicalVolume();
    G4VPhysicalVolume* GetInternalPhysicalVolume();

    void SetELtoTPdistance(G4double);
    // get Z position of calibration ports
    G4double* GetPortZpositions();

    /// Builder
    void Construct();

  private:
    // Dimensions
    const G4double vessel_in_rad_, vessel_thickness_;
    const G4double ics_ep_lip_width_;
    const G4double body_length_;
    const G4double endcap_in_rad_, endcap_in_body_, endcap_theta_;
    const G4double endcap_in_z_width_, endcap_tp_distance_;
    const G4double port_base_height_, port_tube_height_, port_tube_tip_;

    G4double port_x_, port_y_, source_height_, port_z_1a_, port_z_1b_;
    G4double port_z_2a_, port_z_2b_;
    G4double sc_yield_, e_lifetime_;
    G4double pressure_, temperature_;
    G4double gate_tp_distance_;

    // Visibility of the shielding
    G4bool visibility_;

    // Internal logical and physical volumes
    G4LogicalVolume* internal_logic_vol_;
    G4VPhysicalVolume* internal_phys_vol_;

    // Vertex generators
    CylinderPointSampler2020* body_gen_;
    SpherePointSampler*   tracking_endcap_gen_;
    SpherePointSampler*   energy_endcap_gen_;
    CylinderPointSampler2020* tracking_flange_gen_;
    CylinderPointSampler2020* energy_flange_gen_;
    CylinderPointSampler2020* port_gen_;

    G4double perc_endcap_vol_;
    G4double perc_ep_flange_vol_;
    G4double perc_tp_flange_vol_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Type of gas being used
    G4String gas_;
    G4int helium_mass_num_;
    G4double xe_perc_;

  };

  inline void Next100Vessel::SetELtoTPdistance(G4double distance){
    gate_tp_distance_ = distance;
  }

} // end namespace nexus

#endif
