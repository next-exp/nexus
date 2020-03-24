// ----------------------------------------------------------------------------
///  \file
///  \brief
///
///  \author   <miquel.nebot@ific.uv.es>, <jmunoz@ific.uv.es>,
/// <justo.martin-albo@ific.uv.es>, <paola.ferrario@ific.uv.es>
///  \date     9 Sept 2013
///  \version  $Id$
///
///  Copyright (c) 2013 NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef __NEXTNEW_VESSEL__
#define __NEXTNEW_VESSEL__

#include "BaseGeometry.h"
#include "CylinderPointSampler.h"
#include "SpherePointSampler.h"


#include <G4Navigator.hh>

class G4GenericMessenger;

namespace nexus {

  class CalibrationSource;

  class NextNewVessel: public BaseGeometry
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
    G4double  _vessel_in_diam, _vessel_body_length, _vessel_tube_length, _vessel_thickness;
    G4double _flange_out_diam, _flange_length, _flange_z_pos;
    G4double _endcap_in_rad, _endcap_theta, _endcap_thickness, _endcap_in_z_width;

    G4double _lat_nozzle_in_diam, _lat_nozzle_high, _lat_nozzle_thickness, _lat_nozzle_flange_diam, _lat_nozzle_flange_high;
    G4double _up_nozzle_in_diam, _up_nozzle_high, _up_nozzle_thickness, _up_nozzle_flange_diam, _up_nozzle_flange_high;
    G4double  _endcap_nozzle_in_diam, _endcap_nozzle_high, _endcap_nozzle_thickness, _endcap_nozzle_flange_diam, _endcap_nozzle_flange_high;
    G4double  _lat_nozzle_z_pos, _up_nozzle_z_pos, _endcap_nozzle_z_pos;
    //_lat_nozzle_x_pos,  _up_nozzle_y_pos,

     // Dimensions of the source tubes
    G4double _port_tube_diam, _port_tube_thickness, _port_tube_window_thickn;
    G4double _lat_port_tube_length, _lat_port_tube_out;
    G4double _up_port_tube_length, _up_port_tube_out, _axial_port_tube_length;
    G4double _axial_port_tube_out, _axial_port_flange, _axial_distance_flange_endcap;

    // Position of the sources
    // Internal ones, at the bottom of the tube ports
    G4ThreeVector _axial_port_source_pos;
    G4ThreeVector _lateral_port_source_pos;
    G4ThreeVector _upper_port_source_pos;
    // External ones, at the beginning of the tube ports
    G4ThreeVector _lateral_port_source_pos_ext;
    G4ThreeVector _upper_port_source_pos_ext;
    G4ThreeVector _axial_port_source_pos_ext;

    G4double _pressure, _temperature;

    // Visibility of the shielding
    G4bool _visibility;

    // Internal Logical Volume
    G4LogicalVolume* _internal_logic_vol;
    G4VPhysicalVolume* _internal_phys_vol;

    // Vertex generators
    CylinderPointSampler* _body_gen;
    SpherePointSampler*   _tracking_endcap_gen;
    SpherePointSampler*   _energy_endcap_gen;
    CylinderPointSampler* _flange_gen;
    CylinderPointSampler* _screw_gen_lat;
    CylinderPointSampler* _screw_gen_up;
    CylinderPointSampler* _screw_gen_axial;

    G4double _perc_endcap_vol;
    G4double _perc_tube_vol;

    // Geometry Navigator
    G4Navigator* _geom_navigator;

    // Messenger for the definition of control commands
    G4GenericMessenger* _msg;

    // Primary scintillation yield
    G4double _sc_yield;

    // Electron attachment
    G4double _attachment;

    // Gas being used
    G4String _gas;
    G4double _xe_perc;
    G4int _helium_mass_num;

    // Radioactive source being used or not
    //   G4bool _source;
    // Distance of the source piece from the bottom of the tube (0 meaning right in the inner end)
    G4double _source_distance;

    CalibrationSource* _cal;

    // Which calibration port we are using (lateral/axial)
    G4String _calib_port;


  };

} // end namespace nexus

#endif
