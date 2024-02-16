// -----------------------------------------------------------------------------
// nexus | NextNewVessel.h
//
// Vessel of the NEXT-WHITE detector.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef NEXTNEW_VESSEL_H
#define NEXTNEW_VESSEL_H

#include "GeometryBase.h"

#include <G4Navigator.hh>

class G4GenericMessenger;

namespace nexus {

  class CalibrationSource;
  class CylinderPointSamplerLegacy;
  class SpherePointSampler;

  class NextNewVessel: public GeometryBase
  {
  public:
    /// Constructor
    NextNewVessel();
    ///Destructor
    ~NextNewVessel();


    /// Generate a vertex within a given region of the geometry
    G4ThreeVector GenerateVertex(const G4String& region) const;

    /// Returns the logical volume of the inner object
    G4LogicalVolume* GetInternalLogicalVolume() const;
    G4VPhysicalVolume* GetInternalPhysicalVolume() const;

    G4double GetUPNozzleZPosition() const;
    G4double GetLATNozzleZPosition() const;

    G4ThreeVector GetLatExtSourcePosition() const;
    G4ThreeVector GetUpExtSourcePosition() const;
    G4ThreeVector GetAxialExtSourcePosition() const;

    G4double GetOuterRadius() const;
    G4double GetLength() const;

    /// Builder
    void Construct();


  private:
    //Dimensions
    G4double  vessel_in_diam_, vessel_body_length_, vessel_tube_length_, vessel_thickness_;
    G4double flange_out_diam_, flange_length_, flange_z_pos_;
    G4double endcap_in_rad_, endcap_theta_, endcap_thickness_, endcap_in_z_width_;

    G4double lat_nozzle_in_diam_, lat_nozzle_high_, lat_nozzle_thickness_, lat_nozzle_flange_diam_, lat_nozzle_flange_high_;
    G4double up_nozzle_in_diam_, up_nozzle_high_, up_nozzle_thickness_, up_nozzle_flange_diam_, up_nozzle_flange_high_;
    G4double  endcap_nozzle_in_diam_, endcap_nozzle_high_, endcap_nozzle_thickness_, endcap_nozzle_flange_diam_, endcap_nozzle_flange_high_;
    G4double  lat_nozzle_z_pos_, up_nozzle_z_pos_, endcap_nozzle_z_pos_;
    //lat_nozzle_x_pos_,  up_nozzle_y_pos_,

     // Dimensions of the source tubes
    G4double port_tube_diam_, port_tube_thickness_, port_tube_window_thickn_;
    G4double lat_port_tube_length_, lat_port_tube_out_;
    G4double up_port_tube_length_, up_port_tube_out_, axial_port_tube_length_;
    G4double axial_port_tube_out_, axial_port_flange_, axial_distance_flange_endcap_;

    // Position of the sources
    // Internal ones, at the bottom of the tube ports
    G4ThreeVector axial_port_source_pos_;
    G4ThreeVector lateral_port_source_pos_;
    G4ThreeVector upper_port_source_pos_;
    // External ones, at the beginning of the tube ports
    G4ThreeVector lateral_port_source_pos_ext_;
    G4ThreeVector upper_port_source_pos_ext_;
    G4ThreeVector axial_port_source_pos_ext_;

    G4double pressure_, temperature_;

    // Visibility of the shielding
    G4bool visibility_;

    // Internal Logical Volume
    G4LogicalVolume* internal_logic_vol_;
    G4VPhysicalVolume* internal_phys_vol_;

    // Vertex generators
    CylinderPointSamplerLegacy* body_gen_;
    SpherePointSampler*   tracking_endcap_gen_;
    SpherePointSampler*   energy_endcap_gen_;
    CylinderPointSamplerLegacy* flange_gen_;
    CylinderPointSamplerLegacy* screw_gen_lat_;
    CylinderPointSamplerLegacy* screw_gen_up_;
    CylinderPointSamplerLegacy* screw_gen_axial_;

    G4double perc_endcap_vol_;
    G4double perc_tube_vol_;

    // Geometry Navigator
    G4Navigator* geom_navigator_;

    // Messenger for the definition of control commands
    G4GenericMessenger* msg_;

    // Primary scintillation yield
    G4double sc_yield_;

    // Electron lifetime
    G4double e_lifetime_;

    // Gas being used
    G4String gas_;
    G4double xe_perc_;
    G4int helium_mass_num_;

    // Radioactive source being used or not
    //   G4bool source_;
    // Distance of the source piece from the bottom of the tube (0 meaning right in the inner end)
    G4double source_distance_;

    CalibrationSource* cal_;

    // Which calibration port we are using (lateral/axial)
    G4String calib_port_;


  };

} // end namespace nexus

#endif
