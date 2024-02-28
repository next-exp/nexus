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

  class CylinderPointSampler;
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
    void SetGateZpos(G4double);

    G4double GetGateZpos();

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
    const G4double endcap_in_z_width_;
    const G4double port_base_height_, port_tube_height_, port_tube_tip_;
    const G4double port_angle_, port_x_, port_y_;

    G4double sc_yield_, e_lifetime_;
    G4double pressure_, temperature_;

    // Visibility of the shielding
    G4bool visibility_;

    // Type of gas being used
    G4String gas_;
    G4int helium_mass_num_;
    G4double xe_perc_;

    // Th calibration source
    G4String th_source_;
    G4double dist_th_zpos_end_;

    G4double port_z_1a_, port_z_1b_, port_z_2a_, port_z_2b_;
    G4double gate_tp_distance_, gate_z_pos_;

    // Internal logical and physical volumes
    G4LogicalVolume* internal_logic_vol_;
    G4VPhysicalVolume* internal_phys_vol_;

    // Vertex generators
    CylinderPointSampler* body_gen_;
    SpherePointSampler*   tracking_endcap_gen_;
    SpherePointSampler*   energy_endcap_gen_;
    CylinderPointSampler* tracking_flange_gen_;
    CylinderPointSampler* energy_flange_gen_;
    SpherePointSampler* th_port_gen_;
    CylinderPointSampler* th_white_port_gen_;

    G4double perc_endcap_vol_;
    G4double perc_ep_flange_vol_;
    G4double perc_tp_flange_vol_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

  };

  inline void Next100Vessel::SetELtoTPdistance(G4double distance){
    gate_tp_distance_ = distance;
  }

  inline void Next100Vessel::SetGateZpos(G4double pos){
    gate_z_pos_ = pos;
  }

  inline G4double Next100Vessel::GetGateZpos(){
    return gate_z_pos_;
  }

} // end namespace nexus

#endif
